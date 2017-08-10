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