/********************************************
 **  模块名称：criticalsection.h
 **  模块功能:
 **  模块简述: 进程锁
 **  编 写 人:
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#ifndef __LIB_CRITICAL_SECTION_H_
#define __LIB_CRITICAL_SECTION_H_

#include <pthread.h>

class CCriticalSection
{
    pthread_mutex_t m_mutex;
public:
    CCriticalSection()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }

    ~CCriticalSection()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    bool lock()
    {
        return 0 == pthread_mutex_lock(&m_mutex);
    }
    bool unlock()
    {
       return 0 == pthread_mutex_unlock(&m_mutex);
    }

};

class CSmartCriticalSection
{
    CCriticalSection * m_pCS;
public:
    CSmartCriticalSection(CCriticalSection *pcs)
        :m_pCS(pcs)
    {
        m_pCS->lock();
    }

    ~CSmartCriticalSection()
    {
        m_pCS->unlock();
    }
};

#endif
