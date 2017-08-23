/*
 * =====================================================================================
 *
 *       Filename:  tconfigDemo.cpp
 *
 *    Description:  读取配置文件demo
 *
 *        Version:  1.0
 *        Created:  08/23/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *   编译src目录下执行：
 *   g++ -g -o CONFIGDEMO -I../include -L../ -L../../tinyxml -L../../base -lConfigXml -lTinyXml -lCommBase -lrt tconfigDemo.cpp
 * =====================================================================================
 */
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tconfigbase.h"

using namespace std;
int main(int argc, char *argv[])
{
    string strValue;
    int iValue = 0;
    // 加载配置ini文件
    TConfigBase configIniFile;
    configIniFile.LoadIniFile("../../../etc/test.ini");

    // 读取配置文件内容
    cout << "test ini file" << endl;
    strValue = configIniFile.GetTagValue("CONN_TO_DB", "SCHEMA", "");
    cout << "ini SCHEMA = [" << strValue << "]" << endl;
    strValue = configIniFile.GetTagValue("CONN_TO_DB", "CONN_STRING", "");
    cout << "ini CONN_STRING = [" << strValue << "]" << endl;
    iValue   = configIniFile.GetTagValue("CONN_TO_DB", "LINK_NUM", 1);
    cout << "ini LINK_NUM = [" << iValue << "]" << endl;
    strValue = configIniFile.GetTagValue("LOG", "LOG_PATH", "");
    cout << "ini LOG_PATH = [" << strValue << "]" << endl;


    // 加载XML配置文件
    TConfigBase configXmlFile;
    configXmlFile.LoadXmlFile("../../../etc/base.xml");
    cout << "test XML file" << endl;
    strValue = configXmlFile.GetTagValue("LOGINFO", "LOGPATH", "");
    cout << "xml LOGPATH = [" << strValue << "]" << endl;
    strValue = configXmlFile.GetTagValue("LOGINFO", "ERRLOGNAME", "");
    cout << "xml ERRLOGNAME = [" << strValue << "]" << endl;
    iValue   = configXmlFile.GetTagValue("LOGINFO", "LOGLEVEL", 1);
    cout << "xml LOGLEVEL = [" << iValue << "]" << endl;
    strValue = configXmlFile.GetTagValue("DBINFO", "DBURL", "");
    cout << "xml DBURL = [" << strValue << "]" << endl;


    return 0;
}