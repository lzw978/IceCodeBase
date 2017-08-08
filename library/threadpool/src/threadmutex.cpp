/********************************************
 **  模块名称：threadmutex.cpp
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
#include "threadmutex.h"

CThreadConditionMutex::CThreadConditionMutex()
{
    m_IsValid = true;
    // 初始化
    if(0 != pthread_mutex_init(&mutex_h, NULL))
    {
        m_IsValid =false;
    }
    if(0 !=pthread_cond_init(&cond_h, NULL))
    {
         m_IsValid =false;
    }
}

CThreadConditionMutex::~CThreadConditionMutex()
{
    pthread_cond_signal(&cond_h);
    pthread_mutex_unlock(&mutex_h);
    pthread_cond_destroy(&cond_h);
    pthread_mutex_destroy(&mutex_h);
}

// 加锁
bool CThreadConditionMutex::lock()
{
    if(!m_IsValid)
    {
        return false;
    }

    if(0 !=pthread_mutex_lock(&mutex_h))
    {
        return false;
    }

    return true;
}

// 解锁
bool CThreadConditionMutex::unLock()
{
    if(!m_IsValid)
    {
        return false;
    }
    if(0 !=pthread_mutex_unlock(&mutex_h))
    {
        return false;
    }

    return true;
}

// 等待
bool CThreadConditionMutex::wait()
{
    if(!m_IsValid)
    {
        return false;
    }

    bool bRet =true;
    if(0 != pthread_cond_wait(&cond_h,&mutex_h))
    {
         bRet = false;
    }

    return bRet;
}

// 计时等待
bool CThreadConditionMutex::wait(int nWaitTime)
{
    if(!m_IsValid)
    {
        return false;
    }

    int ret = 0;
    struct timespec abstime;

    abstime.tv_sec  = time((time_t*)0)+nWaitTime;   /* seconds */
    abstime.tv_nsec = 0;                            /* and nanoseconds */

    bool bRet =true;

    // nWaitTime
    ret = pthread_cond_timedwait(&cond_h, &mutex_h, &abstime);
    if (ret != 0) 
    {
        bRet = false;
    }

    return bRet;
}

// 唤醒
bool CThreadConditionMutex::notify()
{
    if(!m_IsValid)
    {
        return false;
    }

    pthread_cond_signal(&cond_h);
    return true;
}
