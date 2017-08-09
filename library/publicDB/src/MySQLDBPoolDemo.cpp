/*
 * =====================================================================================
 *
 *       Filename:  MySQLDBPool.cpp
 *
 *    Description:  test logger
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (lzw978), 
 *   Organization:  
 *   编译src目录下执行：
 *   g++ -g -o MYDEMO -I../include -L../ -lPubDB MySQLDBPoolDemo.cpp 
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "MySQLDBPool.h"

using namespace std;
using namespace multidb;

void TestMySQLPool();

int main(int argc, char* argv[])
{
    TestMySQLPool();
    return 0;
}

void TestMySQLPool()
{
    // 配置信息
    int  db_num = 0;
    char schema[32]          = "lzw978";                     // 数据库名
    char dbMySqlConnStr[64]  = "root/root@127.0.0.1:3306";   // 连接串
    char dbMysqlLinkNums[16] = "10";                         // 连接数数

    // 通过连接池连接数据库
    bool succ = MySQLDBPool::GetMySQLPool()->InitPool(db_num, dbMySqlConnStr, atoi(dbMysqlLinkNums), schema);
    int conns = MySQLDBPool::GetMySQLPool()->Size(db_num);

    printf("issucc=[%d] conns=[%d]\n", succ, conns);
    if( !succ || (conns!= atoi(dbMysqlLinkNums)))
    {
        printf("Init db connection failed\n");
        return ;
    }

    // 连接sql
    Connection *m_connect;
    m_connect = MySQLDBPool::GetMySQLPool()->GetConnection(db_num);
    int count =100;
    while(!m_connect)
    {
        count--;
        if(count < 1)
        {
            m_connect = NULL;
            return;
        }
        m_connect = MySQLDBPool::GetMySQLPool()->GetConnection(db_num);
    }
    m_connect->setAutoCommit(true);

    proxStream poxstrm;
    poxstrm.m_con = m_connect;
    if( NULL == poxstrm.m_con )
    {
        printf("link error\n");
        return ;
    }

    // 查询数据库
    try
    {
        poxstrm.m_pstmt = poxstrm.m_con->prepareStatement("select sno from student");

        poxstrm.m_res = poxstrm.m_pstmt->executeQuery();

        while(poxstrm.m_res->next())
        {
            string sno = poxstrm.m_res->getString(1);

            cout << "SNO=[" << sno << "]" << endl;
        }
    }
    catch(SQLException& e)
    {
        printf("exception error code=[%d] errmsg=[%s]\n", e.getErrorCode(), e.what());
    }
    catch(...)
    {
        printf("unknown exception\n");
    }

    //断开连接
    MySQLDBPool::GetMySQLPool()->FreeConnection(m_connect);
}



