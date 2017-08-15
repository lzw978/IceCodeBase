/********************************************
 **  模块名称：sourceInfo.h
 **  模块功能:
 **  模块简述: 代码所在文件、函数、行数信息，用于日志和异常类
 **  编 写 人:
 **  日    期: 2017.08.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_SOURCEINFO_H__
#define __LIB_SOURCEINFO_H__

//GNU C++编译器
#ifdef __GNUC__
#define Z_PRETTY_FUNCTION __PRETTY_FUNCTION__
//Microsoft C++编译器
#elif defined(_MSC_VER_)
#if _MSC_VER_ >= 1300
#define Z_PRETTY_FUNCTION __FUNCDNAME__
#else
#define Z_PRETTY_FUNCTION __FUNCTION__
#endif
// 其他
#else
#define Z_PRETTY_FUNCTION __FUNCTION__
#endif

#define Z_SOURCEINFO SourceInfo(__FILE__, __LINE__, __FUNCTION__)

class SourceInfo
{
public:
    SourceInfo() throw();
    SourceInfo(const SourceInfo& si) throw();
    SourceInfo(const char* _file, unsigned int _line, const char*_func) throw();
    const char* file() const throw();
    unsigned int line() const throw();
    const char* func() const throw();

    SourceInfo& operator=(const SourceInfo& si);
private:
    char m_szFile[512+1];  // 源文件
    char m_szFunc[256+1];  // 函数
    unsigned int m_iLine;  // 行数
};

#endif // !__LIB_SOURCEINFO_H__
