/********************************************
 **  模块名称：strHelper.cpp
 **  模块功能:
 **  模块简述: 字符串工具类实现
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
#include <stdarg.h>
#include <sstream>
#include <algorithm>
#include "strHelper.h"

namespace commbase
{
    // 浮点型转字符串
    string double2string(double dVal, string format)
    {
        char szVal[100] = {0};
        snprintf(szVal, sizeof(szVal), format.c_str(), dVal);
        return szVal;
    }
    // 整形转字符串
    string int2string(int iVal, string format)
    {
        char szVal[100] = {0};
        snprintf(szVal, sizeof(szVal), format.c_str(), iVal);
        return szVal;
    }
    // 计算utf8编码字符串长度
    int strlenUtf8(const char *szStr)
    {
        int i=0;
        int j=0;
        while ( szStr[i])
        {
            if ( (szStr[i] & 0xc0) != 0x80)
                j++;
            i++;
        }
        return j;
    }

    // GBK字符串截取
    string StrHelper::getGbkSubString(string bigGbkStr, size_t from, size_t length)
    {
        string subString;
        if ( bigGbkStr.length()<= 0 || length<=0 )
            return subString;
        if ( from >= bigGbkStr.length())
            return subString;
        if ( (from+length) >= bigGbkStr.length() )
            length = bigGbkStr.length() - from;

        char *pBuffer = new char[bigGbkStr.length()+2];
        Z_ASSERT(pBuffer != NULL);
        memset(pBuffer, 0, bigGbkStr.length()+2);
        memcpy(pBuffer, bigGbkStr.c_str(), bigGbkStr.length());

        // 过滤非法GBK字符
        unsigned char *pStrip = (unsigned char *)pBuffer;
        unsigned char *pStripEnd = pStrip + strlen(pBuffer);

        for (; pStrip<pStripEnd; )
        {
            if ( (*pStrip)>=0x81 && (*pStrip)<=0xFE)
            {
                if ( (pStrip+1) == pStripEnd)
                {
                    *pStrip = ' ';
                    break;
                }
                if ( (*(pStrip+1)) >= 0x40 && (*(pStrip+1)) <= 0xFE )
                {
                    pStrip += 2;
                }
                else
                {
                    *pStrip = ' '; // 非法
                    pStrip++;
                }
            }
            else
            {
                pStrip++;
            }
        }
        // 找到起点
        unsigned char *pStart          = (unsigned char *)pBuffer;
        unsigned char *pStringEnd      = (unsigned char *)pBuffer+from+length;
        unsigned char *pStringTmpStart = (unsigned char *)pBuffer+from;

        for (; (pStart<pStringTmpStart+1) && (pStart<pStringEnd); )
        {
            if ( pStart == pStringTmpStart)
                break;

            if ( (*pStart) >= 0x81 && (*pStart) <= 0xFE)
            {
                if ( pStart == pStringTmpStart)
                {
                    break;
                }
                else if ( pStart+1 == pStringTmpStart)
                {
                    pStart++;
                    *pStart = ' ';
                    break;
                }
                pStart += 2;
            }
            else
            {
                pStart++;
            }
        }
        // 查找结尾, 从pStart查找非法字符
        unsigned char *pTmpEndPos = pStringEnd;
        pStringEnd = pStart;
        for( ; pStringEnd<pTmpEndPos; pStringEnd++)
        {
            if ( (*pStringEnd)>=0x81 && (*pStringEnd)<=0xFE )
            {
                // 只取一个字符, 并且这个字符是非法的.
                if ((pStringEnd+1)==pTmpEndPos)
                {
                    *pStringEnd = ' ';
                    continue;
                }
                // 合法的GBK初始位置
                if ( (*(pStringEnd+1))>=0x40 && (*(pStringEnd+1))<=0xFE )
                {
                    pStringEnd++;
                }
                else
                {
                    break; // 非法, 继续下一个字符
                }
            }
        }

        *pStringEnd = '\0';
        subString = (char*)pStart;

        delete[] pBuffer;
        return subString;
    }

    // [0x81-0xFE][0x40-0xFE] 获取合法的GBK字符串(替换掉非法字符)
    string StrHelper::getValidGbkString(string strOriGbkStr, bool bDelBadChar, char replaceBadChar)
    {
        /*
        GBK字符集
        作用：它是GB2312的扩展，加入对繁体字的支持，兼容GB2312。
        位数：使用2个字节表示，可表示21886个字符。
        范围：高字节从81到FE，低字节从40到FE。

        GB2312字符集
        作用：国家简体中文字符集，兼容ASCII。
        位数：使用2个字节表示，能表示7445个符号，包括6763个汉字，几乎覆盖所有高频率汉字。
        范围：高字节从A1到F7, 低字节从A1到FE。将高字节和低字节分别加上0XA0即可得到编码。
        */
        if (strOriGbkStr.length()<=0)
            return strOriGbkStr;

        char *pBuffer = strdup(strOriGbkStr.c_str());
        Z_ASSERT(pBuffer!=NULL);

        // 过滤非法GBK字符
        unsigned char *pStrip = (unsigned char *)pBuffer;
        unsigned char *pStripEnd = pStrip+strlen(pBuffer);

        int leftLength = 0;
        for (; pStrip<pStripEnd;)
        {
            if ( (*pStrip)>=0x81 && (*pStrip)<=0xFE )
            {
                if ((pStrip+1)==pStripEnd)
                {
                    if (bDelBadChar)
                        *pStrip = '\0';
                    else
                        *pStrip = replaceBadChar;
                    break;
                }

                if ( (*(pStrip+1))>=0x40 && (*(pStrip+1))<=0xFE )
                {
                    pStrip += 2;
                }
                else
                {
                    if (bDelBadChar)
                    {
                        leftLength = strlen((char*)pStrip)-1;
                        pStripEnd -= 1;
                        memmove(pStrip, pStrip+1, leftLength);
                    }
                    else
                    {
                        *pStrip = replaceBadChar;
                    }
                    pStrip++;
                }
            }
            else
            {
                pStrip++;
            }
        }
        *pStripEnd = '\0';

        strOriGbkStr = pBuffer;
        free(pBuffer);
        return strOriGbkStr;
    }

    // 获取随机字符串
    string StrHelper::randomString(int iLength)
    {
        string retStr;
        retStr.resize(iLength, 0);

        static char table[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (int i=0; i<iLength; ++i)
        {
            retStr[i] = table[ (int)ToolKit::random(0, sizeof(table)-1) ];
        }
        return retStr;
    }

    // 递归替换
    string StrHelper::replaceLoop(string str, const string strOldValue, const string strNewValue)
    {
        string::size_type pos(string::npos);
        pos = str.find(strOldValue);

        for (; pos!=string::npos; pos=str.find(strOldValue))
        {
            str.replace(pos, strOldValue.length(), strNewValue);
            if( strNewValue.find(strOldValue) != string::npos)
                break; //  防止死循环,比如 aa -> aaa
        }
        return str;
    }
    // 顺序替换
    string StrHelper::replace(string str, const string strOldValue, const string strNewValue)
    {
        string::size_type pos(string::npos);
        pos = str.find(strOldValue);
        for (; pos!=string::npos; )
        {
            str.replace(pos, strOldValue.length(), strNewValue);
            pos = str.find(strOldValue, pos+strNewValue.length());
        }
        return str;
    }
    wstring StrHelper::replaceW(wstring str, const wstring wstrOldValue, const wstring wstrNewValue)
    {
        wstring::size_type pos(wstring::npos);
        pos = str.find(wstrOldValue);
        for ( ;pos!=wstring::npos; )
        {
            str.replace(pos, wstrOldValue.length(), wstrNewValue);
            pos = str.find(wstrOldValue, pos+wstrNewValue.length());
        }
        return str;
    }
    // 第一个字符大写
    string StrHelper::upperFirstChar(string str)
    {
        if (str == "")
            return str;
        if (str[0]>='a' && str[0]<='z')
            str[0] = str[0] - 'a' + 'A';
        return str;
    }
    // 第一个字符小写
    string StrHelper::lowerFirstChar(string str)
    {
        if (str == "")
            return str;
        if (str[0]>='A' && str[0]<='Z')
            str[0] = str[0] - 'A' + 'a';
        return str;
    }
    // 转换为大写
    string StrHelper::toUpper(string str)
    {
        string upperStr = str;
        for (size_t i=0; i<upperStr.length(); ++i)
        {
            if (upperStr[i]>='a' && upperStr[i]<='z')
                upperStr[i] = upperStr[i] - 'a' + 'A';
        }
        return upperStr;
    }
    // 转换为小写
    string StrHelper::toLower(string str)
    {
        string lowerStr = str;
        for (size_t i=0; i<lowerStr.length(); ++i)
        {
            if (lowerStr[i]>='A' && lowerStr[i]<='Z')
                lowerStr[i] = lowerStr[i] - 'A' + 'a';
        }
        return lowerStr;
    }
    // 去空格
    string StrHelper::trim(string inStr, const char* char2trim)
    {
        char2trim = char2trim==NULL?"\r\n \t":char2trim;
        if (inStr.size() <= 0)
            return inStr;

        string::size_type pos = inStr.find_first_not_of(char2trim);
        if (pos != string::npos)
        {
            inStr.erase(0, pos);
        }
        else
        {
            inStr.erase(0);
        }

        pos = inStr.find_last_not_of(char2trim);
        if (pos != string::npos)
            inStr.erase(pos + 1);

        return inStr;
    }
    // 去左空格
    string StrHelper::trimLeft(string inStr, const char* char2trim)
    {
        char2trim = char2trim==NULL?"\r\n \t":char2trim;
        if (inStr.size() <= 0)
            return inStr;

        string::size_type pos = inStr.find_first_not_of(char2trim);
        if (pos != string::npos)
            inStr.erase(0, pos);
        return inStr;
    }
    // 去右空格
    string StrHelper::trimRight(string inStr, const char*char2trim)
    {
        char2trim = char2trim==NULL?"\r\n \t":char2trim;
        if (inStr.size() <= 0)
            return inStr;

        string::size_type pos = inStr.find_last_not_of(char2trim);
        if (pos != string::npos)
            inStr.erase(pos+1);
        return inStr;
    }
    // 字符串分割
    vector<string> &StrHelper::split(const string &str, char delim, vector<string> &elems)
    {
        stringstream ss(str);
        string item;
        while (getline(ss, item, delim))
        {
            elems.push_back(item);
        }
        return elems;
    }
    // 字符串分割
    vector<string> StrHelper::split(const string &str, char delim)
    {
        vector<string> elems;
        return split(str, delim, elems);
    }
    // 字符串分割
    vector<string> StrHelper::split(const string &str, const string &delim, const bool keep_empty)
    {
        vector<string> result;
        if (delim.empty())
        {
            result.push_back(str);
            return result;
        }
        string::const_iterator subStart = str.begin();
        string::const_iterator subEnd;
        while (true)
        {
            subEnd = search(subStart, str.end(), delim.begin(), delim.end());
            string temp(subStart, subEnd);
            if (keep_empty || !temp.empty())
            {
                result.push_back(temp);
            }
            if (subEnd == str.end())
            {
                break;
            }
            subStart = subEnd + delim.size();
        }
        return result;
    }
    // 字符串填充
    string StrHelper::strpad(string inStr, char charToFill, size_t lastStrLength, strPadDirect_t dir)
    {
        if (inStr.length() >= lastStrLength)
            return inStr;

        size_t sizeToPad = lastStrLength - inStr.length();

        string toAdd;
        toAdd.resize(sizeToPad, charToFill);
        if (dir == eStrPadDirectLeft)
            inStr = toAdd + inStr;
        else
            inStr += toAdd;
        return inStr;
    }
    // 设置字符串key/value:  Key:Val
    string StrHelper::setKeyToExtString(string &bigString, string strKey, string strVal)
    {
        if (bigString.length() >= 1)
        {
            if (bigString.at(0) != ':')
                bigString = ":" + bigString;
        }
        if (bigString.length() >= 2)
        {
            if (bigString.at(bigString.length()-1) != ':')
                bigString += ":";
        }

        string secToFind = ":" + strKey + ":";
        string::size_type posStart = bigString.find(secToFind);
        if( posStart == string::npos)
        {
            bigString += secToFind;
            bigString += strVal + ":";
            return bigString;
        }
        else
        {
            posStart += secToFind.length();
            string::size_type posEnd = bigString.find(":", posStart);
            // ***:abc:****
            if (posEnd == string::npos)
                bigString.replace(posStart, bigString.length()-posStart, strVal);
            else
                bigString.replace(posStart, posEnd-posStart, strVal);
        }
        return bigString;
    }

    // 读取字符串中key对应value： 字符串格式示例= abc:val1:def:val2
    string StrHelper::readKeyFromExtString(string &bigString, string strKey)
    {
        string retStr;
        if (bigString.length()<=2 || strKey.length()<=2)
            return retStr;
        if (bigString.at(0) != ':')
            bigString = ":" + bigString;
        if (bigString.at(bigString.length()-1) != ':')
            bigString += ":";

        if (strKey.at(0) != ':')
            strKey = ":" + strKey;
        if (strKey.at(strKey.length()-1) != ':')
            strKey += ":";

        string::size_type posStart = bigString.find(strKey);
        if (posStart == string::npos)
            return retStr;

        posStart += strKey.length();
        string::size_type posEnd = bigString.find(":", posStart);
        if (posEnd == string::npos)
        {
            retStr = bigString.substr(posStart);
            return retStr;
        }

        retStr = bigString.substr(posStart, posEnd-posStart);
        return retStr;
    }

    // 重复字符串
    string StrHelper::repeat(const string &inStr, size_t count)
    {
        string strOut;
        for (size_t i=0; i<count; ++i)
            strOut += inStr;
        if (count <= 0)
            return "";
        return strOut;
    }

    // 字符串格式化类实现
    formatStr::formatStr(const char* fmt...)
    {
        m_pBuffer = NULL;
        fmt = fmt==NULL?"":fmt;

        if (NULL == strchr(fmt, '%') )
        {
            int size = strlen(fmt) + 1;
            m_pBuffer = (char*)calloc(size, sizeof(char));
            if (NULL != m_pBuffer)
            {
                memcpy(m_pBuffer, fmt, size);
                m_pBuffer[size-1] = '\0';
            }
            return ;
        }

        va_list args;
        size_t nextSize   = 0;
        size_t bufferSize = 0;
        char* pBuffer       = NULL;
        char* pRallocBuffer = NULL;

        while(1)
        {
            if (bufferSize == 0)
            {
                int _size = (int) (strlen(fmt)*1.5);
                if (_size < 512)
                    _size = 512;
                pBuffer = (char*)malloc(_size);
                if (pBuffer == NULL)
                {
                    // 内存错误
                    pBuffer = NULL;
                    break;
                }
                bufferSize = _size;
            }
            else if ((pRallocBuffer = (char*)realloc(pBuffer, nextSize)) != NULL)
            {
                pBuffer = pRallocBuffer;
                bufferSize = nextSize;
            }
            else
            {
                pBuffer = NULL;
                break;
            }

            va_start(args, fmt);
            int realSize = vsnprintf(pBuffer, bufferSize, fmt, args);
            va_end(args);

            if (realSize == -1)
            {
                // 猜测大小
                nextSize = bufferSize * 2;
            }
            else if (realSize == (int)bufferSize)
            {
                // 被截断，大小不知道
                nextSize = bufferSize * 2;
            }
            else if (realSize > (int)bufferSize)
            {
                // 已经知道需要多大内存
                nextSize = realSize + 2;
            }
            else if (realSize == (int)bufferSize - 1)
            {
                // 在有些系统上不明确
                nextSize = bufferSize * 2;
            }
            else
            {
                // 理想情况
                break;
            }
        }

        if (pBuffer != NULL)
        {
            m_pBuffer = pBuffer;
        }
    }

    formatStr::~formatStr()
    {
        if (m_pBuffer != NULL)
        {
            free(m_pBuffer);
            m_pBuffer = NULL;
        }
    }

    formatStr::formatStr(const formatStr& other)
    {
        m_pBuffer = NULL;
        if (other.m_pBuffer != NULL)
        {
            m_pBuffer = (char*)calloc(strlen(other.m_pBuffer)+2, 1);
            strcpy(m_pBuffer, other.m_pBuffer);
        }
    }

    formatStr& formatStr::operator=(const formatStr& other)
    {
        if (&other != this)
        {
            if (other.m_pBuffer != NULL)
            {
                if (m_pBuffer != NULL)
                    free(m_pBuffer);
                m_pBuffer = (char*)calloc(strlen(other.m_pBuffer)+2, 1);
                strcpy(m_pBuffer, other.m_pBuffer);
            }
        }
        return *this;
    }

    string formatStr::str()
    {
        return m_pBuffer==NULL?"":m_pBuffer;
    }

    const char* formatStr::c_str() const
    {
        return m_pBuffer==NULL?"":m_pBuffer;
    }

    formatStr::operator const char*()
    {
        return m_pBuffer==NULL?"":m_pBuffer;
    }

    formatStr::operator char*()
    {
        return m_pBuffer==NULL?(char*)"":m_pBuffer;
    }

    formatStr::operator string()
    {
        return m_pBuffer==NULL?"":m_pBuffer;
    }

    // key/value字符串类实现
    KVString::KVString()
    {
        m_strBigString = ";";
    }

    KVString::KVString(const string &bigString)
    {
        parse(bigString);
    }

    KVString::~KVString()
    {
    }

    string KVString::toString()
    {
        return m_strBigString;
    }

    string KVString::getString(const string &strKey, string strDefVal)
    {
        string str = getStr(strKey, strDefVal);
        unProcessVal(str);
        return str;
    }

    int KVString::getInt(const string &strKey, int iDefVal)
    {
        string str = getStr(strKey, int2string(iDefVal));
        return atoi(str.c_str());
    }

    double KVString::getDouble(const string &strKey, double dDefVal)
    {
        string str = getStr(strKey, double2string(dDefVal));
        return atof(str.c_str());
    }
    // 设置值
    KVString& KVString::setString(const string &strKey, const string &strVal)
    {
        if (m_strBigString.length() >= 1)
        {
            if (m_strBigString.at(0) != ';')
                m_strBigString = ";" + m_strBigString;
        }
        if (m_strBigString.length() >= 2)
        {
            if (m_strBigString.at(m_strBigString.length()-1) != ';')
                m_strBigString += ";";
        }
        string tmpVal = strVal;
        processVal(tmpVal);

        string secToFind = ";" + strKey + "=";
        string::size_type posStart = m_strBigString.find(secToFind);
        if (posStart == string::npos)
        {
            m_strBigString += strKey + "=";
            m_strBigString += tmpVal + ";";
            return *this;
        }
        else
        {
            posStart += secToFind.length();
            // 跳过\;
            string::size_type posStartV0 = posStart;
            string::size_type posTmp = m_strBigString.find("\\;", posStart);
            for (; posTmp!=string::npos; )
            {
                posStart += 2;
                posTmp = m_strBigString.find("\\;", posStart);
            }

            string::size_type posEnd = m_strBigString.find(";", posStart);
            if (posEnd == string::npos)
                m_strBigString.replace(posStartV0, m_strBigString.length()-posStartV0, tmpVal);
            else
                m_strBigString.replace(posStartV0, posEnd-posStartV0, tmpVal);
        }
        return *this;
    }
    KVString& KVString::setInt(const string &strKey, int iVal)
    {
        setString(strKey, int2string(iVal));
        return *this;
    }
    KVString& KVString::setDouble(const string &strKey, double dVal)
    {
        setString(strKey, double2string(dVal));
        return *this;
    }
    // 移除key
    KVString& KVString::removeKey(const string &strKey)
    {
        if (m_strBigString.length() >= 1)
        {
            if (m_strBigString.at(0) != ';')
                m_strBigString = ";" + m_strBigString;
        }
        if (m_strBigString.length() >= 2)
        {
            if (m_strBigString.at(m_strBigString.length()-1) != ';')
                m_strBigString += ";";
        }

        string secToFind = ";" + strKey + "=";
        string::size_type posStart = m_strBigString.find(secToFind);
        if (posStart == string::npos)
        {
            return *this;
        }
        else
        {
            posStart += secToFind.length();
            // 跳过\;
            string::size_type posStartV0 = posStart;
            string::size_type posTmp = m_strBigString.find("\\;", posStart);
            for (; posTmp!=string::npos; )
            {
                posStart += 2;
                posTmp = m_strBigString.find("\\;", posStart);
            }

            string::size_type posEnd = m_strBigString.find(";", posStart);
            if (posEnd == string::npos)
                m_strBigString.replace(posStart-secToFind.length(), m_strBigString.length()-posStartV0, "");
            else
                m_strBigString.replace(posStart-secToFind.length(), posEnd-posStartV0, "");
        }
        return *this;
    }
    // 打印
    void KVString::dump()
    {
        printf("%s\n", m_strBigString.c_str());
    }
    // 解析
    KVString& KVString::parse(const string &bigString)
    {
        m_strBigString = bigString;
        if (m_strBigString.length() > 0)
        {
            if (m_strBigString.at(0) != ';')
                m_strBigString = ";" + m_strBigString;

            if (m_strBigString.at(m_strBigString.length()-1) != ';')
                m_strBigString += ";";
        }
        return *this;
    }

    string KVString::getStr(const string &strKey, string strDefVal)
    {
        string retStr = strDefVal;
        string key = strKey;

        if (m_strBigString.at(0) != ';')
        {
            m_strBigString = ";" + m_strBigString;
        }
        if (m_strBigString.at(m_strBigString.length()-1) != ';')
        {
            m_strBigString += ";";
        }

        key = ";" + key + "=";
        string::size_type posStart = m_strBigString.find(key);
        if (posStart == string::npos)
            return retStr;

        posStart += key.length();

        // 跳过\;
        string::size_type posStartV0 = posStart;
        string::size_type posTmp = m_strBigString.find("\\;", posStart);
        string::size_type posEnd = m_strBigString.find(";", posStart);

        if (posTmp!=string::npos && posEnd==posTmp+1)
        {
            for (;posTmp!=string::npos&&posEnd==posTmp+1; )
            {
                posStart = posTmp + 2;
                posTmp = m_strBigString.find("\\;", posStart);
                posEnd = m_strBigString.find(";", posStart);
            }
        }

        posEnd = m_strBigString.find(";", posStart);
        if (posEnd == string::npos)
        {
            retStr = m_strBigString.substr(posStartV0);
            return retStr;
        }

        retStr = m_strBigString.substr(posStartV0, posEnd-posStartV0);
        return retStr;
    }

    void KVString::processVal(string &strKey)
    {
        if (strKey.length() <= 0)
            return ;

        if (strKey.find(";") != string::npos)
        {
            strKey = "(hex)" + ToolKit::bin2hex(strKey.c_str(), strKey.length(), false);
        }
    }

    void KVString::unProcessVal(string &strKey)
    {
        if (strKey.length() >= 7)
        {
            if (strKey.substr(0, 5) == "(hex)")
            {
                string str = strKey.substr(5);
                if (str.length()%2 == 0)
                {
                    char *buffer = new char[strKey.length()+1];
                    memset(buffer, 0, strKey.length()+1);
                    ToolKit::hex2bin(str, buffer, strKey.length()+1);
                    strKey = buffer;
                    delete buffer;
                }
            }
        }
    }

} // end of namespace
