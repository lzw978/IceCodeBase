/*
 * =====================================================================================
 *
 *       Filename:  base64Demo.cpp
 *
 *    Description:  基础类库测试Demo程序
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o base64Demo -I../include -L../ -lCommBase -lrt base64Demo.cpp
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

#include "base64.h"

using namespace std;
using namespace commbase;

int main(int argc, char** argv)
{
    string strIn = "string to test, encode to base64 string, today is a good day, I am happy.";
    string strOut;
    string strDecode;
    Base64 oBase64;
    oBase64.encode(strIn, strOut);
    cout << "source = [" << strIn << "]" << endl;
    cout << "base64 = [" << strOut << "]" << endl;
    oBase64.decode(strOut, strDecode);
    cout << "decode = [" << strDecode << "]" << endl;

    return 0;
}
