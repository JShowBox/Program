#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
//#include <syslog.h>
#include <unistd.h>
#include <time.h>

#include "clock_zic.h"

const char* G_strMonth[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char* G_strDayOfWeek[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; 

#if 0
#define CLOCK_ZIC_ERR(fmt,arg...) syslog(LOG_INFO,"%s(%d),pid=%d,"fmt,__FUNCTION__,__LINE__,getpid(),##arg)
#define CLOCK_ZIC_DBG(fmt,arg...) syslog(LOG_INFO,"%s(%d),pid=%d,"fmt,__FUNCTION__,__LINE__,getpid(),##arg)
#else
#define CLOCK_ZIC_ERR(fmt,arg...) {}
#define CLOCK_ZIC_DBG(fmt,arg...) {}
#endif

const char *rtStrMonth(int m)
{
	if( (m < 1) || (m > 12) )
		return NULL;
	else
		return G_strMonth[m];
}

const char *rtStrDayOfWeek(int d)
{
	if( (d < 0) || (d > 6) )
		return NULL;
	else
		return G_strDayOfWeek[d];
}

int digitStrValidator(char *strDigital)
{
	int len = 0;
	int i = 0;
	if(!strDigital)
	{
		return eCLOCK_ZIC_RET_BUF_NULL;
	}
	
	len = strlen(strDigital);
	for( i = 0 ; i < len ; i++)
	{
		/*Check if digital?*/
		if(!isdigit(strDigital[i]))
		{
			return eCLOCK_ZIC_RET_ERR_NOT_DIGITAL;
		}
	}
	return eCLOCK_ZIC_RET_OK;
}

int digitalStr2Int(char *strDigital,int min,int max,int *rtDigit)
{
	int digit;
	int ret = eCLOCK_ZIC_RET_OK;
	ret = digitStrValidator(strDigital);
	if( ret != eCLOCK_ZIC_RET_OK)
	{
		return ret;
	}
	
	digit = atoi(strDigital);
	if((digit < min) || (digit > max))
	{
		return eCLOCK_ZIC_RET_ERR_OUT_OF_RANGE;
	}
	*rtDigit = digit;
	return ret;
}

int monthStr2Int(char *strMonth,int *rtMonth)
{
	return digitalStr2Int(strMonth,1,12,rtMonth);
}

int weekStr2Int(char *strWeek,int *rtWeek)
{
	return digitalStr2Int(strWeek,1,5,rtWeek);
}

int dayOfWeekStr2Int(char *strDayOfWeek,int *rtDayOfWeek)
{
	return digitalStr2Int(strDayOfWeek,0,6,rtDayOfWeek);
}

int hourStr2Int(char *strHour,int *rtHour)
{
	return digitalStr2Int(strHour,0,23,rtHour);
}


/*
trim the string to symbol and return the rest string
Example:
 trimStrToSymbo("M10.10.10",strDst,'.') => strDst = M10 and will return 10.10
 trimStrToSymbo("M10.10.10",strDst,'/')	=> strDst = M10.10.10 and will return NULL
*/
char* trimStrToSymbol(char *strSrc,char *strDst,int strLen,char symbol)
{
	int len;
	char *pTmp = NULL;
	if(!strSrc || !strDst)
	{
		return NULL;
	}
	
	if(symbol == 0)
	{
		snprintf(strDst,strLen,"%s",strSrc);
		return strSrc;	
	}
	
	pTmp = strchr(strSrc,symbol);
	if(!pTmp)
	{
		snprintf(strDst,strLen,"%s",strSrc);
		return NULL;	
	}
			
	len = pTmp-strSrc;	
	if(len >= strLen)
 	{
 		return NULL;
	}
	memcpy(strDst,strSrc,len);
	return pTmp+1;	
}

static int dstWeekDayHourFmtParer(char *strDST,sDstInfo *dstInfo)
{
	int ret = eCLOCK_ZIC_RET_OK;
	char strMonth[MAX_LEN_TIME_BUFFER],strWeek[MAX_LEN_TIME_BUFFER],strDayOfWeek[MAX_LEN_TIME_BUFFER],strHour[MAX_LEN_TIME_BUFFER];
	char *pSymbol = NULL;
	char *pNext = NULL;
	int month,week,dayOfWeek,hour;
	int len = 0;
	if(!strDST || !dstInfo)
	{
		CLOCK_ZIC_ERR("Input buffer is null\n");	
		return eCLOCK_ZIC_RET_BUF_NULL;	
	}

    if (strDST[0] != 'M') 
    {
        return eCLOCK_ZIC_RET_UNSUPPORT_DST_FMT;
    }
	
	memset(strMonth,0,sizeof(strMonth));
	memset(strWeek,0,sizeof(strWeek));
	memset(strDayOfWeek,0,sizeof(strDayOfWeek));
	memset(strHour,0,sizeof(strHour));
	
	/*Month*/
	pNext = trimStrToSymbol(strDST+1,strMonth,sizeof(strMonth),'.');
	if(pNext == NULL)
	{
 		return eCLOCK_ZIC_RET_ERR_MONTH_FMT;
	}
	ret = monthStr2Int(strMonth,&month);
	if(ret != eCLOCK_ZIC_RET_OK)
	{
		CLOCK_ZIC_ERR("%s, Wrong month fmt=%s,ret=%d\n",strDST,strMonth,ret);	
		return eCLOCK_ZIC_RET_ERR_MONTH_FMT;
	}
	/*Week*/
	pNext = trimStrToSymbol(pNext,strWeek,sizeof(strWeek),'.');
	if(pNext == NULL)
	{
 		return eCLOCK_ZIC_RET_ERR_WEEK_FMT;
	}
 	ret = weekStr2Int(strWeek,&week);
 	if(ret != eCLOCK_ZIC_RET_OK)
	{
		CLOCK_ZIC_ERR("%s, Wrong week fmt=%s,ret=%d\n",strDST,strWeek,ret);	
		return eCLOCK_ZIC_RET_ERR_MONTH_FMT;
	}	
 	/*DayOfWeek*/
	pNext = trimStrToSymbol(pNext,strDayOfWeek,sizeof(strDayOfWeek),'/');
	if(pNext == NULL)
	{
 		return eCLOCK_ZIC_RET_ERR_DAY_OF_WEEK_FMT;
	}
  	if(ret != eCLOCK_ZIC_RET_OK)
	{
		CLOCK_ZIC_ERR("%s, Wrong dayOfWeek fmt=%s,ret=%d\n",strDST,strDayOfWeek,ret);	
		return eCLOCK_ZIC_RET_ERR_DAY_OF_WEEK_FMT;
	}	
 	
 	/*Hour*/
	pNext = trimStrToSymbol(pNext,strHour,sizeof(strHour),0);
	if(pNext == NULL)
	{
		return eCLOCK_ZIC_RET_ERR_HOUR_FMT;
	}
 	ret = hourStr2Int(strHour,&hour);
  	if(ret != eCLOCK_ZIC_RET_OK)
	{
		CLOCK_ZIC_ERR("%s, Wrong hour fmt=%s,ret=%d\n",strDST,strHour,ret);	
		return eCLOCK_ZIC_RET_ERR_HOUR_FMT;
	}	

	dstInfo->month = month;
	dstInfo->week = week;
	dstInfo->dayOfWeek = dayOfWeek;
	dstInfo->hour = hour;
	dstInfo->min = 0;	/*not used*/
	dstInfo->sec = 0;		/*not used*/
	return eCLOCK_ZIC_RET_OK;
}
/*M10.1.0*/
int dstFmtParser(char *strDST, sDstInfo *dstInfo )
{
    if(!strDST || !dstInfo)
    {
        CLOCK_ZIC_ERR("Input buffer is null\n");
        return eCLOCK_ZIC_RET_BUF_NULL;
    }

    if (strDST[0] == 'M') 
    {
        return dstWeekDayHourFmtParer(strDST,dstInfo);
    }
    else
    {
    	CLOCK_ZIC_ERR("Not supported DST format\n");	
    	return eCLOCK_ZIC_RET_UNSUPPORT_DST_FMT;
	}
}

int gmtTimeFmtValidator(int sign,int hr, int min)
{
    if(sign == eGMT_SIGN_PLUS)
    {
        if( (hr < 0) || (hr > 14))
        {
            CLOCK_ZIC_ERR("(+)Hour should be 0 - 14 (%d)!!\n",hr);
            return eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT;
        } 
    }
    else
    {
        if( (hr < 0) || (hr > 12))
        {
            CLOCK_ZIC_ERR("(-)Hour: should be 0 - 12(%d)!!\n",hr);
            return eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT;
        } 
    }

    if( (min < 0) || (min >= 60))
    {
        CLOCK_ZIC_ERR("Minute: should be 0 - 59(%d)!!\n",min);
        return eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT;
    }
    return eCLOCK_ZIC_RET_OK;
}

/*+0200,+2:00,+2,2, -0500, -5:00  */
int gmtTimeValidAndPaser(int sign,char *time,int *rtHr, int *rtMin)
{
    int i;
    int strLen;
    int hour = 0;
    int min = 0;
    int colonNum = 0;
    char *pColon;
    char aMin[5] = {0};
    char aHour[5] = {0};
    
    if(!time)
    {
        CLOCK_ZIC_ERR("time is null!!\n");
        return eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT;
    }
    strLen = strlen(time);
    if((strLen > 5) || (strLen <= 0))
    {
        CLOCK_ZIC_ERR("time: wrong format (%s)!!\n",time);
        return eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT;    
    }

    for( i = 0 ; i < strLen; i++)
    {
        if(!isdigit(time[i]))
        {
            if(time[i] != ':')
            {
                CLOCK_ZIC_ERR("time: wrong format (%s)!!\n",time);
                return eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT;
            }
            
            if(colonNum > 1)
            {
                CLOCK_ZIC_ERR("time: wrong format (%s)!!\n",time);
                return eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT;
            }
            colonNum++;
        }
    }

    pColon = strchr(time,':');
    if(pColon)
    {
        snprintf(aMin,sizeof(aMin),"%s",pColon+1);
        memcpy(aHour,time,pColon-time);
    }
    else
    {
        if(strLen == 5)
        {
            CLOCK_ZIC_ERR("time: wrong format (%s)!!\n",time);
            return eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT;    
        }
        
        if(strLen > 2)
        {
            if(strLen == 3)
            {
                snprintf(aMin,sizeof(aMin),"%s",time+1);
                memcpy(aHour,time,1);
            }
            else /* == 4*/
            {
                snprintf(aMin,sizeof(aMin),"%s",time+2);
                memcpy(aHour,time,2);
            }
        }
        else
        {
            snprintf(aHour,sizeof(aHour),"%s",time);
            snprintf(aMin,sizeof(aMin),"0");
            
        }
    }

    hour = atoi(aHour);
    min = atoi(aMin);

    if(!gmtTimeFmtValidator(sign,hour, min))
    {
        CLOCK_ZIC_ERR("time: wrong format (%s)!!\n",time);
        return eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT;
    }
    
    if(rtHr && rtMin)
    {
        *rtHr=hour;
        *rtMin=min;
    }
    
    return eCLOCK_ZIC_RET_OK; 
}

int strGmtTimeFmtValidator(int sign,char *time)
{  
    return gmtTimeValidAndPaser(sign,time,NULL,NULL);
}

int gmtValidator(char *gmt)
{
    int sign;
    if(!gmt)
    {
        CLOCK_ZIC_ERR("gmt is null!!\n");
        return eCLOCK_ZIC_RET_ERR_GMT_FMT;
    }

    if(strlen(gmt) > 6)
    {
        CLOCK_ZIC_ERR("gmt: wrong format (%s)!!\n",gmt);
        return eCLOCK_ZIC_RET_ERR_GMT_FMT;
    }

    /*if first byte is not number, it should be + or -*/
    if(isdigit(gmt[0]))
    {
        return strGmtTimeFmtValidator(eGMT_SIGN_PLUS,gmt);
    }
    else if((gmt[0] == '+') || (gmt[0] == '-') )
    {
        sign = gmt[0] == '+'?eGMT_SIGN_PLUS:eGMT_SIGN_MINUS;
        return strGmtTimeFmtValidator(sign,gmt+1);
    }
    else
    {
        CLOCK_ZIC_ERR("first byte should be +,- or number!!\n");
        return eCLOCK_ZIC_RET_ERR_GMT_FMT;
    } 
}

int gmtValidatorAndPaser(char *gmt,int *hr,int *min,int *sign)
{
    if(!gmt)
    {
        CLOCK_ZIC_ERR("gmt is null!!\n");
        return eCLOCK_ZIC_RET_ERR_GMT_FMT;
    }

    if(strlen(gmt) > 6)
    {
        CLOCK_ZIC_ERR("gmt: wrong format (%s)!!\n",gmt);
        return eCLOCK_ZIC_RET_ERR_GMT_FMT;
    }

    /*if first byte is not number, it should be + or -*/
    if(isdigit(gmt[0]))
    {
        *sign=eGMT_SIGN_PLUS;
        return gmtTimeValidAndPaser(eGMT_SIGN_PLUS,gmt,hr,min);
    }
    else if((gmt[0] == '+') || (gmt[0] == '-') )
    {
        *sign = (gmt[0]=='+')?eGMT_SIGN_PLUS:eGMT_SIGN_MINUS;
        return gmtTimeValidAndPaser(*sign,gmt+1,hr,min);
    }
    else
    {
        CLOCK_ZIC_ERR("first byte should be +,- or number!!\n");
        return eCLOCK_ZIC_RET_ERR_GMT_FMT;
    } 
}

int issueZicCommand(char *zicFilename)
{
    int retry = 3;
    char cmdline[CLOCK_MAX_STR_LEN];

	if(!zicFilename)
	{
		CLOCK_ZIC_ERR("Input buffer is null\n");
		return eCLOCK_ZIC_RET_BUF_NULL;		
	}
    //generate the zoneinfo file
    snprintf(cmdline,sizeof(cmdline), ZIC_BINARY " -d %s %s",CLOCK_PATH,zicFilename);
    system(cmdline); 
    CLOCK_ZIC_DBG("cmd:%s\n",cmdline);

    /*check file exist or not*/
    while(access(ZIC_ZONE_FILE,F_OK) < 0)
    {
        if(retry < 0)
        {
            CLOCK_ZIC_ERR("File non-exist:%s\n",ZIC_ZONE_FILE);
            return -1;
        }
        retry--;
        usleep(500000);
    }

    //create link or copy zoneinfo file to replace the current localtime
    snprintf(cmdline,sizeof(cmdline), "/bin/mv %s /etc/localtime", ZIC_ZONE_FILE);
    system(cmdline);
    CLOCK_ZIC_DBG("cmd:%s\n",cmdline);
    return eCLOCK_ZIC_RET_OK;
}

int setTimezoneDaylightSaving(char *gmt,bool bDst,char *dstBegin,char *dstEnd, int fmt)
{
	int sign;
	int ret = eCLOCK_ZIC_RET_OK;
	sTZDst tzDst; 
    //bool IsDstOn;
    bool bDstSame = false;

    char strRuleDstBegin[CLOCK_MAX_STR_LEN]={0};
    char strRuleDstEnd[CLOCK_MAX_STR_LEN]={0};
    char strTimeZone[CLOCK_MAX_STR_LEN]={0};
    char zoneName[CLOCK_MAX_STR_LEN]={0};


	if(!gmt || !dstBegin || !dstEnd)
	{
		CLOCK_ZIC_ERR("Input buffer is null\n");
		return eCLOCK_ZIC_RET_BUF_NULL;
	}
	memset(&tzDst,0,sizeof(tzDst));	
	
	/*timezone gmt offset*/
	ret = gmtValidatorAndPaser(gmt,&tzDst.tz.hour,&tzDst.tz.min,&sign);
	if(ret != eCLOCK_ZIC_RET_OK)
	{
		return eCLOCK_ZIC_RET_ERR_GMT_FMT;
	}
	tzDst.tz.bPositive = (sign == eGMT_SIGN_PLUS)? true:false;
	snprintf(zoneName,sizeof(zoneName),"%c%02d%02d",(tzDst.tz.bPositive == true ? "+" : "-",tzDst.tz.hour, tzDst.tz.min));
	
	/*If DST Start equal to DST End*/
    if(!strcmp(dstBegin,dstEnd))
    {
		bDstSame = true;;
    }
    
	/*DST*/
	if(fmt == eDST_FMT_MONTH_WEEK_DAY)
	{
		if( dstFmtParser(dstBegin, &tzDst.dst.dstBegin ) != eCLOCK_ZIC_RET_OK )
		{
			return 	eCLOCK_ZIC_RET_ERR_DST_BEGIN;
		}
		if( dstFmtParser(dstEnd, &tzDst.dst.dstEnd  ) != eCLOCK_ZIC_RET_OK )
		{
			return 	eCLOCK_ZIC_RET_ERR_DST_BEGIN;
		}		
	}
	else
	{
		/*Other format*/	
	}
	
	/*DST Status*/
	tzDst.dst.bDst = (bDstSame==true)?false:bDst;
	    

    

#if 0 //ex. Year-Month-Day format
    snprintf(ruleline1, sizeof(ruleline1), "%s %s %4d %s %s %s %d %d:%02d:%02d %d:%02d %s\n",
                       "Rule", 
					   RULE_NAME, 
					   daylight_start_tm.tm_year+1900, 
					   "only", 
					   "-",
                       rtStrMonth(tzDst.dst.dstBegin.month), daylight_start_tm.tm_mday,
                       daylight_start_tm.tm_hour, daylight_start_tm.tm_min, daylight_start_tm.tm_sec, (IsDstOn == true)? 1 : 0, 0, "D");

    snprintf(ruleline2, sizeof(ruleline2), "%s %s %4d %s %s %s %d %d:%02d:%02d %d:%02d %s\n",
                       "Rule", RULE_NAME, daylight_end_tm.tm_year+1900, "only", "-",
                       Month[daylight_end_tm.tm_mon], daylight_end_tm.tm_mday,
                       daylight_end_tm.tm_hour, daylight_end_tm.tm_min, daylight_start_tm.tm_sec, 0, 0, "S");

#else
    snprintf(strRuleDstBegin,sizeof(strRuleDstBegin), "%s\t%s\t%4d\t%4d\t%s\t%s\t%s>=%d\t%d:%02d\t%d:%02d\t%s\n",
                       "Rule", 
					   RULE_NAME, 
					   DST_BEGIN_YEAR, 
					   DST_END_YEAR, 
					   "-",
                       rtStrMonth(tzDst.dst.dstBegin.month), 
					   rtStrDayOfWeek(tzDst.dst.dstBegin.dayOfWeek), 
					   (tzDst.dst.dstBegin.week-1)*7+tzDst.dst.dstBegin.dayOfWeek+1,
                       tzDst.dst.dstBegin.hour, 
					   tzDst.dst.dstBegin.min, 
					   (tzDst.dst.bDst == true)? 1 : 0, //if Start And End is same then Dst Off otherwise refer to tzDst.dst.bDst
					   0, 
					   zoneName);
	if(!bDstSame)
	{
	    snprintf(strRuleDstEnd,sizeof(strRuleDstEnd), "%s\t%s\t%4d\t%4d\t%s\t%s\t%s>=%d\t%d:%02d\t%d:%02d\t%s\n",
	                       "Rule", 
						   RULE_NAME, 
						   DST_BEGIN_YEAR, 
						   DST_END_YEAR, 
						   "-",
	                       rtStrMonth(tzDst.dst.dstEnd.month), 
						   rtStrDayOfWeek(tzDst.dst.dstEnd.dayOfWeek), 
						   (tzDst.dst.dstEnd.week-1)*7+tzDst.dst.dstEnd.dayOfWeek+1,
	                       tzDst.dst.dstEnd.hour, 
						   tzDst.dst.dstEnd.min, 
						   0,
						   0, 
						   zoneName);
	}
#endif
    snprintf(strTimeZone,sizeof(strTimeZone), "%s %s %s%d:%02d %s %s\n",
                      "Zone", 
					  ZONE_NAME, 
					  (tzDst.tz.bPositive == true) ? "" : "-", 
					  tzDst.tz.hour, 
					  tzDst.tz.min, 
					  RULE_NAME, 
					  "%s");

	
	return createZicFile(TIMEZONE_FILE,strRuleDstBegin,strRuleDstEnd,strTimeZone);
}

int createZicFile(char *filename,char *rule1,char *rule2,char *tz)
{
	FILE *fp;
	if(!filename)
	{
		CLOCK_ZIC_ERR("Input buffer is null\n");
		return eCLOCK_ZIC_RET_BUF_NULL;	
	}
	
    if ((fp = fopen(filename, "w+")) == NULL)
    {
        printf("open file: %s error\n", filename);
        return eCLOCK_ZIC_RET_OPEN_FILE_FAIL;
    }
    else
    {
        fprintf(fp, "%s", "# Rule  NAME    FROM    TO      TYPE    IN      ON      AT      SAVE    LETTER/S\n");
        if(rule1)
        fprintf(fp, "%s", rule1);
        
        if(rule2)
            fprintf(fp, "%s", rule2);
        fprintf(fp, "%s", "# Zone  NAME    GMTOFF  RULES   FORMAT  [UNTIL]\n");
        if(tz)
        fprintf(fp, "%s", tz);
        fflush(fp);
        fclose(fp);
    }

    return issueZicCommand(filename);
}

/*Don't be needed*/
#if 0
int getTimezoneDaylightSaving(char *filename,sTZDst *rtTzDst)
{
    FILE *fp;

	if(!filename)
	{
		CLOCK_ZIC_ERR("Input buffer is null\n");
		return eCLOCK_ZIC_RET_BUF_NULL;	
	}

    if ((fp = fopen(TIMEZONE_FILE, "r")) == NULL)
    {
        CLOCK_ZIC_ERR("open file: %s error\n", TIMEZONE_FILE);
    }
    else
    {
    	/*Read from zic file*/
	}

#if 0
    //method 3: use zdump to get the value from timezone binary file
    sprintf(cmdline, "zdump -v %s > %s", "/etc/localtime", ZDUMP_LOG);
    system(cmdline); 
    CLOCK_ZIC_DBG("cmd: %s\n", cmdline);
    if ((fp = fopen(ZDUMP_LOG, "r")) == NULL)
    {
        printf("open file: %s error\n", ZDUMP_LOG);
    }
    else
    {
        fclose(fp);
        return eCLOCK_ZIC_RET_FAIL;
    }
#endif

    return eCLOCK_ZIC_RET_OK;
}
#endif

#if 1 /*Unittest*/
int main(int argc, char **argv)
{
 	int ret = 0;
	ret = setTimezoneDaylightSaving("+5",true,"M3.1.0/12","M9.1.0.12",eDST_FMT_MONTH_WEEK_DAY);
    return 0;
}
#endif
