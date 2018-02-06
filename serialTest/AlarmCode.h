
#ifndef ALARMCODE_H_INCLUDED
#define ALARMCODE_H_INCLUDED

#define NO_ERROR_ATO 0UL
#define NO_ERROR     0UL

/* the serious errors and alarms of the system */
#define SERIOUS_SERCOM_OPEN_ERR       1017UL     /*serial com init error*/
#define SERIOUS_SERCOM_READ_ERR       1018UL     /*serial com init error*/
#define SERIOUS_SERCOM_WRITE_ERR      1019UL     /*serial com init error*/
#define SERIOUS_SERCOM_SET_ERR        1020UL     /*serial com setting error*/
#define SERIOUS_SERCOM_SPDSET_ERR     1021UL     /*serial com speed setting error*/
#define SERIOUS_SERCOM_DBSET_ERR      1022UL     /*serial com databits setting error*/
#define SERIOUS_SERCOM_SBSET_ERR      1023UL     /*serial com stopbit setting error*/
#define SERIOUS_SERCOM_PARSET_ERR     1024UL     /*serial com parity setting error*/
#define SERIOUS_SHM_IDGET_ERR         1029UL    /*sharememorys create error*/
#define SERIOUS_SHM_AT_ERR            1030UL
#define SERIOUS_SHM_DEATTCH_ERR       1031UL
#define SERIOUS_SHM_CTL_ERR           1032UL
#define SERIOUS_SEM_IDGET_ERR         1033UL    /*semaphore create error*/
#define SERIOUS_SEM_SET_ERR           1034UL    /*semaphore init error*/
#define SERIOUS_SEM_GET_ERR           1035UL    /*semaphore init error*/
#define SERIOUS_SEM_RELEASE_ERR       1036UL    /*semaphore init error*/
#define SERIOUS_SEM_DEL_ERR           1037UL

#define SERIOUS_POINTER_NULL_ERROR    1058UL

#endif /* ALARMCODE_H_INCLUDED*/
