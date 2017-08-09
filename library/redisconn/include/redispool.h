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

#include "redisconnpool.h"
#include "criticalsection.h"
#include "SEGIredisClient.h"

class RedisPool
{
public:
    RedisPool(int nLinkCnt=1):m_nLinkCnt(nLinkCnt), m_nTimeout(10)
    {
    }

    ~RedisPool()
    {
    }

    void setLinkCnt(int nLinkCnt)
    {
        this->m_nLinkCnt = nLinkCnt;
    }

    void setTimeout(int nTimeout)
    {
        this->m_nTimeout = nTimeout;
    }

    bool init(const string &ip,const int &port, const int &timeout)
    {
        CSmartCriticalSection scs(&m_cs);

        m_strIp= ip;
        m_nPort= port;
        m_nTimeout= timeout;

        SEGIRedisClient *p = NULL;
        for(int i=0; i<m_nLinkCnt; i++)
        {
            p = new SEGIRedisClient;
            m_pool.append(p);

            if(true != p->connect(m_strIp,m_nPort,m_nTimeout))
            {
                return false;
            }
        }

        return true;
    }

    SEGIRedisClient* getLink(int timeout=5000) //单位微秒
    {
        SEGIRedisClient *p = NULL;
        if(!m_pool.pop(p, timeout))
        {
            if (!increaseLink())
            {
                return NULL;
            }
            if(!m_pool.pop(p, timeout))
            {
                return NULL;
            }
        }

        return p;
    }

    bool increaseLink()
    {
        CSmartCriticalSection scs(&m_cs);

        SEGIRedisClient *p = NULL;
        p = new SEGIRedisClient;
        if (p == NULL)
        {
            return false;
        }

        if(true != p->connect(m_strIp,m_nPort,m_nTimeout))
        {
            delete p;
            p = NULL;
            return false;
        }
        m_pool.append(p);
        return true;
    }

    void putLink(SEGIRedisClient *redis, bool bLinkGood=true)
    {
        if (m_pool.size() >= m_nLinkCnt)
        {
            redis->disconnect();
            delete redis;
            redis = NULL;
        }
        else
        {
            if (bLinkGood)
            {
                m_pool.append(redis);
            }
            else
            {
                redis->disconnect();
                delete redis;
                redis = NULL;
                increaseLink();
            }
        }
    }

    int size()
    {
        return m_pool.size();
    }

    static RedisPool* instance()
    {
        if(NULL == _instance)
        {
            _instance = new RedisPool;
        }
        return _instance;
    }

    void SetTid( const string& strTid )
    {
        snprintf( m_strTid, 32, "%s", strTid.c_str() );
    }

    const char* GetTid()const { return m_strTid; }

private:
    static RedisPool *_instance;
    RedisConnPool  * m_pool;
    int    m_nLinkCnt;
    int    m_nTimeout;
    char   m_strTid[32];

    string m_strIp;
    int    m_nPort;
    int    m_nTimeout;

    CCriticalSection m_cs;
};

#endif

