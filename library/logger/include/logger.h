/********************************************************************
�ļ�����logger.h
�����ˣ�lzw978@163.com
��  �ڣ�2017-08-08
�޸��ˣ�
��  �ڣ�
��  ������¼��־
��  ����
********************************************************************/

#ifndef __LIB_LOGGER_H__
#define __LIB_LOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ��־�ȼ�
enum LOG_LEVEL
{ 
    L_SYS       = 0,
    L_IMPORTANT = 1,
    L_ERROR     = 2,
    L_INFO      = 3,
    L_DEBUG     = 4
};

// ������־���ֵ
void SetLogMaxSize(int iLogMaxSize);
// ������־�ȼ�
void SetLogLevel(int iLogLevel);
// ������־·��
void SetLogPath(const char *pLogPath);
// ������־����
void SetLogFileName(const char *pLogFileName);
// ��־������
void SetNewLogFileName(char *pLogFileName, char *pMsgId=NULL);
// ������־����
void ReSetLogFileName();
// ���ô�����־
void SetErrLogFile(const char *pErrLogFile);
// д��־����
void _Trace(const char* pFileName, int iFileLine, LOG_LEVEL level, const char* errcode, const char* format...);
// ���ú�
#define Trace(level, errcode, fmt, ...) _Trace(__FILE__, __LINE__, (level), (errcode), (fmt), ##__VA_ARGS__)

#endif


