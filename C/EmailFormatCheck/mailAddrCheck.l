/*         define section         */
%{
    #include <stdio.h>
    int _LexInputFromMemory(char *buffer, size_t maxSize);
    #undef YY_INPUT
    #define YY_INPUT(b,r,ms) ( r = _LexInputFromMemory(b,ms))
    /*avoid compiler error*/
    #define YY_NO_INPUT 1
    #undef yyunput
    #undef ECHO
    #define ECHO fprintf(yyout,"%s",yytext);
    #include "mailAddrCheck.tab.h"
#if LEX_ALLOC_MEMORY
    #define YY_READ_BUF_SIZE 8192
#endif
%}

IPv6 IPv6
Letter [a-zA-Z]+
Number [0-9]+
Dot	\.
Quote \"
hyphen [\-]+
At @
Colon \:
LSBrackets \[
RSBrackets \]
LBrackets \(
RBrackets \)
Backslash \\
SLBrackets [\\][\(]
SRBrackets [\\][\)]
SQuote ([\\][\"])+
SBackslash ([\\][\\])+
Space [\\]?[\ \t]+
NormalSymbol [\!\#\$\%\&\`\*\+\/\=\?\^\_\'\{\|\}\~]+
SpecialSymbol [\,\;\<\>]+
MultiDot [\.]{2,}
ChangeLine \n

%%
  /*      Rules section      */
{IPv6}		{return IPV6;}
{Letter}	{return LETTER;}
{Number} 	{return NUMBER;}
{Dot} 		{return DOT;}
{Quote} 	{return QUOTE;}
{hyphen} 	{return HYPHEN;}
{At}		{return AT;}
{Colon}		{return COLON;}
{LSBrackets}	{return LSB;} 
{RSBrackets}	{return RSB;}
{LBrackets}	{return LB;}
{RBrackets}	{return RB;}
{Backslash}	{return BS;}
{SLBrackets}	{return SLB;}
{SRBrackets}	{return SRB;}
{SQuote}	{return SQUOTE;}
{SBackslash}	{return SBS;} 
{Space}		{return SPACE;}
{NormalSymbol}	{return NSYMBOL;} 
{SpecialSymbol}	{return SSYMBOL;}
{MultiDot}	{return MDOT;}
{ChangeLine} 	{return 0;}

%%

int BufSize = 0;
int LexInStrSize = 0;
char* pLexInStr = NULL ;

#if LEX_ALLOC_MEMORY
    char* LexInStr = NULL ;
#else
    #ifdef YY_READ_BUF_SIZE
        char LexInStr[YY_READ_BUF_SIZE];
    #else
        char LexInStr[512];
    #endif
#endif


int min(int a, int b){
    if(a >= b)
        return b;
    return a;
}

int _LexInputFromMemory(char *buffer, size_t maxSize){
    int n = min(maxSize,LexInStrSize);
    //printf("%d\n",maxSize);
    if( n > 0){
        memcpy(buffer,pLexInStr,n);
        pLexInStr += n;
    }
    //printf("%s\n",buffer);
    LexInStrSize -= n;
    return n;
}
#if LEX_ALLOC_MEMORY
int allocLexStringBuffer(int size){
    LexInStr = malloc(sizeof(char) * size);
    if(LexInStr == NULL){
        return -1;
    }
    BufSize = size;
    return 0;
}

int deallocLexStringBuffer(void){	
    if(LexInStr == NULL){
        return -1;
    }
    free(LexInStr);
    return 0;
}
#endif

int LexInputStr(char* str,int len){
    if(str == NULL || len < 0){
        return -1;
    }
#if LEX_ALLOC_MEMORY
    if(!LexInStr){
        printf("plz alloc memory for LexInStr by allocLexStringBuffer()\n")
        return -1;
    }
#endif
    memset(LexInStr,0,BufSize);
    memcpy(LexInStr,str,len);
    LexInStrSize = len;
    pLexInStr = LexInStr;
    return 0;
}

int yywrap(void){
    return 1;
    /*avoid compiler error*/
    yyunput(1,NULL);
}


