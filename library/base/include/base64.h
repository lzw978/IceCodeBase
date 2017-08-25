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

        // 编码(add_crlf是否增加换行符)
        void encode(FILE *pFile, std::string& strOut, bool add_crlf = false);
        void encode(const std::string& strIn, std::string& strOut, bool add_crlf = false);
        void encode(const char *szIn, size_t len, std::string& strOut, bool add_crlf = false);
        void encode(const unsigned char *szIn, size_t len, std::string& strOut, bool add_crlf = false);
        // 解码
        void decode(const std::string& strIn, std::string& strOut);
        void decode(const std::string& strIn, unsigned char *szOut, size_t& len);
        // 长度
        size_t decodeLength(const std::string& str64);

    private:
        Base64(const Base64& ) {}
        Base64& operator=(const Base64& ) { return *this; }

        static const char *bstr;
        static const char rstr[128];
    };
}

#endif
