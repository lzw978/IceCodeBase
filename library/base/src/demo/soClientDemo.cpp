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
 * g++ -g -o soClientDemo -I../../include -L../../ -L/usr/lib64/mysql -lmysqlclient -lCommBase -lrt -ldl soClientDemo.cpp
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
#define MAXLINE 1024

int main(int argc, char** argv)
{
    //char *servInetAddr = "127.0.0.1";
    const char *servInetAddr = "192.168.128.1";
    int socketfd;
    struct sockaddr_in sockaddr;
    char recvline[MAXLINE] = {0};
    char sendline[MAXLINE] = {0};
    int n;

    memset(&sockaddr, 0, sizeof(sockaddr));
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port   = htons(10004);
    inet_pton(AF_INET, servInetAddr, &sockaddr.sin_addr);
    if ((connect(socketfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))) < 0)
    {
        cout << "connect error" << endl;
        return 0;
    }

    cout << "send msg to server" << endl;
    while(1)
    {
        fgets(sendline, 1024, stdin);
        if (send(socketfd, sendline, strlen(sendline), 0) < 0 )
        {
            cout << "send msg error" << endl;
            return 0;
        }
    }

    close(socketfd);
    return 0;
}