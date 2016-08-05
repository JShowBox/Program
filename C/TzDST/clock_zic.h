#ifndef	_PM_CLOCK_ZIC_H
#define	_PM_CLOCK_ZIC_H

#define RULE_NAME "DST" /* user_rule */
#define ZONE_NAME "TZONE" /* user_zone */

#define CLOCK_PATH  "/etc/"
#define ZIC_ZONE_FILE   CLOCK_PATH ZONE_NAME 
#define TIMEZONE_FILE   CLOCK_PATH"user_tz_info"
#define ZIC_BINARY "/usr/sbin/zic"

#define ZDUMP_LOG "zdump.txt"
#define MAX_PARAM_LEN 20
#define CLOCK_MAX_STR_LEN 256
#define MAX_LEN_TIME_BUFFER 16

#define DST_BEGIN_YEAR	2000
#define DST_END_YEAR	2089

typedef enum
{
	eDST_FMT_MONTH_WEEK_DAY = 0,
	eDST_FMT_MAX
}eDST_FMT;

typedef enum
{
    eGMT_SIGN_MINUS=0,
    eGMT_SIGN_PLUS,
    eGMT_SIGN_MAX
}eGMT_SIGN;

typedef enum
{
	eCLOCK_ZIC_RET_OK = 0,
	eCLOCK_ZIC_RET_BUF_NULL,
	eCLOCK_ZIC_RET_UNSUPPORT_DST_FMT,
	eCLOCK_ZIC_RET_ERR_NOT_DIGITAL,
	eCLOCK_ZIC_RET_ERR_OUT_OF_BUF_SIZE,
	eCLOCK_ZIC_RET_ERR_OUT_OF_RANGE,
	eCLOCK_ZIC_RET_ERR_MONTH_FMT,
	eCLOCK_ZIC_RET_ERR_WEEK_FMT,
	eCLOCK_ZIC_RET_ERR_DAY_OF_WEEK_FMT,
	eCLOCK_ZIC_RET_ERR_HOUR_FMT,
	eCLOCK_ZIC_RET_ERR_DST_BEGIN,
	eCLOCK_ZIC_RET_ERR_DST_END,
	eCLOCK_ZIC_RET_ERR_GMT_TIME_FMT,
	eCLOCK_ZIC_RET_ERR_GMT_FMT,
	eCLOCK_ZIC_RET_OPEN_FILE_FAIL,
	eCLOCK_ZIC_RET_FAIL
}eCLOCK_ZIC_RET_CODE;

typedef struct _sTime
{
	int hour;
	int min;
	int sec;
}sTime;

typedef struct _sDate
{
	int year;
	int month;
	int day;
}sDate;

typedef struct _sDateTime
{
	sDate date;
	sTime time;	
}sDateTime;

/*Time Zone Structure*/
typedef struct _sTimeZoneInfo 
{
	bool bPositive;
	int hour;
	int min;
	int sec;
}sTimeZoneInfo;
/*Daylight Saving Time*/
typedef struct _sDstInfo
{
	int month;
	int week;
	int dayOfWeek;
	int hour;
	int min;	/*not used*/
	int sec;		/*not used*/
}sDstInfo;

typedef struct _sDst
{
	bool bDst;
	sDstInfo dstBegin;
	sDstInfo dstEnd;
}sDst;

typedef struct sTZDst
{
	sTimeZoneInfo tz;
	sDst dst;	
}sTZDst;

typedef struct _sDateTimeAndTimeSetting
{
	sDateTime *dateTime;
	sTZDst *tz;	
}sDateTimeAndTimeSetting;

typedef struct _sTimeInfo
{
	char strCurDate[MAX_LEN_TIME_BUFFER];
	char strCurTime[MAX_LEN_TIME_BUFFER];
	char strTimeZone[MAX_LEN_TIME_BUFFER];
	char strDST[MAX_LEN_TIME_BUFFER];
	char strDSTBegin[MAX_LEN_TIME_BUFFER];
	char strDSTEnd[MAX_LEN_TIME_BUFFER];
}sTimeInfo;

extern int setTimezoneDaylightSaving(char *gmt,bool bDst,char *dstBegin,char *dstEnd, int fmt);


#endif

