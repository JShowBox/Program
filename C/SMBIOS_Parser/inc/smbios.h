#ifndef _SMBIOS_H
#define _SMBIOS_H

#include <stdint.h>

/*define for debug*/
#define SMBIOS_TOOL_ERR(fmt,argv...) printf(fmt,##argv)
#if 1
#define SMBIOS_TOOL_DBG(fmt,argv...)
#else
#define SMBIOS_TOOL_DBG(fmt,argv...) printf(fmt,##argv)
#endif
/*enum*/
typedef enum
{
	eSMBIOS_HDR_VER_2_1 = 0,
	eSMBIOS_HDR_VER_3_0,
	eSMBIOS_HDR_VER_UNKNOWN
}eSMBIOS_HDR_VER;

typedef enum
{
	eSMBIOS_RET_OK = 0,
	eSMBIOS_RET_FAIL,
	eSMBIOS_RET_NULL_BUFF,
	eSMBIOS_RET_UNKNOWN_SMBIOS_VER,
	eSMBIOS_RET_VER_ERR,
	eSMBIOS_RET_ALLCOATE_FAIL,
	eSMBIOS_RET_STRING_PARSER_FAIL,
	eSMBIOS_RET_STRING_OUT_OF_RANGE,
	eSMBIOS_RET_CANNOT_OPEN_FILE,
}eSMBIOS_RET_CODE;

/*structure*/
typedef struct _smbios_comm_header
{
	uint8_t  u8Type;
	uint8_t  u8Length;
	uint16_t u16Handle;	
}sSMbios_comm_hdr;


#endif
