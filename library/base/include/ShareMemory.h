/********************************************
 **  模块名称：ShareMemory.h
 **  模块功能:
 **  模块简述: 共享内存类
 **  编 写 人: lzw978
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/
#ifndef __LIB_SHARE_MEMORY_H__
#define __LIB_SHARE_MEMORY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <pthread.h>

#define INVALID_SHAREMEM_HANDLE -1

class CShareMemory
{
public:
    CShareMemory(void);
    CShareMemory(const char* pszName, int nSize);
    ~CShareMemory();

    // 创建共享内存
    int Create(const char* pszName, int nSize);
    // 销毁已经创建的共享内存
    void Destroy(void);
    // 创建是否已创建的共享内存对象
    bool IsCreated(void);
    // 返回在本实例创建前，共享内存对象是否已存在
    bool IsExistedBeforeCreate(void);
    // 取共享内存地址
    void* GetMemoryAddress(void);
    // 加锁：锁定共享内存,禁止其它进程访问
    bool LockShareMemory(bool bWriteLock);
    // 解锁：解除对共享内存的,允许其它进程访问
    void UnlockShareMemory(void);
private:
    bool  m_bExistFlag;             // 标识共享内存实例创建前，是否已经存在
    int   m_nSize;                  // 共享内存大小
    int   m_hShareMemHandle;        // 共享内存句柄
    char  m_szShareMemName[256+1];  // 共享内存名字
    void* m_pMemory;                // 指向共享内存区域的指针
};

#endif
