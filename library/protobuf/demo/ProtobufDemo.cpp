/********************************************
 **  模块名称：组装和解析protobuf示例程序
 **  模块功能:
 **  模块简述: 
 **  编 写 人: lzw978
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 EntryProtocol.protobuf 定义的结构如下：
 Entry  
    Header  
        logfileName   [binlog文件名]  
        logfileOffset [binlog position]  
        executeTime   [发生的变更]  
        schemaName   
        tableName  
        eventType [insert/update/delete类型]  
    entryType   [事务头BEGIN/事务尾END/数据ROWDATA]  
    storeValue  [byte数据,可展开，对应的类型为RowChange]  
      
RowChange  
    isDdl       [是否是ddl变更操作，比如create table/drop table]  
    sql         [具体的ddl sql]  
    rowDatas    [具体insert/update/delete的变更数据，可为多条，1个binlog event事件可对应多条变更，比如批处理]  
        beforeColumns [Column类型的数组]  
        afterColumns  [Column类型的数组]  
          
Column   
    index         
    sqlType     [jdbc type]  
    name        [column name]  
    isKey       [是否为主键]  
    updated     [是否发生过变更]  
    isNull      [值是否为null]  
    value       [具体的内容，注意为文本]  

 **  问    题：
 **  编译依赖于安装好的protobuf开源库
 g++ -g -o ProtobufDEMO -I ../include/ -L../lib -lprotobuf ProtobufDemo.cpp EntryProtocol.pb.cc
*********************************************/

#include <unistd.h>
#include <iostream>
#include <string>
#include "EntryProtocol.pb.h"

using namespace std;
using namespace com;
using namespace alibaba;
using namespace otter;
using namespace canal;
using namespace protocol;

int main(int argc, char *argv[])
{
    string strHead;
    string strBody;
    protocol::Entry* msg_test = new protocol::Entry();
    msg_test->set_entrytype(protocol::ROWDATA);

    // 设置头
    protocol::Header* msg_head = msg_test->mutable_header();
    msg_head->set_tablename("TEST");
    msg_head->set_eventtype(protocol::INSERT);


    // 设置体：存放在头的二进制数组中
    protocol::RowChange* msg_rowchange = new protocol::RowChange();
    msg_rowchange->set_tableid(1);
    msg_rowchange->set_eventtype(protocol::INSERT);

    protocol::RowData* msg_rowdata = msg_rowchange->add_rowdatas();
    protocol::RowData* msg_rowdata2 = msg_rowchange->add_rowdatas();

    // rowdata1-column1
    protocol::Column *msg_column = msg_rowdata->add_aftercolumns();
    msg_column->set_index(0);
    msg_column->set_name("name");
    msg_column->set_value("lzw978");
    msg_column->set_updated(true);
    // rowdata1-column2
    protocol::Column *msg_column2 = msg_rowdata->add_aftercolumns();
    msg_column2->set_index(1);
    msg_column2->set_name("address");
    msg_column2->set_value("shenzhen");
    msg_column2->set_updated(true);

    // rowdata2-column1
    protocol::Column *msg_column3 = msg_rowdata2->add_aftercolumns();
    msg_column3->set_index(0);
    msg_column3->set_name("name");
    msg_column3->set_value("中文");
    msg_column3->set_updated(true);
    // rowdata2.column2
    protocol::Column *msg_column4 = msg_rowdata2->add_aftercolumns();
    msg_column4->set_index(1);
    msg_column4->set_name("address");
    msg_column4->set_value("深圳");
    msg_column4->set_updated(true);


    // 序列化转换报文体
    msg_rowchange->SerializeToString(&strBody);
    cout << "body = [" <<  strBody << "] "  << "  length=" << strBody.length()<< endl;

    // 序列化转换头
    msg_test->set_storevalue(strBody);
    msg_test->SerializeToString(&strHead);
    cout << "head = [" <<  strHead << "] " << "  length=" << strHead.length() << endl;

    
    
    // 解析测试
    Entry parseMsg;
    parseMsg.ParseFromString(strHead);
    cout << "head tablename = [" << parseMsg.header().tablename() << "]" << endl;
    if(parseMsg.has_header())
    {
        RowChange rowChgBody;
        rowChgBody.ParseFromString(parseMsg.storevalue());

        int nRowDatas = rowChgBody.rowdatas_size();
        for(int i=0; i<nRowDatas; ++i)
        {
            int nColumn = rowChgBody.rowdatas(i).aftercolumns_size();
            for(int j=0; j<nColumn; ++j)
            {
                 Column afterColumn = rowChgBody.rowdatas(i).aftercolumns(j);
                 cout << "index = [" << afterColumn.index() << "] name = [" << afterColumn.name() << "] value = ["<< afterColumn.value() << "]"<< endl;
            }
        }
    }

    return 0;
}
