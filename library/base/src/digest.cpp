/********************************************
 **  模块名称：digest.h
 **  模块功能:
 **  模块简述: 常用摘要算法基类实现
 **  编 写 人:
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <memory>
#include "digest.h"
#include "membuffer.h"
#include "file.h"
#include "logger.h"


using namespace commbase;

Digest::Digest()
{
}

Digest::~Digest()
{
}

// 从字符串读取
string Digest::fromString(string buffer)
{
    clear();
    update(buffer.c_str(), buffer.size());
    return digest();
}

// 从文件读取
string Digest::fromFile(string filePath) throw (Exception)
{
    clear();

    try
    {
        File doFile(filePath.c_str(), "rb");
        FileInfo fileInfo = File::getFileInfo(filePath.c_str());

        // 申请缓存区
        const size_t kBufferSize = 1024 * 10;
        //auto_ptr<char> pBuffer(new char[kBufferSize+1]);
        MemBuffer buf;
        buf.truncate(kBufferSize+1);
        char *pBuffer = buf.getBuffer();

        off_t fileSize = fileInfo.size(); // 文件长度(字节)
        off_t readedAllBytes = 0;         // 已读字节

        size_t perBytesToRead = kBufferSize;  // 预读长度
        size_t perBytesReaded = 0;            // 已读长度

        // 读取文件内容
        for ( ;readedAllBytes<fileSize; )
        {
            // 判断文件产地与已读长度
            if( (fileSize-readedAllBytes) < (int)kBufferSize)
            {
                // 当文件长度-已读长度<缓存区长度时, 预读长度为两者之差, 发生在最后一次读取时
                perBytesToRead = (size_t) (fileSize - readedAllBytes);
            }
            else
            {
                //如果文件长度-已读长度>=缓冲区长度，则 预读长度=缓冲区长度
                perBytesToRead = kBufferSize;
            }
            // 初始化
            memset(pBuffer, 0x00, kBufferSize);
            // 安装设定长度读取文件内容
            perBytesReaded = doFile.read(pBuffer, perBytesToRead);
            // 更新摘要
            update(pBuffer, perBytesReaded);
            // 增加已读长度
            readedAllBytes += perBytesReaded;
        }
    }
    catch (Exception& e)
    {
        Trace(L_ERROR, NULL, "digest error[%s]", e.what());
    }

    return digest();
}