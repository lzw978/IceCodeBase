/********************************************
 **  模块名称：redisClient.cpp
 **  模块功能:
 **  模块简述: redis客户端实现文件
 **  编 写 人: lzw978
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#include "redisClient.h"

RedisClient::RedisClient()
{
}

RedisClient::~RedisClient()
{
    if(m_pCtx != NULL)
    {
        redisFree(m_pCtx);
        m_pCtx = NULL;
    }
}

// 执行redis命令
bool RedisClient::ExecuteCmd(const char *cmd, string &response)
{
    redisReply *reply = ExecuteCmd(cmd);
    if(reply == NULL)
        return false;
    char svalue[32] = {0};
    AutoFreeRedisReply AutoReply(reply);
    if(reply->type == REDIS_REPLY_INTEGER)
    {
        sprintf(svalue, "%ld", reply->integer);
        response = svalue;
        return true;
    }
    else if(reply->type == REDIS_REPLY_STRING)
    {
        response.assign(reply->str, reply->len);
        return true;
    }
    else if(reply->type == REDIS_REPLY_STATUS)
    {
        response.assign(reply->str, reply->len);
        return true;
    }
    else if(reply->type == REDIS_REPLY_NIL)
    {
        response = "";
        return true;
    }
    else if(reply->type == REDIS_REPLY_ERROR)
    {
        response.assign(reply->str, reply->len);
        return false;
    }
    else if(reply->type == REDIS_REPLY_ARRAY)
    {
        response = "Not Support Array Result!!!";
        return false;
    }
    else
    {
        response = "Undefine Reply Type";
        return false;
    }
}

// 带返回结果的命令执行
redisReply* RedisClient::ExecuteCmd(const char *cmd)
{
    if(cmd == NULL) return NULL;

    redisReply *reply = (redisReply*)redisCommand(m_pCtx, cmd);
    if(reply->type == REDIS_REPLY_ERROR)
    {
        freeReplyObject(reply);
        return NULL;
    }
    return reply;
}

// 连接redis
redisContext* RedisClient::connect(string ip, int port,int timeout)
{
    m_timeout    = timeout;
    m_serverPort = port;
    m_setverIp   = ip;
    m_beginInvalidTime = 0;

    // 获取当前时间
    time_t now = time(NULL);
    if(now < m_beginInvalidTime + m_maxReconnectInterval)
        return NULL;

    struct timeval tv;
    tv.tv_sec = m_timeout / 1000;
    tv.tv_usec = (m_timeout % 1000) * 1000;;
    m_pCtx = redisConnectWithTimeout(m_setverIp.c_str(), m_serverPort, tv);
    if(m_pCtx == NULL || m_pCtx->err != 0)
    {
        if(m_pCtx != NULL)
            redisFree(m_pCtx);

        m_beginInvalidTime = time(NULL);
        return NULL;
    }

    return m_pCtx;
}

// 释放资源
void RedisClient::ReleaseContext(redisContext *ctx, bool active)
{
    if(m_pCtx == NULL) return;
    if(!active) {redisFree(m_pCtx); return;}
}

// 选择数据库
bool RedisClient::SetDbNumber(int idbnum)
{
    char cmd[256]={0};
    snprintf(cmd, sizeof(cmd), "SELECT %d",idbnum);

    redisReply *reply = (redisReply*)redisCommand(m_pCtx, cmd);

    AutoFreeRedisReply AutoReply(reply);
    if(reply->type == REDIS_REPLY_ERROR)
    {
        return false;
    }

    return true;
}

// 验证服务器状态
bool RedisClient::CheckStatus(redisContext *ctx)
{
    redisReply *reply = (redisReply*)redisCommand(m_pCtx, "ping");
    if(reply == NULL) return false;

    AutoFreeRedisReply AutoReply(reply);

    if(reply->type != REDIS_REPLY_STATUS)
        return false;
    if(strcasecmp(reply->str,"PONG") != 0)
        return false;

    return true;
}

// 断开连接
void RedisClient::disconnect()
{
    if(m_pCtx != NULL)
    {
        redisFree(m_pCtx);
        m_pCtx = NULL;
    }
    return ;
}
