#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <mfclassic_nxp_types.h>
#include <mfclassic.h>
#include <keys_parser.h>


//	To remember if key a/b found
static bool key_a_found = false;
static bool key_b_found = false;

void print_key_found(int block, key_types_t key_type, char *key_string){
	fflush(stdout);
	if (key_type == MIFARE_KEY_TYPE_A)
		printf("\r[+] Key found for block %d! key type is %c. Key is %s", block, 'A', key_string);
	else if (key_type == MIFARE_KEY_TYPE_B) 
		printf("\r[+] Key found for block %d! key type is %c. Key is %s", block, 'B', key_string);
	else {
		printf("\r[E] Function print_key_found need MIFARE_KEY_TYPE_A or MIFARE_KEY_TYPE_B as key_type!");
		exit(1);
	}
}


int mfclassic_list_brute(unsigned int nfc_handle, key_list_t *list, int block){
	uint8_t resp_bytes[255];
	memset(resp_bytes,  0, sizeof(resp_bytes));

	key_a_found = false;
	key_b_found = false;
	
	int one_percent = list->keys_count/100;

	//	progress in percents.
	int current_progress = 0;
	int keys_tried = 0;
	while (list->next != NULL) {
		if (keys_tried % one_percent == 0 && current_progress < 100) current_progress++;
		fflush(stdout);
		printf("\r%d%%", current_progress);

		char *key_string = list->string_from_file;
		key_types_t key_type = list->key_type;

		int result = AUTH_FAIL;

		if (!key_a_found && key_type == MIFARE_KEY_TYPE_A) {
			result = send_mifare_auth(nfc_handle, block, list->key_bytes, MIFARE_KEY_TYPE_A, resp_bytes, 255);
		} else if (!key_b_found && key_type == MIFARE_KEY_TYPE_B){
			result = send_mifare_auth(nfc_handle, block, list->key_bytes, MIFARE_KEY_TYPE_B, resp_bytes, 255);
		} else if (key_type == MIFARE_KEY_TYPE_BOTH) {
			if (!key_a_found)
				result = send_mifare_auth(nfc_handle, block, list->key_bytes, MIFARE_KEY_TYPE_A, resp_bytes, 255);
			if (result == AUTH_SUCCESS){
				print_key_found(block, MIFARE_KEY_TYPE_A, key_string);
				key_a_found = true;
			}
			result = AUTH_FAIL;

			if (!key_b_found) 
				result = send_mifare_auth(nfc_handle, block, list->key_bytes, MIFARE_KEY_TYPE_B, resp_bytes, 255);
			if (result == AUTH_SUCCESS){
				print_key_found(block, MIFARE_KEY_TYPE_B, key_string);
				key_b_found = true;
			}
		}

		if (result == AUTH_SUCCESS){
			if (key_type == MIFARE_KEY_TYPE_A) {
				print_key_found(block, MIFARE_KEY_TYPE_A, key_string);
				key_a_found = true;
			}

			if (key_type == MIFARE_KEY_TYPE_B) {
				print_key_found(block, MIFARE_KEY_TYPE_B, key_string);
				key_b_found = true;
			}

		}

		if (key_a_found && key_b_found) {
			fflush(stdout);
			printf("\r[+] Both keys for block %d are found. Stopping bruter for this block.\n", block);
			return AUTH_SUCCESS;
		}

		list = list->next;
		keys_tried++;
	}
	fflush(stdout);
	printf("\r[!] Both or one of keys not found for block %d\n", block);
	return AUTH_FAIL;
}
