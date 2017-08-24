
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "inc/smbiosLib.h"

void smbios_free(void *dat)
{
	if(dat)
	{
		free(dat);	
	}	
}

void* smbios_alloc(uint64_t u64count, uint64_t u64size)
{
	void *ptr = NULL;
	ptr = calloc(u64count,u64size);
	return ptr;
}
/*
    This API is used to return current verison of the smbios-table 
*/
eSMBIOS_HDR_VER smbios_version(uint8_t *pu8Anchor)
{
	if(!pu8Anchor)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [pu8Anchor] IS NULL\n");
		return eSMBIOS_HDR_VER_UNKNOWN;
	}
	
	if( !strncmp(pu8Anchor,SMBIOS_3_0_ANCHOR,5))
	{
		return eSMBIOS_HDR_VER_3_0;
	}
	
	if( !strncmp(pu8Anchor,SMBIOS_2_1_ANCHOR,4))
	{
		return eSMBIOS_HDR_VER_2_1;
	}
	
	return eSMBIOS_HDR_VER_UNKNOWN;
}

eSMBIOS_RET_CODE smbios_string_part_parser(sSMbios_string_part *psStringPart,uint8_t *pu8String)
{
	uint8_t u8Idx = 0;
	uint8_t *pu8StrPtr = NULL;
	uint32_t u32StrLen = 0;
	if(NULL == psStringPart)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [psStringPart] IS NULL\n");
		return eSMBIOS_RET_NULL_BUFF;		
	}
	
	if(NULL == pu8String)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [pu8Sting] IS NULL\n");
		return eSMBIOS_RET_NULL_BUFF;		
	}

	memset(psStringPart,0,sizeof(sSMbios_string_part));
	pu8StrPtr = pu8String;
	u32StrLen = strlen(pu8StrPtr)+1;
	/*if there is string part in the structure*/
	if( u32StrLen == 1 )
	{
		u32StrLen++;	
	}
	else
	{
		while( (1 < u32StrLen) )	
		{
			
			if( SMBIOS_MAX_STRING_PER_STRUCTURE <= u8Idx )
			{
				SMBIOS_TOOL_ERR("NUMBER OF STRUCTURE STRING IS OUT OF RANGE\n");
				return eSMBIOS_RET_STRING_OUT_OF_RANGE;
			}
			
			u8Idx = psStringPart->u8Count;
			psStringPart->pau8String[u8Idx] = (uint8_t*) smbios_alloc(1,u32StrLen);
			
			if(NULL == psStringPart->pau8String[u8Idx])
			{
				SMBIOS_TOOL_ERR("ALLOCATE MEMORY FOR [psStringPart->pau8String[%d]] FAIL\n",u8Idx);
				return eSMBIOS_RET_ALLCOATE_FAIL;
			}
			
			memcpy(psStringPart->pau8String[u8Idx],pu8StrPtr,u32StrLen);
			SMBIOS_TOOL_DBG("string[%d] = %s\n",u8Idx+1,psStringPart->pau8String[u8Idx]);
			psStringPart->u8Count++;
			psStringPart->u32Lens += u32StrLen;
			/*pointer to Next String*/
			pu8StrPtr = pu8StrPtr + u32StrLen;
			u32StrLen = strlen(pu8StrPtr)+1;	
		}
	}
	psStringPart->u32Lens += u32StrLen;
	return eSMBIOS_RET_OK;
}

eSMBIOS_RET_CODE smbios_structure_parser(sSMBios *psSmbios,uint8_t *pu8Data,uint64_t u64DataLen)
{
	uint8_t *pu8strPtr = NULL;
	uint8_t *pu8CurrPtr = NULL;

	sSMbios_comm_hdr *hdr = NULL;
	sSMbios_string_part *smbString = NULL;
	sSMbios_type *sSmType = NULL;
	sSMbios_data *sSmData = NULL;
	sSMbios_data *sSmDatPtr = NULL;
	eSMBIOS_RET_CODE eRtCode = eSMBIOS_RET_OK;

	pu8CurrPtr = (uint8_t*)pu8Data;
	hdr = (sSMbios_comm_hdr *) pu8CurrPtr;
	while( (hdr->u8Type != SMBIOS_END_OF_TABLE) && (pu8CurrPtr <  pu8Data + u64DataLen) )
	{
		SMBIOS_TOOL_DBG("Parser Type = %d\n",hdr->u8Type);
		sSmType = &psSmbios->type[hdr->u8Type];
		sSmData = (sSMbios_data *)smbios_alloc(1,sizeof(sSMbios_data));
		if( NULL == sSmData)
		{
			SMBIOS_TOOL_ERR("ALLOCATE MEMORY FOR [sSmData] FAIL\n");
			return eSMBIOS_RET_ALLCOATE_FAIL;
		}
			
		sSmData->pu8Data = (uint8_t *)smbios_alloc(1,hdr->u8Length * sizeof(uint8_t));
		if( NULL == sSmData->pu8Data)
		{
			free(sSmData);
			SMBIOS_TOOL_ERR("ALLOCATE MEMORY FOR [sSmData] FAIL\n");
			return eSMBIOS_RET_ALLCOATE_FAIL;
		}		
		
		/*pointer to string part of structure*/
		pu8strPtr = pu8CurrPtr + hdr->u8Length;
		smbString = smbios_alloc(1,sizeof(sSMbios_string_part));
		if( NULL == smbString )
		{
			free(sSmData->pu8Data);
			free(sSmData);
			SMBIOS_TOOL_ERR("ALLOCATE MEMORY FOR [smbString] FAIL\n");
			return eSMBIOS_RET_ALLCOATE_FAIL;
		}
		/*parser structure string*/
		eRtCode = smbios_string_part_parser(smbString,pu8strPtr);
		if( eSMBIOS_RET_OK !=  eRtCode )
		{
			free(sSmData->pu8Data);
			free(sSmData);
			free(smbString);
			SMBIOS_TOOL_ERR("PARSER STRUCTURE STRING FAIL,RET[%d]\n",eRtCode);	
			return eSMBIOS_RET_STRING_PARSER_FAIL;	
		}
		
		/*assign to type structure*/
		memcpy(sSmData->pu8Data,pu8CurrPtr,hdr->u8Length);
		sSmData->psStr = smbString;
		
		/*assign smbios structure string part*/	
		if(sSmType->u8Count == 0)
		{
			sSmType->data = sSmData;
		}
		else
		{
			sSmDatPtr = sSmType->data;
			while(sSmDatPtr->next != NULL)
			{
				sSmDatPtr = sSmDatPtr->next;	
			}
			sSmDatPtr->next = sSmData;	
		}
		sSmType->u8Count++;
		/*pointer to next Structure*/
		//SMBIOS_TOOL_DBG("Parser Type = %d, len = %d , string len = %d\n",hdr->u8Type,hdr->u8Length,smbString->u32Lens);
		pu8CurrPtr = pu8CurrPtr + hdr->u8Length + smbString->u32Lens;
		hdr = (sSMbios_comm_hdr *) pu8CurrPtr;
	}
	return eSMBIOS_RET_OK;
}

eSMBIOS_RET_CODE smbios_parser(sSMBios *psSmbios,uint8_t *pu8SmData,uint64_t u64DataLen)
{
	
	uint8_t *pu8DataPtr = NULL;
	uint64_t u64Len = 0;
	eSMBIOS_RET_CODE eRtCode = eSMBIOS_RET_OK;
	eSMBIOS_HDR_VER eSmbios_version;

	if(NULL == psSmbios)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [psSmbios] IS NULL\n");
		return eSMBIOS_RET_NULL_BUFF;
	}

	if(NULL == pu8SmData)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [pu8SmData] IS NULL\n");
		return eSMBIOS_RET_NULL_BUFF;
	}
	
	/*Check smbios table version*/
	eSmbios_version = smbios_version(pu8SmData);
	if(eSMBIOS_HDR_VER_UNKNOWN == eSmbios_version)
	{
		SMBIOS_TOOL_ERR("UNKNOWN SMBIOS VERSION\n");
		return eSMBIOS_RET_UNKNOWN_SMBIOS_VER;
	}	
	else if(eSMBIOS_HDR_VER_2_1 == eSmbios_version)
	{
		eRtCode = smbios_2_1_parser(&psSmbios->smbios_header.bit32header,pu8SmData,u64DataLen);
		if(eSMBIOS_RET_OK == eRtCode)
		{
			psSmbios->ver = eSMBIOS_HDR_VER_2_1;
			pu8DataPtr = pu8SmData + psSmbios->smbios_header.bit32header.u8Length;
			u64Len = u64DataLen - psSmbios->smbios_header.bit32header.u8Length;
		}
		else
		{
			SMBIOS_TOOL_ERR("SMBIOS 2.1 HEADER PARSER FAIL,RET[%d]\n",eRtCode);
			return eRtCode;
		}
	}
	else if(eSMBIOS_HDR_VER_3_0 == eSmbios_version)
	{
		eRtCode = smbios_3_0_parser(&psSmbios->smbios_header.bit64header,pu8SmData,u64DataLen);
		if(eSMBIOS_RET_OK == eRtCode)
		{
			psSmbios->ver = eSMBIOS_HDR_VER_3_0;
			pu8DataPtr = pu8SmData + psSmbios->smbios_header.bit64header.u8Length;
			u64Len = u64DataLen - psSmbios->smbios_header.bit64header.u8Length;
		}
		else
		{
			SMBIOS_TOOL_ERR("SMBIOS 3.0 HEADER PARSER FAIL,RET[%d]\n",eRtCode);
			return eRtCode;
		}
	}
	
	/*Handler Data*/
	eRtCode = smbios_structure_parser(psSmbios,pu8DataPtr,u64Len);
	if(eSMBIOS_RET_OK != eRtCode)
	{
		SMBIOS_TOOL_ERR("SMBIOS STRUCTURE PARSER FAIL,RET[%d]\n",eRtCode);
		return eRtCode;
	}
	return eRtCode;
}

eSMBIOS_RET_CODE smbios_parser_from_file(sSMBios *psSmbios, const uint8_t* filename)
{
	uint32_t u32Len;
	long len = 0;
	FILE *fp = NULL;
	uint8_t *pu8Buf = NULL;
	eSMBIOS_RET_CODE eRtCode = eSMBIOS_RET_OK;
	if(NULL == filename)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [filename] IS NULL \n");
		return eSMBIOS_RET_NULL_BUFF;	
	}
	
	if(NULL == psSmbios)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [psSmbios] IS NULL\n");
		return eSMBIOS_RET_NULL_BUFF;
	}
	
	fp = fopen(filename,"rb");
	if(NULL == fp)
	{
		SMBIOS_TOOL_ERR("FILE [%s] CAN NOT BE OPENED\n",filename);	
		return eSMBIOS_RET_CANNOT_OPEN_FILE;
	}
		
	if( 0 != fseek(fp,0,SEEK_END) )
	{
		fclose(fp);
		SMBIOS_TOOL_ERR("FILE [%s] CAN NOT BE SEEK TO THE END\n",filename);	
		return eSMBIOS_RET_CANNOT_OPEN_FILE;
	}	
	
	len = ftell(fp);
	SMBIOS_TOOL_DBG("FILE LENGTH = %d\n",len);
	if( len < 0  )
	{
		fclose(fp);
		SMBIOS_TOOL_ERR("FILE [%s] CAN NOT BE TELL THE CURRENT INDEX\n",filename);	
		return eSMBIOS_RET_CANNOT_OPEN_FILE;
	}
	pu8Buf = smbios_alloc(1,len+1);
	if(NULL == pu8Buf)
	{
		SMBIOS_TOOL_ERR("ALLOCATE MEMORY FOR [pu8Buf] FAIL\n");
		return eSMBIOS_RET_STRING_PARSER_FAIL;		
	}
	
	if( 0 != fseek(fp,0,SEEK_SET) )
	{
		fclose(fp);
		SMBIOS_TOOL_ERR("FILE [%s] CAN NOT BE SEEK TO THE START\n",filename);	
		return eSMBIOS_RET_CANNOT_OPEN_FILE;
	}	
		
	u32Len = fread(pu8Buf,1,len,fp);
	if( u32Len != len )
	{
		fclose(fp);
		SMBIOS_TOOL_ERR("READ COUNT IS NOT ENOUGH IN FILE [%s]\n",filename);	
		return eSMBIOS_RET_CANNOT_OPEN_FILE;		
	}
	eRtCode = smbios_parser(psSmbios,pu8Buf,u32Len);
	free(pu8Buf);
	return eRtCode;
}

void smbios_release(sSMBios *psSmbios)
{
	int i = 0;
	int j = 0;
	sSMbios_data *psDat = NULL;
	sSMbios_data *psDatNext = NULL;
	if(NULL == psSmbios)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [psSmbios] IS NULL\n");
		return;
	}
	
	for( i = 0; i < SMBIOS_MAX_TYPE; i++)
	{
		psDat = psSmbios->type[i].data;
		while(psDat)
		{
			psDatNext = psDat->next;	
			smbios_free(psDat->pu8Data);
			psDat->pu8Data = NULL;
			if(psDat->psStr)
			{
				for(j = 0; j < psDat->psStr->u8Count; j++)
				{
					smbios_free(psDat->psStr->pau8String[j]);
				}
				smbios_free(psDat->psStr);
			}
			smbios_free(psDat);
			psDat = psDatNext;
		}	
	}
	return;	
}

void smbios_dump_struct(uint8_t *pu8Buf)
{
	int i = 0;
	uint8_t u8len = 0;
	if(NULL == pu8Buf)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [pu8Buf] IS NULL\n");
		return;
	}
	
	u8len = pu8Buf[1];
	
	for ( i = 1 ; i <= u8len; i++)
	{
		SMBIOS_MSG("0x%02x ", pu8Buf[i-1]);
		//if( 0 == (i % 16) )
		//{
		//	SMBIOS_MSG("\n");
		//}
	}
	SMBIOS_MSG("\n");
	return;
}

static void _smbios_dump_type(uint8_t u8type, sSMbios_type *psSmType)
{
	int i = 0;
	sSMbios_data *psData;
	if(NULL == psSmType)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [psSmType] IS NULL\n");
		return;
	}	

	if(0 == psSmType->u8Count )
	{
		return;
	}
		 
	SMBIOS_MSG("************SMBIOS TYPE %-03d************\n", u8type);
	psData = psSmType->data;
	while(psData)
	{
		smbios_dump_struct(psData->pu8Data);
		for( i = 0; i < psData->psStr->u8Count; i++)
		{
			SMBIOS_MSG("STRING[%02d] = %s\n", i,psData->psStr->pau8String[i]);		
		}
		SMBIOS_MSG("################################################################################\n");		
		psData = psData->next;	
	}
	return;
}

void smbios_dump_all(sSMBios *psSmbios)
{
	int i = 0;
	
	if(NULL == psSmbios)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [psSmbios] IS NULL\n");
		return;
	}
	
	for( i = 0 ; i < SMBIOS_MAX_TYPE; i++)
	{
		_smbios_dump_type(i,&psSmbios->type[i]);		
	}
	return;
}

void smbios_dump_type(sSMBios *psSmbios,uint8_t u8Type)
{
	
	if(NULL == psSmbios)
	{
		SMBIOS_TOOL_ERR("INPUT PARAMETER [psSmbios] IS NULL\n");
		return;
	}
	
	if(psSmbios->type[u8Type].u8Count == 0)
	{
		SMBIOS_MSG("NO TYPE[%d] DATA\n",u8Type);
		return;
	}
	
	_smbios_dump_type(u8Type,&psSmbios->type[u8Type]);		
	return;
}


