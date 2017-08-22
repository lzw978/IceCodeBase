/********************************************
 **  模块名称：toolKit.h
 **  模块功能:
 **  模块简述: 工具类
 **  编 写 人:
 **  日    期: 2017.08.22
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/
#ifndef __LIB_TOOLKIT_H__
#define __LIB_TOOLKIT_H__

#include <string>
#include <vector>

namespace commbase
{
    using namespace std;

    class ToolKit
    {
    public:
        // 替代系统的assert
        static void assertTrue(int iLine, const char* pFile, const char* pValStr, bool bVal);
        // 将二进制数据每字节转换为十六字节字符
        static string bin2hex(const void* inDat, size_t iLength, bool bUpper=false);
        static void bin2hex(const void* inDat, size_t inLength, unsigned char* outBuf, size_t outBufSize, bool bUpper=false);
        // 将十六字节字符转换为二进制数据
        static bool hex2bin(string inHexStr, void* outBuffer, size_t bufferLen, bool bZeroTail=false);
        // 取伪随机数
        static double random(double dStart, double dEnd);
    };

    // 断言宏
    #define Z_ASSERT(v) ToolKit::assertTrue(__LINE__, __FILE__, #v, (v))
}

#endif