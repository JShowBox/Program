#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "inc/smbiosLib.h"
#define HELP "%s -f [filename] -t [type]\r\n\
    -f filename of SMBIOS table\r\n\
    -t display a specify type\r\n"

int main(int argc,char *argv[])
{
	int ch = 0;
	bool bDumpAll = true;
	uint8_t u8Type = 0;
	sSMBios smbios;
	eSMBIOS_RET_CODE eRetCode = eSMBIOS_RET_OK;
	char filename[128] = "C:\\Users\\jkao\\Desktop\\SMBIOS-Table_Parser\\smbios-table";
#if 1
	while((ch = getopt(argc,argv,"f:t:h")) != -1)
	{
		switch(ch)
		{
			case 'f':
				memcpy(filename,optarg,128);
				break;
			case 't':
				bDumpAll = false;
				/*check if format is correct*/
				u8Type = atoi(optarg);
				break;
			case 'h':
				fprintf(stdout,HELP,basename(argv[0]));
				return;
			default :
				fprintf(stdout,"Unsupport -%c parameter\n",ch);
				return;
		}	
	}	
#endif
	memset(&smbios,0,sizeof(sSMBios));
	
	eRetCode = smbios_parser_from_file(&smbios, filename);
	if(eSMBIOS_RET_OK != eRetCode)
	{
		/*free smbios, not implement yet*/
		smbios_release(&smbios);
		return -1;
	}
	
	if(true == bDumpAll)
	{
		smbios_dump_all(&smbios);
	}
	else
	{
		smbios_dump_type(&smbios,u8Type);
	}
	smbios_release(&smbios);
	printf("OK\n");
	system("pause");
	return 0;
}
