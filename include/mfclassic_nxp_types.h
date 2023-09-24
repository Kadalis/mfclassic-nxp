#ifndef __MFCLASSIC_NXP_TYPES__H__
#define __MFCLASSIC_NXP_TYPES__H__

typedef enum key_types {
	MIFARE_KEY_TYPE_A,
	MIFARE_KEY_TYPE_B,
	MIFARE_KEY_TYPE_BOTH
} key_types_t;

typedef struct key_list_node {
	int keys_count;
	//	Sequence number of key in list
	int key_num;
	//	A/B/Both
	key_types_t key_type;
	uint8_t key_bytes[6];
	char *string_from_file;
	struct key_list_node *next;
} key_list_t;

#endif