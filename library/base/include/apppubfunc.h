/********************************************
 **  模块名称：apppubfunc.h
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

#ifndef __LIB_APPPUB_FUNC_H__
#define __LIB_APPPUB_FUNC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <sys/time.h>

using namespace std;
enum TimeFormat
{
	eOnlyDateISO = 1,   // yyyy-mm-dd
	eOnlyDate8   = 2,   // yyyymmdd
	eOnlyTimeISO = 11,  // HH:MI:SS
	eOnlyTime6   = 12,  // HHMISS
	eOnlyTime12  = 13,  // HH:MI:SS.MSS
	eOnlyTime9   = 14,  // HHMISSMSS
	eDateTimeISO = 21,  // yyyy-mm-ddTHH:MI:SS
	eDateTime19  = 22,  // yyyy-mm-dd HH:MI:SS
	eDateTime14  = 23   // yyyymmddHHMISS
};

// 获取当前时间（取值格式参见TimeFormat枚举)
char* GetCurrTime(int iFormat);

// 字符串去空格
char* Trim(char* pszStr);
string Trim(string& strStr);

#endif


