/*
* @file        memBuffer.h
* @brief       原始内存块简单封装
*/
#ifndef __LIB_MEMBUFFER_H__
#define __LIB_MEMBUFFER_H__

#include <cstring>
#include <string>
#include <malloc.h>

namespace commbase
{
    using namespace std;

    //! 轻量级内存操作类
    class MemBuffer
    {
    public:
        MemBuffer();
        MemBuffer(const MemBuffer&buf);
        MemBuffer& operator=(const MemBuffer&buf);
        ~MemBuffer();

        //! 返回可用内存区(末尾置0)
        char * getBuffer();

        //! 返回可用内存区
        void * getRawBuffer() const;

        //! 已有数据大小
        const unsigned int& size() const ;

        //! 总共分配内存大小
        unsigned int totalMemory();

        //! 保证有len空闲
        int more(unsigned int len);

        //! 添加数据
        int append(const void *data, unsigned int datlen);

        //! 添加数据
        int append(MemBuffer&buf);

        //! 添加数据
        int append(const std::string&str);

        //! 至少总计size内存
        void reserve(unsigned int size_);

        //! 重置并初始化内存为'\0'
        void reset();

        //! 截断至长度len
        void truncate( unsigned int len );

        //! 打印到缓存
        int format( const char *fmt, ... );

        //! 保存到文件
        int saveToFile(const string filePath, bool bAppend=false) const;

        //! 从文件读入
        int loadFromFile(const string filePath, unsigned int nOffset=0, unsigned int nCount=0);

        //! 调试信息
        void dumpInfo(unsigned int dumpCount=96);

    private:
        void _init(unsigned int preAlloc);

        // 用效内存移到头部
        void align();

        char *pBuffer_;
        char *pOrigBuffer_;

        unsigned int nMisalign_;
        unsigned int nTotalLength_;
        unsigned int nOff_;
    };

}

#endif
