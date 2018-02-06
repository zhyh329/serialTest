/*******************************************************************************************************/
/* Project Name: MTC-I ATO System                                                                      */
/* Configuration:typedef Document                                                                      */
/* Author:MengJun                                                                                      */
/* Date:2013.9                                                                                         */
/* Record:                                                                                             */
/* Number	  Date     Name     Version	    Reason	      Description                                  */
/* 01       2013.9   MengJun  v-0.0.1     Distribution                                                 */
/* 02       2013.10  wangchao v-0.0.2     codereview                                                   */
/* 03       2013.10  wangchao v-0.0.3     StaticTest                                                   */
/* 04       2014.3   MengJun  v-0.0.4     ResearchTest  P_NULL is added                                                   */
/*******************************************************************************************************/
#ifndef TYPEDEF_H_INCLUDED
#define TYPEDEF_H_INCLUDED


/*=========================== Data Type Definitions =============================*/
#ifndef  _APPDataTypeDef_h 
typedef unsigned char       BOOLEAN;
typedef unsigned char       uint8;
typedef char                int8;
typedef unsigned short      uint16;
typedef short               int16;
typedef unsigned long       uint32;
typedef unsigned long long  uint64;
typedef long                int32;
typedef long long           int64;
typedef float               float32;
typedef double              float64;
#endif
/*=========================== Macro Definitions =============================*/
#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1!=1)
#endif

#ifndef P_NULL
#define P_NULL        0u
#endif

#define mmemset memset
#define mmemcpy memcpy


#endif /*TYPEDEF_H_INCLUDED*/
