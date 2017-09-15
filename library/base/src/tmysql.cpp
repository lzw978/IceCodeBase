/********************************************
 **  模块名称：tmysql.cpp
 **  模块功能:
 **  模块简述: mysql操作基类实现
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <sstream>
#include <string.h>
#include "tmysql.h"
#include "exception.h"

using namespace commbase;

// 构造函数
TMySql::TMySql() : m_bConnected(false)
{
    m_pstMsql = mysql_init(NULL);
}
// 构造函数
TMySql::TMySql(const string &sHost, const string &sUser, const string &sPasswd, const string &sDatabase, const string &sCharSet, int port, int iFlag): m_bConnected(false)
{
    init(sHost, sUser, sPasswd, sDatabase, sCharSet, port, iFlag);
    m_pstMsql = mysql_init(NULL);
}
// 构造函数
TMySql::TMySql(const ST_DBConf &stDBconf) : m_bConnected(false)
{
    m_DbConf = stDBconf;
    m_pstMsql  = mysql_init(NULL);
}
// 析构函数
TMySql::~TMySql()
{
    if (m_pstMsql != NULL)
    {
        mysql_close(m_pstMsql);
        m_pstMsql = NULL;
    }
}
// 初始化
void TMySql::init(const string &sHost, const string &sUser, const string &sPasswd, const string &sDatabase, const string &sCharSet, int port, int iFlag)
{
    m_DbConf.m_strHost    = sHost;
    m_DbConf.m_strUser    = sUser;
    m_DbConf.m_strPwd     = sPasswd;
    m_DbConf.m_strDbName  = sDatabase;
    m_DbConf.m_strCharSet = sCharSet;
    m_DbConf.m_iPort      = port;
    m_DbConf.m_iFlag      = iFlag;
}
// 初始化
void TMySql::init(const ST_DBConf &stDBconf)
{
    m_DbConf = stDBconf;
}
// 连接数据库
void TMySql::connect()
{
    disconnect();
    if (m_pstMsql == NULL)
    {
        m_pstMsql = mysql_init(NULL);
    }

    // 设置字符集
    if ( !m_DbConf.m_strCharSet.empty())
    {
        if (mysql_options(m_pstMsql, MYSQL_SET_CHARSET_NAME, m_DbConf.m_strCharSet.c_str()))
        {
            m_sErrDesc = "connect : set charset error [" + m_DbConf.m_strCharSet + "]:" + string(mysql_error(m_pstMsql));
            throw Exception(Z_SOURCEINFO, -1, m_sErrDesc);
        }
    }
    // 连接数据库
    if (mysql_real_connect(m_pstMsql, m_DbConf.m_strHost.c_str(), m_DbConf.m_strUser.c_str(), m_DbConf.m_strPwd.c_str(),
                           m_DbConf.m_strDbName.c_str(), m_DbConf.m_iPort, NULL, m_DbConf.m_iFlag) == NULL )
    {
        m_sErrDesc = "connect : mysql_real_connect error: " + string(mysql_error(m_pstMsql));
        throw Exception(Z_SOURCEINFO, -1, m_sErrDesc);
    }
    m_bConnected = true;
}
// 检测连接
void TMySql::chkConn()
{
    if (!m_bConnected)
    {
        connect();
    }
}
// 断开连接
void TMySql::disconnect()
{
    if (m_pstMsql != NULL)
    {
        mysql_close(m_pstMsql);
        m_pstMsql = mysql_init(NULL);
    }
    m_bConnected = false;
}
// 获取数据库变量
string TMySql::getVariables(const string &sName)
{
    string sql = "SHOW VARIABLES LIKE '" + sName + "'";

    MysqlData data = queryRecord(sql);
    if (data.size() == 0)
    {
        return "";
    }

    if (sName == data[0]["Variable_name"])
    {
        return data[0]["Value"];
    }
    return "";
}
// 直接获取数据库指针
MYSQL* TMySql::getMysql()
{
    return m_pstMsql;
}
// 字符转义
string TMySql::escapeString(const string &sFrom)
{
    chkConn();

    string sTo;
    string::size_type iLen = sFrom.length() * 2 + 1;
    char *pTo = (char *)malloc(iLen);
    memset(pTo, 0, iLen);

    mysql_real_escape_string(m_pstMsql, pTo, sFrom.c_str(), sFrom.length());
    sTo = pTo;
    free(pTo);

    return sTo;
}
// 插入或更新数据
void TMySql::execute(const string &sSql)
{
    chkConn();

    m_sLastSql = sSql;
    int iRet = mysql_real_query(m_pstMsql, sSql.c_str(), sSql.length());
    if (iRet != 0)
    {
        // 获取错误码,判断是否连接失效
        int iErrno = mysql_errno(m_pstMsql);
        if (iErrno == 2013 || iErrno == 2006)
        {
            // 重连再次查询
            connect();
            int iRet = mysql_real_query(m_pstMsql, sSql.c_str(), sSql.length());
        }
    }
    if (iRet != 0)
    {
        m_sErrDesc = "query: mysql_real_query error: [" + sSql + "] errmsg=" + string(mysql_error(m_pstMsql));
        throw Exception(Z_SOURCEINFO, -1, m_sErrDesc);
    }
}

// 更新记录
size_t TMySql::updateRecord(const string &sTableName, const RECORD_DATA &mpColumns, const string &sCondition)
{
    string sSql = buildUpdateSQL(sTableName, mpColumns, sCondition);
    execute(sSql);
    return mysql_affected_rows(m_pstMsql);
}
// 插入记录
size_t TMySql::insertRecord(const string &sTableName, const RECORD_DATA &mpColumns)
{
    string sSql = buildInsertSQL(sTableName, mpColumns);
    execute(sSql);
    return mysql_affected_rows(m_pstMsql);
}
// 替换记录
size_t TMySql::replaceRecord(const string &sTableName, const RECORD_DATA &mpColumns)
{
    string sSql = buildReplaceSQL(sTableName, mpColumns);
    execute(sSql);
    return mysql_affected_rows(m_pstMsql);
}
// 删除记录
size_t TMySql::deleteRecord(const string &sTableName, const string &sCondition)
{
    ostringstream sSql;
    sSql << "delete from " << sTableName << " " << sCondition;
    execute(sSql.str());
    return mysql_affected_rows(m_pstMsql);
}
// 获取table查询结果总笔数
size_t TMySql::getRecordCount(const string &sTableName, const string &sCondition)
{
    ostringstream sSql;
    sSql << "select count(1) as num from " << sTableName << " " << sCondition;
    MysqlData data = queryRecord(sSql.str());
    long n = atol(data[0]["num"].c_str());
    return n;
}
// 获取sql返回结果集的个数
size_t TMySql::getSqlCount(const string &sCondition)
{
    ostringstream sSql;
    sSql << "select count(1) as num " << sCondition;
    MysqlData data = queryRecord(sSql.str());
    long n = atol(data[0]["num"].c_str());
    return n;
}
// 获取查询影响数
size_t TMySql::getAffectedRows()
{
    return mysql_affected_rows(m_pstMsql);
}
// 判断记录是否存在
bool TMySql::existRecord(const string &sSql)
{
    return queryRecord(sSql).size() > 0;
}
// 获取字段最大值
int TMySql::getMaxValue(const string &sTableName, const string &sFieldName, const string &sCondition)
{
    ostringstream sSql;
    sSql << "select " << sFieldName << " as f from " << sTableName << " " << sCondition << " order by f desc limit 1";
    MysqlData data = queryRecord(sSql.str());
    int n = 0;
    if (data.size() == 0)
    {
        n = 0;
    }
    else
    {
        n = atoi(data[0]["f"].c_str());
    }
    return n;
}
// 获取自增长列最后插入的id值
long TMySql::lastInsertID()
{
    return mysql_insert_id(m_pstMsql);
}
// 构建insert-sql语句
string TMySql::buildInsertSQL(const string &sTableName, const RECORD_DATA &mpColumns)
{
    ostringstream sColumnNames;
    ostringstream sColumnValues;

    RECORD_DATA::const_iterator itEnd = mpColumns.end();
    for (RECORD_DATA::const_iterator it = mpColumns.begin(); it != itEnd; ++it)
    {
        // 数据库列名增加``,值为数字类型保持原来，字符串类型则增加''
        if (it == mpColumns.begin())
        {
            sColumnNames << "`" << it->first << "`";
            if (it->second.first == DB_INT)
            {
                sColumnValues << it->second.second;
            }
            else
            {
                sColumnValues << "'" << escapeString(it->second.second) << "'";
            }
        }
        else
        {
            sColumnNames << ",`" << it->first << "`";
            if(it->second.first == DB_INT)
            {
                sColumnValues << "," + it->second.second;
            }
            else
            {
                sColumnValues << ",'" + escapeString(it->second.second) << "'";
            }
        }
    }

    ostringstream os;
    os << "insert into " << sTableName << " (" << sColumnNames.str() << ") values (" << sColumnValues.str() << ")";
    return os.str();
}
// 构建replace-sql语句
string TMySql::buildReplaceSQL(const string &sTableName, const RECORD_DATA &mpColumns)
{
    ostringstream sColumnNames;
    ostringstream sColumnValues;

    RECORD_DATA::const_iterator itEnd = mpColumns.end();
    for (RECORD_DATA::const_iterator it = mpColumns.begin(); it != itEnd; ++it)
    {
        // 数据库列名增加``,值为数字类型保持原来，字符串类型则增加''
        if (it == mpColumns.begin())
        {
            sColumnNames << "`" << it->first << "`";
            if (it->second.first == DB_INT)
            {
                sColumnValues << it->second.second;
            }
            else
            {
                sColumnValues << "'" << escapeString(it->second.second) << "'";
            }
        }
        else
        {
            sColumnNames << ",`" << it->first << "`";
            if(it->second.first == DB_INT)
            {
                sColumnValues << "," + it->second.second;
            }
            else
            {
                sColumnValues << ",'" + escapeString(it->second.second) << "'";
            }
        }
    }
    ostringstream os;
    os << "replace into " << sTableName << " (" << sColumnNames.str() << ") values (" << sColumnValues.str() << ")";
    return os.str();
}
// 构建update-sql语句
string TMySql::buildUpdateSQL(const string &sTableName, const RECORD_DATA &mpColumns, const string &sCondition)
{
    ostringstream sColumnNameValueSet;
    RECORD_DATA::const_iterator itEnd = mpColumns.end();

    for (RECORD_DATA::const_iterator it = mpColumns.begin(); it != itEnd; ++it)
    {
        if (it == mpColumns.begin())
        {
            sColumnNameValueSet << "`" << it->first << "`";
        }
        else
        {
            sColumnNameValueSet << ",`" << it->first << "`";
        }

        if(it->second.first == DB_INT)
        {
            sColumnNameValueSet << "= " << it->second.second;
        }
        else
        {
            sColumnNameValueSet << "= '" << escapeString(it->second.second) << "'";
        }
    }
    ostringstream os;
    os << "update " << sTableName << " set " << sColumnNameValueSet.str() << " " << sCondition;
    return os.str();
}
// 查询数据库记录
TMySql::MysqlData TMySql::queryRecord(const string &sSql)
{
    chkConn();

    MysqlData data;
    m_sLastSql = sSql;

    int iRet = mysql_real_query(m_pstMsql, sSql.c_str(), sSql.length());
    if (iRet != 0)
    {
        int iErrno = mysql_errno(m_pstMsql);
        if (iErrno == 2013 || iErrno == 2006)
        {
            connect();
            iRet = mysql_real_query(m_pstMsql, sSql.c_str(), sSql.length());
        }
    }

    if (iRet != 0)
    {
        m_sErrDesc = "query: queryRecord error: [" + sSql + "] errmsg=" + string(mysql_error(m_pstMsql));
        throw Exception(Z_SOURCEINFO, -1, m_sErrDesc);
    }

    MYSQL_RES *pstRes = mysql_store_result(m_pstMsql);
    if (pstRes == NULL)
    {
        m_sErrDesc = "query: mysql_store_result error: [" + sSql + "] errmsg=" + string(mysql_error(m_pstMsql));
        throw Exception(Z_SOURCEINFO, -1, m_sErrDesc);
    }

    vector<string> vtFields;
    MYSQL_FIELD *field;
    while ((field  = mysql_fetch_field(pstRes)))
    {
        vtFields.push_back(field->name);
    }

    map<string, string> mpRow;
    MYSQL_ROW stRow;
    while ((stRow = mysql_fetch_row(pstRes)) != (MYSQL_ROW)NULL)
    {
        mpRow.clear();
        unsigned long *lengths = mysql_fetch_lengths(pstRes);
        for (size_t i = 0; i < vtFields.size(); ++i )
        {
            if (stRow[i])
            {
                mpRow[vtFields[i]] = string(stRow[i], lengths[i]);
            }
            else
            {
                mpRow[vtFields[i]] = "";
            }
        }
        data.data().push_back(mpRow);
    }
    mysql_free_result(pstRes);
    return data;
}
///////////////////// 内部类 MysqlRecord //////////////////////
TMySql::MysqlRecord::MysqlRecord(const map<string, string> &record) : m_record(record)
{
}
// 根据字段名获取数据值
const string& TMySql::MysqlRecord::operator[](const string &s)
{
    map<string, string>::const_iterator it = m_record.find(s);
    if (it == m_record.end())
    {
        throw Exception(Z_SOURCEINFO, -1, "field [" + s + "] not exists");
    }
    return it->second;
}

///////////////////// 内部类 MysqlData//////////////////////
// 所有数据
vector<map<string, string> >& TMySql::MysqlData::data()
{
    return m_data;
}
// 数据记录数
size_t TMySql::MysqlData::size()
{
    return m_data.size();
}
// 获取某条数据
TMySql::MysqlRecord TMySql::MysqlData::operator[](size_t i)
{
    return MysqlRecord(m_data[i]);
}
