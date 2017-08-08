/********************************************
 **  模块名称：thread.h
 **  模块功能:
 **  模块简述: 线程基类
 **  编 写 人: lzw978
 **  日    期: 2017.07.25
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/
#ifndef __LIB_THREAD_H__
#define __LIB_THREAD_H__

#include <string>
#include <iostream>

using namespace std;

#define MAX_TID_LEN 32

class CThread
{
private:
    pthread_t m_ThreadID;

protected:
    char m_strTid[MAX_TID_LEN];
    static void* threadFunction(void *pHandle);
    bool m_shutdown;

public:
    CThread();
    virtual ~CThread();

    // 初始化
    virtual int init();
    // 执行
    virtual void run(void)=0;
    // 终止线程
    void terminate(void);
    // 启动线程
    bool start();
    // 获取线程ID
    pthread_t getThreadID(void){return (m_ThreadID);} 
    // 线程加入
    void join(void);

    void SetTid( const string& strTid );
    const char* GetTid() { return m_strTid; }

};
#endif
