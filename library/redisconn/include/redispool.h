/********************************************
 **  模块名称：redispool.h
 **  模块功能:
 **  模块简述: redis连接池
 **  编 写 人:
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/
#ifndef __LIB_REDISPOOL_H__
#define __LIB_REDISPOOL_H__

#include "criticalsection.h"
#include "redisconnpool.h"
#include "redisClient.h"

typedef RedisConnPool<RedisClient*, true>
RedisQueue;

class RedisPool
{
public:
    // 设置连接数
    void setLinkCnt(int nLinkCnt);
    // 设置超时时间
    void setTimeout(int nTimeout);
    // 设置数据库
    void setDbnumber(int nDbnumber);
    // 初始化
    bool init(const string ip, int port, int timeout);
    // 获取一个连接
    RedisClient* getLink(int timeout=5000); //单位微秒
    // 新建连接
    bool increaseLink();
    // 空闲连接放回池中
    void putLink(RedisClient *redis, bool bLinkGood=true);
    // 连接池大小
    inline int size()
    {
        return m_pool.size();
    }
    // 单例
    static RedisPool* instance();
    // 使用线程id
    void SetTid( const string& strTid);
    // 返回使用线程id
    const char* GetTid() const;

private:
    RedisPool(int nLinkCnt=1);
    ~RedisPool();
    static RedisPool *_instance;  // 单例
    RedisQueue     m_pool;        // 连接池
    int    m_nLinkCnt;            // 连接数
    int    m_nTimeout;            // 超时时间
    int    m_nDbnumber;           // 数据库序号索引
    char   m_strTid[32];          // 线程id

    string m_strIp;               // redis服务器ip
    int    m_nPort;               // redis服务器端口
    CCriticalSection m_cs;        // 线程锁
};

#endif

