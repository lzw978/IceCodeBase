/********************************************
 **  模块名称：base64.h
 **  模块功能:
 **  模块简述: base64编码/解码
 **  编 写 人:
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/
#ifndef __LIB_BASE64_H__
#define __LIB_BASE64_H__

#include <string>
#include <sstream>

namespace commbase
{
    //! Base64编码解码
    class Base64
    {
    public:
        Base64();

        // 编码
        void encode(FILE *, std::string& , bool add_crlf = false);
        void encode(const std::string&, std::string& , bool add_crlf = false);
        void encode(const char *, size_t, std::string& , bool add_crlf = false);
        void encode(const unsigned char *, size_t, std::string& , bool add_crlf = false);
        // 解码
        void decode(const std::string&, std::string& );
        void decode(const std::string&, unsigned char *, size_t&);
        // 长度
        size_t decodeLength(const std::string& );

    private:
        Base64(const Base64& ) {}
        Base64& operator=(const Base64& ) { return *this; }

        static const char *bstr;
        static const char rstr[128];
    };
}

#endif
