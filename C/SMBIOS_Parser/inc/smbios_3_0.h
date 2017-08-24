#ifndef SMBIOS_3_0_H
#define SMBIOS_3_0_H

#include <stdint.h>
#include "smbios.h"

/*Define*/
#define SMBIOS_3_0_ANCHOR    "_SM3_"
#define SMBIOS_3_0_HDR_LEN_OFFSET 0x06

/*structure*/
typedef struct _smbios_3_0_header
{
	uint8_t  au8Anchor[5];	/*_SM3_*/
	uint8_t  u8CheckSum;
	uint8_t  u8Length;	
	uint8_t  u8MajorVer;
	uint8_t  u8MinorVer;
	uint8_t  u8Docrev;
	uint8_t	 u8Revision;
	uint8_t	 u8Reserved;
	uint32_t u32MaxStructSize;
	uint64_t u64StructTableAddr;
}sSMbios_3_0_header;


#endif
