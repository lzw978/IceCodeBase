/********************************************
 **  模块名称：charset.cpp
 **  模块功能: 字符串编码转换实现
 **  模块简述:
 **  编 写 人:
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <errno.h>
#include <iconv.h>
#include "charset.h"
#include "logger.h"
#include "strHelper.h"
#include "utf8/checked.h"
#include "utf8/unchecked.h"

using namespace commbase;

static const iconv_t invalid = reinterpret_cast<iconv_t>(-1);
struct IConv::privateHandle_t
{
    iconv_t cd;
    bool bOpened;

    string fromCode;
    string toCode;
};

// 全局对象
IConv* IConv::gTranslator = NULL;
Mutex  IConv::gTranslatorLock;


IConv::IConv(string fromCode, string toCode, bool) : mHandle(new privateHandle_t)
{
    mHandle->cd = iconv_open(toCode.c_str(), fromCode.c_str());
    if (mHandle->cd == invalid)
        return;

    mHandle->toCode = toCode;
    mHandle->fromCode = fromCode;
    mHandle->bOpened = true;
}
IConv::~IConv()
{
    if (mHandle->cd != invalid)
        iconv_close(mHandle->cd);
    delete mHandle;
}

bool IConv::isValid()
{
    return mHandle->bOpened;
}

string IConv::tr(const string& inStr)
{
    string toString;
    if (isValid())
    {
#ifdef Z_HAVE_GNU_LIBICONV
        // GNU libiconv
        const char *inp = reinterpret_cast<const char *>(inStr.data());
#else
        // POSIX compliant iconv(3)
        char *inp = reinterpret_cast<char *>(const_cast<string::value_type *>(inStr.data()));
#endif
        const string::size_type in_size = inStr.size() * sizeof(string::value_type);
        string::size_type in_bytes = in_size;

        toString.resize(in_size);

        char *outp = reinterpret_cast<char *>(const_cast<string::value_type *>(toString.data()));
        string::size_type out_size = toString.size() * sizeof(string::value_type);
        string::size_type out_bytes = out_size;

        do
        {
#ifdef Z_HAVE_GNU_LIBICONV
            size_t l = iconv(mHandle->cd, (char**)&inp, &in_bytes, (char**)&outp, &out_bytes);
#else
            size_t l = iconv(mHandle->cd, &inp, &in_bytes, &outp, &out_bytes);
#endif
            if (l == (size_t) -1)
            {
                switch (errno)
                {
                case EILSEQ:
                case EINVAL:
                    {
                        const string::size_type off = in_size - in_bytes + 1;
#ifdef Z_HAVE_GNU_LIBICONV
                        // GNU libiconv
                        inp = reinterpret_cast<const char *>(inStr.data()) + off;
#else
                        // POSIX compliant iconv(3)
                        inp = reinterpret_cast<char *>(const_cast<string::value_type *>(inStr.data()));
#endif
                        in_bytes = in_size - off;
                        break;
                    }
                case E2BIG:
                    {
                        const string::size_type off = out_size - out_bytes;
                        toString.resize(toString.size() * 2);
                        out_size = toString.size() * sizeof(string::value_type);

                        outp = reinterpret_cast<char *>(const_cast<string::value_type *>(toString.data())) + off;
                        out_bytes = out_size - off;
                        break;
                    }
                default:
                    toString = string();
                    return toString;
                }
            }
        } while (in_bytes != 0);

    }

    if (toString.length()!=strlen(toString.c_str()))
        toString = toString.c_str();

    return toString;
}

// 全局线程安全的编码转换函数
string IConv::trTS(const string& inStr)
{
    Mutex::ScopedLock lock(IConv::gTranslatorLock);
    IConv*pTranslator = IConv::gTranslator;
    if (pTranslator==NULL)
    {
#ifndef WIN32
        //自适应当前环境变量
        string defCharset = getDefaultCharset();
        if (defCharset=="GBK")
            IConv::setupGlobalTranslator(new NullCharsetTranslator(), true);
        else
            IConv::setupGlobalTranslator(new IConv("GBK", defCharset.c_str()), true);
#else
        IConv::setupGlobalTranslator(new NullCharsetTranslator());
#endif
        pTranslator = IConv::gTranslator;
    }

    return pTranslator->tr(inStr);
}

// 安装转换句柄
void IConv::setupGlobalTranslator(IConv*pNewTranslator, bool bIHaveTheLocker)
{
    static TranslatorAutoRelease autoFree;

    // 取锁
    if (!bIHaveTheLocker)
        IConv::gTranslatorLock.lock();

    // 进程退出时自动释放
    if (autoFree.m_pPointer==NULL)
        autoFree.m_pPointer = pNewTranslator;

    // 如果要安装的对象已经存在
    if (pNewTranslator==NULL || pNewTranslator==IConv::gTranslator)
    {
        if (!bIHaveTheLocker)
            IConv::gTranslatorLock.unlock();
        return ;
    }

    // 删除以前的对象, 更新全局变量
    delete IConv::gTranslator;
    IConv::gTranslator = pNewTranslator;

    if (autoFree.m_pPointer!=pNewTranslator)
        autoFree.m_pPointer = pNewTranslator;

    //解锁
    if (!bIHaveTheLocker)
        IConv::gTranslatorLock.unlock();
}

// 获取默认编码
string commbase::getDefaultCharset()
{
    string LC_CTYPE_;

    const char*pCharset = getenv("LC_CTYPE");
    pCharset = getenv ("LC_ALL");
    if (pCharset == NULL || pCharset[0] == '\0')
    {
        pCharset = getenv ("LC_CTYPE");
        if (pCharset == NULL || pCharset[0] == '\0')
            pCharset = getenv ("LANG");
    }

    pCharset = pCharset==NULL ? "": pCharset;
    LC_CTYPE_ = pCharset;

    if (LC_CTYPE_=="")
        LC_CTYPE_ = "GBK";

    LC_CTYPE_ = StrHelper::toLower(LC_CTYPE_);

    if (LC_CTYPE_.find("utf8")!=string::npos||LC_CTYPE_.find("utf-8")!=string::npos)
        LC_CTYPE_ = "UTF-8";
    else
        LC_CTYPE_ = "GBK";

    return LC_CTYPE_;
}

//! GBK转UTF8
std::string commbase::gbk2utf8( const string& inStr )
{
    IConv conv("GBK", "UTF-8");
    return conv.tr(inStr);
}

//! UTF8转GBK
std::string commbase::utf82gbk( const string& inStr )
{
    IConv conv("UTF-8", "GBK");
    return conv.tr(inStr);
}


std::wstring commbase::utf82wstring( const std::string& src )
{
    wstring wstr;
    if (sizeof(wchar_t)==2)
    {
        vector<unsigned short> utf16line;
        utf8::utf8to16(src.begin(), src.end(), back_inserter(utf16line));
        wstr.append(reinterpret_cast<wchar_t*>(&utf16line[0]), utf16line.size());
    }
    else if (sizeof(wchar_t)==4)
    {
        vector<unsigned int> utf32line;
        utf8::utf8to32(src.begin(), src.end(), back_inserter(utf32line));
        wstr.append(reinterpret_cast<wchar_t*>(&utf32line[0]), utf32line.size());
    }
    else
    {
        Trace(L_ERROR, NULL, "wchar_t not support on this platform!");
    }
    return wstr;
}


std::string commbase::wstring2utf8( const std::wstring& src )
{
    string ustr;
    vector<unsigned char> utf8line;
    if (sizeof(wchar_t)==2)
    {
        utf8::utf16to8(src.begin(), src.end(), back_inserter(utf8line));
    }
    else if (sizeof(wchar_t)==4)
    {
        utf8::utf32to8(src.begin(), src.end(), back_inserter(utf8line));
    }
    else
    {
        Trace(L_ERROR, NULL, "wchar_t not support on this platform!");
    }
    ustr.append(reinterpret_cast<char*>(&utf8line[0]), utf8line.size());
    return ustr;
}
