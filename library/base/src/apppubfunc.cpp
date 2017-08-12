/********************************************
 **  模块名称：apppubfunc.cpp
 **  模块功能:
 **  模块简述: 公共函数实现类
 **  编 写 人: lzw978
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include "apppubfunc.h"

// 获取当前时间
char* GetCurrTime(int iFormat)
{
    static char szCurrTime[32] = {0};
    struct timeval tp;
    struct tm *p;
    time_t timep;

    time(&timep);
    p = localtime(&timep);
    gettimeofday(&tp, NULL);

    switch ((TimeFormat)iFormat)
    {
        // yyyy-mm-dd
        case eOnlyDateISO:
            sprintf(szCurrTime, "%04d-%02d-%02d", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday);
            break;
        // yyyymmdd
        case eOnlyDate8:
            sprintf(szCurrTime, "%04d%02d%02d", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday);
            break;
        // HH:MI:SS
        case eOnlyTimeISO:
            sprintf(szCurrTime, "%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec);
            break;
        // HHMISS
        case eOnlyTime6:
            sprintf(szCurrTime, "%02d%02d%02d", p->tm_hour, p->tm_min, p->tm_sec);
            break;
        // HH:MI:SS.MSS
        case eOnlyTime12:
            sprintf(szCurrTime, "%02d:%02d:%02d.%03d", p->tm_hour, p->tm_min, p->tm_sec, (int)tp.tv_usec);
            szCurrTime[12] = 0;
            break;
        // HHMISSMSS
        case eOnlyTime9:
            sprintf(szCurrTime, "%02d%02d%02d%03d", p->tm_hour, p->tm_min, p->tm_sec, (int)tp.tv_usec);
            szCurrTime[9] = 0;
            break;
        // yyyy-mm-ddTHH:MI:SS
        case eDateTimeISO:
            sprintf(szCurrTime, "%04d-%02d-%02dT%02d:%02d:%02d", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
            break;
        // yyyy-mm-dd HH:MI:SS
        case eDateTime19:
            sprintf(szCurrTime, "%04d-%02d-%02d %02d:%02d:%02d", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
            break;
        // yyyymmddHHMISS
        case eDateTime14:
            sprintf(szCurrTime, "%04d%02d%02d%02d%02d%02d", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
            break;
        default:
            strcpy(szCurrTime, "");
            break;
    }

    return szCurrTime;
}

// 去除当前空格
char* Trim(char* pszStr)
{
    if( NULL == pszStr)
    {
        return NULL;
    }
    if( 0 == pszStr[0])
    {
        return pszStr;
    }
    // 去掉字符串头部的空格、水平制表符、回车换行符
    char* pszTmp = pszStr;
    while((*pszTmp == ' ') || (*pszTmp == '\t') || (*pszTmp == '\r') || (*pszTmp == '\n'))
    {
        pszTmp++;
    }
    // 去掉字符串尾部的空格、水平制表符、回车换行符
    unsigned int nLen = strlen(pszTmp);
    while( (nLen>0) && ((pszTmp[nLen-1] == ' ') || (pszTmp[nLen-1] == '\t')|| (pszTmp[nLen-1] == '\r') || (pszTmp[nLen-1] == '\n')))
    {
        nLen--;
    }
    pszTmp[nLen] = '\0';
    memmove(pszStr, pszTmp, nLen+1);

    return pszStr;
}
string Trim(string& strStr)
{
    if( strStr.length() <= 0 )
    {
        return strStr;
    }
    // 去掉字符串头部的空格、水平制表符、回车换行符
    string::size_type nBeginPos = 0;
    const char* lpTmp = strStr.c_str();
    while((*lpTmp == ' ') || (*lpTmp == '\t') || (*lpTmp == '\r') || (*lpTmp == '\n'))
    {
        nBeginPos++;
        lpTmp++;
    }
    // 去掉字符串尾部的空格、水平制表符、回车换行符
    string::size_type nLen = strlen(lpTmp);
    while((nLen>0) &&((lpTmp[nLen-1] == ' ') || (lpTmp[nLen-1] == '\t') || (lpTmp[nLen-1] == '\r') || (lpTmp[nLen-1] == '\n')))
    {
        nLen--;
    }
    strStr.erase(0, nBeginPos);
    strStr.erase(nLen);

    return strStr;
}
