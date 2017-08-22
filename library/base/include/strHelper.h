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
    string double2string(double dVal, string format="0.2lf");
    // 整形转字符串
    string int2string(int iVal, string format="%d");
    // 计算utf8编码字符串长度
    int strlenUtf8(const char *szStr);


    // 字符串工具集
    class StrHelper
    {
    public:
        // GBK编码字符串截取函数
        static string getGbkSubSting(string bigGbkStr, size_t from, size_t length);
        // GBK编码字符串截取函数(替换非法字符)
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
        // 字符串分割
        static vector<string> &split(const string &str, char delim, vector<string> &elems);
        // 字符串分割
        static vector<string> split(const string &str, char delim);
        // 字符串分割
        static vector<string> split(const string &str, const string &delim, const bool keep_empty = true);
        // 字符串填充
        static string strpad(string inStr, char charToFill, size_t lastStrLength, strPadDirect_t dir=eStrPadDirectRight);
        // 设置字符串中key
        static string setKeyToExtString(string &bigString, string strKey, string strVal);
        // 读取字符串中key对应value
        static string readKeyFromExtString(string &bigString, string strKey);
        // 重复
        static string repeat(const string &inStr, size_t count);
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

    // KeyValue格式字符串
    class KVString
    {
    public:
        KVString();
        KVString(const string &bigString);
        ~KVString();

        // 转换为字符串
        string toString();
        // 根据key获取值
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