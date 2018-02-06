#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include "AlarmCode.h"
#include "ATOSys.h"

/*====================basic macro===========================*/
#define YES		1U
#define NO		0U

#define MAX_TIME_STR_LEN    64U
#define MAX_FILE_NAME_LEN   256U
#define MAX_PATH_NAME_LEN   256U

/*record files*/
#define RECORD_PATH             "..//record"
#define LOG_FILE                "syslog"


/*====================ATOLog.c===========================*/
#define MSG_TYPE_STRING_LOG       1UL
#define MSG_TYPE_HEX_DATA         2UL


#endif /* STDDEF_H_INCLUDED*/
