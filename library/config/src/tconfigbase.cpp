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
#include "apppubfunc.h"
#include "tinyxml.h"
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

// 加载配置文件(XML格式)
int TConfigBase::LoadXmlFile(string sPathName)
{
    FILE *fConfig = NULL;
    char szBuff[MAX_CONFIG_LEN] = {0};
    TiXmlDocument docParse;

    // 加载文件
    if(NULL == fConfig)
    {
        printf("Open file [%s] error\n", sPathName.c_str());
        return -1;
    }

    fread(szBuff, MAX_CONFIG_LEN, 1, fConfig);
    fclose(fConfig);

    // 解析文件
    docParse.Parse(szBuff, NULL, TIXML_DEFAULT_ENCODING);
    if( true == docParse.Error())
    {
        printf("Parse file failed.[%s]\n", docPrase.ErrorDesc());
        return -2;
    }

    // 根节点
    TiXmlElement *pRootElement = docParse.RootElement();
    if( NULL == pRootElement)
    {
        printf("Parse file failed.[%s]\n", docPrase.ErrorDesc());
        return -3;
    }

    MAP_KEYVALS mapKeyVals;
    string sAttributeName = "";
    string sSection = "";
    string sTag = "";
    string sVal = "";

    TiXmlElement* pSectionElement = pRootElement->FirstChildElement();
    while( NULL != pSectionElement)
    {
        mapKeyVals.clear();

        string sSectionValue = pSectionElement->Value();
        Trim(sSectionValue);

        if( TAG_SECTION != sSectionValue)
        {
            pSectionElement = pSectionElement->NextSiblingElement();
            continue;
        }

        // 解析部件
        for( TiXmlAttribute* a = pSectionElement->FirstAttribute(); a; a= a->Next())
        {
            sAttributeName = a->Name();
            Trim(sAttributeName);

            if( TAG_NAME == sAttributeName)
            {
                sSection = a->Value();
                if( "" != sSection)
                {
                    break;
                }
            }
        }

        if( "" == sSection)
        {
            pSectionElement = pSectionElement->NextSiblingElement();
            continue;
        }

        // 解析选项
        TiXmlElement* pOptionElement = pSectionElement->FirstChildElement();
        while( NULL != pOptionElement)
        {
            string sOption = pOptionElement->Value();
            Trim(sOption);
            if( TAG_OPTION != sOption && TAG_PID != sOption)
            {
                pOptionElement = pOptionElement->NextSiblingElement();
                continue;
            }

            // 进程关键字
            if( TAG_PID == sOption)
            {
                sSection = DEFAULT_SECTION;
                for( TiXmlAttribute* a = pOptionElement->FirstAttribute(); a; a->Next())
                {
                    sAttributeName = a->Name();
                    Trim(sAttributeName);

                    if( TAG_NAME == sAttributeName)
                    {
                        sSection = a->Value();
                        if( "" != sSection)
                        {
                            break;
                        }
                    }
                }

                mapKeyVals.clear();
                TiXmlElement* pTmpElement = pOptionElement->FirstChildElement();
                while( NULL != pTmpElement)
                {
                    string sOption = pTmpElement->Value();
                    Trim(sOption);

                    if( TAG_OPTION != sOption)
                    {
                        pTmpElement = pTmpElement->NextSiblingElement();
                        continue;
                    }

                    for(TiXmlAttribute* a = pTmpElement->FirstAttribute(); a; a=a->Next())
                    {
                        sAttributeName = a->Name();
                        Trim(sAttributeName);

                        if( TAG_NAME == sAttributeName)
                        {
                            sTag = a->Value();
                            if( "" != sTag)
                            {
                                break;
                            }
                        }
                    }

                    if( "" == sTag)
                    {
                        pTmpElement = pTmpElement->NextSiblingElement();
                        continue;
                    }

                    sVal = "";
                    if( NULL != pTmpElement->GetText())
                    {
                        sVal = pTmpElement->GetText();
                    }
                    Trim(sVal);

                    mapKeyVals.insert(make_pair<string, string>(sTag, sVal));
                    pTmpElement = pTmpElement->NextSiblingElement();
                }
                m_mapSections.insert(make_pair<string, MAP_KEYVALS>(sSection, mapKeyVals));
            }
            else
            {
                for( TiXmlAttribute* a = pOptionElement->FirstAttribute(); a; a = a->Next())
                {
                    sAttributeName = a->Name();
                    Trim(sAttributeName);

                    if( TAG_NAME == sAttributeName)
                    {
                        sTag = a->Value();
                        if( "" != sTag)
                        {
                            break;
                        }
                    }
                }

                if( "" == sTag)
                {
                    pOptionElement = pOptionElement->NextSiblingElement();
                    continue;
                }

                sVal = pOptionElement->GetText();
                Trim(sVal);
                mapKeyVals.insert(make_pair<string, string>(sTag, sVal));
            }
            pOptionElement = pOptionElement->NextSiblingElement();
        }
        m_mapSection.insert(make_pair<string, MAP_KEYVALS>(sSection, mapKeyVals));
        pSectionElement = pSectionElement->NextSiblingElement();
    }

    return 0;
}
