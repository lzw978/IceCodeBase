/********************************************
 **  模块名称：MySQLDBPool.h
 **  模块功能:
 **  模块简述: Mysql数据库连接池
 **  编 写 人: 
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#ifndef __DB__MYSQL_POOL_H__
#define __DB__MYSQL_POOL_H__

#include <list>
#include "mysql_connection.h"
#include "mysql_driver.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include "pthread.h"

using namespace sql;

class dbpolllock
{
private:
    void lock();
    void unlock();
public:
    dbpolllock();
    ~dbpolllock();
};

// 多数据库
namespace multidb
{
    class MySQLDBPool
    {
    public:
        // 获取缓冲池对象的一个指针
        static MySQLDBPool *GetMySQLPool();

        // 释放缓冲池对象
        void ReleaseAll();

        /****************************************************************************************
        *@input        : nCount:实例化的数据库连接的个数:nCount>0.
        *@output       :
        *@return       : 初始化成功返回TRUE；失败返回FALSE
        *@description  : 初始化数据库连接的池对象, 第一次使用数据库连接池之前调用,连接串读配置获取
        ******************************************************************************************/
        bool InitPool(int dbid, const char* connectstr, int nCount, const char* vschema=NULL);

        /****************************************************************************************
        *@input        : nCount:实例化的数据库连接的个数:nCount>0.
        *@output       :
        *@return       : 初始化成功返回TRUE；失败返回FALSE
        *@description  : 初始化数据库连接的池对象, 第一次使用数据库连接池之前调用,连接串读配置获取
        ******************************************************************************************/
        bool InitConnects(int dbid, const char* connectstr, int nCount, const char* vschema=NULL);

        /****************************************************************************************
        *@input        :
        *@output       :
        *@return       : 返回一个数据库连接的指针
        *@description  : 从池中取出一个数据库连接：注意必须和FreeConnection函数一一对应
        ******************************************************************************************/
        Connection* GetConnection(int dbid);

        /****************************************************************************************
        *@input        : 一个数据库连接的执政
        *@output       :
        *@return       : 需要释放一个数据库连接时
        *@description  : 把一个数据库连接放入池中：注意必须和GetConnection函数一一对应
        ******************************************************************************************/
        void FreeConnection(Connection *pOraConn);

        /****************************************************************************************
        *@input        :
        *@output       :
        *@return       : 获取池中有多少个连接
        *@description  : 需要获取池中有多少个连接时
        ******************************************************************************************/
        int Size(int dbid=1);

        /****************************************************************************************
        *@input        :
        *@output       :
        *@return       :
        *@description  : 从数据库连接池中获取dbid
        ******************************************************************************************/
        int getdbid();

    private:
        // 初始化连接
        bool InitConn(int dbid, const char* connectstr, int nCount, const char* vschema=NULL);

        // 拆分连接串
        bool spliteStr(const char* constr, char* user, char* pass, char* url);

        // 构造
        MySQLDBPool();

        // 析构
        ~MySQLDBPool();

    private:
        Driver* m_driver;
        std::map<int, std::list<Connection*>* > m_maplink;          // 数据库句柄队列
        std::list<Connection*>* m_OraqueueUnUse;                    // 未使用数据库句柄队列
        std::map<Connection* , int > m_mapused;                     // 存放已经使用的连接和db的连接关系
        std::map<int, std::list<Connection*>* >::iterator m_mapItr; // 连接句柄迭代器
        static MySQLDBPool m_OradbPool;                             // 采用单例模式，设置static变量
        static int m_dbid;                                          // dbid,保证一个数据库连接串一个dbid
    };

    class proxStream
    {
    public:
        proxStream()
        {
            m_res   = NULL;
            m_con   = NULL;
            m_stmt  = NULL;
            m_pstmt = NULL;
        }

        ~proxStream()
        {
            if(m_res)
                delete m_res;
            if(m_stmt)
                delete m_stmt;
            if(m_pstmt)
                delete m_pstmt;
            if(m_con)
            {
                MySQLDBPool::GetMySQLPool()->FreeConnection(m_con);
            }
        }

        Connection* m_con;          // 连接句柄
        ResultSet*  m_res;          // 结果集
        Statement*  m_stmt;         // 查询语句
        PreparedStatement* m_pstmt; // 预处理查询语句
    };
};


// 单数据库
namespace singledb
{
class MySQLDBPool
    {
    public:
        // 获取缓冲池对象的一个指针
        static MySQLDBPool *GetMySQLPool();

        // 释放缓冲池对象
        void ReleaseAll();

        /****************************************************************************************
        *@input        : nCount:实例化的数据库连接的个数:nCount>0.
        *@output       :
        *@return       : 初始化成功返回TRUE；失败返回FALSE
        *@description  : 初始化数据库连接的池对象, 第一次使用数据库连接池之前调用,连接串读配置获取
        ******************************************************************************************/
        bool InitPool(const char* connectstr, int nCount, const char* vschema=NULL);

        /****************************************************************************************
        *@input        :
        *@output       :
        *@return       : 返回一个数据库连接的指针
        *@description  : 从池中取出一个数据库连接：注意必须和FreeConnection函数一一对应
        ******************************************************************************************/
        Connection* GetConnection();

        /****************************************************************************************
        *@input        : 一个数据库连接的执政
        *@output       :
        *@return       : 需要释放一个数据库连接时
        *@description  : 把一个数据库连接放入池中：注意必须和GetConnection函数一一对应
        ******************************************************************************************/
        void FreeConnection(Connection *pOraConn);

        /****************************************************************************************
        *@input        :
        *@output       :
        *@return       : 获取池中有多少个连接
        *@description  : 需要获取池中有多少个连接时
        ******************************************************************************************/
        int Size() const;

    private:
        // 初始化连接
        bool InitConn(int nCount, const char* vschema=NULL);

        // 拆分连接串
        bool spliteStr(const char* constr, char* user, char* pass, char* url);

        // 构造
        MySQLDBPool();

        // 析构
        ~MySQLDBPool();

    private:
        Driver* m_driver;
        int m_nCount;                             // 当前数据库连接个数
        char m_szService_name[256];               // 数据库连接串
        std::list<Connection*> m_OraqueueUnUse;   // 未使用数据库句柄队列
        static MySQLDBPool m_OradbPool;           // 采用单例模式，设置static变量
    };

    class proxStream
    {
    public:
        proxStream()
        {
            m_res   = NULL;
            m_con   = NULL;
            m_stmt  = NULL;
            m_pstmt = NULL;
        }

        ~proxStream()
        {
            if(m_res)
                delete m_res;
            if(m_stmt)
                delete m_stmt;
            if(m_pstmt)
                delete m_pstmt;
            if(m_con)
            {
                MySQLDBPool::GetMySQLPool()->FreeConnection(m_con);
            }
        }

        Connection* m_con;          // 连接句柄
        ResultSet*  m_res;          // 结果集
        Statement*  m_stmt;         // 查询语句
        PreparedStatement* m_pstmt; // 预处理查询语句
    };
};

#endif
