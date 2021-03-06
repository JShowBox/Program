#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "inc/smbios_3_0.h"

eSMBIOS_RET_CODE smbios_3_0_parser(sSMbios_3_0_header *psSmbiosHdr,uint8_t *pu8SmData,uint64_t u64DataLen)
{
	uint8_t *pu8DataPtr = NULL;
	eSMBIOS_RET_CODE eRtCode = eSMBIOS_RET_OK;
	eSMBIOS_HDR_VER eSmbios_version;
	if(NULL == psSmbiosHdr)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [psSmbiosHdr] IS NULL\n");
		return eSMBIOS_RET_NULL_BUFF;				
	}
	
	if(NULL == pu8SmData)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [pu8SmData] IS NULL\n");
		return eSMBIOS_RET_NULL_BUFF;
	}	
	
	/*at least include Entry Point Length*/
	if(0x7 > u64DataLen)
	{
		SMBIOS_TOOL_ERR("SMBIOS 3.0 HEADER SIZE IS TOO SMALL\n");	
	}
	
	eSmbios_version = smbios_version(pu8SmData);
	if(eSMBIOS_HDR_VER_3_0 != eSmbios_version)
	{
		SMBIOS_TOOL_ERR("IT IS NOT SMBIOS 3.0 VERSION\n");
		return eSMBIOS_RET_VER_ERR;
	}
	
	memcpy(psSmbiosHdr,pu8SmData,pu8SmData[SMBIOS_3_0_HDR_LEN_OFFSET]);

	return eRtCode;	
}



