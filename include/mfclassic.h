#ifndef __MFCLASSIC__H__
#define __MFCLASSIC__H__

#include "mfclassic_nxp_types.h"

#define MIFARE_CMD_AUTH_A (0x60)           ///< Auth A
#define MIFARE_CMD_AUTH_B (0x61)           ///< Auth B
#define MIFARE_CMD_READ (0x30)             ///< Read
#define MIFARE_CMD_WRITE (0xA0)            ///< Write
#define MIFARE_CMD_TRANSFER (0xB0)         ///< Transfer
#define MIFARE_CMD_DECREMENT (0xC0)        ///< Decrement
#define MIFARE_CMD_INCREMENT (0xC1)        ///< Increment
#define MIFARE_CMD_STORE (0xC2)            ///< Store

#define AUTH_SUCCESS (0)
#define AUTH_FAIL (1)


uint8_t *get_mifare_auth_cmd(key_types_t key_type, uint8_t block, uint8_t *key);
int send_mifare_auth(unsigned int nfc_handle, int block, uint8_t *key, key_types_t key_type, uint8_t *resp_bytes, int response_buf_len);
//uint8_t mifareReadBlock(uint8_t block, uint8_t *response);

#endif