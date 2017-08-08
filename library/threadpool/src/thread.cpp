/********************************************
 **  模块名称：thread.cpp
 **  模块功能:
 **  模块简述: 线程基类
 **  编 写 人: lzw978
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/
#include <pthread.h>
#include <stdio.h>
#include "thread.h"

#define STRING_NULL ""
#define MAX_TID_LEN 32

CThread::CThread(): m_ThreadID(0),m_shutdown(false)
{
    snprintf(m_strTid, MAX_TID_LEN, "%s", STRING_NULL);
}

CThread::~CThread() 
{
}

// 线程处理函数句柄
void* CThread::threadFunction(void *pHandle)
{
    CThread* pUser=(CThread*)pHandle;

    // 执行
    pUser->run();
    return NULL;
}

// 初始化
int CThread::init()
{
    return 0;
}

// 启动线程
bool CThread::start()
{
   pthread_attr_t  attr;

   m_shutdown = false;
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // 创建线程
    if(-1 == pthread_create(&m_ThreadID, &attr,threadFunction, this))
    {
       pthread_attr_destroy(&attr);
       return false;
    }

    pthread_attr_destroy(&attr);
    return true;
}

// 线程终止
void CThread::terminate(void)
{
   m_shutdown = true;
}

// 线程加入
void CThread::join(void)
{
   if(0 != m_ThreadID)
   pthread_join(m_ThreadID, NULL);
}

// 设置线程ID
void CThread::SetTid( const string& strTid )
{
    snprintf( m_strTid, 32, "%s", strTid.c_str() );
}
