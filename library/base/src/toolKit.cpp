/********************************************
 **  模块名称：toolKit.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "toolKit.h"

namespace commbase
{
    void ToolKit::assertTrue(int iLine, const char* pFile, const char* pValStr, bool bVal)
    {
        if (!bVal)
        {
            fprintf(stderr, "Assertion failed: [%s], file=[%s], line=[%d]\r\n",
                    pValStr, pFile, iLine);
            exit(-1);
        }
    }
    // 将二进制数据每字节转换为十六字节字符
    string ToolKit::bin2hex(const void* inDat, size_t iLength, bool bUpper)
    {
        Z_ASSERT( (inDat != NULL));
        string hex;
        hex.resize(iLength*2+1, 0);
        ToolKit::bin2hex(inDat, iLength, (unsigned char*)hex.c_str(), iLength*2+1, bUpper);
        return hex;
    }
    void ToolKit::bin2hex(const void* inDat, size_t inLength, unsigned char* outBuf, size_t outBufSize, bool bUpper)
    {
        const char* ConvertUpper = "0123456789ABCDEF";
        const char* ConvertLower = "0123456789abcdef";

        unsigned char* pDat = (unsigned char*)inDat;
        int pos = 0;
        if (bUpper)
        {
            for (size_t i=0; i<inLength; ++i, pos+=2)
            {
                outBuf[pos] = ConvertUpper[(unsigned char)pDat[i] >> 4];
                outBuf[pos+1] = ConvertUpper[(unsigned char)pDat[i] & 0xf];
            }
            outBuf[pos] = 0;
        }
        else
        {
            for (size_t i=0; i<inLength; ++i, pos+=2)
            {
                outBuf[pos] = ConvertLower[(unsigned char)pDat[i] >> 4];
                outBuf[pos+1] = ConvertLower[(unsigned char)pDat[i] & 0xf];
            }
            outBuf[pos] = 0;
        }
    }
    // 将十六字节字符转换为二进制数据
     bool ToolKit::hex2bin(string inHexStr, void* outBuffer, size_t bufferLen, bool bZeroTail)
     {
        Z_ASSERT( (outBuffer!=NULL) && (bufferLen>0) );
        if (inHexStr.length()%2 != 0)
            return false;

        char szBuffer[3] = {0,0,0};
        unsigned char charVal;
        unsigned char *pBuffer = (unsigned char*)outBuffer;

        size_t i = 0;
        for (; i<inHexStr.length()/2 && i<bufferLen; ++i)
        {
            charVal = 0;
            memcpy(szBuffer, inHexStr.c_str()+i*2, 2);

            if (szBuffer[0] >= '0' && szBuffer[0] <= '9')
                charVal = szBuffer[0] - '0';
            else if (szBuffer[0] >= 'A' && szBuffer[0] <= 'F')
                charVal = szBuffer[0] - 'A' + 10;
            else
                charVal = szBuffer[0] - 'a' + 10;

            charVal *= 16;

            if (szBuffer[1] >= '0' && szBuffer[1] <= '9')
                charVal += szBuffer[1] - '0';
            else if (szBuffer[1] >= 'A' && szBuffer[1] <= 'F')
                charVal += szBuffer[1] - 'A' + 10;
            else
                charVal += szBuffer[1] - 'a' + 10;

            pBuffer[i] = charVal;
        }
        if (bZeroTail && i<bufferLen)
            pBuffer[i] = '\0';

        return true;
     }
    // 取伪随机数
    double ToolKit::random(double dStart, double dEnd)
    {
        static bool bInit = false;
        if (!bInit)
        {
            bInit = true;
            srand(unsigned(time(0)));
        }
        return dStart+(dEnd-dStart)*rand()/(RAND_MAX + 1.0);
    }
}
