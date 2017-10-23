/********************************************
 **  模块名称：tthread_rwlock.h
 **  模块功能:
 **  模块简述: 线程读写锁
 **  编 写 人:
 **  日    期: 2017.08.22
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/
#ifndef __LIB_TTHREAD_RWLOCK_H__
#define __LIB_TTHREAD_RWLOCK_H__


#include <pthread.h>

namespace commbase
{
    class T_Thread_RWLocker
    {
    public:
        // 构造函数
        T_Thread_RWLocker();
        // 析构函数
        ~T_Thread_RWLocker();
        // 读锁定
        void ReadLock() const;
        // 写锁定
        void WriteLock() const;
        // 尝试读锁定
        void TryReadLock() const;
        // 尝试写锁定
        void TryWriteLock() const;
        // 解锁
        void Unlock() const;
    private:
        mutable pthread_rwlock_t m_sect;
        // 防止复制
        T_Thread_RWLocker(const T_Thread_RWLocker&);
        T_Thread_RWLocker& operator=(const T_Thread_RWLocker&);
    };
}

#endif
