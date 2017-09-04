/*
 * =====================================================================================
 *
 *       Filename:  md5Demo.cpp
 *
 *    Description:  获取字符串md5、文件md5demo
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (lzw978),
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o md5Demo -I../include -L../ -lCommBase -lrt md5Demo.cpp
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "md5.h"

using namespace commbase;
using namespace std;
int main(int argc, char* argv[])
{
    MD5Digest md5Test;
    string strMsg = "MY cpp to test md5 string and file digest md5, It is work";
    string strFilePath = "./md5Demo.cpp";
    string strRet;


    strRet = md5Test.fromString(strMsg);
    cout << strRet << endl;
    try
    {
        strRet = md5Test.fromFile(strFilePath);
        cout << strRet << endl;
    }
    catch(...)
    {
        cout << "deal file error" << endl;
        return -1;
    }



    return 0;
}