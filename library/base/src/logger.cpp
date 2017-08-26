/********************************************************************
文件名：logger.cpp
创建人：lzw978
日  期：2017-08-08
修改人：
日  期：
描  述：记录日志
版  本：
********************************************************************/

#include <string>
#include <stdarg.h>
#include <assert.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <iconv.h>

using namespace std;

#include "logger.h"

#define LOG_BUF 1024*1024*2   // 日志缓存区大小,默认2M

int  g_iMaxFileSize = 0;       // 日志最大容量
int  g_iLogLevel;              // 日志等级
char g_szLogPath[128+1];       // 日志路径
char g_szLogFileName[64+1];    // 日志名称
char g_szLogFileNameOld[64+1]; // 旧日志名称
char g_szErrLogFile[64+1];     // 错误日志名称
char g_szMsgId[30+1];          // 消息ID值
char *g_szTmpG;                // 打印信息

// 获取当前系统时间
char* GetErrTime()
{
    struct timeval  tp;
    struct tm       *p;
    time_t          timep;

    static char szErrTime[30] = {0};

    time(&timep);

    p = localtime(&timep);
    gettimeofday(&tp, NULL);
    sprintf(szErrTime, "%04d/%02d/%02d %02d:%02d:%02d", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

    return szErrTime;
}

// 获取日志时间路径
const char* GetLOGTimePath(char* lpBuf, char* lpTime)
{
    struct timeval  tp;
    struct tm       *p;
    time_t          timep;

    if(NULL == lpBuf)
    {
        return NULL;
    }

    time(&timep);
    p = localtime(&timep);

    gettimeofday(&tp, NULL);
    sprintf(lpBuf , "%04d%02d%02d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
    sprintf(lpTime, "%02d:%02d:%02d:%06d", p->tm_hour, p->tm_min, p->tm_sec, (int)tp.tv_usec);

    //example: lpbuf=20170808 lpTime=11:22:33.123
    return lpBuf;
}

// 判断日志是否已经存在
bool LOGFileExist(const string& sFileName)
{
#ifdef WIN32
    if(_access(sFileName.c_str(), 0) != 0)
    {
        return false;
    }
#else
    if(access(sFileName.c_str(), F_OK) != 0)
    {
        return false;
    }
#endif

    return true;
}

// 返回日志大小
long LOGFileSize(const string& sFileName)
{
    struct stat  FileStat;
#ifdef WIN32
    if(_stat(sFileName.c_str(), &FileStat) != 0)
    {
        return -1;
    }
#else
    if(stat(sFileName.c_str(), &FileStat) != 0)
    {
        return -1;
    }

#endif

    return FileStat.st_size;
}

// 判断日志是否为utf8编码
bool IsUTF8ToLOG(const void* pBuffer, long size)
{
    bool IsUTF8 = true;
    if(NULL == pBuffer)
    {
        return IsUTF8;
    }

    unsigned char* start = (unsigned char*)pBuffer;
    unsigned char* end = (unsigned char*)pBuffer + size;

    while(start < end)
    {
        if (*start < 0x80)
        {
            start++;
        }

        else if (*start < (0xC0))
        {
            IsUTF8 = false;
            break;
        }

        else if (*start < (0xE0))
        {
            if (start >= end - 1)
                break;

            if ((start[1] & (0xC0)) != 0x80)
            {
                IsUTF8 = false;
                break;
            }

            start += 2;
        }
        else if (*start < (0xF0))
        {
            if (start >= end - 2)
                break;

            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
            {
                IsUTF8 = false;
                break;
            }

            start += 3;
        }
        else
        {
            IsUTF8 = false;
            break;
        }
    }

    return IsUTF8;
}

// 创建日志路径
bool CreatePathDir(const char* lpPathName)
{
    int  nLen = 0;
    char szPath[256+1] = { 0 };  //路径临时缓冲区
    char cSeparator   = '/';

    //确保输入参数正确
    if(NULL == lpPathName || 0 == strlen(lpPathName))
    {
        return false;
    }

    //目录名中不用于出现的字符
    strncpy(szPath, lpPathName, sizeof(szPath) - 1);
    nLen = strlen(szPath);

    for (int i = 0; i <= nLen; i++)
    {
        if (((szPath[i] == '\\') || (szPath[i] == '/') || (szPath[i] == '\0')) && (i > 0))
        {
            cSeparator = szPath[i];

            szPath[i] = 0;    //设置字符串结束符

            //在创建多级目录时，如果目录已存在，则不认为是错误，
            //继续创建下一级目录；如果创建目录失败，但失败原因
            //并不是目录已存在，则终止创建，函数返回FALSE。
            mode_t mMode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

            if ((mkdir(szPath, mMode) == -1) && (errno != EEXIST))
            {
                return false;
            }
            szPath[i] = cSeparator;  //恢复字符串
        }
    }

    return true;
}

// 设置日志大小
void SetLogMaxSize(int iLogMaxSize)
{
    g_iMaxFileSize = iLogMaxSize;
}

// 设置日志等级
void SetLogLevel(int iLogLevel)
{
    g_iLogLevel = iLogLevel;
}

// 设置日志路径
void SetLogPath(const char *pLogPath)
{
    strncpy(g_szLogPath, pLogPath, sizeof(g_szLogPath)-1);
    g_szTmpG = new char[LOG_BUF];
}

// 设置日志名称
void SetLogFileName(const char *pLogFileName)
{
    strncpy(g_szLogFileName   , pLogFileName, sizeof(g_szLogFileName)-1);
    strncpy(g_szLogFileNameOld, pLogFileName, sizeof(g_szLogFileNameOld)-1);
    memset(g_szMsgId, 0, sizeof(g_szMsgId));
}

// 设置日志新名称
void SetNewLogFileName(char *pLogFileName, char *pMsgId)
{
    memset(g_szMsgId, 0, sizeof(g_szMsgId));
    snprintf(g_szLogFileName, sizeof(g_szLogFileName), "%s_%s", g_szLogFileNameOld, pLogFileName);

    if (NULL != pMsgId)
        strncpy(g_szMsgId, pMsgId, sizeof(g_szMsgId)-1);
}

// 重置日志名称
void ReSetLogFileName()
{
    strncpy(g_szLogFileName, g_szLogFileNameOld, sizeof(g_szLogFileName)-1);
    memset(g_szMsgId, 0, sizeof(g_szMsgId));
}

// 设置错误日志
void SetErrLogFile(const char *pErrLogFile)
{
    strncpy(g_szErrLogFile, pErrLogFile, sizeof(g_szErrLogFile)-1);
}

// 日志打印函数
void _Trace(const char* pFileName, int iFileLine, LOG_LEVEL level, const char* szErrcode, const char* format...)
{
    char szDate[8+1]     = {0}; // 当前日期
    char szTime[15+1]     = {0}; // 当前时间
    char szErrType[8+1]   = {0}; // 错误类型
    char szPath[128+1]    = {0}; // 日志文件路径
    char szFile[256+1]    = {0}; // 日志名称
    char szErrFile[256+1] = {0}; // 错误记录日志
    FILE* logFile;

    if (level > g_iLogLevel)
    {
        return;
    }

    // 设置默认日志大小(默认100M)
    if (g_iMaxFileSize == 0)
    {
        g_iMaxFileSize = 1024*1024*100;
    }

    // 设置错误类型和默认错误码
    switch(level)
    {
    case L_SYS:
        szErrcode = szErrcode==NULL?"SYS":szErrcode;
        sprintf(szErrType, "%7s", "eSys");
        break;
    case L_IMPORTANT:
        szErrcode = szErrcode==NULL?"IMP":szErrcode;
        sprintf(szErrType, "%7s", "eImpt");
        break;
    case L_ERROR:
        szErrcode = szErrcode==NULL?"ERR":szErrcode;
        sprintf(szErrType, "%7s", "eError");
        break;
    case L_INFO:
        szErrcode = szErrcode==NULL?"INF":szErrcode;
        sprintf(szErrType, "%7s", "eInfo");
        break;
    case L_DEBUG:
        szErrcode = szErrcode==NULL?"DBG":szErrcode;
        sprintf(szErrType, "%7s", "eDebug");
        break;
    default:
        szErrcode = szErrcode==NULL?"UKW":szErrcode;
        sprintf(szErrType, "%7s", "eUnknow");
        break;
    }

    //取当前系统日期，如20090909
    GetLOGTimePath(szDate, szTime);
    //合成路径
    snprintf(szPath   , sizeof(szPath), "%s/%s", g_szLogPath, szDate);         // 日志文件目录
    snprintf(szFile   , sizeof(szFile), "%s/%s.log", szPath, g_szLogFileName); // 日志文件名
    snprintf(szErrFile, sizeof(szFile), "%s/%s.log", szPath, g_szErrLogFile);  // 错误日志文件名
    // 创建日志文件目录
    bool bIsSucc = CreatePathDir(szPath);
    if ( !bIsSucc)
    {
        printf("create [%s] error[%s]", szPath, strerror(errno));
        return ;
    }

    // 初始化日志缓存
    memset(g_szTmpG, 0, LOG_BUF);
    va_list argList;
    va_start(argList, format);
    vsnprintf(g_szTmpG, LOG_BUF, format, argList);
    va_end(argList);

    // 超过每个日志文件最大容量
    if (g_iMaxFileSize > 0)
    {
        struct stat stbuf;

        if ( 0 == stat(szFile, &stbuf) )
        {
            if (stbuf.st_size >= g_iMaxFileSize)
            {
                char szNewFile[256+1] = {0};
                snprintf(szNewFile, sizeof(szNewFile), "%s/%s.%s.log", szPath, g_szLogFileName, szTime);
                rename(szFile, szNewFile);
            }
        }
    }

    if((logFile = fopen(szFile, "a+")) != NULL)
    {
        // 文件名过长,只保留前30位
        char szFileName[30+1] = {0};
        int iFileLen = strlen(pFileName);
        if (iFileLen > 30)
        {
            memcpy(szFileName, pFileName+iFileLen-30, 30);
        }
        else
        {
            strncpy(szFileName, pFileName, sizeof(szFileName)-1);
        }

        fprintf(logFile, "[%07d|%11d|%15s|%30s|%06d|%7s|%8s|%s]: %s\n", getpid(), (int)pthread_self(), szTime, szFileName, iFileLine, szErrType, szErrcode, g_szMsgId, g_szTmpG);
        fclose(logFile);

        if (L_ERROR == level)
        {
            if (NULL == szErrcode)
                return;

            // 错误都记录
            if (strlen(g_szErrLogFile) > 0)
            {
                // 超过每个日志文件最大容量
                if (g_iMaxFileSize > 0)
                {
                    struct stat stbuf;

                    if ( 0 == stat(szErrFile, &stbuf) )
                    {
                        if (stbuf.st_size >= g_iMaxFileSize)
                        {
                            char szNewFile[256+1] = {0};
                            snprintf(szNewFile, sizeof(szNewFile), "%s/%s.%s.log", szPath, g_szErrLogFile, szTime);
                            rename(g_szErrLogFile, szNewFile);
                        }
                    }
                }
                FILE *pError = fopen(szErrFile, "a+");
                if (NULL != pError)
                {
                    fprintf(pError, "[%07d|%11d|%8s_%15s|%30.30s|%06d|%7s|%8s|%s]: %s\n", getpid(), (int)pthread_self(), szDate, szTime, szFileName, iFileLine, szErrType, szErrcode, g_szMsgId, g_szTmpG);
                    fclose(pError);
                }
            }
        }
    }
    else
    {
        printf("Write log error:[%s|%s]:%s\n", szTime, szFile, strerror(errno));
        return ;
    }
}

