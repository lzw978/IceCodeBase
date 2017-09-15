/********************************************
 **  模块名称：tmysql.h
 **  模块功能:
 **  模块简述: mysql操作基类
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_TMYSQL_H__
#define __LIB_TMYSQL_H__

#include <map>
#include <vector>
#include <stdlib.h>
#include <string>
#include "mysql.h"

namespace commbase
{
    using namespace std;

    struct ST_DBConf
    {
        string m_strHost;     // 主机地址
        string m_strUser;     // 用户名
        string m_strPwd;      // 密码
        string m_strDbName;   // 数据库
        string m_strCharSet;  // 字符集
        int    m_iPort;       // 端口
        int    m_iFlag;       // 客户端标识
        // 构造函数
        ST_DBConf() : m_iPort(0), m_iFlag(0) {}
        // map读取数据库配置
        void loadFromMap(const map<string, string> &mpParam)
        {
            map<string, string> mpTmp = mpParam;
            m_strHost    = mpTmp["dbhost"];
            m_strUser    = mpTmp["dbuser"];
            m_strPwd     = mpTmp["dbpass"];
            m_strDbName  = mpTmp["dbname"];
            m_strCharSet = mpTmp["charset"];
            m_iFlag      = 0;
            if (mpTmp["dbport"] == "")
            {
                m_iPort = 3306;
            }
            else
            {
                m_iPort = atoi(mpTmp["dbport"].c_str());
            }
        }
    };

    class TMySql
    {
    public:
        // 构造函数
        TMySql();
        // 构造函数
        TMySql(const string &sHost, const string &sUser = "", const string &sPasswd = "", const string &sDatabase = "", const string &sCharSet = "", int port = 0, int iFlag = 0);
        // 构造函数
        TMySql(const ST_DBConf &stDBconf);
        // 析构函数
        ~TMySql();
        // 初始化
        void init(const string &sHost, const string &sUser = "", const string &sPasswd = "", const string &sDatabase = "", const string &sCharSet = "", int port = 0, int iFlag = 0);
        // 初始化
        void init(const ST_DBConf &stDBconf);
        // 连接数据库
        void connect();
        // 检测连接,断开则进行重连
        void chkConn();
        // 断开连接
        void disconnect();
        // 获取数据库变量
        string getVariables(const string &sName);
        // 直接获取数据库指针
        MYSQL *getMysql();
        // 字符转义
        string escapeString(const string &sFrom);
        // 插入或更新数据
        void execute(const string &sSql);
        // 内部类：mysql的一条记录
        class MysqlRecord
        {
        public:
            // 构造函数
            MysqlRecord(const map<string, string> &record);
            // 根据字段名获取数据值
            const string& operator[](const string &s);
        protected:
            const map<string, string> &m_record;
        };
        // 内部类: 查询处理的mysql数据集
        class MysqlData
        {
        public:
            // 所有数据
            vector<map<string, string> >& data();
            // 数据记录数
            size_t size();
            // 获取某条数据
            MysqlRecord operator[](size_t i);
        protected:
            vector<map<string, string> > m_data;
        };

        // 根据sql查询记录
        MysqlData queryRecord(const string &sSql);

        // 定义字段类型
        enum FT
        {
            DB_INT, // 数字类型
            DB_STR  // 字符串类型
        };

        // 数据记录
        typedef map<string, pair<FT, string> > RECORD_DATA;

        // 更新记录
        size_t updateRecord(const string &sTableName, const RECORD_DATA &mpColumns, const string &sCondition);
        // 插入记录
        size_t insertRecord(const string &sTableName, const RECORD_DATA &mpColumns);
        // 替换记录
        size_t replaceRecord(const string &sTableName, const RECORD_DATA &mpColumns);
        // 删除记录
        size_t deleteRecord(const string &sTableName, const string &sCondition = "");
        // 获取table查询结果总笔数
        size_t getRecordCount(const string &sTableName, const string &sCondition = "");
        // 获取sql返回结果集的个数
        size_t getSqlCount(const string &sCondition = "");
        // 获取查询影响数
        size_t getAffectedRows();
        // 判断记录是否存在
        bool existRecord(const string &sSql);
        // 获取字段最大值
        int getMaxValue(const string &sTableName, const string &sFieldName, const string &sCondition = "");
        // 获取自增长列最后插入的id值
        long lastInsertID();
        // 构建insert-sql语句
        string buildInsertSQL(const string &sTableName, const RECORD_DATA &mpColumns);
        // 构建replace-sql语句
        string buildReplaceSQL(const string &sTableName, const RECORD_DATA &mpColumns);
        // 构建update-sql语句
        string buildUpdateSQL(const string &sTableName, const RECORD_DATA &mpColumns, const string &sCondition);
        // 获取最后执行的sql语句
        string getLastSQL()
        {
            return m_sLastSql;
        }
    protected:
        // 只声明不定义,保证不被使用
        TMySql(const TMySql &tMysql);
        TMySql & operator=(const TMySql &tMysql);
    private:
        MYSQL    *m_pstMsql;    // 数据库指针
        ST_DBConf m_DbConf;     // 数据库配置
        string    m_sLastSql;   // 最后执行的sql
        string    m_sErrDesc;   // 错误信息
        bool      m_bConnected; // 是否已连接
    };
}

#endif