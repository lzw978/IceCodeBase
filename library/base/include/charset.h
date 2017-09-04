/********************************************
 **  模块名称：charset.h
 **  模块功能: 字符串编码转换
 **  模块简述:
 **  编 写 人:
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_CHARSET_H__
#define __LIB_CHARSET_H__

#include <string>
#include <stdio.h>
#include "mutex.h"

namespace commbase
{
    using namespace std;

    // 编码转换类
    class IConv
    {
    public:
        IConv(string fromCode, string toCode, bool bErrorContinue=true);
        virtual ~IConv();

        bool isValid();
        virtual string tr(const string& inStr);
        // 全局安全的转换函数
        static string trTS(const string& inStr);
        // 安装转换句柄
        static void setupGlobalTranslator(IConv* pTranslator, bool bIHaveTheLocker=false);
    private:
        IConv(const IConv&other);
        IConv& operator=(const IConv&other);

        struct privateHandle_t;
        privateHandle_t *mHandle;

        //全局共用的转换对象
        static IConv* gTranslator;
        static Mutex  gTranslatorLock;
    };

    // 用于释放全局的编码转换对象
    class TranslatorAutoRelease
    {
    public:
        TranslatorAutoRelease() : m_pPointer(NULL)
        {
        }
        ~TranslatorAutoRelease()
        {
            if (m_pPointer != NULL)
            {
                delete m_pPointer;
                m_pPointer = NULL;
            }
        }
        IConv* m_pPointer;
    };

    // 什么也不做的转换类
    class NullCharsetTranslator : public IConv
    {
    public:
        NullCharsetTranslator() : IConv("", "", true){}

        inline string tr(const string& inStr)
        { return inStr; }
    };

    typedef unsigned long  UTF32; // at least 32 bits
    typedef unsigned short UTF16; // at least 16 bits
    typedef unsigned char  UTF8;  // typically 8 bits

    typedef enum
    {
        conversionOK    = 0, // conversion successful
        sourceExhausted = 1, // partial character in source, but hit end
        targetExhausted = 2, // insuff. room in target for conversion
        sourceIllegal   = 3  // source sequence is illegal/malformed
    } ConvtRet_t;

    typedef enum
    {
        strictConversion  = 0,
        lenientConversion = 1
    } ConvtFlags_t;

    // 获取系统默认编码
    static string getDefaultCharset();
    // GBK转UTF8
    std::string gbk2utf8( const string& inStr );
    // UTF8转GBK
    std::string utf82gbk( const string& inStr );
    //  utf-8转换成wstring
    std::wstring utf82wstring(const std::string& src);
    // wstring转换成utf-8
    std::string wstring2utf8(const std::wstring& src);

    //! 编码转换宏
    #define _G2A(x) commbase::IConv::trTS(x).c_str()
    #define _G2U(x) gbk2utf8(x).c_str()
    #define _U2G(x) utf82gbk(x).c_str()
}


#endif