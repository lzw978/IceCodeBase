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
 * 先把文件编码设置为GBK, 验证GBK转换UTF8, 注释34行
 * 再把文件编码设置为UTF8，验证UTF8转换为GBK, 注释35行
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
    string codeMsg = "中文编码转换为信息,验证是否可以正常显示";
    cout << "before = [" << codeMsg  << "]" << endl;
    cout << "after  = [" << _U2G(codeMsg) << "]" << endl;
    //cout << "after  = [" << _G2U(codeMsg) << "]" << endl;
    return -1;
}