#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <getopt.h>
#include <stdbool.h>
#include <errno.h>

#include <linux_nfc_api.h>

#include <mfclassic.h>
#include <keys_parser.h>
#include <bruter.h>

//unsigned char MifareReadCmd[] = {0x30U,  /*block*/ 0x00};

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

nfcTagCallback_t g_TagCB;
nfc_tag_info_t g_tagInfos;

void onTagArrival(nfc_tag_info_t *pTagInfo){
    puts("[+] Tag detected");
    unsigned int i = 0;
    printf("[+] UID: ");
    for(i = 0x00; i < (*pTagInfo).uid_length; i++){
        printf("%02X ", (unsigned char) (*pTagInfo).uid[i]);
    }
    puts("");
    g_tagInfos = *pTagInfo;
    pthread_cond_signal(&condition);
}

void onTagDeparture(void){
    printf("[*] Tag removed\n-------------\n");
    printf("\n-------------\n[*] Waiting for tag...\n");    
}

void print_help_and_exit(char *prog_name);

int main(int argc, char *argv[]) {
	FILE *keys_file = NULL;

	//	Will not change key types, if keys_flag = 0
	int keys_flag = 0;

	struct option long_options[] =
		{
			//	flags
			{"keys_a",		no_argument, &keys_flag, 1},
			{"keys_b",		no_argument, &keys_flag, 2},
			{"keys_both",	no_argument, &keys_flag, 3},
			//	options with args
			{"sector",  	required_argument, 0, 's'},
			{"block",   	required_argument, 0, 'b'},
			{"keysfile",  	required_argument, 0, 'f'},
			//	options without args
			{"help",		no_argument, 0, 'h'},
			{0, 0, 0, 0}
        };

	int option_index = 0;

	unsigned long block_num = 0;
	int sector_num = -1;

	int c = 0;

	while ((c = getopt_long(argc, argv, "f:b:s:h", long_options, &option_index)) != -1L) {
		switch (c)
		{
			case 'f': {
				if (!access(optarg, R_OK)) {
					keys_file = fopen(optarg, "rt");
					if (keys_file == NULL) {
						puts("[E] Strange, there is an error trying to open file that was accessable a moment ago... better to exit, I think.");
						exit(1);
					}
				} else {
					fprintf(stderr, "[E] Have no access to file %s : %s\n", optarg,strerror(errno));
					exit(2);
				}

				break;
			}

			case 'b': {
				char *endptr = NULL;

				block_num = strtoul(optarg, &endptr, 0);
				if (endptr == optarg){
					puts("[E] Block number not found!");
					print_help_and_exit(argv[0]);
				}

				break;
			}

			case 's': {
				char *endptr = NULL;

				sector_num = strtoul(optarg, &endptr, 0);
				if (endptr == optarg){
					sector_num = -1;
					puts("[E] Sector number not found!");
					print_help_and_exit(argv[0]);
				}

				break;
			}
			case 'h': {
					print_help_and_exit(argv[0]);
				}
		}
	}

	if (keys_file == NULL) {
		puts("[E] Looks like keys dictionary not supplied. Exiting.");
		print_help_and_exit(argv[0]);
	}

	if (sector_num != -1) {
		block_num = 4 * sector_num;
		printf("[*] Sector is %d, bruting block %ld\n", sector_num, block_num);
	}

	if (block_num == 0 && sector_num == -1) {
		puts("[*] Block or sector is not set. Bruting 0 block as default");
	}



	key_list_t *key_list = malloc(sizeof(key_list_t));
	int keys_read = parse_keys_file(keys_file, key_list);
	printf("[*] %d keys loaded.\n", keys_read);


	if (keys_flag){
		key_types_t new_key_type = MIFARE_KEY_TYPE_BOTH;
		if (keys_flag == 1) {
			puts("[*] Treating all keys as A type");
			new_key_type = MIFARE_KEY_TYPE_A;
		}
		if (keys_flag == 2) {
			puts("[*] Treating all keys as B type");
			new_key_type = MIFARE_KEY_TYPE_B;
		}
		if (keys_flag == 3) {
			puts("[*] Treating all keys as A&B types.");
			new_key_type = MIFARE_KEY_TYPE_BOTH;
		}
		change_all_keys_types(key_list, new_key_type);
	} else puts("[*] Using defined-in-file key types.");

	puts("[*] Initializing linux_libnfc-nci...");
	int init_result = nfcManager_doInitialize();
	if (init_result){
		puts("[E] Failed to init linux_libnfc-nci. Maybe chip is unsupported.");
		return 3;
	}
	puts("[+] linux_libnfc-nci init successful!");

    g_TagCB.onTagArrival = onTagArrival;
    g_TagCB.onTagDeparture = onTagDeparture;

    nfcManager_registerTagCallback(&g_TagCB);
    nfcManager_enableDiscovery(DEFAULT_NFA_TECH_MASK, 0x01, 0, 0);

    puts("-------------\n[*] Waiting for tag...");
    do{
        pthread_cond_wait(&condition, &mutex);
       	if (g_tagInfos.technology != TARGET_TYPE_MIFARE_CLASSIC){
       		puts("[!] This tag is not Mifare Classic, ignoring.");
       		continue;
       	}
        puts("[*] Found MF Classic tag. Starting bruter.");
		mfclassic_list_brute(g_tagInfos.handle, key_list, block_num);
		puts("[*] Bruter completed it's work for this tag.");
	} while(1);
	nfcManager_doDeinitialize();

	free_keys_in_chain(key_list);
	return 0;
}

void print_help_and_exit(char *prog_name) {
	printf(
		"\t%s - tool for bruting mifare classic keys using NXP chips via linux_libnfc-nci.\n"
		"\tCommon usage:\n"
		"\t%s <options>\n"
		"\n"
		"\tMandatory parameters:\n"
		"\t\t-f/--keys_file <filepath> - use this file as keys dictionary\n"
		"\n"
		"\tOptions:\n"
		"\t\t-b/--block <block_num> - brute <block_num> block\n"
		"\t\t-s/--sector <sector_num> - brute first block of <sector_num> sector\n"
		"\n"
		"\tFlags:\n"
		"\t\t-h/--help - print this help message and exit\n"
		"\t\t--keys_a - try all keys only as A key\n"
		"\t\t--keys_b - try all keys only as B key\n"
		"\t\t--keys_both - try all keys as both A and B key\n",
		prog_name, prog_name
		);

	exit(1);
}
