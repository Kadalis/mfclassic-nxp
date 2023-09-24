#ifndef __KEYS_PARSER__H__
#define __KEYS_PARSER__H__

#include "mfclassic_nxp_types.h"

int parse_keys_file(FILE *keys_file, key_list_t *key_list_ptr);
void print_keys_in_chain(key_list_t *key_list_to_print);
void free_keys_in_chain(key_list_t *key_list_to_free);
void change_all_keys_types(key_list_t *key_list_to_change, key_types_t new_key_type);

#endif