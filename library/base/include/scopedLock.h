/********************************************
 **  模块名称：scopedLock.h
 **  模块功能: 自动释放锁模板
 **  模块简述:
 **  编 写 人:
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/
#ifndef __LIB_SCOPEDLOCK_H__
#define __LIB_SCOPEDLOCK_H__

namespace commbase
{
    // 互斥量：用于实现自动加锁解锁
    template <class M>
    class ScopedLock
    {
    public:
        ScopedLock(M& mutex) : m_mutex(mutex)
        {
            m_mutex.lock();
        }
        ~ScopedLock()
        {
            m_mutex.unlock();
        }
    private:
        M& m_mutex;
        ScopedLock();
        ScopedLock(const ScopedLock&);
        ScopedLock& operator=(const ScopedLock&);
    };

    // 用于实现自动解锁加锁
    template <class M>
    class ScopedUnlock
    {
    public:
        ScopedUnlock(M& mutex, bool unlockNow=true) : m_mutex(mutex)
        {
            if (unlockNow)
            {
                m_mutex.unlock();
            }
        }
        ~ScopedUnlock()
        {
            m_mutex.lock();
        }
    private:
        M& m_mutex;
        ScopedUnlock();
        ScopedUnlock(const ScopedUnlock&);
        ScopedUnlock& operator=(const ScopedUnlock&);
    };
}

#endif // !__LIB_SCOPEDLOCK_H__