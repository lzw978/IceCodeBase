/********************************************
 **  模块名称：threadgroup.h
 **  模块功能:
 **  模块简述: ͬ同步链表，适用于有多线程同时访问链表的情况
 **  编 写 人: lzw978
 **  日    期: 2017.07.25
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/
#ifndef __LIB_THREAD_GROUP_H_
#define __LIB_THREAD_GROUP_H_

#include <assert.h>
#include <list>
#include <algorithm>

#include "thread.h"

using namespace std;

class ThreadGroup
{
public:
    int nChainLen;
    ThreadGroup(){nChainLen =0; pthread_mutex_init(&m_mutex_h, NULL);};
    virtual ~ThreadGroup();

    // 插入
    void insert(int index,  CThread * pNode);
    // 尾部增加
    void append( CThread * pNode);
    // 查找
    CThread* find(int index);
    // 删除
    CThread* del(int index);
    // 查询node
    int search(const CThread* pNode);
    // 列表大小
    int size();

    virtual int init();
    virtual void thr_run();

protected:
    list<CThread *>  m_list;                     // 存放数据的列表
    typedef list<CThread *>::iterator NODE_ITEM; // 元素指针
    pthread_mutex_t m_mutex_h ;                  // 线程锁
};

#endif



