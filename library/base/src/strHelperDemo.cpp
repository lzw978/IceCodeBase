/*
 * =====================================================================================
 *
 *       Filename:   strHelperDemo.cpp
 *
 *    Description:   字符串工具类Demo程序
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o strHelperDemo -I../include -L../ -lCommBase -lrt strHelperDemo.cpp
 * =====================================================================================
 */

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include "strHelper.h"

using namespace std;
using namespace commbase;

int main()
{
    string strOut;
    int iLen = 0;
    double dTestNum = 123.14;
    int iTestNum = 150;
    // UTF8字符串（测试UTF8函数前需要先将文件编码修改为UTF8)
    string UTF8string = "utf8字符串";
    // GBK字符串（测试GBK函数前需要先将本文将编码修改为GBK)
    string GBKstring = "中文GBK测试字符串";

    // 待替换字符串
    string toRepStr = "MYTEST 123 test 978 is ok 测试文本ok";
    // 旧值
    string oldVal = "ok";
    // 新值
    string newVal = "YES";
    // 带空格字符串
    string spaceStr = "     MYTEST 测试文本 ok     ";

    // 截取gbk字符串
    //strOut = StrHelper::getGbkSubString(GBKstring, 0, 7);
    //cout << "StrHelper::getGbkSubString = [" << strOut << "]" << endl;
    // double转字符串
    strOut = double2string(dTestNum);
    cout << "double2string = " << strOut << endl;
    // int转字符串
    strOut = int2string(iTestNum);
    cout << "int2string = " << strOut << endl;
    // 计算utf8字符串长度，输出为7，每个汉字计算为长度1
    iLen = strlenUtf8(UTF8string.c_str());
    cout << "strlenUtf8 = " << iLen << endl;
    // 获取随机字符串(产生一个长度10的随机字符串)
    strOut = StrHelper::randomString(10);
    cout << "StrHelper::randomString = [" << strOut << "]" << endl;
    // 递归替换
    strOut = StrHelper::replaceLoop(toRepStr, oldVal, newVal);
    cout << "StrHelper::replaceLoop = [" << strOut << "]" << endl;
    // 顺序替换
    strOut = StrHelper::replace(toRepStr, oldVal, newVal);
    cout << "StrHelper::replace = [" << strOut << "]" << endl;
    // 第一个字符大写
    strOut = StrHelper::upperFirstChar(oldVal);
    cout << "StrHelper::upperFirstChar = [" << strOut << "]" << endl;
    // 第一个字符小写
    strOut = StrHelper::lowerFirstChar(newVal);
    cout << "StrHelper::lowerFirstChar = [" << strOut << "]" << endl;
    // 转大写
    strOut = StrHelper::toUpper(oldVal);
    cout << "StrHelper::toUpper = [" << strOut << "]" << endl;
    // 转小写
    strOut = StrHelper::toLower(newVal);
    cout << "StrHelper::toLower = [" << strOut << "]" << endl;
    // 去左右空格
    strOut = StrHelper::trim(spaceStr);
    cout << "StrHelper::trim = [" << strOut << "]" << endl;
    // 去左空格
    strOut = StrHelper::trimLeft(spaceStr);
    cout << "StrHelper::trimLeft = [" << strOut << "]" << endl;
    // 去右空格
    strOut = StrHelper::trimRight(spaceStr);
    cout << "StrHelper::trimRight = [" << strOut << "]" << endl;
    // 字符串分割1
    string splitStr = "abc;123;bcd;456";
    vector<string> vecTest = StrHelper::split(splitStr, ";");
    for(vector<string>::iterator it=vecTest.begin();it!=vecTest.end();++it)
    {
        cout << "StrHelper::split = [" << it->c_str() << "]" << endl;
    }
    // 字符串分割2
    string splitStr2 = "ABC|987|BCD|中文||654";
    vector<string> vecTest2 = StrHelper::split(splitStr2, "|", false);
    for (vector<string>::iterator it = vecTest2.begin(); it != vecTest2.end(); ++it)
    {
        cout << "StrHelper::split2 = [" << it->c_str() << "]" << endl;
    }
    // 字符串左填充
    strOut = StrHelper::strpad(oldVal, '-', 10, eStrPadDirectLeft);
    cout << "StrHelper::strpad(left)= [" << strOut << "]" << endl;
    // 字符串右填充
    strOut = StrHelper::strpad(oldVal, '-', 10, eStrPadDirectRight);
    cout << "StrHelper::strpad(right)= [" << strOut << "]" << endl;
    // 获取字符串中key对应value
    string strKeyValue = "abc:123:def:456:tag:value";
    strOut = StrHelper::readKeyFromExtString(strKeyValue, "def");
    cout << "StrHelper::readKeyFromExtString= [" << strOut << "]" << endl;
    // 设置字符串中key对应value
    strOut = StrHelper::setKeyToExtString(strKeyValue, "def", "978");
    cout << "StrHelper::setKeyToExtString= [" << strOut << "]" << endl;
    // 重复字符串
    strOut = StrHelper::repeat(newVal, 5);
    cout << "StrHelper::repeat= [" << strOut << "]" << endl;

    // 测试字符串格式化类
    formatStr myForStr("str=[%s] int=[%d] double=[%.2lf]", "format test", 100, 13.14);
    cout << myForStr.c_str() << endl;

    // KVString测试
    string KeyValString = "abc=kvString;bcd=101;def=13.14";
    string val1;
    KVString myKVString(KeyValString);
    cout << "KeyValString: string=" << myKVString.getString("abc") << endl;
    cout << "KeyValString: double=" << myKVString.getDouble("def") << endl;
    cout << "KeyValString: int=" << myKVString.getInt("ABC", 10) << endl;
    myKVString.setString("abc", "testOk");
    myKVString.setDouble("def", 33.44);
    myKVString.setInt("bcd", 103);
    cout << "KeyValString: toString=" << myKVString.toString() << endl;
    cout << "KeyValString: string=" << myKVString.getString("abc") << endl;
    cout << "KeyValString: double=" << myKVString.getDouble("def") << endl;
    cout << "KeyValString: int=" << myKVString.getInt("bcd", 10) << endl;

    return 0;
}