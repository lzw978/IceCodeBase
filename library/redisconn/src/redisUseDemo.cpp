/*
 * =====================================================================================
 *
 *       Filename:  redisUseDemo.cpp
 *
 *    Description:  test use lib to use redis
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (lzw978),
 *   Organization:
 *   编译src目录下执行：
 *   g++ -g -o MYREDIS -I../include -I../../hiredis-vip -L../ -L../../hiredis-vip -lRedisConn -lhiredis_vip redisUseDemo.cpp
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "redispool.h"

using namespace std;

int main(int argc, char** argv)
{
    int nRet = 0;
    char szIP[30]  ="127.0.0.1";  // IP
    int  iPort     = 6379;        // 端口号
    int  iTimeout  = 5000;        // 超时时间
    string response;
    string cmd;
    RedisPool *pRedisPool = NULL;

    // 获取实例
    pRedisPool = RedisPool::instance();
    if( pRedisPool == NULL)
    {
        printf("Get redispool instance error\n");
        return -1;
    }

    // 设置连接数、超时时间
    pRedisPool->setLinkCnt(1);
    pRedisPool->setTimeout(1000);

    // 初始化
    bool isInitOK = pRedisPool->init(szIP, iPort, iTimeout);
    if( !isInitOK)
    {
        return -1;
    }

    RedisClient *p = NULL;
    p = RedisPool::instance()->getLink();
    if(!p)
    {
        return -1;
    }

    // 执行命令
    cmd = "PING";
    nRet = p->ExecuteCmd(cmd.c_str(), response);
    if(nRet< 0)
    {
        return -1;
    }
    cout << response << endl;

    // 在redis中使用set myname xxx,这边进行获取
    cmd = "GET myname";
    nRet = p->ExecuteCmd(cmd.c_str(), response);
    if(nRet< 0)
    {
        return -1;
    }
    cout << response << endl;

    return 0;
}