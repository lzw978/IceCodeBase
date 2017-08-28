/********************************************
 **  模块名称：stream.h
 **  模块功能:
 **  模块简述: 输入输出流基类
 **  编 写 人:
 **  日    期: 2017.08.21
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_STREAM_H__
#define __LIB_STREAM_H__

#include "exception.h"

namespace commbase
{
    // 流对象基类
    class Stream
    {
    public:
        Stream(){};
        virtual ~Stream();
        // 关闭
        virtual void close() = 0;
        // 判断是否打开
        virtual bool isOpened() = 0;
        // 写入流
        virtual size_t write(const char* buffer, size_t count) = 0;
        // 读取流
        virtual size_t read(char* buffer, size_t count) = 0;
        // 偏移
        virtual bool seek(off_t offset, int mode) = 0;
    };
}

#endif