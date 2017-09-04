/*
 * =====================================================================================
 *
 *       Filename:  charSetDemo.cpp
 *
 *    Description:  编码转换示例程序
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o charsetDemo -I../include -L../ -lCommBase -lrt charsetDemo.cpp
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include "charset.h"
using namespace commbase;
using namespace std;
int main(int argc, char** argv)
{
    string utf8Msg = "test中文UTF8编码转换为GBK信息,验证是否可以正常显示";
    cout << "before = [" << utf8Msg  << "]" << endl;
    cout << "after  = [" << _U2G(utf8Msg) << "]" << endl;
    return -1;
}