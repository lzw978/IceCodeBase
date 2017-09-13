/*
 * =====================================================================================
 *
 *       Filename:   socketServerDemo.cpp
 *
 *    Description:   scoket示例程序
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o socketServerDemo -I../../include -L../../ -lCommBase -lrt -ldl socketServerDemo.cpp
 * =====================================================================================
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <cerrno>
#include <iostream>
#include "tsocket.h"
#include "tepoller.h"
#include "tsocketclient.h"

using namespace commbase;
using namespace std;

#define INVALID_SOCKET -1
int main(int argc, char *argv[])
{
    CTcpClient tc;
    tc.init("192.168.128.1", 9876, 3000);

    int i = 3;
    while( i > 0 )
    {
        string sMsg = "test";
        char c[1024] = {0};
        size_t length = 4;
        int iRet = tc.send(sMsg.c_str(), sMsg.length());
        if ( iRet < 0)
        {
            cout << "send recv error: " << iRet << ":" << c << endl;
        }
        i--;
    }
    return 0;
}