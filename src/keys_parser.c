#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include <mfclassic_nxp_types.h>


static int current_line_num = 0;

static void fill_keys_count(key_list_t *key_list_to_fill, int keys_count){
	while(key_list_to_fill->next != NULL){
		key_list_to_fill->keys_count = keys_count;
		key_list_to_fill = key_list_to_fill->next;
	}
}

/* From
 *	0xA to b00001010
 *	0x9 to b00001001
 *	0xF to b00001111
 *	etc...
*/
static int8_t hex_char_to_lower_bits(char symbol) {
	int8_t lower_bits = 0x00;

	if (symbol >= 48 && symbol <= 57)
		lower_bits = symbol & 0x0F;
	else switch (symbol) {
			case 'A':
			case 'a':
				lower_bits = 0xA;
				break;
			case 'B':
			case 'b':
				lower_bits = 0xB;
				break;
			case 'C':
			case 'c':
				lower_bits = 0xC;
				break;
			case 'D':
			case 'd':
				lower_bits = 0xD;
				break;
			case 'E':
			case 'e':
				lower_bits = 0xE;
				break;
			case 'F':
			case 'f':
				lower_bits = 0xF;
				break;
			default:
				printf("[E] Unexpected symbol found while parsing 6-byte MFClassic key on line %d", current_line_num);
				exit(2);
	}
	return lower_bits;
}


//	Return 0 if key creaton is successful.
static int parse_key(char *line, int symbols_in_line, key_list_t *key_list, int keys_read_earlier) {
	// 12 bytes for key and 13-th byte for \n (or +2 for :A/:B suffixes)

	if (symbols_in_line != 13 && !(symbols_in_line == 15 && line[12] == ':')){
		printf("[!] Found malformed key on line %d, ignoring\n", current_line_num);
		return 1;
	}
	key_list_t *new_key_file_node_ptr = malloc(sizeof(key_list_t));
	memset(new_key_file_node_ptr, '\0', sizeof(key_list_t));

	key_list->next = new_key_file_node_ptr;
	key_list->string_from_file = strdup(line);
	key_list->key_num = keys_read_earlier+1;

	for (int i = 0; i < 6; i++) {
		uint8_t key_byte = 0x00;
		char first_symbol_of_pair = line[i * 2];
		key_byte = hex_char_to_lower_bits(first_symbol_of_pair) << 4;
		
		char second_symbol_of_pair = line[1 + i*2];
		key_byte += hex_char_to_lower_bits(second_symbol_of_pair);
	
		key_list->key_bytes[i] = key_byte;
	}

	if (symbols_in_line == 13)
		key_list->key_type = MIFARE_KEY_TYPE_BOTH;
	else switch (line[13]){
			case 'A':
				key_list->key_type = MIFARE_KEY_TYPE_A;
				break;
			case 'B':
				key_list->key_type = MIFARE_KEY_TYPE_B;
				break;
			default:
				printf("[!] Found malformed key on line %d, ignoring\n", current_line_num);

				//	Cleaning already created parameters
				free(new_key_file_node_ptr);
				key_list->next = NULL;
				free(key_list->string_from_file);
				key_list->string_from_file = NULL;
				key_list->key_num = 0;
				return 1;
				break;
		}
	return 0;
}

int parse_keys_file(FILE *keys_file, key_list_t *key_list_ptr) {
	char *current_line = NULL;
	size_t line_len = 0;
	key_list_t *old_key_list_ptr = key_list_ptr;

	int symbols_read, keys_read = 0;
	while ((symbols_read = getline(&current_line, &line_len, keys_file)) != -1) {
		current_line_num++;
		char first_symbol = *current_line;
		if (isxdigit(first_symbol)) {
			// next node allocation is inside parse_key function
		   	if (parse_key(current_line, symbols_read, key_list_ptr, keys_read) == 0){
		   		keys_read++;
		   		// make key_list_ptr point to next node, to fill next node on next iteration
		   		key_list_ptr = key_list_ptr->next;
		   	}
		} else continue;
	}
	free(current_line);
	fill_keys_count(old_key_list_ptr, keys_read);

	return keys_read;
}


void print_keys_in_chain(key_list_t *key_list_to_print) {
	while (key_list_to_print->next != NULL) {
		for (int i = 0; i < 6; i++)
			printf("%02X ", key_list_to_print->key_bytes[i]);

		puts("");
		printf("Key type is %d\n", key_list_to_print->key_type);
		puts("");

		key_list_to_print = key_list_to_print->next;
	}
}

void free_keys_in_chain(key_list_t *key_list_to_free) {
	while (key_list_to_free->next != NULL){
		free(key_list_to_free->string_from_file);
		key_list_t *toBeFreed = key_list_to_free;
		key_list_to_free = key_list_to_free->next;
		free(toBeFreed);
	}

	// if next node is NULL, then current node is empty
	// because any non-empty node have 'next' pointing to other non-empty node, or to empty node
	free(key_list_to_free);
}

void change_all_keys_types(key_list_t *key_list_to_change, key_types_t new_key_type) {
	while (key_list_to_change->next != NULL) {
		key_list_to_change->key_type = new_key_type;

		key_list_to_change = key_list_to_change->next;
	}
}
