#ifndef _SMBIOS_LIB_H
#define _SMBIOS_LIB_H

#include "smbios.h"
#include "smbios_2_1.h"
#include "smbios_3_0.h"

#define SMBIOS_MSG(fmt,argv...) printf(fmt,##argv)

/*define*/
#define SMBIOS_MAX_TYPE	127

/*define for type*/

#define SMBIOS_END_OF_TABLE 127
#define SMBIOS_MAX_STRING_PER_STRUCTURE	16

/*structure*/
typedef struct _smbios_string_part
{
	uint8_t u8Count;
	uint32_t u32Lens; /*This field includes last two '\0'*/
	char *pau8String[SMBIOS_MAX_STRING_PER_STRUCTURE];
}sSMbios_string_part;

typedef struct _smbios_data
{
	uint8_t  *pu8Data;
	sSMbios_string_part *psStr;
	struct _smbios_data *next;
}sSMbios_data;

typedef struct _smbios_type
{
	uint32_t u8Count;
	sSMbios_data *data;
}sSMbios_type;

typedef struct _smbios
{
	eSMBIOS_HDR_VER ver;
	union
	{
		sSMbios_2_1_header bit32header;
		sSMbios_3_0_header bit64header;		
	}smbios_header;	
	sSMbios_type type[SMBIOS_MAX_TYPE];
	
}sSMBios;

/*function*/
eSMBIOS_RET_CODE smbios_parser_from_file(sSMBios *psSmbios, const uint8_t* filename);
void smbios_dump_all(sSMBios *psSmbios);
void smbios_dump_type(sSMBios *psSmbios,uint8_t u8Type);
#endif
