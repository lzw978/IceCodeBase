/********************************************
 **  模块名称：digest.h
 **  模块功能:
 **  模块简述: 常用摘要算法基类
 **  编 写 人:
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_DIGEST_H__
#define __LIB_DIGEST_H__

#include <string>
#include "exception.h"

namespace commbase
{
    using namespace std;

    class Digest
    {
    public:
        Digest();
        virtual ~Digest();

        // 更新摘要：一般摘要都是对大块内存分块计算
        virtual void update(const char* buffer, size_t len) = 0;
        // 处理字符串
        string fromString(string buffer);
        // 从文件读取
        string fromFile(string filePath) throw (Exception);
        // 获取结果
        virtual string digest() = 0;
        // 清除结果
        virtual void clear() = 0;
    };
}

#endif