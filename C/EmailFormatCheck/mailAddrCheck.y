%{
#include <stdio.h>
//LEX_ALLOC_MEMORY = 1 Dynamic Allocated Memory 
//LEX_ALLOC_MEMORY = 0 Use Array
#define LEX_ALLOC_MEMORY 1
extern int yylex (void);
int yyerror(char *err);
//used to present mail address format 
int mailCheckFlag;
%}

%token IPV6 LETTER NUMBER DOT QUOTE HYPHEN AT COLON LSB RSB LB RB BS SLB SRB SQUOTE SBS SPACE NSYMBOL SSYMBOL MDOT

%%

mailAddr: 
    LocalPart  //check for Local Part
    {
        mailCheckFlag = 1;
    }
    //Check all mail address include @ and domain part
    | LocalPart at DomainPart {mailCheckFlag = 0;}
    //""."".""."" Null Quote string
    | NullQuoteString {mailCheckFlag = 0;}
//  | normalString commentSyntax normalString {mailCheckFlag = 0;}
    | error '\n' {yyerrok;}
;



/***domain***/
DomainPart:dpSyntax
    | mailIPv4
    | mailIPv6
    ;
dpSyntax:
            dpSyntax HYPHEN dpNormalString
            | dpSyntax dot dpNormalString
            | dpSyntax dot dpcommentAddNormal
            | dpSyntax dpcommentAddNormal
            | dpNormalString
            | dpcommentAddNormal
;

dpNormalString: LETTER
            | NUMBER
            | dpNormalString NUMBER 
            | dpNormalString LETTER 
;
dpcommentAddNormal: commentSyntax
            | dpcommentAddNormal dpNormalString
;

mailIPv4: LSB dpIPV4 RSB;
dpIPV4:  NUMBER dot NUMBER dot NUMBER dot NUMBER;
mailIPv6: LSB dpIPV6 RSB;
dpIPV6: IPV6;
    | dpIPV6 COLON
    | dpIPV6 LETTER
    | dpIPV6 NUMBER
    | dpIPV6 dpIPV4
;

/***local***/
LocalPart:lpSyntax;
lpSyntax: lpSyntax dot quoteString
        | lpSyntax dot normalString
        | lpSyntax dot commentAddNormal
        | lpSyntax dot commentAddQuoteString 
//      | lpSyntax commentAddNormal
        | commentAddNormal
        | commentAddQuoteString
        | normalString
        | quoteString
;	
commentAddNormal: commentSyntax  normalString
        | normalString commentSyntax
        | commentSyntax normalString commentSyntax
;

commentAddQuoteString: commentSyntax  quoteString
        | quoteString commentSyntax
;

NullQuoteString: QUOTE QUOTE
        | NullQuoteString dot QUOTE QUOTE
;
quoteString: QUOTE quotelContent QUOTE
        | QUOTE QUOTE
;

quotelContent: 	quotelContent normalString
        | quotelContent specialString
        | quotelContent SBS
        | quotelContent SQUOTE
        | quotelContent SPACE
        | quotelContent dot
        | quotelContent MDOT
        | quotelContent SRB
        | quotelContent SLB
        | quotelContent at
        | quotelContent LSB
        | quotelContent RSB
        | quotelContent COLON
        | normalString
        | specialString
        | SBS
        | SQUOTE
        | SPACE
        | dot
        | MDOT
        | SRB
        | SLB
        | at
        | LSB
        | RSB
        | COLON
;

commentSyntax: comment
;

comment2: comment
        | comment2 comment
;
comment: LB commentContent RB
        | LB RB
        | LB comment2 RB
;

commentContent:	commentContent normalString
        | commentContent specialString
        | commentContent BS
        | commentContent SQUOTE
        | commentContent SPACE
        | commentContent dot
        | commentContent MDOT
        | commentContent SLB
        | commentContent SRB
        | commentContent QUOTE
        | commentContent SBS
        | commentContent COLON
        | commentContent LSB
        | commentContent RSB
        | commentContent at
        | commentContent comment
        | normalString
        | specialString
        | BS	
        | SBS
        | SQUOTE
        | SPACE
        | dot
        | MDOT
        | SLB
        | SRB
        | QUOTE
        | at
        | COLON
        | LSB
        | RSB

normalString: LETTER
        | NUMBER
        | NSYMBOL 
        | HYPHEN
        | normalString LETTER
        | normalString NUMBER
        | normalString NSYMBOL
        | normalString HYPHEN
;
specialString: SSYMBOL;
dot: DOT;
at: AT;

%%
int yyerror(char *err){
    mailCheckFlag = 0;
    return 0;
}

