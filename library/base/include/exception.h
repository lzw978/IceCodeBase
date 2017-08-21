/********************************************
 **  模块名称：exception.h
 **  模块功能:
 **  模块简述: 异常类
 **  编 写 人:
 **  日    期: 2017.08.21
 **  修 改 人: lzw978
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_EXCEPTION_H__
#define __LIB_EXCEPTION_H__

#include <string>
#include "sourceInfo.h"

namespace ips
{

using namespace std;

class Exception
{
public:
    // 构造函数
    Exception();
    Exception(const char* szFile, int iLine, const char* szFunc, int iCode, const string& strErrMsg);
    Exception(SourceInfo sourceInfo, int iCode, const string& strErrMsg);
    Exception(SourceInfo sourceInfo, const string& strCode, const string& strErrMsg);
    // 析构函数
    virtual ~Exception();
    // 拷贝构造函数
    Exception(const Exception& e);
    // 赋值运算符
    Exception& operator=(const Exception& e);
    // 返回出错文件名
    const char* file() const;
    // 返回出错文件行数
    int line() const;
    // 返回出错函数
    const char* func() const;
    // 返回错误码
    int code() const;
    // 返回错误码
    const char* codes() const;
    // 返回错误信息
    const char* what() const;
    // 默认异常处理函数
    static void defaultExceptionHandle(Exception& ex);

    // 设置错误码
    void setCode(int iCode);
    // 设置错误信息
    void setWhat(const string& strMsg);
    // 设置源码信息
    void setSourceInfo(SourceInfo sourceInfo);
protected:
    string m_strErrMsg;
    string m_strCode;
    int m_iErrCode;
    SourceInfo m_cSourceInfo;
};

// 写错误码文件
void fatalError(const char* fmt...);

}

#endif // !__LIB_EXCEPTION_H__
