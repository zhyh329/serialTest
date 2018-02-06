#ifndef STRUCT_TYPE_DEF_H_INCLUDED
#define STRUCT_TYPE_DEF_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include "typedef.h"
#include "StdDef_ATO.h"

/******************SerialCom.c*******************/
typedef struct
{
  int32 fd;
  int8 port[64U];
  int32 baudrate;
  uint8 dataBits;
  uint8 stopBit;
  uint8 parity;
}SerialCfgStruct;

#define MAX_LOGMSG_SIZE  1024U
typedef struct
{
  uint32 msgType;/*must be >0*/

  int8 path[MAX_PATH_NAME_LEN];
  int8 file[MAX_FILE_NAME_LEN];
  uint8 from;
  uint32 msgLen;
  int8 msgBuf[MAX_LOGMSG_SIZE];

  /*for disk clean*/
  uint32 minSizeInM;
  uint32 targetSizeInM;
}LogMsgStruct;

#endif /* STRUCT_TYPE_DEF_H_INCLUDED*/
