/*
 * =====================================================================================
 *
 *       Filename:  logDemo.cpp
 *
 *    Description:  test log lib
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *    g++ -g -o logDemo -I../include main.cpp logDemo.cpp
 * =====================================================================================
 */
#include <stdlib.h>
#include "logger.h"


int main(int argc, char* argv[])
{
    char szTest[]    = "Test logger libso";
    char szErrCode[] = "ERR0101";
    int iCode = 1024;

    // 设置日志大小
    SetLogMaxSize(1000000);
    // 设置日志等级
    SetLogLevel(4);
    // 设置路径
    SetLogPath(".");
    // 错误日志名称
    SetErrLogFile("ERRORLOG");
    // 日志名称
    SetLogFileName("MYTEST");

    // 调试日志
    Trace(L_INFO, NULL,  "%s|%d", szTest, iCode);
    // 错误日志
    Trace(L_ERROR, szErrCode,  "%s|%d", szTest, iCode);

    return 0;
}

