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
 * g++ -g -o socketClientDemo -I../../include -L../../ -lCommBase -lrt -ldl socketClientDemo.cpp
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
#include <stdio.h>
#include "tsocket.h"
#include "tepoller.h"
#include "tsocketclient.h"

using namespace commbase;
using namespace std;

#define INVALID_SOCKET -1
/* test ok
int main(int argc, char *argv[])
{
    char sendline[1024+1] = {0};
    try
    {
        TSocket tcSocket;
        tcSocket.createSocket();
        tcSocket.connect("192.168.128.1", 8765);

        // 从命令行读入,发送到服务器
        while(1)
        {
            fgets(sendline, 1024, stdin);
            tcSocket.send(sendline, strlen(sendline));
        }
        tcSocket.close();
    }
    catch(...)
    {
        cout << "socket error" << endl;
        return -1;
    }
    return 0;
}
*/

int main(int argc, char *argv[])
{
    char sendline[1024+1] = {0};
    try
    {
        CTcpClient tc;
        tc.init("192.168.128.128", 8765, 3);
        while(1)
        {
            char recvline[1024+1] = {0};
            size_t length = 1024;
            fgets(sendline, 1024, stdin);
            //int iRet = tc.send(sendline, strlen(sendline));
            int iRet = tc.sendRecv(sendline, strlen(sendline), recvline, length);
            if( iRet < 0)
            {
                cout << "send and recv error" << iRet << endl;
            }
            cout << "recv msg =[" << recvline << "]" << endl;
        }
    }
    catch(...)
    {
        cout << "socket error" << endl;
        return -1;
    }
    return 0;
}