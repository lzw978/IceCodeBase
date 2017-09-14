/********************************************
 **  模块名称：tshm.h
 **  模块功能:
 **  模块简述: 共享内存封装
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_TSHM_H__
#define __LIB_TSHM_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>

namespace commbase
{
    class TShm
    {
    public:
        // 构造函数(是否拥有共享内存默认false)
        TShm(bool bOwner = false) : m_bOwner(bOwner), m_shmSize(0), m_shmKey(0), m_bCreate(true), m_iShmId(-1), m_pShm(NULL)
        {}
        // 构造函数
        TShm(size_t iShmSize, key_t iKey, bool bOwner = false);
        // 析构函数
        ~TShm();

        // 初始化
        void init(size_t iShmSize, key_t iKey, bool bOwner = false);
        // 判断是生成的内存还是连接的内存
        bool isCreate()
        {
            return m_bCreate;
        }
        // 获取共享内存的指针
        void *getPointer()
        {
            return m_pShm;
        }
        // 获取共享内存key
        key_t getKey()
        {
            return m_shmKey;
        }
        // 获取共享内存ID
        int getId()
        {
            return m_iShmId;
        }
        // 获取共享内存大小
        size_t size()
        {
            return m_shmSize;
        }
        // 解除共享内存
        int detach();
        // 删除共享内存
        int del();
    protected:
        bool m_bOwner;    // 是否拥有共享内存
        size_t m_shmSize; // 共享内存大小
        key_t  m_shmKey;  // 共享内存key
        bool   m_bCreate; // 是否是生成的共享内存
        int    m_iShmId;  // 共享内存ID
        void  *m_pShm;    // 共享内存指针
    };
}

#endif