/*
 * =====================================================================================
 *
 *       Filename:   tmysqlDemo.cpp
 *
 *    Description:   数据库类Demo程序
 *
 *        Version:  1.0
 *        Created:  9/18/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o tmysqlDemo -I../../include -I/usr/include/mysql -L../../ -lCommBase -L/usr/lib64/mysql -lmysqlclient -lrt -ldl tmysqlDemo.cpp
 * =====================================================================================
 */

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include "tmysql.h"

using namespace std;
using namespace commbase;



int main(int argc, char **argv)
{
    TMySql mysql;
    try
    {
        mysql.init("192.168.128.1", "root", "root", "uhome");
        mysql.connect();
    }
    catch(...)
    {
        cout << "connect database error" << endl;
        return -1;
    }

    try
    {
        TMySql::MysqlData data;
        data = mysql.queryRecord("select * from base_code_define");
        for(size_t i = 0; i < data.size(); ++i)
        {
            cout << data[i]["base_id"] << endl;
            cout << data[i]["base_name"] << endl;
        }
        //mysql.execute();
    }
    catch(...)
    {
        cout << "execute sql error" << endl;
        return -1;
    }

    return 0;
}