/********************************************
 **  模块名称：tthread_rwlock.cpp
 **  模块功能:
 **  模块简述: 线程读写锁实现
 **  编 写 人:
 **  日    期: 2017.09.22
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <cerrno>
#include <string.h>
#include <string>
#include <iostream>
#include "tthread_rwlock.h"
#include "exception.h"

using namespace std;
using namespace commbase;

// 构造函数
T_Thread_RWLocker::T_Thread_RWLocker()
{
    int iRet = ::pthread_rwlock_init(&m_sect, NULL);
    if (iRet != 0)
    {
        throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_init error");
    }
}

// 析构函数
T_Thread_RWLocker::~T_Thread_RWLocker()
{
    int iRet = ::pthread_rwlock_destroy(&m_sect);
    if (iRet != 0)
    {
        cerr<< "T_Thread_RWLocker::~T_Thread_RWLocker pthread_rwlock_destroy error:" << string(strerror(iRet)) << endl;
    }
}
// 读锁定
void T_Thread_RWLocker::ReadLock() const
{
    int iRet = ::pthread_rwlock_rdlock(&m_sect);
    if (iRet != 0)
    {
        if (iRet == EDEADLK)
        {
            throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_rdlock dead lock error");
        }
        else
        {
            throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_rdlock error");
        }
    }
}
// 写锁定
void T_Thread_RWLocker::WriteLock() const
{
    int iRet = ::pthread_rwlock_wrlock(&m_sect);
    if (iRet != 0)
    {
        if (iRet == EDEADLK)
        {
            throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_wrlock dead lock error");
        }
        else
        {
            throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_wrlock error");
        }
    }
}
// 尝试读锁定
void T_Thread_RWLocker::TryReadLock() const
{
    int iRet = ::pthread_rwlock_tryrdlock(&m_sect);
    if (iRet != 0)
    {
        if (iRet == EDEADLK)
        {
            throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_tryrdlock dead lock error");
        }
        else
        {
            throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_tryrdlock error");
        }
    }
}
// 尝试写锁定
void T_Thread_RWLocker::TryWriteLock() const
{
    int iRet = ::pthread_rwlock_trywrlock(&m_sect);
    if (iRet != 0)
    {
        if (iRet == EDEADLK)
        {
            throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_trywrlock dead lock error");
        }
        else
        {
            throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_trywrlock error");
        }
    }
}
// 解锁
void T_Thread_RWLocker::Unlock() const
{
    int iRet = ::pthread_rwlock_unlock(&m_sect);
    if (iRet != 0)
    {
        throw Exception(Z_SOURCEINFO, iRet, "pthread_rwlock_unlock error");
    }
}