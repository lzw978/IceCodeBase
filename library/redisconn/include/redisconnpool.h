/********************************************
 **  模块名称：redisconnpool.h
 **  模块功能:
 **  模块简述: redis连接池
 **  编 写 人:
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/


#ifndef __LIB_REDIS_CONN_POOL_H__
#define __LIB_REDIS_CONN_POOL_H__

#include <assert.h>
#include <list>
#include <deque>
#include <algorithm>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

#include "redisClient.h"

using namespace std;

template<bool isPtr=false>
class Bool2Type
{
    enum{eType = isPtr};
};

template<typename T, bool isPtr=false, typename storage= deque<T > >

class RedisConnPool
{
public:

    typedef void (*FUNC)(T &t);
    RedisConnPool();
    virtual ~RedisConnPool();


    void SetMaxSize(unsigned int nMaxSize);
    void desDel(Bool2Type<false>);
    void desDel(Bool2Type<true>);
    void insert(int index,  const RedisClient & pNode);

    bool append(const RedisClient &pNode, int nMaxWaitSeconds = 1);


    //单位微妙
    int waitToNotEmpty(int timeout=5000000);

    //从队列中删除元素单位微秒1us=1/1000000s
    bool pop(RedisClient & t, int timeout_us=5000);

    //timeout -- 毫秒
    bool get(RedisClient &t, int timeout=5000, int index=0);

    bool erase(int index);


    bool erase(RedisClient & t, int timeout=5000, int index=0);

    int size();
    void each(FUNC func);

protected:
    //微秒1s=1000ms=1000000us=1000000000ns
    void maketime(struct timespec *abstime, int timeout);
    //存放数据 的列表
    storage  m_list;
    //元素指针
    typedef typename storage::iterator NODE_ITEM;
    //线程锁
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_cond;
    bool m_bInit;
    unsigned int m_nListMaxSize;

};


#endif


