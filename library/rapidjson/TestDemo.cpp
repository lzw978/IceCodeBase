/*
 * =====================================================================================
 *
 *       Filename:  TestDemo.cpp
 *
 *    Description:  测试rapidJson组多笔循环Json报文
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (lzw978),
 *   Organization:
 *   目录下执行编译：g++ -g -o JsonDemo TestDemo.cpp
 * =====================================================================================
 */
#include <stdio.h>
#include <string>
#include <iostream>

#include "document.h"
#include "prettywriter.h"
#include "filestream.h"
#include "stringbuffer.h"

using namespace std;
using namespace rapidjson;

int main(int argc, char *argv[])
{
    Document document;
    Document::AllocatorType& allocator = document.GetAllocator();

    Value root(kObjectType);
    Value array(kArrayType);

    // 根节点
    root.AddMember("name", "test", allocator);
    root.AddMember("address", "shenzhen", allocator);
    root.AddMember("age", "20", allocator);

    // 增加数组
    for(int i=0;i<2;++i)
    {
        Value object(kObjectType);
        object.AddMember("id", i, allocator);
        object.AddMember("class", "TestOk", allocator);

        // 添加到数组中
        array.PushBack(object, allocator);
    }
    root.AddMember("data", array, allocator);

    // 打印组出来之后的结果
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    root.Accept(writer);
    string result = buffer.GetString();

    cout << "result :" << result << " size=: " << result.size() << endl;


    // 解析测试
    Document parseJsonMsg;
    parseJsonMsg.Parse<0>(result.c_str());
    Value &rootName = parseJsonMsg["name"];
    Value &rootAddr = parseJsonMsg["address"];
    Value &rootAge  = parseJsonMsg["age"];
    Value &rootArray= parseJsonMsg["data"];
    cout << "name = " << rootName.GetString() << endl;
    cout << "addr = " << rootAddr.GetString() << endl;
    cout << "age  = " << rootAge.GetString()  << endl;

    if( rootArray.IsArray())
    {
        for(int i=0; i< rootArray.Size(); ++i)
        {
            Value &arrValue = rootArray[i];
            if(arrValue.HasMember("id") && arrValue["id"].IsString())
            {
                cout << "id = " << arrValue["id"].GetString() << endl;
            }
            if(arrValue.HasMember("class") && arrValue["class"].IsString())
            {
                cout << "class = " << arrValue["class"].GetString() << endl;
            }
        }
    }

    return 0;
}