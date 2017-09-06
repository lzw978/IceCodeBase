/********************************************
 **  模块名称：mutex.cpp
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

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "logger.h"
#include "mutex.h"


using namespace commbase;

struct Mutex::mutex_handle_t
{
    pthread_mutex_t mutex;
    int             shmid;
    unsigned int    refcount;
};
// 构造函数
Mutex::Mutex(const char* name) throw (Exception)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    if (name)
    {
        key_t shmkey = ftok(name, 0);

        int shmid = shmget(shmkey, sizeof(mutex_handle_t), IPC_CREAT);
        int r = errno;
        if (shmid == -1)
        {
            throw Exception(Z_SOURCEINFO, r, "shmget fail");
        }

        mHandle = (mutex_handle_t*)shmat(shmid, 0, 0);
        mHandle->shmid = shmid;
        mHandle->refcount = 1;
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    }
    else
    {
        mHandle = new mutex_handle_t;
        mHandle->shmid    = -1;
        mHandle->refcount = 1;
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
    }
    pthread_mutex_init(&mHandle->mutex, &attr);
}
// 析构函数
Mutex::~Mutex()
{
    if ((--mHandle->refcount) == 0)
    {
        pthread_mutex_destroy(&mHandle->mutex);
    }

    if (mHandle->shmid != -1)
    {
        int shmid = mHandle->shmid;
        if (!mHandle->refcount)
        {
            shmctl(shmid, IPC_RMID, 0);
        }
        shmdt((void*)mHandle);
    }
    else
    {
        if (NULL != mHandle)
        {
            delete mHandle;
            mHandle = NULL;
        }
    }
}
// 加锁
void Mutex::lock() throw (Exception)
{
    int ret = pthread_mutex_lock(&mHandle->mutex);
    if(ret != 0)
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "mutex lock error");
    }
}
// 解锁
void Mutex::unlock() throw (Exception)
{
    int ret = pthread_mutex_unlock(&mHandle->mutex);
    if(ret != 0)
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "mutex unlock error");
    }
}