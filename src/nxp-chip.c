#include <stdio.h>
#include <ncihal/phNxpNciHal.h>
#include <linux_nfc_api.h>

typedef struct
{
    UINT8           rom_code_version;    /* ROM code Version  */
    UINT8           major_version;       /* Major Version */
    UINT8           minor_version;       /* Minor Version  */
} tNFC_FW_VERSION;

// variable from linux_libnfc-nci
// current firmware version
//extern uint32_t wFwVerRsp;
extern tNFC_FW_VERSION wFwVerRsp;


int main() {
	int nfc_initialize_status = nfcManager_doInitialize();
	if (nfc_initialize_status) {
		printf("NFC initialization failed with error code %d", nfc_initialize_status);
		return nfc_initialize_status;
	}

	tNFC_chipType chipNum = phNxpNciHal_getChipType();
	char *chipName = "unknown";

	switch (chipNum) {
		case pn547C2:
			chipName = "pn547C2";
			break;
		case pn65T:
			chipName = "pn65T";
			break;
		case pn548C2:
			chipName = "pnDefault/pn548C2";
			break;
		case pn66T:
			chipName = "pn66T";
			break;
		case pn551:
			chipName = "pn551";
			break;
		case pn67T:
			chipName = "pn67T";
			break;
		case pn553:
			chipName = "pn553";
			break;
		case pn80T:
			chipName = "pn80T";
			break;
		case pn557:
			chipName = "pn557";
			break;
		case pn81T:
			chipName = "pn81T";
			break;
		case pnInvalid:
			chipName = "pnInvalid";
			break;
		case pnUnknown:
		default:
			chipName = "unknown";
			break;
	}

	printf("HAL found %s chip\n", chipName);
	printf("FW version: %d.%d.%d\n", wFwVerRsp.rom_code_version, wFwVerRsp.major_version, wFwVerRsp.minor_version);
}
