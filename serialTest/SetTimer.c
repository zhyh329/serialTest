#include "SetTimer.h"
#include "ATOSys.h"
#include <time.h>
#include "ATOLog.h"

/********************variables declare***************************************************/

/********************local variables ***************************************************/

void FormatTimeToMillisec_ST(int8 *pTimeStr,int32 size)/*****FormatTimeToMillisec_ST******/
{
  struct tm tmTime;
	struct timeb tp;
  int32 tmpYear = 0;
  int32 tmpMonth = 0;
  int32 tmpDate=0;
  int32 tmpHour=0;
  int32 tmpMin=0;
  int32 tmpSec=0;
  int32 tmpMSec=0;

	/*clear struct*/
	memset(&tp,0,sizeof(tp));
  memset(&tmTime,0,sizeof(tmTime));

	if(pTimeStr!=NULL)
	{
    ftime(&tp);/**/
    LocalTime_SYS((int32*)(&(tp.time)),&tmTime);
    tmpYear = 1900+tmTime.tm_year;/*yearTmp*/
    tmpMonth = 1+tmTime.tm_mon;
    tmpDate = tmTime.tm_mday;
    tmpHour = tmTime.tm_hour;
    tmpMin = tmTime.tm_min;
    tmpSec = tmTime.tm_sec;
    tmpMSec = tp.millitm;
    snprintf(pTimeStr,size,"%04ld-%02ld-%02ld %02ld:%02ld:%02ld:%03ld",tmpYear,tmpMonth,tmpDate,tmpHour,tmpMin,tmpSec,tmpMSec);/*snprintf*/
	}
}

void FormatTimeToHour_ST(int8 *pTimeStr,int32 size)/*****FormatTimeToHour_ST******/
{
	struct timeb tp;
  struct tm tmTime;

	/*clear struct*/
	memset(&tp,0,sizeof(tp));
  memset(&tmTime,0,sizeof(tmTime));

	if(pTimeStr!=NULL)
	{
    ftime(&tp);/*ftime*/
    LocalTime_SYS((int32*)(&(tp.time)),&tmTime);
    snprintf(pTimeStr,size,"%02d",tmTime.tm_hour);/*snprintf*/
	}
}

void FormatTimeToDate_ST(int8 *pTimeStr,int32 size)/*FormatTimeToDate_ST*/
{
	struct timeb tp;
  struct tm tmTime;
  int32 tmpYear = 0;
  int32 tmpMonth = 0;
  int32 tmpDate = 0;

	/*clear struct*/
	memset(&tp,0,sizeof(tp));
  memset(&tmTime,0,sizeof(tmTime));

	if(pTimeStr!=NULL)
	{
		/*get time*/
    ftime(&tp);
    LocalTime_SYS((int32*)(&(tp.time)),&tmTime);

		/*tmp set*/
    tmpYear = 1900+tmTime.tm_year;
    tmpMonth = 1+tmTime.tm_mon;
    tmpDate = tmTime.tm_mday;

		/*put str into pTimeStr*/
    snprintf(pTimeStr,size,"%04ld-%02ld-%02ld",tmpYear,tmpMonth,tmpDate);
	}
}

/*ts@men: This returns milliseconds */
uint32 GetTickCount_ST(void)/*GetTickCount_ST*/
{
	struct timespec ts = {0,0};
	int32 ms = 0;
	uint32 ret = 0UL;

	/*get ns tick from clock*/
	clock_gettime(CLOCK_MONOTONIC,&ts);

	/*cal ms count*/
	ms = ts.tv_sec*1000+ts.tv_nsec/1000000;

	/*data change*/
	ret = (uint32)(ms);

	return ret;
}

void LocalTime_SYS(int32 *pTimeCount,struct tm *pTmTime)/* LocalTime_SYS */
{
  struct tm *pResult;

  if((pTimeCount!=NULL)&&(pTmTime!=NULL))
  {
    pResult = localtime(pTimeCount);/* localtime */

    *pTmTime = *pResult;
  }
}
