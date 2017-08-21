/********************************************
 **  模块名称：exception.cpp
 **  模块功能:
 **  模块简述: 异常类实现
 **  编 写 人:
 **  日    期: 2017.08.21
 **  修 改 人: lzw978
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/
#include <stdio.h>
#include <stdarg.h>
#include "exception.h"
#include "datetime.h"

namespace ips
{

// 构造函数
Exception::Exception()
{
    m_strErrMsg = "";
    m_iErrCode  = -1;
}
Exception::Exception(const char* szFile, int iLine, const char* szFunc, int iCode, const string& strErrMsg)
{
    szFile = szFile!=NULL?szFile:"";
    szFunc = szFunc!=NULL?szFunc:"";

    m_iErrCode    = iCode;
    m_strErrMsg   = strErrMsg;
    m_cSourceInfo = SourceInfo(szFile, iLine, szFunc);
}
Exception::Exception(SourceInfo sourceInfo, int iCode, const string& strErrMsg)
{
    m_iErrCode    = iCode;
    m_strErrMsg   = strErrMsg;
    m_cSourceInfo = SourceInfo(sourceInfo);
}
Exception::Exception(SourceInfo sourceInfo, const string& strCode, const string& strErrMsg)
{
    m_iErrCode    = -1;
    m_strCode     = strCode;
    m_strErrMsg   = strErrMsg;
    m_cSourceInfo = SourceInfo(sourceInfo);
}
// 拷贝构造函数
Exception::Exception(const Exception& e)
{
    m_iErrCode    = e.m_iErrCode;
    m_strCode     = e.m_strCode;
    m_strErrMsg   = e.m_strErrMsg;
    m_cSourceInfo = e.m_cSourceInfo;
}
// 赋值运算符
Exception& Exception::operator=(const Exception& e)
{
    if( &e != this)
    {
        m_iErrCode    = e.m_iErrCode;
        m_strCode     = e.m_strCode;
        m_strErrMsg   = e.m_strErrMsg;
        m_cSourceInfo = e.m_cSourceInfo;
    }
    return *this;
}
// 析构函数
Exception::~Exception()
{
}
// 默认异常处理方法
void Exception::defaultExceptionHandle(Exception& e)
{
    fatalError("Exception catched: file=[%s] line=[%d] function=[%s] code=[%d][%s] msg=[%s]\n",
               e.file(), e.line(), e.func(), e.code(), e.codes(), e.what());
}

// 返回文件
const char* Exception::file() const
{
    return m_cSourceInfo.file();
}
// 返回行数
int Exception::line() const
{
    return m_cSourceInfo.line();
}
// 返回错误码
int Exception::code() const
{
    return m_iErrCode;
}
const char* Exception::codes() const
{
    return m_strCode.c_str();
}
// 返回函数
const char* Exception::func() const
{
    return m_cSourceInfo.func();
}
// 返回错误信息
const char* Exception::what() const
{
    return m_strErrMsg.c_str();
}
// 设置错误码
void Exception::setCode(int iCode)
{
    m_iErrCode = iCode;
}
// 设置错误信息
void Exception::setWhat(const string& strMsg)
{
    m_strErrMsg = strMsg;
}
// 设置源码信息
void Exception::setSourceInfo(SourceInfo sourceInfo)
{
    m_cSourceInfo = sourceInfo;
}

// 写错误码文件
void fatalError(const char* fmt ...)
{
    char buffer[1024+1] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    buffer[sizeof(buffer)-1] = 0;

    FILE *fp = fopen("ErrorMsg.log", "ab");
    if( fp)
    {
        Datetime now = Datetime::now();
        fprintf(fp, "%s %s", now.toCStr(), buffer);
        fprintf(stderr, "%s %s", now.toCStr(), buffer);
        fclose(fp);
    }
    va_end(args);
}

}