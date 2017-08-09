/********************************************
 **  模块名称：MySQLDBPool.h
 **  模块功能:
 **  模块简述: Mysql数据库连接池实现
 **  编 写 人:
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MySQLDBPool.h"

using namespace std;

int multidb::MySQLDBPool::m_dbid = 1;
multidb::MySQLDBPool multidb::MySQLDBPool::m_OradbPool;
pthread_mutex_t dbpolllock_lock = PTHREAD_MUTEX_INITIALIZER;

// 加锁
void dbpolllock::lock()
{
    pthread_mutex_lock(&dbpolllock_lock);
}
// 解锁
void dbpolllock::unlock()
{
    pthread_mutex_unlock(&dbpolllock_lock);
}

dbpolllock::dbpolllock()
{
    lock();
}
dbpolllock::~dbpolllock()
{
    unlock();
}

multidb::MySQLDBPool::MySQLDBPool()
{
}

/****************************************************************************************
*@input      :
*@output     :
*@return     :
*@description: 析构，释放资源
******************************************************************************************/
multidb::MySQLDBPool::~MySQLDBPool()
{
    ReleaseAll();
}

/****************************************************************************************
*@input      :
*@output     :
*@return     :
*@description: 不需要缓冲池时，释放缓冲池的对象
******************************************************************************************/
void multidb::MySQLDBPool::ReleaseAll()
{
    Connection* pConn;
    int i = 0;

    try{
        m_mapItr = m_maplink.begin();
        // 遍历连接句柄
        while(m_mapItr != m_maplink.end())
        {
            m_OraqueueUnUse = m_mapItr->second;
            while(!m_OraqueueUnUse->empty())
            {
                // 取出未使用的数据库连接
                pConn = m_OraqueueUnUse->back();
                m_OraqueueUnUse->pop_back();

                i++;

                // 关闭未使用的数据库连接
                if( pConn)
                {
                    pConn->close();
                    delete pConn;
                    pConn = NULL;
                }
            }
            ++m_mapItr;
        }
    }
    catch(...)
    {
        printf("ReleaseAll catch unknow exception\n");
    }
}

/****************************************************************************************
*@input      :
*@output     :
*@return     :
*@description: 返回池对象的大小
******************************************************************************************/
int multidb::MySQLDBPool::Size(int dbid)
{
    m_mapItr = m_maplink.find(dbid);
    if(m_mapItr != m_maplink.end())
    {
        return m_mapItr->second->size();
    }
    return 0;
}

/****************************************************************************************
*@input      : nCount 实例化数据库连接的个数
*@output     :
*@return     : ture:success false:fail
*@description: 初始化数据库连接的池对象(第一次使用数据库连接池之前调用，连接串通过配置读取)
******************************************************************************************/
bool multidb::MySQLDBPool::InitConnects(int dbid, const char* connectstr, int nCount, const char* vschema)
{
    return InitConn(dbid, connectstr, nCount, vschema);
}

/****************************************************************************************
*@input      :
*@output     :
*@return     : 缓冲池对象的一个指针
*@description: 提供一个static的接口，实现单例模式
******************************************************************************************/
multidb::MySQLDBPool* multidb::MySQLDBPool::GetMySQLPool()
{
    return &m_OradbPool;
}

/****************************************************************************************
*@input      : nCount 实例化的数据库连接的个数 nCount>0
*@output     :
*@return     : true:success false:fail
*@description: 初始化数据库连接的池对象(InitPool调用)
******************************************************************************************/
bool multidb::MySQLDBPool::InitConn(int dbid, const char* connectstr, int nCount, const char* vschema)
{
    // 已存在的dbid不能再次初始化连接
    m_mapItr = m_maplink.find(dbid);
    if( m_mapItr != m_maplink.end())
    {
        return false;
    }

    char url[128] = {0}; // 连接url
    char user[32] = {0}; // 用户名
    char pass[32] = {0}; // 密码

    // 解析连接串
    spliteStr(connectstr, user, pass, url);
    // 获取数据库连接驱动
    m_driver = get_driver_instance();

    Connection* pConn;
    m_OraqueueUnUse = new std::list<Connection*>();

    // 按照配置连接数量建立连接句柄，存放到未使用句柄队列中
    for(int i=1;i<=nCount;i++)
    {
        pConn = NULL;
        pConn = m_driver->connect(url, user, pass);
        if(pConn == NULL)
        {
            ReleaseAll();
            return false;
        }
        if(vschema)
        {
            pConn->setSchema(vschema);
        }
        m_OraqueueUnUse->push_back(pConn);
    }

    // 插入到映射句柄队列中
    m_maplink.insert(pair<int, std::list<Connection*>* >(dbid, m_OraqueueUnUse));

    return true;
}

/****************************************************************************************
*@input      :
*@output     :
*@return     :
*@description: 拆分连接串
******************************************************************************************/
bool multidb::MySQLDBPool::spliteStr(const char* constr, char* user, char* pass, char* url)
{
    const char *p = strchr(constr, '/');
    const char *pp;
    // 获取用户名
    if(p)
    {
        strncpy(user, constr, p-constr);
        user[p-constr] = '\0';
    }

    // 获取密码
    pp = ++p;
    p = strchr(p, '@');
    if(p)
    {
        strncpy(pass, pp, p-pp);
        pass[p-pp] = '\0';
    }

    // 获取url连接串
    if(++p)
    {
        strcpy(url, p);
    }
    return true;
}

/****************************************************************************************
*@input      :
*@output     :
*@return     : 返回一个数据库连接的指针
*@description: 需要数据库连接时，从池中取出一个数据库连接
******************************************************************************************/
Connection* multidb::MySQLDBPool::GetConnection(int dbid)
{
    Connection* pconn = NULL;
    dbpolllock lc;

    // 从未使用的句柄队列中获取一个连接
    m_mapItr = m_maplink.find(dbid);
    if(m_mapItr != m_maplink.end())
    {
        m_OraqueueUnUse = m_mapItr->second;

        if( m_OraqueueUnUse->size() == 0)
            return NULL;

        // 弹出该连接
        pconn = m_OraqueueUnUse->back();
        m_OraqueueUnUse->pop_back();

        // 插入该连接到已使用句柄队列中
        m_mapused.insert(pair<Connection* , int>(pconn, dbid));
    }
    return pconn;
}

/****************************************************************************************
*@input      : pOraConn：一个数据库连接的指针
*@output     :
*@return     :
*@description: 需要释放一个数据库连接时，把一个数据库连接放入池中
******************************************************************************************/
void multidb::MySQLDBPool::FreeConnection(Connection* pOraConn)
{
    if( NULL != pOraConn)
    {
        dbpolllock lc;
        std::map<Connection* , int>::iterator itrmp = m_mapused.find(pOraConn);
        if(itrmp != m_mapused.end())
        {
            m_mapItr = m_maplink.find(itrmp->second);
            m_mapItr->second->push_back(pOraConn);
            m_mapused.erase(itrmp);
        }
    }
}

/****************************************************************************************
*@input      :
*@output     :
*@return     :
*@description: 从数据库连接池中获取dbid
******************************************************************************************/
int multidb::MySQLDBPool::getdbid()
{
    return m_dbid++;
}

/****************************************************************************************
*@input      : nCount 实例化的数据库连接的个数 nCount>0
*@output     :
*@return     : true:success false:fail
*@description: 初始化数据库连接的池对象，第一次使用数据库连接池之前调用，连接串通过配置读取
******************************************************************************************/
bool multidb::MySQLDBPool::InitPool(int dbid, const char* connectstr, int nCount, const char* vschema)
{
    return InitConnects(dbid, connectstr, nCount, vschema);
}


// 单数据库
singledb::MySQLDBPool singledb::MySQLDBPool::m_OradbPool;

singledb::MySQLDBPool::MySQLDBPool()
{
    m_nCount = 0;
}

/****************************************************************************************
*@input      :
*@output     :
*@return     :
*@description: 维护数据库句柄的缓冲池对象死亡时，进行清除
******************************************************************************************/
singledb::MySQLDBPool::~MySQLDBPool()
{
    ReleaseAll();
}


/****************************************************************************************
*@input      :
*@output     :
*@return     :
*@description: 不需要缓冲池时，释放缓冲池的对象
******************************************************************************************/
void singledb::MySQLDBPool::ReleaseAll()
{
    Connection* pConn;
    int i = 0;
    try
    {
        while(!m_OraqueueUnUse.empty())
        {
            pConn = m_OraqueueUnUse.back();
            m_OraqueueUnUse.pop_back();

            i++;

            if( pConn)
            {
                pConn->close();
                delete pConn;
                pConn = NULL;
            }
        }
        m_nCount = 0;
    }
    catch(...)
    {
        printf("ReleaseAll catch unknow exception!\n");
    }
}


/****************************************************************************************
*@input      :
*@output     :
*@return     :
*@description: 返回当前连接池中大小
******************************************************************************************/
int singledb::MySQLDBPool::Size() const
{
    return m_OraqueueUnUse.size();
}


/****************************************************************************************
*@input      : nCount 实例化的数据库连接的个数 nCount>0
*@output     :
*@return     : true:success false:fail
*@description: 初始化数据库连接的池对象，第一次使用数据库连接池之前调用，连接串通过配置读取
******************************************************************************************/
bool singledb::MySQLDBPool::InitPool(const char* connectstr, int nCount, const char* vschema)
{
    strcpy(m_szService_name, connectstr);
    return InitConn(nCount, vschema);
}

/****************************************************************************************
*@input      :
*@output     :
*@return     : 缓冲池对象的一个指针
*@description: 提供一个static的接口，借以实现Singleton模式
******************************************************************************************/
singledb::MySQLDBPool* singledb::MySQLDBPool::GetMySQLPool()
{
    return &m_OradbPool;
}

/****************************************************************************************
*@input      : nCount, 示例化的数据库连接个数 nCount>0
*@output     :
*@return     : true:success  false:fail
*@description: 初始化数据库连接池对象（InitPool调用)
******************************************************************************************/
bool singledb::MySQLDBPool::InitConn(int nCount, const char* vschema)
{
    char url[128] = {0};
    char user[32] = {0};
    char pass[32] = {0};

    spliteStr(m_szService_name, user, pass, url);

    m_driver = get_driver_instance();

    Connection* pConn;
    for(int i=1;i<=nCount;i++)
    {
        pConn = NULL;
        pConn = m_driver->connect(url, user, pass);
        if(pConn == NULL)
        {
            ReleaseAll();
            return false;
        }

        if(vschema)
        {
            pConn->setSchema(vschema);
        }
        m_OraqueueUnUse.push_back(pConn);
    }
    return true;
}

/****************************************************************************************
*@input      :
*@output     : 拆分连接串
*@return     : ture：success, false:failed
*@description: 拆分数据库连接串
******************************************************************************************/
bool singledb::MySQLDBPool::spliteStr(const char* constr, char* user, char* pass, char* url)
{
    const char *p = strchr(constr,'/');
    const char *pp;
    if(p)
    {
        strncpy(user, constr , p - constr );
        user[p - constr]='\0';
    }

    pp = ++p;
    p = strchr(p,'@');
    if(p)
    {
        strncpy(pass, pp , p - pp );
        pass[p - pp]='\0';
    }

    if(++p)
    {
        strcpy(url, p);
    }

    return true;
}

/****************************************************************************************
*@input      :
*@output     :
*@return     : 返回一个数据库连接的指针
*@description: 需要数据库连接时，从池中获取一个数据库连接
******************************************************************************************/
Connection* singledb::MySQLDBPool::GetConnection()
{
    Connection* pconn;
    dbpolllock lc;
    if( m_OraqueueUnUse.size() == 0)
        return NULL;

    pconn = m_OraqueueUnUse.back();
    m_OraqueueUnUse.pop_back();

    return pconn;
}

/****************************************************************************************
*@input      : pOraConn:一个数据库连接的指针
*@output     :
*@return     :
*@description: 把一个数据库连接放入池中
******************************************************************************************/
void singledb::MySQLDBPool::FreeConnection(Connection* pOraConn)
{
    if( NULL != pOraConn)
    {
        dbpolllock lc;
        m_OraqueueUnUse.push_back(pOraConn);
    }
}