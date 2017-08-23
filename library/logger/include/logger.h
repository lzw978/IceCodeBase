/********************************************************************
文件名：logger.h
创建人：lzw978@163.com
日  期：2017-08-08
修改人：
日  期：
描  述：记录日志
版  本：
********************************************************************/

#ifndef __LIB_LOGGER_H__
#define __LIB_LOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 日志等级
enum LOG_LEVEL
{
    L_SYS       = 0,
    L_IMPORTANT = 1,
    L_ERROR     = 2,
    L_INFO      = 3,
    L_DEBUG     = 4
};

// 设置日志最大值
void SetLogMaxSize(int iLogMaxSize);
// 设置日志等级
void SetLogLevel(int iLogLevel);
// 设置日志路径
void SetLogPath(const char *pLogPath);
// 设置日志名称
void SetLogFileName(const char *pLogFileName);
// 日志重命名
void SetNewLogFileName(char *pLogFileName, char *pMsgId=NULL);
// 重置日志名称
void ReSetLogFileName();
// 设置错误日志
void SetErrLogFile(const char *pErrLogFile);
// 写日志函数
void _Trace(const char* pFileName, int iFileLine, LOG_LEVEL level, const char* errcode, const char* format...);
// 调用宏
#define Trace(level, errcode, fmt, ...) _Trace(__FILE__, __LINE__, (level), (errcode), (fmt), ##__VA_ARGS__)

#endif


