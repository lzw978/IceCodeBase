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

using namespace commbase;
using namespace std;

#define INVALID_SOCKET -1
int main(int argc, char *argv[])
{
    int n;
    char buff[1024+1] = {0};
    try
    {
        TSocket tcSocket;
        tcSocket.createSocket();
        tcSocket.bind("192.168.128.128", 8765);
        tcSocket.listen(5);

        while(1)
        {
            TSocket tcClient;
            struct sockaddr_in sockaddr;
            socklen_t iSockLen = sizeof(sockaddr_in);
            tcSocket.accept(tcClient, (struct sockaddr*)&sockaddr, iSockLen);

            n = tcClient.recv(buff, 1024, 0);
            buff[n] = '\0';
            cout << "recv msg from client: " << endl;
            cout << buff << endl;
            tcClient.close();
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