/********************************************
 **  模块名称：sourceInfo.cpp
 **  模块功能:
 **  模块简述: 代码所在文件、行数、函数信息，用于日志和异常类
 **  编 写 人:
 **  日    期: 2017.08.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <string.h>
#include <stdio.h>
#include "sourceInfo.h"

SourceInfo::SourceInfo() throw()
{
    m_iLine = 0;
    memset(m_szFunc, 0, sizeof(m_szFunc));
    memset(m_szFile, 0, sizeof(m_szFile));
}

SourceInfo::SourceInfo(const SourceInfo& si) throw()
{
    m_iLine = si.m_iLine;
    snprintf(m_szFile, sizeof(m_szFile), "%s", si.m_szFile);
    snprintf(m_szFunc, sizeof(m_szFunc), "%s", si.m_szFunc);
}

SourceInfo::SourceInfo(const char* _file, unsigned int _line, const char* _func) throw()
{
    _file = _file!=NULL?_file:"";
    _func = _func!=NULL?_func:"";

    m_iLine = _line;
    snprintf(m_szFile, sizeof(m_szFile), "%s", _file);
    snprintf(m_szFunc, sizeof(m_szFunc), "%s", _func);
}

SourceInfo& SourceInfo::operator=(const SourceInfo& si) 
{
    if( &si != this)
    {
        memcpy(m_szFile, si.m_szFile, sizeof(m_szFile));
        memcpy(m_szFunc, si.m_szFunc, sizeof(m_szFunc));
        m_iLine = si.m_iLine;
    }
    return *this;
}

// 返回文件名
const char* SourceInfo::file() const throw()
{
    return m_szFile;
}

// 返回函数名
const char* SourceInfo::func() const throw()
{
    return m_szFunc;
}
// 返回行数
unsigned int SourceInfo::line() const throw()
{
    return m_iLine;
}
