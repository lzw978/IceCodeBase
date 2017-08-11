/********************************************
 **  模块名称：tconfigbase.cpp
 **  模块功能:
 **  模块简述: 配置文件实现类
 **  编 写 人: lzw978
 **  日    期: 2017.08.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "tconfigbase.h"

TConfigBase::TConfigBase()
{
    m_mapSections.clear();
}

TConfigBase::~TConfigBase()
{
}

// 根据tag值获取参数值
string TConfigBase::GetTagValue(string strSection, string strKey, string strDef)
{
    // 配置项上级节点名为空，设置默认为“XXXXXXXXXXXXXX”节点
    if( "" == strSection)
    {
        strSection = DEFAULT_SECTION;
    }

    // 查询父级节点名
    MAP_SECTIONS::iterator itSection = m_mapSections.find(strSection);
    if( itSection == m_mapSections.end())
    {
        return strDef;
    }
    // 查询子级节点key，获取value值
    MAP_SECTIONS::iterator itKeyVal = itSection->second.find(strKey);
    if( itKeyVal == itSection->second.end())
    {
        return strDef;
    }

    return itKeyVal->second;
}
int TConfigBase::GetTagValue(string strSection, string strKey, int iDef)
{
    // 配置项上级节点名为空，设置默认为“XXXXXXXXXXXXXX”节点
    if( "" == strSection)
    {
        strSection = DEFAULT_SECTION;
    }

    // 查询父级节点名
    MAP_SECTIONS::iterator itSection = m_mapSections.find(strSection);
    if( itSection == m_mapSections.end())
    {
        return iDef;
    }
    // 查询子级节点key，获取value值
    MAP_SECTIONS::iterator itKeyVal = itSection->second.find(strKey);
    if( itKeyVal == itSection->second.end())
    {
        return iDef;
    }

    return atoi(itKeyVal->second.c_str());
}

// 根据tag值设置参数值
string TConfigBase::SetTagValue(string strSection, string strKey, string strDef)
{
    // 如果为空,设置默认值
    if( "" == strSection )
    {
        strSection = DEFAULT_SECTION;
    }

    // 根据父级节点名查询
    MAP_SECTIONS::iterator itSection = m_mapSections.find(strSection);
    if( itSection == m_mapSections.end())
    {
        return strDef;
    }
    // 根据子级节点名key设置value
    MAP_SECTIONS::iterator itKeyVal = itSection->second.find(strKey);
    if( itKeyVal == itSection->second.end())
    {
        itSection->second.insert(make_pair<string,string>(strKey, strDef));
    }
    else
    {
        ifKeyVal->second = strDef;
    }
    return strDef;
}
int TConfigBase::SetTagValue(string strSection, string strKey, int iDef)
{
    if( "" == strSection)
    {
        strSection = DEFAULT_SECTION;
    }
    // 根据父级节点名查询
    MAP_SECTIONS::iterator itSection = m_mapSections.find(strSection);
    if( itSection == m_mapSections.end())
    {
        return iDef;
    }
    // 根据子级节点名key设置value
    char szDef[10+1] = {0};
    sprintf(szDef, "%d", iDef);
    string strDef = szDef;

    MAP_SECTIONS::iterator itKeyVal = itSection->second.find(strKey);
    if( itKeyVal == itSection->second.end())
    {
        itSection->second.insert(make_pair<string,string>(strKey, strDef));
    }
    else
    {
        ifKeyVal->second = strDef;
    }
    return iDef;
}

// 加载配置文件(ini格式)
int TConfigBase::LoadIniFile(string sPathName)
{
    File *hFile = NULL;
    char szData[2048+1] = {0};
    char *pTmp = NULL;
    char szOldSection[100+1] = {0};
    char szNewSection[100+1] = {0};
    char szTag[100+1] = {0};
    char szVal[256+1] = {0};

    // 打开配置文件
    hFile = fopen(sPathName.c_str(), "rb");
    if(NULL == hFile)
    {
        printf("Open file [%s] error\n", sPathName.c_str());
        return -1;
    }

    MAP_KEYVALS mapKeyVals;
    while(1)
    {
        memset(szData, 0, sizeof(szData));
        if( NULL == fgets(szData, 1024, hFile))
        {
            break;
        }
        Trim(szData)
        // 过滤空行
        if( 0 == szData[0])
        {
            continue;
        }
        // 过滤注释
        if( '#' == szData[0] || ';' == szData[0])
        {
            continue;
        }
        // 解析节点
        if('[' == szData[0])
        {
            szData[strlen(szData)-1] = '\0';
            strcpy(szNewSection, szData+1);
            if( 0 == strlen(szOldSection))
            {
                strcpy(szOldSection, szNewSection);
                continue;
            }
            m_mapSections.insert(make_pair<string, MAP_KEYVALS>(szOldSection, mapKeyVals));
            strcpy(szOldSection, szNewSection);
            mapKeyVals.clear();
            continue;
        }

        // 解析选项
        pTmp = strstr(szData, "=");
        if( NULL != pTmp)
        {
            memset(szTag, 0, sizeof(szTag));
            memset(szVal, 0, sizeof(szVal));
            memcpy(szTag, szData, pTmp-szData);
            strcpy(szVal, pTmp+1);

            Trim(szTag);
            Trim(szVal);

            mapKeyVals.insert(make_pair<string, string>(szTag, szVal));
            continue;
        }
    }
    fclose(hFile);

    if( 0 != strlen(szOldSection))
    {
        m_mapSections.insert(make_pair<string, MAP_KEYVALS>(szOldSection, mapKeyVals));
    }
    else
    {
        m_mapSections.insert(make_pair<string, MAP_KEYVALS>(DEFAULT_SECTION, mapKeyVals));
    }
}