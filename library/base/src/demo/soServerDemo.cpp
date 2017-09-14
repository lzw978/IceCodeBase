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
 * g++ -g -o soServerDemo -I../../include -L../../ -lCommBase -lrt -ldl soServerDemo.cpp
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
#define MAXLINE 1024
int main(int argc, char *argv[])
{
    int listenfd, connfd;
    struct sockaddr_in sockaddr;
    char buff[MAXLINE] = {0};
    int n;

    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(8765);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    listen(listenfd, 1024);

    cout << "Please wait for the client information" << endl;

    while(1)
    {
        if((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL))  == -1)
        {
            cout << "accept socket errno " << errno  << endl;
            continue;
        }

        n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        cout << "recv msg from client :" << buff << endl;
        n = send(connfd, "Recv:OK", 7, 0);
        close(connfd);
    }
    close(listenfd);
    return 0;
}