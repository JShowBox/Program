#ifndef SMBIOS_2_1_H
#define SMBIOS_2_1_H

#include <stdint.h>
#include "smbios.h"
/*Define*/
#define SMBIOS_2_1_ANCHOR    "_SM_"
#define SMBIOS_2_1_HDR_LEN_OFFSET 0x05

/*structure*/
typedef struct _smbios_2_1_header
{
	uint8_t  au8Anchor[4];	/*_SM_*/
	uint8_t  u8CheckSum;
	uint8_t  u8Length;
	uint8_t  u8MajorVer;
	uint8_t  u8MinorVer;
	uint16_t u16MaxStructSize;
	uint8_t	 u8Revision;
	uint8_t  au8Area[5];
	uint8_t  au8InterAnchor[5]; /*_DMI_*/
	uint8_t  u8InterCheckSum;
	uint16_t u16StructTableLength;
	uint32_t u32StructTableAddr;
	uint16_t u16NumOfSMbiosStructure;
	uint8_t  u8BcdRevision;
}sSMbios_2_1_header;


#endif
