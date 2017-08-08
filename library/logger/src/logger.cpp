/********************************************************************
�ļ�����logger.cpp
�����ˣ�lzw978
��  �ڣ�2017-08-08
�޸��ˣ�
��  �ڣ�
��  ������¼��־
��  ����
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

#define LOG_BUF 1024*1024*3   // ��־��������С,Ĭ��3M

int  g_iMaxFileSize = 0;      // ��־�������
char g_szLogPath[100];        // ��־·��
char g_szLogFileName[35];     // ��־����
char g_szLogFileNameOld[35];  // ����־����
int  g_iLogLevel;             // ��־�ȼ�
char g_szMsgId[64+1];         // ��ϢIDֵ
char g_szErrLogFile[300];     // ������־
char *g_szTmpG;               // ��ӡ��Ϣ


// ��ȡ��ǰϵͳʱ��
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

// ��ȡ��־ʱ��·��
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
    sprintf(lpBuf, "%04d%02d%02d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
    sprintf(lpTime, "%02d:%02d:%02d:%06d", p->tm_hour, p->tm_min, p->tm_sec, (int)tp.tv_usec);

    //2017-01-02 11:22:33.123
    return lpBuf;
}

// �ж���־�Ƿ��Ѿ�����
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

// ������־��С
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

// �ж���־�Ƿ�Ϊutf8����
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

// ������־·��
bool CreatePathDir(const char* lpPathName)
{
    //ȷ�����������ȷ
    if(NULL == lpPathName || 0 == strlen(lpPathName))
    {
        return false;
    }

    char    szPath[1024]    = { 0 };  //·����ʱ������
    char    cSeparator              = '/';
    int     nLen;

    //Ŀ¼���в����ڳ��ֵ��ַ�
    strncpy(szPath, lpPathName, sizeof(szPath) - 1);
    nLen = strlen(szPath);

    for (int i = 0; i <= nLen; i++)
    {
        if (((szPath[i] == '\\') || (szPath[i] == '/') || (szPath[i] == '\0')) && (i > 0))
        {
            cSeparator = szPath[i];

            szPath[i] = 0;    //�����ַ���������

            //�ڴ����༶Ŀ¼ʱ�����Ŀ¼�Ѵ��ڣ�����Ϊ�Ǵ���
            //����������һ��Ŀ¼���������Ŀ¼ʧ�ܣ���ʧ��ԭ��
            //������Ŀ¼�Ѵ��ڣ�����ֹ��������������FALSE��
            mode_t mMode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

            if ((mkdir(szPath, mMode) == -1) && (errno != EEXIST))
            {
                return false;
            }
            szPath[i] = cSeparator;  //�ָ��ַ���
        }
    }

    return true;
}

// ������־��С
void SetLogMaxSize(int iLogMaxSize)
{
    g_iMaxFileSize = iLogMaxSize;
}

// ������־�ȼ�
void SetLogLevel(int iLogLevel)
{
    g_iLogLevel = iLogLevel;
}

// ������־·��
void SetLogPath(const char *pLogPath)
{
    strcpy(g_szLogPath, pLogPath);
    g_szTmpG = new char[LOG_BUF];
}

// ������־����
void SetLogFileName(const char *pLogFileName)
{
    strcpy(g_szLogFileName, pLogFileName);
    strcpy(g_szLogFileNameOld, pLogFileName);
    memset(g_szMsgId, 0, sizeof(g_szMsgId));
}

// ������־������
void SetNewLogFileName(char *pLogFileName, char *pMsgId)
{
    memset(g_szMsgId, 0, sizeof(g_szMsgId));

    strcpy(g_szLogFileName, g_szLogFileNameOld);
    strcat(g_szLogFileName, "_");
    strcat(g_szLogFileName, pLogFileName);

    if (NULL != pMsgId)
        strcpy(g_szMsgId, pMsgId);
}

// ������־����
void ReSetLogFileName()
{
    strcpy(g_szLogFileName, g_szLogFileNameOld);
    memset(g_szMsgId, 0, sizeof(g_szMsgId));
}

// ���ô�����־
void SetErrLogFile(const char *pErrLogFile)
{
    strcpy(g_szErrLogFile, pErrLogFile);
}

// ��־��ӡ����
void _Trace(const char* pFileName, int iFileLine, LOG_LEVEL level, const char* errcode, const char* format...)
{
    string sPath         = "";
    char szTime[32]      = { 0 };
    char szDate[32]      = { 0 };
    char szErrType[32]   = { 0 };
    char szYear[4 + 1]   = { 0 };
    char szMonth[2 + 1]  = { 0 };
    char szToday[2 + 1]  = { 0 };
    string strFile       = "";
    FILE* logFile;

    if(level > g_iLogLevel)
    {
        return;
    }

    // ��ʼ����־����
    memset(g_szTmpG, 0, LOG_BUF);

    va_list argList;
    va_start(argList, format);
    vsnprintf(g_szTmpG, LOG_BUF, format, argList);
    va_end(argList);

    //ȡ��ǰϵͳ���ڣ���20090909
    GetLOGTimePath(szDate, szTime);
    memcpy(szYear, szDate, sizeof(szYear) - 1);
    memcpy(szMonth, szDate + 4, sizeof(szMonth) - 1);
    memcpy(szToday, szDate + 6, sizeof(szToday) - 1);
    szTime[23] = '\0';

    //�ϳ�·��
    strFile  = g_szLogPath;
    strFile += "/";
    strFile += szYear;
    strFile += szMonth;
    strFile += szToday;

    //��־�ļ�·��
    sPath = strFile;
    strFile += "/";
    strFile += g_szLogFileName;
    strFile += ".log";

    switch(level)
    {
    case L_SYS:
        sprintf(szErrType, "%7s", "eSys");
        break;
    case L_IMPORTANT:
        sprintf(szErrType, "%7s", "eImpt");
        break;
    case L_ERROR:
        sprintf(szErrType, "%7s", "eError");
        break;
    case L_INFO:
        sprintf(szErrType, "%7s", "eInfo");
        break;
    case L_DEBUG:
        sprintf(szErrType, "%7s", "eDebug");
        break;
    default:
        sprintf(szErrType, "%7s", "eUnknow");
    }

    CreatePathDir(sPath.c_str());

    // ����ÿ����־�ļ��������
    if (g_iMaxFileSize > 0)
    {
        struct stat stbuf;

        if ( 0 == stat(strFile.c_str(), &stbuf) )
        {
            if (stbuf.st_size >= g_iMaxFileSize)
            {
                string strNewFile = "";
                //�ϳ�·��
                strNewFile  = g_szLogPath;
                strNewFile += "/";
                strNewFile += szYear;
                strNewFile += szMonth;
                strNewFile += szToday;
                strNewFile += "/";
                strNewFile += g_szLogFileName;
                strNewFile += ".";
                strNewFile += szTime;
                strNewFile += ".log";

                rename(strFile.c_str(), strNewFile.c_str());
            }
        }
    }

    if((logFile = fopen(strFile.c_str(), "a+")) != NULL)
    {
        char szFileName[30+1] = {0};
        int iFileLen = strlen(pFileName);
        if (iFileLen > 30)
        {
            memcpy(szFileName, pFileName+iFileLen-30, 30);
        }
        else
        {
            strcpy(szFileName, pFileName);
        }

        fprintf(logFile, "[%07d|%11d|%s|%s|%30s|%06d|%7s]%s\n", getpid(), (int)pthread_self(), szTime, g_szMsgId, szFileName, iFileLine, szErrType, g_szTmpG);

        fclose(logFile);

        if (L_ERROR == level)
        {
            if (NULL == errcode)
                return;

            // ���Զ���¼
            if (strlen(g_szErrLogFile) > 0)
            {
                FILE *pError = fopen(g_szErrLogFile, "a+");
                if (NULL != pError)
                {
                    fprintf(pError, "[%07d|%11d|%s%s|%s|%30.30s|%06d|%7s]%s\n", getpid(), (int)pthread_self(), szDate,szTime, g_szMsgId, szFileName, iFileLine, szErrType, g_szTmpG);

                    fclose(pError);
                }
            }
        }
    }
    else
    {
        printf("Write log error:[%s|%s]%s\n", szTime, strFile.c_str(), strerror(errno));
    }
}

