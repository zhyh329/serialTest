#ifndef ATOLOG_H_INCLUDED
#define ATOLOG_H_INCLUDED
#include "StdDef_ATO.h"
#include "StructTypeDef.h"

void WriteInLogFile_AL(uint32 code);
void TraceIntoHexFile2_AL(uint8 buff[],uint32 len,const int8 classPath[]);
void RecordStringInFile_AL(int8 *pStr,const int8* pFileName);
uint8 CheckFileExist_AL(const int8* buff);
void LogMsgProcess_AL(void);

#endif /* ATOLOG_H_INCLUDED*/
