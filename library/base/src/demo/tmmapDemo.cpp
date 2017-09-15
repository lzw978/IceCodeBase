/*
 * =====================================================================================
 *
 *       Filename:   tmmapDemo.cpp
 *
 *    Description:   mmap共享内存demo程序
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o tmmapDemo -I../../include -L../../ -lCommBase -lrt -ldl tmmapDemo.cpp
 * =====================================================================================
 */

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <string.h>
#include "tmmap.h"

using namespace std;
using namespace commbase;

void testCreate()
{
    size_t n = 50;
    TMMap mmap;
    mmap.mmap("mmap.dat", n);
    mmap.munmap();
}

void testWrite()
{
    string sMsg = "hello World";
    TMMap mmap;
    mmap.mmap("mmap.dat", 1000);
    memcpy(mmap.getPointer(), sMsg.c_str(), sMsg.length());

    sleep(10);

    mmap.munmap();
}

void testRead()
{
    TMMap mmap;
    mmap.mmap("mmap.dat", 1000);
    string sRead;
    sRead.assign((char*)mmap.getPointer(), mmap.getSize());

    cout << sRead << endl;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "param error" << endl;
        return -1;
    }
    cout << sizeof(int) << endl;
    try
    {
        // 创建
        if ( 0 == strcmp(argv[1], "t"))
            testCreate();
        // 写入
        else if( 0 == strcmp(argv[1], "w"))
            testWrite();
        // 读取
        else if( 0 == strcmp(argv[1], "r"))
            testRead();
    }
    catch(...)
    {
        cout << "mmap error" << endl;
    }

    return 0;
}