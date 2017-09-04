/********************************************
 **  模块名称：mutex.h
 **  模块功能: 互斥量，用于对被多个线程同时访问的资源加锁
 **  模块简述:
 **  编 写 人:
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#ifndef __LIB_MUTEX_H__
#define __LIB_MUTEX_H__

#include "scopedLock.h"
#include "exception.h"

namespace commbase
{
    class Mutex
    {
    public:
        typedef commbase::ScopedLock<Mutex> ScopedLock;

        Mutex(const char* szName=0) throw (Exception);
        ~Mutex();

        // 加锁
        void lock() throw (Exception);
        // 解锁
        void unlock() throw (Exception);
    private:
        Mutex(const Mutex&);
        Mutex& operator=(const Mutex&);

        struct mutex_handle_t;
        mutex_handle_t* mHandle;
    };
}

#endif