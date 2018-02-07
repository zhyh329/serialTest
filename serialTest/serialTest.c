#include <time.h>
#include <termios.h>
#include <stddef.h>
#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "string.h"
#include "AlarmCode.h"
#include "serialTest.h"
#include "typedef.h"
#include "ATOLog.h"
#include "StructTypeDef.h"
#include "SetTimer.h"

#define S4_PORT     	"/dev/ttyS4"
#define S4_BAUDRATE  	B115200
#define S4_DATABITS  	8U
#define S4_STOPBIT   	1U
#define S4_PARITY    	'E'

#define S5_PORT     	"/dev/ttyS5"
#define S5_BAUDRATE  	B115200
#define S5_DATABITS  	8U
#define S5_STOPBIT   	1U
#define S5_PARITY    	'E'

#define S6_PORT        	"/dev/ttyS6"
#define S6_BAUDRATE    	B115200
#define S6_DATABITS    	8U
#define S6_STOPBIT     	1U
#define S6_PARITY      	'N'

#define S7_PORT      	"/dev/ttyS7"
#define S7_BAUDRATE  	B115200
#define S7_DATABITS  	8U
#define S7_STOPBIT   	1U
#define S7_PARITY    	'E'

#define S8_PORT      	"/dev/ttyS8"
#define S8_BAUDRATE  	B115200
#define S8_DATABITS  	8U
#define S8_STOPBIT   	1U
#define S8_PARITY    	'E'

#define S9_PORT      	"/dev/ttyS9"
#define S9_BAUDRATE  	B115200
#define S9_DATABITS  	8U
#define S9_STOPBIT   	1U
#define S9_PARITY    	'N'

#define S10_PORT      	"/dev/ttyS10"
#define S10_BAUDRATE  	B115200
#define S10_DATABITS  	8U
#define S10_STOPBIT   	1U
#define S10_PARITY    	'N'

#define S11_PORT      	"/dev/ttyS11"
#define S11_BAUDRATE  	B115200
#define S11_DATABITS  	8U
#define S11_STOPBIT   	1U
#define S11_PARITY    	'N'

#define TX_BUFF_SIZE		1024U
#define REC_BUFF_SIZE     	1024U
#define NR_BYTES_TO_SEND	200
#define NR_OF_UARTS		8
#define TX_INTERVAL 		100	/* milliseconds */

/* offset to loop indices to display real ttyS number */
#define UART_BASE_OFFS 		4

static SerialCfgStruct gSerialCfg[NR_OF_UARTS] =
{
  {0, S4_PORT,      S4_BAUDRATE,      S4_DATABITS,      S4_STOPBIT,       S4_PARITY},
  {0, S5_PORT,      S5_BAUDRATE,      S5_DATABITS,      S5_STOPBIT,       S5_PARITY},
  {0, S6_PORT,      S6_BAUDRATE,      S6_DATABITS,      S6_STOPBIT,       S6_PARITY},
  {0, S7_PORT,      S7_BAUDRATE,      S7_DATABITS,      S7_STOPBIT,       S7_PARITY},
  {0, S8_PORT,      S8_BAUDRATE,      S8_DATABITS,      S8_STOPBIT,       S8_PARITY},
  {0, S9_PORT,      S9_BAUDRATE,      S9_DATABITS,      S9_STOPBIT,       S9_PARITY},
  {0, S10_PORT,     S10_BAUDRATE,     S10_DATABITS,     S10_STOPBIT,      S10_PARITY},
  {0, S11_PORT,     S11_BAUDRATE,     S11_DATABITS,     S11_STOPBIT,      S11_PARITY}
};

pid_t gParentPidNum = 0;
int32 gLogMsgId = 0;

static void CtrlCHandle_PG(void);
static void SerialReceive_SCOM(uint8 index);
static void TimerLoop(void);
static void ReleaseSysResource_MAIN();
static int32 SerialWritableCheck(uint8 index, uint32 uSec);
static void ResetSerialPort(uint8 index);
uint32 InitSerialCom_SCOM(void);
static uint32 InitSerialCom(SerialCfgStruct *pCfg);
static uint32 OpenDev(SerialCfgStruct *pCfg);
static uint32 SetSerialCom(SerialCfgStruct cfg);
static uint32 SetSpeed(SerialCfgStruct cfg);
static uint32 SetParity(SerialCfgStruct cfg);
static uint32 SetControlMode(SerialCfgStruct cfg);
void CloseSerialPort_SCOM(void);
uint32 MsgIdCreate(int32 *pMsgId);
uint32 MsgIdRemove(int32 msgId);


int main()
{
  uint8 i = 0U;
  pid_t pidNum = 0;

  gParentPidNum = getpid();
  printf("\nparent pid %d\n",gParentPidNum);

  /*register Ctrl+C handle process*/
  signal(SIGINT,(void*)(CtrlCHandle_PG));
  signal(SIGTERM,(void*)(CtrlCHandle_PG));

  InitSerialCom_SCOM();

  /*create log msg id */
  MsgIdCreate(&(gLogMsgId));

  /* spawn the logging process */
  if(fork() == 0)
  { /* I am child process */
    pidNum = getpid();
    printf("logMsg pid %d\n",pidNum);
    usleep(100000U);
    LogMsgProcess_AL(); /* <- runs forever */
  }

  /* spawn NR_OF_UARTS receiver processes */
  for(i=0U;i < NR_OF_UARTS ;i++)
  {
    if(fork() == 0)
    {
      pidNum = getpid();
      printf("S%d pid %d\n",i+UART_BASE_OFFS,pidNum);
      usleep(100000U);
      SerialReceive_SCOM(i); /* <- runs forever */
    }
  }

  /* spawn the sending process */
  if(fork() == 0)
  {
    pidNum = getpid();
    printf("timer pid %d\n",pidNum);
    usleep(100000U);
    TimerLoop(); /* <- runs forever */
  }

  while(1);

  return 0;
}


static void SerialReceive_SCOM(uint8 index)
{

  uint8 receiveBuf[REC_BUFF_SIZE];/*the receive buffer of ATP interface*/
  int32 val = 0;
  int8 recvFileName[128U];

  /*clear array*/
  memset(receiveBuf,0,sizeof(receiveBuf));
  memset(recvFileName,0,sizeof(recvFileName));

  tcflush(gSerialCfg[index].fd,TCIOFLUSH);/*clear the cache buffer of the com*/

  snprintf(recvFileName,128,"S%d_Recv",index+4U);

  while(1U)
  {
    val = read(gSerialCfg[index].fd,receiveBuf,sizeof(receiveBuf)); /*read data from the serial*/

    if(val != 0)
    {	/*YES there is data*/
      TraceIntoHexFile2_AL(receiveBuf,val,recvFileName);
    }
  }
}

/* ts@men this is called every 100ms for every UART */
static uint32 SendDataToSerial_SCOM(uint8 index, uint8 buff[],uint16 len)
{
  int32 val = 0;
  uint32 ret = NO_ERROR;
  /* ts@men: use 5ms instead 1ms, give the OS time for other tasks */
  val = SerialWritableCheck(index, 5000U);
  if(val<=0)
  {
    ResetSerialPort(index);
  }

  val = write(gSerialCfg[index].fd,buff,len);

  fsync(gSerialCfg[index].fd);
  if(val == -1)
  {
    ret = SERIOUS_SERCOM_WRITE_ERR;
  }

  return ret;
}

/* ts@men.de: TimerLoop Thread */
static void TimerLoop(void)
{
  uint8 i = 0U;
  uint32 count = 0;
  uint32 tickRec = 0;
  uint32 tickCount = 0;
  uint8 buff[TX_BUFF_SIZE] = {0};
  int8 datebuff[64];

  memset(buff,0,sizeof(buff));

  tickRec = GetTickCount_ST();

  while(1U)
  {
    tickCount = GetTickCount_ST();

    if(tickCount-tickRec >= TX_INTERVAL )
    {
      tickRec = tickCount;
      /* printf("tickCount %d\n",(int)tickCount); */
      count ++;
      buff[0]=(uint8)(count>>24U);
      buff[1]=(uint8)(count>>16U);
      buff[2]=(uint8)(count>>8U);
      buff[3]=(uint8)(count);

      buff[4]=(uint8)(tickRec>>24U);
      buff[5]=(uint8)(tickRec>>16U);
      buff[6]=(uint8)(tickRec>>8U);
      buff[7]=(uint8)(tickRec);

      memset( datebuff, 0x0, sizeof(datebuff));
      FormatTimeToMillisec_ST( datebuff, sizeof(datebuff));
      fprintf( stdout, "%s %07ld ", datebuff, count );
      fflush( stdout );

      for(i=0U;i < NR_OF_UARTS ;i++)
      {
	(void)(SendDataToSerial_SCOM(i, buff, NR_BYTES_TO_SEND ));
      }

      /* closing linefeed for this cycle */
      fprintf( stdout, "\n" );
      fflush( stdout );

      if(count==0xFFFFFFFF)
      {
        count = 1U;
      }
    }
  }
}


static void CtrlCHandle_PG(void)
{
  pid_t pid=0U;

  pid = getpid();

  if(pid!=gParentPidNum)
  {
    printf("Exit %d\n",pid);
    exit(0U);
  }
  else
  {
    usleep(100000U);
    ReleaseSysResource_MAIN();
    printf("Exit %d\n",pid);
    exit(0U);
  }
}

static void ReleaseSysResource_MAIN()
{
  CloseSerialPort_SCOM();
  MsgIdRemove(gLogMsgId);
}

static int32 SerialWritableCheck(uint8 index, uint32 uSec)
{
  int32 val = 0;
  struct timeval tim;
  uint32 t1,t2;
  fd_set fdSet;

  tim.tv_sec = 0;
  tim.tv_usec = uSec;
  FD_ZERO(&fdSet);
  FD_SET(gSerialCfg[index].fd, &fdSet);

  /* ts@men: check if select() blocks really the specified time */
  t1 = GetTickCount_ST();
  val = select(gSerialCfg[index].fd+1,NULL,&fdSet,NULL,&tim);
  t2 = GetTickCount_ST();
  /* this is supposed to have no linefeed, LF for one 100ms cycle is added in TimerLoop */
  fprintf( stdout, "S%d: %d ", (int)( index + UART_BASE_OFFS ), (int)(t2 - t1) );
  fflush( stdout );

  return val;
}

static void ResetSerialPort(uint8 index)
{
  int8 str[128U]; /*the array used to store string*/
  memset(str,0,sizeof(str));
  tcflush(gSerialCfg[index].fd,TCOFLUSH);
  fsync(gSerialCfg[index].fd);
  snprintf(str,128U,"Serial ttyS%u Reset!\n",index + UART_BASE_OFFS );
  printf("%s",str);
  RecordStringInFile_AL(str,"Event");
}



uint32 InitSerialCom_SCOM(void)
{
  uint32 ret = NO_ERROR;
  uint8 i = 0U;

  printf("Init serial com ....\n");

  for(i=0;(i < NR_OF_UARTS )&&(ret==NO_ERROR);i++)
  {
    ret = InitSerialCom(&(gSerialCfg[i]));
    if(ret!=NO_ERROR)
    {
      printf("S%i..fail\n",i + UART_BASE_OFFS );
    }
  }

  if(ret==NO_ERROR)
  {
    printf("serial com init ok\n");
  }

  return ret;
}

static uint32 InitSerialCom(SerialCfgStruct *pCfg)
{
  uint32 ret = NO_ERROR;
  /*open the port*/
  ret = OpenDev(pCfg);
  if(ret == NO_ERROR)
  {/*set the para for the serial com*/
    ret = SetSerialCom(*pCfg);
  }
  return ret;
}


static uint32 OpenDev(SerialCfgStruct *pCfg)
{
  uint32 ret = NO_ERROR;
  int32 val = 0;
  int32 fdTmp = 0;

  if(pCfg->port!=NULL)
  {
    /*open the serial port interface*/
    fdTmp = open(pCfg->port, O_RDWR | O_NOCTTY | O_NDELAY );
    if(fdTmp == -1)
    {
      perror("OpenDev port");
      ret = SERIOUS_SERCOM_OPEN_ERR;
    }
    else
    {
      val = fcntl(fdTmp, F_SETFL, 0);/*set the file status flag*/
      if(val == -1)
      {
        perror("InitSerialCom_SCOM fcntl");
        ret = SERIOUS_SERCOM_SET_ERR;
      }
    }

    pCfg->fd = fdTmp;
  }
  else
  {
    ret = SERIOUS_SERCOM_OPEN_ERR;
  }

  return ret;
}

static uint32 SetSerialCom(SerialCfgStruct cfg)
{
  uint32 ret = NO_ERROR;

  /*set speed for the serial com*/
  ret = SetSpeed(cfg);

  if(ret == NO_ERROR)
  {
    /*set the databits stopbit parity for the serial com*/
    ret = SetParity(cfg);
  }

  if(ret == NO_ERROR)
  {
    /*set the control mode for the serial com*/
    ret = SetControlMode(cfg);
  }

  return ret;
}

static uint32 SetSpeed(SerialCfgStruct cfg)
{
  uint32 ret = NO_ERROR;
  int32 val = 0;
  struct termios opt;

  /*get the baudrate for the SRP serial com*/
  tcgetattr(cfg.fd,&opt);/*get the attribute of the com*/
  cfsetispeed(&opt,cfg.baudrate);/*set the input baudrate for the com*/
  cfsetospeed(&opt,cfg.baudrate);/*set the output baudrate for the com*/
  tcflush(cfg.fd,TCIOFLUSH);/*clear the cache buffer of the com*/
  val = tcsetattr(cfg.fd,TCSANOW,&opt);/*set the com new para*/

  if(val != 0)
  {
    perror("SetSpeed tcsetattr");
    ret = SERIOUS_SERCOM_SPDSET_ERR;
  }

  return ret;
}

static uint32 SetParity(SerialCfgStruct cfg)
{
  uint32 ret = NO_ERROR;
  int32 val = 0;
  struct termios opt;

  /*get databits stopbit parity for the SRP interface*/
  val = tcgetattr(cfg.fd,&opt);
  if(val != 0)
  {
    perror("SetParity tcgetattr");
    ret = SERIOUS_SERCOM_PARSET_ERR;
  }
  if(ret==NO_ERROR)
  {
    cfmakeraw(&opt);

    opt.c_cflag &= ~CSIZE;
    switch(cfg.dataBits)
    {
    case 7:
      opt.c_cflag |= CS7;
      break;
    case 8:
      opt.c_cflag |= CS8;
      break;
    default:
      fprintf(stderr,"databits error!\n");
      ret = SERIOUS_SERCOM_DBSET_ERR;
      break;
    }
  }
  if(ret==NO_ERROR)
  {
    switch(cfg.stopBit)
    {
    case 1:
      opt.c_cflag &= ~CSTOPB;
      break;
    case 2:
      opt.c_cflag |= CSTOPB;
      break;
    default:
      fprintf(stderr,"stopbit error!\n");
      ret = SERIOUS_SERCOM_SBSET_ERR;
      break;
    }
  }
  if(ret==NO_ERROR)
  {
    switch(cfg.parity)
    {
    case 'n':
    case 'N':
      opt.c_cflag &= ~PARENB;/*clear parity enable*/
      opt.c_iflag &= ~INPCK;/*disable parity checking*/
      break;
    case 'o':
    case 'O':
      opt.c_cflag |= (PARODD | PARENB);/*set ODD check*/
      opt.c_iflag |= INPCK;/*enable parity checking*/
      break;
    case 'e':
    case 'E':
      opt.c_cflag |=  PARENB;/*enable parity*/
      opt.c_cflag &= ~PARODD;/*transfer to E checking*/
      opt.c_iflag |= INPCK;/*enable parity checking*/
      break;
    case 's':
    case 'S':
      opt.c_cflag &= ~PARENB;
      opt.c_cflag &= ~CSTOPB;
      break;
    default:
      fprintf(stderr,"parity error!\n");
      ret = SERIOUS_SERCOM_PARSET_ERR;
      break;
    }
  }
  if(ret==NO_ERROR)
  {
    if(cfg.parity!='n')
    {
      opt.c_iflag |= INPCK;
    }

    tcflush(cfg.fd,TCIOFLUSH);/*clear the cache buffer of the com*/
    val = tcsetattr(cfg.fd,TCSANOW,&opt);
    if(val != 0)
    {
      perror("SetParity tcsetattr");
      ret = SERIOUS_SERCOM_SET_ERR;
    }
  }
  return ret;
}

static uint32 SetControlMode(SerialCfgStruct cfg)
{
  uint32 ret = NO_ERROR;
  int32 val = 0;
  struct termios opt;

  /*get databits stopbit parity for the SRP interface*/
  val = tcgetattr(cfg.fd,&opt);

  if(val != 0)
  {
    perror("SetControlMode tcgetattr");
    ret = SERIOUS_SERCOM_SET_ERR;
  }
  if(ret==NO_ERROR)
  {
    opt.c_lflag&=~(ICANON|ECHO|ECHOE|ISIG);
    opt.c_iflag&=~(IXON|IXOFF|IXANY);
    opt.c_iflag&=~(INLCR|IGNCR|ICRNL);
    opt.c_cc[VTIME] = 150;/*set the timeover 1 second*/
    opt.c_cc[VMIN] = 0;/*update the options and do it now*/
    tcflush(cfg.fd,TCIOFLUSH);/*clear the cache buffer of the com*/
    val = tcsetattr(cfg.fd,TCSANOW,&opt);
    if(val != 0)
    {
      perror("SetControlMode tcsetattr");
      ret = SERIOUS_SERCOM_SET_ERR;
    }
  }
  return ret;
}

void CloseSerialPort_SCOM(void)
{
  uint8 i = 0U;
  int32 val = 0;

  printf("close serial ports\n");

  for(i=0U;i < NR_OF_UARTS ;i++)
  {
    val = close(gSerialCfg[i].fd);
    if(val<0)
    {
      printf("close S%i fail\n",i + UART_BASE_OFFS);
    }
    else
    {
      printf("close S%i ok\n",i + UART_BASE_OFFS);
    }
  }
}

uint32 MsgIdCreate(int32 *pMsgId)
{
  uint32 ret = NO_ERROR;
  int32 result = 0;

  if(pMsgId==NULL)
  {
    ret = SERIOUS_POINTER_NULL_ERROR;
  }
  else
  {
    result = msgget(IPC_PRIVATE,IPC_CREAT|438);
    if(result < 0)
    {
      perror("MsgIdCreate error");
      ret = 7003U/*SERIOUS_MSG_GET_ERR*/;
    }
    else
    {
      *pMsgId = result;
    }
  }

  return ret;
}

uint32 MsgIdRemove(int32 msgId)
{
  uint32 ret = NO_ERROR;
  int32 result = 0;

  result = msgctl(msgId,IPC_RMID,0);

  if(result<0)
  {
    ret = 7004U/*SERIOUS_MSG_ID_RELEASE_ERROR*/;
  }

  return ret;
}

