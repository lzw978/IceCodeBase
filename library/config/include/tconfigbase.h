/********************************************
 **  模块名称：tconfigbase
 **  模块功能:
 **  模块简述: 配置文件基类
 **  编 写 人: lzw978
 **  日    期: 2017.08.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_TCONFIGBASE_H__
#define __LIB_TCONFIGBASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <map>

using namespace std;

typedef map<string, string> MAP_KEYVALS;
typedef map<string, MAP_KEYVALS> MAP_SECTIONS;

#define MAX_CONFIG_LEN  1024*32
#define DEFAULT_SECTION "XXXXXXXXXXXXXX"

class TConfigBase
{
public:
    TConfigBase();
    virtual ~TConfigBase();

    // 加载配置文件(ini格式)
    int LoadIniFile(string sPathName);
    // 加载配置文件(xml格式)
    //int LoadXmlFile(string sPathName);

    // 根据tag值获取参数值
    string GetTagValue(string strSection, string strKey, string strDef = "");
    int GetTagValue(string strSection, string strKey, int iDef = 0);

    // 根据tag值设置参数值
    string SetTagValue(string strSection, string strKey, string strDef = "");
    int SetTagValue(string strSection, string strKey, int iDef = 0);
protected:
    // 参数值Map
    MAP_SECTIONS m_mapSections;
};

#endif