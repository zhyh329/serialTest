#include "ATOLog.h"
#include "SetTimer.h"
#include "ATOSys.h"
#include "StructTypeDef.h"

/********************variables declare***************************************************/
extern int32 gLogMsgId;

/********************local variables ***************************************************/
static  LogMsgStruct gLogMsg;
/********************local function ***************************************************/
static uint32 LogMsgProcessStringLog(LogMsgStruct *pLogMsg);
static uint32 LogMsgProcessHexData(LogMsgStruct *pLogMsg);
static uint32 MakePath(int8 dirPath[],uint32 size,const int8* classPath);


void WriteInLogFile_AL(uint32 code)/*******WriteInLogFile_AL********/
{
  int32 val = 0;
  uint32 tickCountTmp = 0UL;
  int8 timeStrHour[MAX_TIME_STR_LEN];/*the array used to store the time*/
  int8 timeStrMillisec[MAX_TIME_STR_LEN];/*the array used to store the time*/

  /*clear arrays*/
  memset(timeStrHour,0,sizeof(timeStrHour));
  memset(timeStrMillisec,0,sizeof(timeStrMillisec));
  memset(&gLogMsg,0,sizeof(gLogMsg));

  tickCountTmp = GetTickCount_ST();
  FormatTimeToHour_ST(timeStrHour,sizeof(timeStrHour));
  FormatTimeToMillisec_ST(timeStrMillisec,sizeof(timeStrMillisec));

  gLogMsg.msgType = MSG_TYPE_STRING_LOG;
  snprintf(gLogMsg.path,sizeof(gLogMsg.path),"%s",LOG_FILE);/*get fileName*/
  snprintf(gLogMsg.file,sizeof(gLogMsg.file),"%s_%s.txt",LOG_FILE,timeStrHour);/*get fileName*/
  snprintf(gLogMsg.msgBuf,sizeof(gLogMsg.msgBuf),"%s,%lu,Code is %lu\r\n",timeStrMillisec,tickCountTmp,code);
  gLogMsg.msgLen = strlen(gLogMsg.msgBuf);

  val = msgsnd(gLogMsgId,&gLogMsg,sizeof(gLogMsg)-sizeof(uint32),IPC_NOWAIT);
  if(val<0)
  {
    printf("%lu,pid %d,msgsnd() write msg fail,errno=%d[%s]\n",tickCountTmp,getpid(),errno,strerror(errno));
  }

}

void RecordStringInFile_AL(int8 *pStr,const int8* pFileName)
{
  int32 val = 0;
  uint32 tickCountTmp = 0UL;
  int8 timeStrHour[MAX_TIME_STR_LEN];/*the array used to store the time*/
  int8 timeStrMillisec[MAX_TIME_STR_LEN];/*the array used to store the time*/

  /*clear arrays*/
  memset(timeStrHour,0,sizeof(timeStrHour));
  memset(timeStrMillisec,0,sizeof(timeStrMillisec));
  memset(&gLogMsg,0,sizeof(gLogMsg));

  tickCountTmp = GetTickCount_ST();

  FormatTimeToHour_ST(timeStrHour,sizeof(timeStrHour));
  FormatTimeToMillisec_ST(timeStrMillisec,sizeof(timeStrMillisec));

  gLogMsg.msgType = MSG_TYPE_STRING_LOG;
  snprintf(gLogMsg.path,sizeof(gLogMsg.path),"%s",pFileName);/*get fileName*/
  snprintf(gLogMsg.file,sizeof(gLogMsg.file),"%s_%s.txt",pFileName,timeStrHour);/*get fileName*/
  snprintf(gLogMsg.msgBuf,sizeof(gLogMsg.msgBuf),"%s\n %s\r\n",timeStrMillisec,pStr);
  gLogMsg.msgLen = strlen(gLogMsg.msgBuf);

  val = msgsnd(gLogMsgId,&gLogMsg,sizeof(gLogMsg)-sizeof(uint32),IPC_NOWAIT);
  if(val<0)
  {
    printf("%lu,pid %d,msgsnd() write msg fail,errno=%d[%s]\n",tickCountTmp,getpid(),errno,strerror(errno));
  }
}


void TraceIntoHexFile2_AL(uint8 buff[],uint32 len,const int8 classPath[])
{
  int32 val = 0;
  uint32 index = 0UL;
  uint32 i = 0UL;
  int8 timeStrHour[MAX_TIME_STR_LEN];/*the array used to store the time*/
  int8 timeStrMillisec[MAX_TIME_STR_LEN];/*the array used to store the time*/

  /*clear arrays*/
  memset(timeStrHour,0,sizeof(timeStrHour));
  memset(timeStrMillisec,0,sizeof(timeStrMillisec));
  memset(&gLogMsg,0,sizeof(gLogMsg));

  FormatTimeToHour_ST(timeStrHour,sizeof(timeStrHour));
  FormatTimeToMillisec_ST(timeStrMillisec,sizeof(timeStrMillisec));/*FormatTimeToMillisec_ST*/

  gLogMsg.msgType = MSG_TYPE_HEX_DATA;
  snprintf(gLogMsg.path,sizeof(gLogMsg.path),"%s",classPath);/*get fileName*/
  snprintf(gLogMsg.file,sizeof(gLogMsg.file),"%s_%s.txt",classPath,timeStrHour);/*get fileName*/
  /*msg records data*/
  snprintf(gLogMsg.msgBuf,sizeof(gLogMsg.msgBuf),"%s\t",timeStrMillisec);

  /*snprintf*/
  index = strlen(gLogMsg.msgBuf);

  for(i=0UL;(i<len)&&(index+4U<sizeof(gLogMsg.msgBuf));i++)
  {
    snprintf(&(gLogMsg.msgBuf[index]),sizeof(gLogMsg.msgBuf)-index,"%02x ",buff[i]);/*snprintf*/
    index = strlen(gLogMsg.msgBuf);
  }

  snprintf(&(gLogMsg.msgBuf[index]),sizeof(gLogMsg.msgBuf)-index,"\n");/*snprintf*/
  gLogMsg.msgLen = strlen(gLogMsg.msgBuf);

  val = msgsnd(gLogMsgId,&gLogMsg,sizeof(gLogMsg)-sizeof(uint32),IPC_NOWAIT);

  if(val<0)
  {
    printf("%s,pid %d,msgsnd() write msg fail,errno=%d[%s]\n",timeStrMillisec,getpid(),errno,strerror(errno));
  }
}


/*========================================in msg log process=========================================*/
void LogMsgProcess_AL(void)
{
  uint32 ret = NO_ERROR;
  int32 val = 0;
  LogMsgStruct logMsg;

  while(1U)
  {
    memset(&logMsg,0,sizeof(logMsg));

    val = msgrcv(gLogMsgId,&logMsg,sizeof(logMsg)-sizeof(uint32),0,0);
    if(val>=0)
    {
      if(logMsg.msgType == MSG_TYPE_STRING_LOG)
      {
        ret = LogMsgProcessStringLog(&logMsg);
      }
      else if(logMsg.msgType == MSG_TYPE_HEX_DATA)
      {
        ret = LogMsgProcessHexData(&logMsg);
      }
      else
      {
        /*do nothing*/
      }
    }
    else/*fail*/
    {
      printf("\n\n gShmKey.logMsgId=%ld\n\n",gLogMsgId);
      printf("MsgRcv_SYS() rcv msg fail,errno=%d[%s]\n",errno,strerror(errno));
      ret = 7000U/*SERIOUS_MSG_RCV_ERR*/;
    }
  }
}

static uint32 LogMsgProcessStringLog(LogMsgStruct *pLogMsg)
{
  uint32 ret = NO_ERROR;
  int32 fd;
  uint32 lineLen = 0U;
  int8 dirPath[MAX_PATH_NAME_LEN];/*the array used to store the time*/
  int8 fileName[MAX_FILE_NAME_LEN];/*the array used to store the time*/

  memset(dirPath,0,sizeof(dirPath));
  memset(fileName,0,sizeof(fileName));

  ret = MakePath(dirPath,sizeof(dirPath),pLogMsg->path);

  if(ret==NO_ERROR)
  {
    snprintf(fileName,sizeof(fileName),"%s//%s",dirPath,pLogMsg->file);

    fd = open(fileName,O_CREAT|O_WRONLY|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
    if(fd!=-1)
    {
      lineLen = strlen(pLogMsg->msgBuf);
      write(fd,pLogMsg->msgBuf,lineLen);
      close(fd);
    }
    else
    {
      ret = 7001U/*SERIOUS_MSG_FILE_ERR*/;
    }
  }

  return ret;
}


static uint32 LogMsgProcessHexData(LogMsgStruct *pLogMsg)
{
  uint32 ret = NO_ERROR;
  int32 fd;
  uint32 lineLen = 0U;
  int8 dirPath[MAX_PATH_NAME_LEN];/*the array used to store the time*/
  int8 fileName[MAX_FILE_NAME_LEN];/*the array used to store the time*/

  memset(dirPath,0,sizeof(dirPath));
  memset(fileName,0,sizeof(fileName));

  ret = MakePath(dirPath,sizeof(dirPath),pLogMsg->path);
  if(ret==NO_ERROR)
  {
    snprintf(fileName,sizeof(fileName),"%s//%s",dirPath,pLogMsg->file);

    fd = open(fileName,O_CREAT|O_WRONLY|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
    if(fd!=-1)
    {
      lineLen = strlen(pLogMsg->msgBuf);
      //Write_SYS(fd,pLogMsg->msgBuf,lineLen);
      write(fd,pLogMsg->msgBuf,lineLen);
      //Close_SYS(fd);
      close(fd);
    }
    else
    {
      ret = 7001U/*SERIOUS_MSG_FILE_ERR*/;
    }
  }
  return ret;
}


static uint32 MakePath(int8 dirPath[],uint32 size,const int8* classPath)/************MakePath***********/
{
  uint32 ret = NO_ERROR;
  int8 timeStr[MAX_TIME_STR_LEN];/*the array used to store the time*/
  int8 dirPathTmp1[MAX_PATH_NAME_LEN];/*the array used to store the time*/
  int8 dirPathTmp2[MAX_PATH_NAME_LEN];/*the array used to store the time*/
  uint8 fileExist = NO;
  int32 val = 0;

  memset(timeStr,0,sizeof(timeStr));
  memset(dirPathTmp1,0,sizeof(dirPathTmp1));
  memset(dirPathTmp2,0,sizeof(dirPathTmp2));

  /*make data dir*/
  FormatTimeToDate_ST(timeStr,sizeof(timeStr));
  snprintf(dirPathTmp1,sizeof(dirPathTmp1),"%s//%s",RECORD_PATH,timeStr);
  fileExist = CheckFileExist_AL(dirPathTmp1);

  if(fileExist!=YES)/*File is or not exist*/
  {
    mkdir(dirPathTmp1,(uint32)(S_IRWXU|S_IRWXG|S_IRWXO));
  }

  /*make class dir*/
  snprintf(dirPathTmp2,sizeof(dirPathTmp2),"%s//%s",dirPathTmp1,classPath);/*get dirpath*/
  fileExist = CheckFileExist_AL(dirPathTmp2);

  if(fileExist!=YES)/*File is or not exist*/
  {
    val = mkdir(dirPathTmp2,(uint32)(S_IRWXU|S_IRWXG|S_IRWXO));
    if(val == -1)
    {
      ret = 7002U/*SERIOUS_MK_DIR_ERR*/;
    }
  }

  if((sizeof(dirPathTmp2)<=size)&&(size>0))
  {
    strncpy(dirPath,dirPathTmp2,size-1);
  }

  return ret;
}


/*================================in msg log process/main process/mnt related=========================================*/
uint8 CheckFileExist_AL(const int8* buff)/*CheckFileExist_AL*/
{
  uint8 ret = NO;
  int32 val = 0;

  if(buff==NULL)/*buff is or not NULL*/
  {
    ret = NO;
  }
  else
  {
    val = access(buff,F_OK);
    if(val == 0)/*if return ok, the file is exist*/
    {
      ret = YES;
    }
    else
    {
      ret = NO;
    }
  }

  return ret;/*get return*/
}
