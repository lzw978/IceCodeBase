/********************************************
 **  模块名称：redisclient.h
 **  模块功能:
 **  模块简述: redis客户端
 **  编 写 人: lzw978
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#ifndef __LIB_REDIS_CLIENT_H__
#define __LIB_REDIS_CLIENT_H__

#include <string>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "hiredis.h"

using namespace std;

class RedisClient
{
public:
    RedisClient();
    virtual ~RedisClient();

    // 选择数据库
    bool SetDbNumber(int idbnum = 0);
    // 连接redis
    redisContext* connect(string ip, int port, int timeout = 2000);
    // 执行redis命令
    bool ExecuteCmd(const char *cmd, string &response);
    // 带返回结果的命令执行
    redisReply* ExecuteCmd(const char *cmd);
    // 断开连接
    void disconnect();

private:
    int m_timeout;     // 超时时间
    int m_serverPort;  // redis服务器端口
    string m_setverIp; // redis服务器IP

    time_t m_beginInvalidTime;                    // 开始时间
    static const int m_maxReconnectInterval = 10; // 连接重试次数

    redisContext* m_pCtx;
    redisContext* CreateContext();
    // 释放资源
    void ReleaseContext(redisContext *ctx, bool active);
    // 测试服务器状态
    bool CheckStatus(redisContext *ctx);
};

struct AutoFreeRedisReply
{
    redisReply* m_pReply;
    AutoFreeRedisReply(redisReply* pReply)
    {
        m_pReply = pReply;
    }
    ~AutoFreeRedisReply()
    {
        if(m_pReply != NULL)
        {
            freeReplyObject(m_pReply);
        }
    }
};
#endif

