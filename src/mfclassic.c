#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <mfclassic.h>
#include <linux_nfc_api.h>

uint8_t *get_mifare_auth_cmd(key_types_t key_type, uint8_t block, uint8_t *key){
	uint8_t cmd_key_type;

	switch (key_type){
		case MIFARE_KEY_TYPE_A:
			cmd_key_type = MIFARE_CMD_AUTH_A;
			break;
		case MIFARE_KEY_TYPE_B:
			cmd_key_type = MIFARE_CMD_AUTH_B;
			break;
		case MIFARE_KEY_TYPE_BOTH:
		default:
			puts("Function get_mifare_auth_cmd needs MIFARE_KEY_TYPE_A or MIFARE_KEY_TYPE_B as key_type");
			exit(3);
	}
	//	Auth cmd used in linux_libnfc-nci's nfcDemoApp
	//	Key is last 6 bytes.
	//	linux_libfc-nci stack actually need those four bytes.
	//	I don't know why, but without those bytes, card answers "0x40 0x03" - doesn't matter correct key or not.
	//	At least one difference between libnfc and linux_libnfc-nci
	
	uint8_t mifare_auth_cmd[] = {cmd_key_type, block, 0x02, 0x02, 0x02, 0x02,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	//	Auth cmd used with libnfc:
	/*uint8_t mifare_auth_cmd[] = {cmd_key_type, block,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};*/
	uint8_t *res = malloc(sizeof(mifare_auth_cmd));

	memcpy(&mifare_auth_cmd[6], key, 6);
	memset(res, 0, sizeof(mifare_auth_cmd));
	memcpy(res, mifare_auth_cmd, sizeof(mifare_auth_cmd));
	return res;
}

int send_mifare_auth(unsigned int nfc_handle, int block, uint8_t *key, key_types_t key_type, uint8_t *resp_bytes, int response_buf_len){
	if (key_type == MIFARE_KEY_TYPE_BOTH) {
			puts("Function send_mifare_auth needs MIFARE_KEY_TYPE_A or MIFARE_KEY_TYPE_B as key_type");
			exit(3);
	}

	uint8_t *auth_cmd = get_mifare_auth_cmd(key_type, block, key);
	/*printf("auth_cmd: ");
	for (int i = 0; i < 12; i++) {
            printf("%02X", auth_cmd[i]);
        }
	puts("");*/
	int recv_bytes = nfcTag_transceive(nfc_handle, auth_cmd, 12, resp_bytes, response_buf_len, 500);
	free(auth_cmd);

	if (recv_bytes > 0 && (resp_bytes[0] == 0x40 && resp_bytes[1] == 0x00))
		return AUTH_SUCCESS;
	else
		return AUTH_FAIL;
}

