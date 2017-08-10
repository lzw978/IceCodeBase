/********************************************
 **  模块名称：redispool.cpp
 **  模块功能:
 **  模块简述: redis连接池实现
 **  编 写 人:
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/
#include "redispool.h"

RedisPool* RedisPool::_instance = NULL;

// 单例
RedisPool* RedisPool::instance()
{
    if(NULL == _instance)
    {
        _instance = new RedisPool;
    }
    return _instance;
}

// 构造函数
RedisPool::RedisPool(int nLinkCnt) : m_nLinkCnt(nLinkCnt), m_nTimeout(10)
{
}
// 析构函数
RedisPool::~RedisPool()
{
}

// 设置连接数
void RedisPool::setLinkCnt(int nLinkCnt)
{
    this->m_nLinkCnt = nLinkCnt;
}

// 设置超时时间
void RedisPool::setTimeout(int nTimeout)
{
    this->m_nTimeout = nTimeout;
}

// 设置数据库序号索引
void RedisPool::setDbnumber(int nDbnumber)
{
    this->m_nDbnumber = nDbnumber;
}

// 初始化
bool RedisPool::init(const string ip, int port, int timeout)
{
    CSmartCriticalSection scs(&m_cs);

    m_strIp    = ip;      // redis服务器IP
    m_nPort    = port;    // redis服务器端口
    m_nTimeout = timeout; // 超时时间

    // 新建连接到连接池中
    RedisClient *p = NULL;
    for(int i=0; i<m_nLinkCnt; i++)
    {
        p = new RedisClient;
        m_pool.append(p);

        if(NULL == p->connect(m_strIp,m_nPort,m_nTimeout))
        {
            return false;
        }
        if(!p->SetDbNumber(m_nDbnumber))
        {
            return false;
        }
    }
    return true;
}

// 获取一个连接
RedisClient* RedisPool::getLink(int timeout) //单位微秒
{
    RedisClient *p = NULL;
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

// 新建连接
bool RedisPool::increaseLink()
{
    CSmartCriticalSection scs(&m_cs);

    RedisClient *p = NULL;
    p = new RedisClient;
    if (p == NULL)
    {
        return false;
    }

    if(NULL == p->connect(m_strIp,m_nPort,m_nTimeout))
    {
        delete p;
        p = NULL;
        return false;
    }
    if( !p->SetDbNumber(m_nDbnumber) )
    {
        delete p;
        p = NULL;
        return false;
    }
    m_pool.append(p);
    return true;
}

// 空闲连接放回池中
void RedisPool::putLink(RedisClient *redis, bool bLinkGood)
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

// 使用线程ID
void RedisPool::SetTid( const string& strTid )
{
    snprintf( m_strTid, 32, "%s", strTid.c_str() );
}

// 获取线程ID
const char* RedisPool::GetTid() const
{
    return m_strTid;
}


