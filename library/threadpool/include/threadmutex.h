/********************************************
 **  模块名称：threadmutex.h
 **  模块功能:
 **  模块简述: ͬ线程lock,unLock,wait,notify操作
 **  编 写 人: lzw978
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#ifndef __LIB_THREAD_MUTEX_H__
#define __LIB_THREAD_MUTEX_H__

#include <pthread.h>

class CThreadConditionMutex 

{
private:
    // 互斥量
    pthread_mutex_t     mutex_h;
    // 条件变量
    pthread_cond_t      cond_h;
    bool                m_IsValid;
protected: 
    
public: 
    CThreadConditionMutex(); 
    virtual ~CThreadConditionMutex(); 

    // 等待
    bool wait();
    // 计时等待
    bool wait(int nWaitTime);
    // 加锁
    bool lock();
    // 解锁
    bool unLock();
    // 唤醒
    bool notify();

}; 

class ScopedLock {
    private:
        CThreadConditionMutex& _mutex;

    public:
    inline ScopedLock(CThreadConditionMutex& mutex) :
        _mutex(mutex) {
        _mutex.lock();
    }

    inline ~ScopedLock() {
        _mutex.unLock();
    }
};

#endif
