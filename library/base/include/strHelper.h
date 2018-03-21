/********************************************
 **  模块名称：strHelper.h
 **  模块功能:
 **  模块简述: 字符串工具类
 **  编 写 人:
 **  日    期: 2017.08.22
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/
#ifndef __LIB_STRHELPER_HPP__
#define __LIB_STRHELPER_HPP__

#include <string>
#include <vector>
#include "toolKit.h"

namespace commbase
{
    using namespace std;

    enum strPadDirect_t
    {
        eStrPadDirectLeft  = 0,
        eStrPadDirectRight = 1
    };

    //! 字符串公共函数
    // 浮点型转字符串
    string double2string(double dVal, string format="%0.2lf");
    // 整形转字符串
    string int2string(int iVal, string format="%d");

    // 计算utf8编码字符串长度(一个汉字按照单位长度1进行计算)
    int strlenUtf8(const char *szStr);


    // 字符串工具集
    class StrHelper
    {
    public:
        // GBK编码字符串截取函数(一个汉字按照单位长度2进行计算)
        static string getGbkSubString(string bigGbkStr, size_t from, size_t length);
        // 获取合法的GBK字符串(替换掉非法字符)
        static string getValidGbkString(string strOriGbkStr, bool bDelBadChar=false, char replaceBadChar=' ');
        // 获取随机字符串
        static string randomString(int iLength);
        // 递归替换，直到不出现需要查找的字符为止
        static string replaceLoop(string str, const string strOldValue, const string strNewValue);
        // 顺序替换
        static string replace(string str, const string strOldValue, const string strNewValue);
        static wstring replaceW(wstring str, const wstring wstrOldValue, const wstring wstrNewValue);
        // 第一个字符大写
        static string upperFirstChar(string str);
        // 第一个字符小写
        static string lowerFirstChar(string str);
        // 转换为大写
        static string toUpper(string str);
        // 转换为小写
        static string toLower(string str);
        // 去空格
        static string trim(string inStr, const char* char2trim = "\r\n \t");
        // 去左空格
        static string trimLeft(string inStr, const char* char2trim = "\r\n \t");
        // 去右空格
        static string trimRight(string inStr, const char*char2trim = "\r\n \t");
        // 字符串分割(待分割字符串结尾不能带分割符,否则最后一个元素为空)
        static vector<string> &split(const string &str, char delim, vector<string> &elems);
        // 字符串分割
        static vector<string> split(const string &str, char delim);
        // 字符串分割(keep_empty用于控制是否保存分割符中为空的字串,true表示保留)
        static vector<string> split(const string &str, const string &delim, const bool keep_empty = true);
        // 字符串填充
        static string strpad(string inStr, char charToFill, size_t lastStrLength, strPadDirect_t dir=eStrPadDirectRight);
        // 设置字符串中key对应的value值
        static string setKeyToExtString(string &bigString, string strKey, string strVal);
        // 读取字符串中key对应value(字符串格式示例= abc:val1:def:val2)
        static string readKeyFromExtString(string &bigString, string strKey);
        // 返回重复输入次数的字符串
        static string repeat(const string &inStr, size_t count);
        // 获取字符串hashcode
        static int hashCode(const char* pData, int iLen);
        static int hashCode(const std::string& s);
    };

    // 字符串格式化
    class formatStr
    {
    public:
        formatStr(const char* fmt...);
        ~formatStr();

        formatStr(const formatStr &other);
        formatStr& operator=(const formatStr &other);
        string str();
        const char* c_str() const;
        operator const char*();
        operator char*();
        operator string();
    private:
        formatStr();
        char* m_pBuffer;
    };

    // KeyValue格式字符串,示例：;abc=testOk;bcd=103;def=33.44;
    class KVString
    {
    public:
        KVString();
        KVString(const string &bigString);
        ~KVString();

        // 转换为字符串
        string toString();
        // 根据key获取值(如果获取不到，取第二个参数设置的默认值)
        string getString(const string &strKey, string strDefVal="");
        int    getInt(const string &strKey, int iDefVal=0);
        double getDouble(const string &strKey, double dDefVal=0);
        // 设置值
        KVString& setString(const string &strKey, const string &strVal);
        KVString& setInt(const string &strKey, int iVal);
        KVString& setDouble(const string &strKey, double dVal);
        // 移除key
        KVString& removeKey(const string &strKey);
        // 打印
        void dump();
        // 解析串
        KVString& parse(const string &bigString);
    private:
        string m_strBigString;
        string getStr(const string &strKey, string strDefVal);
        void processVal(string &strKey);
        void unProcessVal(string &strKey);
    };
}

#endif
