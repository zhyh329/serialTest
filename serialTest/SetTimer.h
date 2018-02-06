#ifndef SETTIMER_H_INCLUDED
#define SETTIMER_H_INCLUDED
#include "StdDef_ATO.h"
#include "StructTypeDef.h"

void FormatTimeToMillisec_ST(int8 *pTimeStr,int32 size);
void FormatTimeToHour_ST(int8 *pTimeStr,int32 size);
void FormatTimeToDate_ST(int8 *pTimeStr,int32 size);
uint32 GetTickCount_ST(void);
void LocalTime_SYS(int32 *pTimeCount,struct tm *pTmTime);/* LocalTime_SYS */

#endif /* SETTIMER_H_INCLUDED*/
