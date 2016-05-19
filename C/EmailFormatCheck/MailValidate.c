#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include "mailAddrCheck.tab.h"
#include "avct_valid.h"

//#if !USE_EXTERN_CHECK 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#endif  //end of USE_EXTERN_CHECK

//LEX & YACC function
extern int LexInputStr(char* str,int len);
extern int yyparse (void);

static int eliminate_bs(char *str,int len);
static int match_character(char *str , int strLen , char fcc, char bcc);

//USE LEX & YACC 
//1 : use LEX & YACC check local part
//0 : it doesn't check local part
#define USE_LEX_YACC 1

//Use extern check function to check ip format
//1 : use extern isValidAddress and IsValidDomain to check 
//0 : use self check function to check domain part
#define USE_EXTERN_CHECK  1

//Eliminate local part comment (.....)
//can disable because LEX & YACC wll check the comment
// 1 : Eliminate local part comment
// 0 : doesn't eliminate local part comment
#define ELIMINATE_LOCAL_PART_COMMENT 0

//Debug
//1 : turn on
//0 : turn off
#define IMM2_DEBUG 1
/* used to debuf */
#if IMM2_DEBUG
#include <syslog.h>
#else
#define syslog(level,format,...) do{ }while(0);
#endif

//ignore comments in front of @
// 1 : comment in front of @ is allowed
// 0 : comment in front of @ is disallowed
#if !ELIMINATE_LOCAL_PART_COMMENT
#define IGNORE_COMMENTS_AROUND_AT 1
#endif

//Define max of string length to local domain and total part
//define in avct_valid.h
#define MAX_OF_MAILADDRESS_STRING_LENGTH MAX_EMAIL_LEN
#define MAX_OF_MAILADDR_LOCAL_PART_STRING_LENGTH MAX_EMAIL_UNAME_LEN
#define MAX_OF_MAILADDR_DOMAIN_PART_STRING_LENGTH MAX_EMAIL_DOMAIN_LEN
#define MAX_OF_SUBDOMAIN_STRING_LENGTH MAX_EMAIL_SUBDOMAIN_LEN

#if USE_LEX_YACC
//size of lex buffer size
//YY_READ_BUF_SIZE is define in mailAddrCheck.l
#define LEX_YACC_BUF_SIZE YY_READ_BUF_SIZE
#endif


#if USE_LEX_YACC
extern int mailCheckFlag;
int LexYaccMailAddrCheck(char *str, int size){
    if(!str || size < 0){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return -1;
    }
#ifdef LEX_ALLOC_MEMORY //define in mailAddrCheck.l
    if(allocLexStringBuffer(LEX_YACC_BUF_SIZE)){
        syslog(LOG_USER|LOG_WARNING,"%s() Allocate Lex Buffer Failed\n",__FUNCTION__);
        return -1;
    }
#endif
    if(LexInputStr(str,size)){
        syslog(LOG_USER|LOG_WARNING,"%s() Lex Input String Failed\n",__FUNCTION__);
        return -1;
    }
    yyparse();
#ifdef LEX_ALLOC_MEMORY
    deallocLexStringBuffer();
#endif
    if(mailCheckFlag){
        //match
        return 0;
    }else{
        //dismatch
        return 1;
    }
}
#endif

//Description:
//separate_mail_address is used to split MailAddr into local part and domain part
//MailAddr is a e-mail address
//local is a local part in e-mail address
//lpLen is length of local part
//domain is domain part in e-mail address
//dpLen is length of domain part
//separate mail address into localpart and domain part
static bool separate_mail_address(char *MailAddr,char *local,int *lpLen,char *domain, int *dpLen){
    int len = 0;
    int lp_len = 0;
    int dp_len = 0;
    char *paddr = NULL;

    if((MailAddr == NULL) || (local == NULL) || (lpLen == NULL) || (domain == NULL) || (dpLen == NULL)){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return false;
    }
    len = strlen(MailAddr);
    // if Mail Address format length > max length of spec
    if(len > MAX_OF_MAILADDRESS_STRING_LENGTH){
        syslog( LOG_USER|LOG_WARNING ,"%s() mail address larger than MAX \n",__FUNCTION__);
        return false;
    }

    //find the last @ in the mail address 
    paddr = strrchr(MailAddr,'@');	//return last @ place
    if(!paddr){
        syslog( LOG_USER|LOG_WARNING ,"%s() is not mail address format \n",__FUNCTION__);
        return false;
    }

    //doesn't have local part
    if(!strcmp(paddr , MailAddr)){
        syslog( LOG_USER|LOG_WARNING ,"%s() doesn't have local part \n",__FUNCTION__);
        return false;
    }

    //doesn't have domain part
    if(strlen(paddr) <= 1){
        syslog( LOG_USER|LOG_WARNING ,"%s() doesn't have domain part \n",__FUNCTION__);
        return false;
    }

    //paddr is address  at @
    //local part copy
    lp_len = paddr - MailAddr;
    memcpy(local,MailAddr,lp_len);
    local[lp_len] = '\0';
    //domain part copy
    //reduce 1 because @
    dp_len = len - lp_len - 1;
    memcpy(domain,paddr+1, dp_len + 1);	//add 1 beacuse of "\0"

    *lpLen = lp_len;
    *dpLen = dp_len;
    return true;
}

//Description:
//hostname_validate is used to check the hostname format
//Return:
// < 0 : incorrect
// = 0 : correct
#if !USE_EXTERN_CHECK
static bool _IsValidHostname(char *host,int len){
    char *pHost = NULL;
    char buf[512] = {0};
    char *pStr1 = NULL;
    char *pStr2 = NULL;
    int i = 0;

    pHost = host;
    if(host == NULL){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return false;
    }

    //first character must be alpha character?
    if(!isalnum(pHost[0])){
        syslog( LOG_USER|LOG_WARNING ,"%s() first character is not number or alpha\n",__FUNCTION__ );
        return false;
    }

    //check ".." "-." ".-" situation
    for( i = 0 ; i < len-1 ; i++){
        if(pHost[i] == '.' || pHost[i] == '-'){
            if(pHost[i+1] == '.'){
                syslog( LOG_USER|LOG_WARNING ,"%s() include (..) (.-) (-.)\n",__FUNCTION__ );
                return false;
            }
        }
    }//end of for loop

    //last character can't be . or -
    if(!isalnum(pHost[len - 1])){
        syslog( LOG_USER|LOG_WARNING ,"%s() last character cna't be . or -\n",__FUNCTION__ );
        return false;
    }

    //check every subdomain smaller than MAX_OF_SUBDOMAIN_STRING_LENGTH
    memcpy(buf,host,len);
    pStr1 = buf;
    while(1){  
        pStr2 = strchr(pStr1,'.');
            if(pStr2 == NULL){
                if(strlen(pStr1) > MAX_OF_SUBDOMAIN_STRING_LENGTH){
                    syslog( LOG_USER|LOG_WARNING ,"%s() subdomain is lager than MAX_OF_SUBDOMAIN_STRING_LENGTH\n",__FUNCTION__ );
                    return false;
                }else{
                    return true;
                }
            }// end of pStr2 == NULL
        if((pStr2 - pStr1) > MAX_OF_SUBDOMAIN_STRING_LENGTH){
            syslog( LOG_USER|LOG_WARNING ,"%s() subdomain is lager than MAX_OF_SUBDOMAIN_STRING_LENGTH\n",__FUNCTION__ );
            return false;
        }//end of (pStr2 - pStr1) > MAX_OF_SUBDOMAIN_STRING_LENGTH
        pStr1 = pStr2 + 1;
    }//end of while(1)
}

//Used to check ipv6 format
static int ipv6_validate(char *ipv6,int len){
    struct in6_addr addr;
    if(inet_pton(AF_INET6,ipv6,&addr)<= 0){
        syslog( LOG_USER|LOG_WARNING ,"%s() IPv6 Format error\n",__FUNCTION__ );
        return -1;
    }
    
    if(IN6_IS_ADDR_LOOPBACK(&addr) 	   || 
        /*	   
        IN6_IS_ADDR_LINKLOCAL(&addr)    ||
        IN6_IS_ADDR_SITELOCAL(&addr)    ||
        IN6_IS_ADDR_V4MAPPED(&addr)     ||
        IN6_IS_ADDR_V4COMPAT(&addr)     ||
        IN6_IS_ADDR_MC_NODELOCAL(&addr) ||
        IN6_IS_ADDR_MC_LINKLOCAL(&addr) ||
        IN6_IS_ADDR_MC_SITELOCAL(&addr) ||
        IN6_IS_ADDR_MC_ORGLOCAL(&addr)  ||
        IN6_IS_ADDR_MC_GLOBAL(&addr)    ||
        */
        IN6_IS_ADDR_MULTICAST(&addr)){
        return -1;
    }
    
    return 0;
}
#endif //end of USE_EXTERN_CHECK


//It's used to delete the string between fcc and bcc , include fcc bcc.
//static int match_character(char *str , int strLen , char fcc, char bcc);
//it's used to delete the comments (...) 
static int separate_comment(char *str,int strLen){
    if((str == NULL)){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return -1;
    } 
    return match_character(str,strLen,'(',')');
}

//delete the string between fcc and bcc in str
//for example 
// fcc = (
// bcc = )
// str = abc(abd)daf(dae)
// result is abcdaf
static int match_character(char *str , int strLen , char fcc, char bcc){
    char *pStr = NULL;
    int paCnt = 0;
    int flag = 0;
    int backslash_flag = 0;
    char* pStart = NULL;
    int cnt = 0;
    if(str == NULL){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return -1; //error
    }

    pStr = str;

    while( (pStr) && (*pStr != '\0') ){
        cnt++;
        if(*pStr == '\\'){
            backslash_flag = 1;
        }else if(*pStr == fcc){
            if(!backslash_flag){
                if(!paCnt){
                    pStart = pStr;
                }
                paCnt++;
            }else{
                backslash_flag = 0;
            }
        }else if(*pStr == bcc){
            if(!backslash_flag){
                if(paCnt <= 0){
                    return -1;
                }else{
                    paCnt--;
                    if(!paCnt){
                        flag = 1;
                        memcpy(pStart,pStr+1,strlen(pStr+1) + 1);
                        pStr = pStart;
                        continue;
                    }
                }
            }else{
                backslash_flag = 0;
            }
        }else{  // *pStr != \\ && bcc & fcc
            backslash_flag = 0;
        } 
        pStr++;
    } //end of while

    if(paCnt){  //lost one ( or )
        syslog( LOG_USER|LOG_WARNING ,"%s() lost ( or )\n",__FUNCTION__ );
        return -1;
    }

    return flag;
}

//Description:
//ipaddrValidation is used to check the IPv4 or IPv6 syntax.
//format = 0 check for IPv4
//format = 1 check for IPv6
static int ipaddrValidation(char *str, int len,int format){
#if !USE_EXTERN_CHECK
    char* pStr1 = NULL;
    char* pStr2 = NULL;
    char val[32] = {0};
    int i = 0;
    int len_t = 0;
#endif
    if(!str){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return -1;
    }
    if(!format){
#if USE_EXTERN_CHECK
        if(!IsValidAddress(AF_INET,str,NULL)){
            syslog( LOG_USER|LOG_WARNING ,"%s() IPv4 validation fail\n",__FUNCTION__ );
            return -1;
        }
#else
        if(!isdigit(str[0])){
            return -1;
        }
        for( i = 1  ; i < len ; i++){
            if(!isdigit(str[i])){
                if(str[i] != '.'){
                    syslog( LOG_USER|LOG_WARNING ,"%s() first is not number\n",__FUNCTION__ );
                    return -1;
                }
            }
        }//end of for loop
        pStr2 = str;
        for( i = 0 ; i < 3 ; i++){
            pStr1 = strchr(pStr2,'.');
            if(pStr1 == NULL){
                syslog( LOG_USER|LOG_WARNING ,"%s() is not IPv4 format\n",__FUNCTION__ );
                return -1;
            }
            len_t = pStr1 - pStr2;
            memcpy(val,pStr2,len_t);
            if(atoi(val) > 255){
                return -1;
            } 
            memset(val,0,4);
            pStr2 = pStr1 + 1;
        }//end of for loop
        len_t = strlen(pStr2);
        memcpy(val,pStr2,len_t);
        if(atoi(val) > 255){
            syslog( LOG_USER|LOG_WARNING ,"%s() ipv4 > 255 l\n",__FUNCTION__ );
            return -1;
        }
        pStr1 = strchr(pStr2,'.');
        if(pStr1){
            syslog( LOG_USER|LOG_WARNING ,"%s() more than 4 .\n",__FUNCTION__ );
            return -1;
        }
#endif //end of USE_EXTERN_CHECK
    }else{
#if USE_EXTERN_CHECK
        //check for ipv6
        if(!IsValidAddress(AF_INET6,str,NULL)){
            syslog( LOG_USER|LOG_WARNING ,"%s() IPv6 validation fail\n",__FUNCTION__ );
            return -1;
        }
#else
        if(ipv6_validate(str,len)){
            syslog( LOG_USER|LOG_WARNING ,"%s() IPv6 validation fail\n",__FUNCTION__ );
            return -1;
        }
#endif //end of USE_EXTERN_CHECK
    }// end of if(!format)
return 0;
}


//format = 0 check for hostname  
//format = 1 check for IP
//Check Domain Part Char
static bool IsValidDPChar(char *domain,int len,int format){
    int i = 0;
    for( i = 0 ; i < len ; i++){
        if(!isalnum(domain[i])){
            if(format){
                if(domain[i] != '.' && domain[i] != ':' && domain[i] != '[' && domain[i] != ']'){
                    syslog( LOG_USER|LOG_WARNING ,"%s() have invalid character\n",__FUNCTION__ );
                    return false;
                }
            }else{ //format == 0 check for hostname 
                // host name just can have . , - , number & letter
                if(domain[i] != '.' &&  domain[i] != '-'){
                    syslog( LOG_USER|LOG_WARNING ,"%s() have invalid character\n",__FUNCTION__ );
                    return false;
                }
            }
        } //end of if(!isalnum(domain[i]))
    } // end for for loop i
    return true;
}//end of function


//Description: 
//get_ipstr is used to get the ipv4 or ipv6 address in the domain part
//if ip4Len = 0 means it doesn't have ipv4 address
//if ip6Len = 0 means it doesn't have ipv6 address
static int get_ipstr(char *str,int len, char *ip4Str, int *ip4Len,char *ip6Str,int *ip6Len){
    char buf[6]={0};
    char *pStr = NULL;
    int cpLen = 0;
    if(str == NULL || ip4Str == NULL || ip6Str == NULL){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return -1;
    }
    pStr = str;
    //copy (IPv6:) to buf 
    memcpy(buf,str+1,5); 
    cpLen = len;
    //Check IPv6: String 
    //if first five words is IPv6: mean is IPv6 format
    if(!strcmp(buf,"IPv6:")){
        pStr = pStr + 6;
        cpLen = cpLen - 6 - 1 ;
        memcpy(ip6Str,pStr,cpLen);
        *ip6Len = strlen(ip6Str);
        *ip4Len = 0;
    }else{	//IPv4
        pStr++;
        cpLen = len - 2;
        memcpy(ip4Str,pStr,cpLen);
        *ip4Len = strlen(ip4Str);
        *ip6Len = 0;
    }
    return 0;
}
//Domain part format check
static bool IsValidDomainPart(char *domain,int len){  
    char *pdp;
    int c_flag = 0;
    char ip4[128];
    int ip4Len = 0;
    char ip6[128];
    int ip6Len = 0;
    if(!domain){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return false;
    }

    if(len > MAX_OF_MAILADDR_DOMAIN_PART_STRING_LENGTH){
        syslog( LOG_USER|LOG_WARNING ,"%s() domain part length is too long\n",__FUNCTION__ );
        return false;
    }
    //comment 0 1 -1
    if((c_flag = separate_comment(domain,len)) < 0){
        return false;
    }

    len = strlen(domain);
    pdp = domain;

    memset(ip4,0,128);
    memset(ip6,0,128);
    if(*pdp == '['){  //IPv4 or IPv6
    //It can not have comment 
        if(c_flag){
            syslog( LOG_USER|LOG_WARNING ,"%s() Can not have comment\n",__FUNCTION__ );
            return false;
        }
        if(!IsValidDPChar(pdp,len,1)){
            syslog( LOG_USER|LOG_WARNING ,"%s() Have invalid character\n",__FUNCTION__ );
            return false;
        }else if(pdp[len-1] != ']'){		//last char is not ]
            syslog( LOG_USER|LOG_WARNING ,"%s() last character can not be a ]\n",__FUNCTION__ );
            return false;
        }

        if(get_ipstr(pdp,len,ip4,&ip4Len,ip6,&ip6Len)){
            syslog( LOG_USER|LOG_WARNING ,"%s() Get IP address fail\n",__FUNCTION__ );
            return false;
        }

        if(!(ip4Len || ip6Len)){
            syslog( LOG_USER|LOG_WARNING ,"%s() None IPv4 and IPv6\n",__FUNCTION__ );
            return false;
        }
        if(ip4Len != 0){
        //check ipv4 format here
            if(ipaddrValidation(ip4,ip4Len,0)){
                syslog( LOG_USER|LOG_WARNING ,"%s() IPv4 validation fail\n",__FUNCTION__ );
                return false;
            }
        }
        if(ip6Len != 0){
            //check ipv6 format here
            if(ipaddrValidation(ip6,ip6Len,1)){
                syslog( LOG_USER|LOG_WARNING ,"%s() IPv6 validation fail\n",__FUNCTION__ );
                return false;
            }
        }
    // it's not ip format (if(*pdp == '['))
    }else{
        if(!IsValidDPChar(pdp,len,0)){
            syslog( LOG_USER|LOG_WARNING ,"%s() have invalid character\n",__FUNCTION__ );
            return false;
        }
#if USE_EXTERN_CHECK
        if(!IsValidDomain(pdp)){
            return false;
        }
#else
        if(!_IsValidHostname(pdp,len)){
            return false;
        }
#endif
    }  //end of if(*pdp == '[')
return true;
}


//local part format check
static bool IsValidLocalPart(char *localStr,int len){
    if(!localStr){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return false;
    }
    
    //if local part final char is space char
    if(len > MAX_OF_MAILADDR_LOCAL_PART_STRING_LENGTH){
        syslog( LOG_USER|LOG_WARNING ,"%s() local part length is too long \"@\n",__FUNCTION__);
        return false;
    }

#if ELIMINATE_LOCAL_PART_COMMENT
    if(separate_comment(localStr,len) < 0 ){
        syslog(LOG_USER|LOG_WARNING,"%s() comment error\n",__FUNCTION__);
        return false;
    }else{
        len = strlen(localStr);
    }
#endif

    if(eliminate_bs(localStr,len)){
         return false;
    }

    // space in front of @ is disallow
    // example : "abab "@ & abab @
    if(localStr[len-1] == '\"'){
        if(localStr[len-2] == '\t' || localStr[len - 2] == ' '){
            syslog( LOG_USER|LOG_WARNING ,"%s() is have space before \"@\n",__FUNCTION__ );
            return false;
        }
    }else if(localStr[len-1] == '\t' || localStr[len-1] ==  ' '){
        syslog( LOG_USER|LOG_WARNING ,"%s() is have space before @\n",__FUNCTION__ );
        return false;
    }

#if !IGNORE_COMMENTS_AROUND_AT
    else if(localStr[len-1] == ')'){
        syslog( LOG_USER|LOG_WARNING ,"%s() is have comment before @\n",__FUNCTION__ );
        return false;
    }
#endif 

//Check local Part By Lex & Yacc
#if USE_LEX_YACC
    if(!LexYaccMailAddrCheck(localStr,len)){
        return true;
    }else{
        return false;
    }
#else
    return true;
#endif
}

//eliminate blackslash
//if blachslash(\) in "...", and not follow the other \ and " then eliminate it
//example: str = "abc\da"."aba\\"."adf\"af"
//         result is "abcda"."aba\\"."adf\"af"
static int eliminate_bs(char *str,int len){
    int bs_flag = 0;
    int idx = 0;
    int i;
    int quote_flag = 0;
    char* buf;
    char* pStr = NULL;
    if(str == NULL || len == 0){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return -1;
    }
    pStr = str;
    buf = malloc(len);
    memset(buf,0,len);
    idx = 0;
    for( i = 0 ; i < len ; i++){
        if(*pStr == '\\'){
            if(bs_flag == 1){
                bs_flag = 0;
                buf[idx] = *pStr;
                idx++;
            }else{
                if(quote_flag == 1){
                    bs_flag = 1;
                    pStr++;
                    continue;
                }
            }
        }else if(*pStr == '\"'){
            if(bs_flag == 1){
                bs_flag = 0;
                buf[idx] = '\\';
                idx++;
            }
            else{
                if(quote_flag == 0){
                    quote_flag = 1;
                }else{
                    quote_flag = 0;
                }
            }
            bs_flag = 0;
        }else if(*pStr == '(' || *pStr == ')'){
            if(bs_flag == 1){
                bs_flag = 0;
                buf[idx] = '\\';
                idx++;
            }
        }else{
            bs_flag = 0;
        }

        buf[idx] = *pStr;
        idx++;
        pStr++;
    } //end of while loop
    memcpy(str,buf,idx-1);
    free(buf);
    return 0;
}
//mail address format check
bool IsValidEmailFormat(char *szName){
    int lp_len = 0;
    int dp_len = 0;
    char lp[512];
    char dp[512];

    if(!szName){
        syslog( LOG_USER|LOG_WARNING ,"%s() input string is NULL\n",__FUNCTION__ );
        return false;
    }
    //separate local part and domain part,and check number of characters
    if(!separate_mail_address(szName,lp,&lp_len,dp,&dp_len)){
        syslog(LOG_USER|LOG_WARNING,"%s() is not mail address\n",__FUNCTION__);
        return false;
    }

    if(!IsValidLocalPart(lp,lp_len)){
        return false;
    }

    //Domain part's eliminate comment is in dp_validate function
    // check domain part syntax
    if(!IsValidDomainPart(dp,dp_len)){
        return false;
    }
    return true;
}

