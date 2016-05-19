#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
//#include "MailValidate.h"
#include "avct_valid.h"
//extern bool IsValidMail(char *addr); 

#define LOG_FILE ".log"

FILE* openLogFile(char *filename){
        	
    FILE *fp = NULL;
    char fn[512]={0};
    	
    memcpy(fn,filename,strlen(filename));
    strncat(fn,".log",5);	
    fp = fopen(fn,"wb+");
    if(!fp){
        printf("Can't create log file %s\n",filename);
        return NULL;
    }	
    //fwrite("",,fp);
    return fp;	
}

void closeLogFile(FILE *fp){
    if(fp ==NULL){
        printf("%s() fp is null\n",__FUNCTION__);
        return;
    }
    fclose(fp);
}
/*void wLog(FILE *fp,char *str){
    if(fp == NULL){
        printf("fp is null\n");
        return;
    }
    fwrite(str,strlen(str)
}*/

void help(char* filename){
    printf("Test from a file...\n");
    printf("\t%s file valid(0|1) \"filename\"\n",filename);
    printf("Test from input string...\n");
    printf("\t%s string \"string\"\n",filename);
}
int main(int argc,char *argv[]){	
    char inStr[512] = {0};
    int len = 0;
    bool ret = false;
    int valid = 0;
    int idx = 0;
    int wrCnt = 0; //wrong count
    FILE *fp = NULL;
    FILE *logFd = NULL;
    if(argc < 2 ){
        //printf("Syntax error: EX===> \n\t%s \"input string\"\n",argv[0]);
        help(argv[0]);
        exit(-1);
    }
    
    if(!strcmp("file",argv[1])){
        if(argc < 4){
            help(argv[0]);
            exit(-1);
        }
        fp = fopen(argv[3],"rb");
        if(fp == NULL){
            printf("Can't open %s....\n",argv[3]);
            exit(-1);
        }//end of if(fp == NULL)
        if(!isdigit(argv[2][0])){
            help(argv[0]);
            exit(-1);
        }
        valid = atoi(argv[2]);
        logFd = openLogFile(argv[3]);
        if(valid == 0){  //invalid
            fprintf(logFd,"/***** It's A Invalid Mail Address Format *****/\n");
            fprintf(logFd,"Below Is Valid Mail Address But In Invalid File\n");		
        }else if(valid == 1){
            fprintf(logFd,"/****** It's A Valid Mail Address Format ******/\n");
            fprintf(logFd,"Below Is Invalid Mail Address But In Valid File\n");
        }else{
            closeLogFile(logFd);
            help(argv[0]);
            exit(-1);
        }
        fprintf(logFd,"\nidx\tMail\n");
        fprintf(logFd,"------------------------------------------------\n");
        //logFd = openLogFile(char *filename);
        while(fgets(inStr,sizeof(inStr),fp)){
            idx++;		
            len = strlen(inStr);
            inStr[len - 1] = '\0';
            if(inStr[len-2] == 0xd){
                inStr[len-2] = '\0';
            }
            //inStr[len - 2] = '\0';
            len -= 2;
            //ret = mailAddr_validate(inStr);
            ret = IsValidEmailFormat(inStr);	
            //if file content is all valid format valid = 1 else valid = 0		
            //printf("%d\n",idx);
            if(valid){
                if(!ret){
                    fprintf(logFd,"%3d\t%s\n",idx,inStr);
                    wrCnt++;
                }
            }else{
                if(ret){
                    fprintf(logFd,"%3d\t%s\n",idx,inStr);
                    wrCnt++;
                }
            }
            memset(inStr,0,sizeof(inStr));
        }//end of while(fgets(..))	
        fprintf(logFd,"\ntotal:%3d Wrong:%3d\n",idx,wrCnt);
        fprintf(logFd,"/**************** End Of File ****************/\n");
        closeLogFile(logFd);
        return 0;
    }else if(!strcmp("string",argv[1])){
        if(argc < 3){
            help(argv[0]);
            exit(-1);
        }
        len = strlen(argv[2]);
        memcpy(inStr,argv[2],len);
        //ret = mailAddr_validate(inStr);
        ret = IsValidEmailFormat(inStr);
        if(ret){
            printf("OK...\n");
            return 0;
        }else{
            printf("FAIL...\n");
            return 1;
        }
    }else{
        help(argv[0]);
        exit(-1);
    }
}
