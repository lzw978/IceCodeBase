/********************************************
 **  模块名称：threadgroup.cpp
 **  模块功能:
 **  模块简述: ͬ同步链表，适用于有多线程同时访问链表的情况
 **  编 写 人: lzw978
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#include "threadgroup.h"

int ThreadGroup::init()
{
    return 0;
}

void ThreadGroup::thr_run()
{
    return;
}

ThreadGroup::~ThreadGroup()
{
    list< CThread*>::iterator iter;
    for ( iter = m_list.begin(); iter !=m_list.end(); iter ++ )
    {
        delete ( *iter );
    }
    // 解锁
    pthread_mutex_unlock(&m_mutex_h);
    // 释放锁
    pthread_mutex_destroy(&m_mutex_h);
}

// 插入
void ThreadGroup::insert(int index,  CThread * pNode)
{
    pthread_mutex_lock(&m_mutex_h);
    list<CThread* >::iterator iter = m_list.begin();
    advance( iter, index);
    m_list.insert( iter, pNode );
    pthread_mutex_unlock(&m_mutex_h);
}

// 尾部增加
void ThreadGroup::append( CThread * pNode)
{
    pthread_mutex_lock(&m_mutex_h);
    m_list.push_back( pNode );
    pthread_mutex_unlock(&m_mutex_h);
}

// 当前容量
int ThreadGroup::size()
{
    return m_list.size();
}

// 查询
CThread* ThreadGroup::find(int index)
{
    pthread_mutex_lock(&m_mutex_h);
    list< CThread*>::iterator iter = m_list.begin();
    advance( iter, index );
    pthread_mutex_unlock(&m_mutex_h);
    return *iter;
}

// 删除
CThread* ThreadGroup::del(int index)
{
    pthread_mutex_lock(&m_mutex_h);
    list< CThread*>::iterator iter = m_list.begin();
    advance( iter, index );
    assert( iter != m_list.end() );
    CThread* pVal = *iter;
    m_list.erase( iter );
    pthread_mutex_unlock(&m_mutex_h);
    return pVal;
}

// 搜索
int ThreadGroup::search(const CThread* pNode)
{
    pthread_mutex_lock(&m_mutex_h);
    list< CThread*>::iterator iter;
    int i = 0;
    for ( iter = m_list.begin(); iter != m_list.end(); iter ++ )
    {
        if( *iter == pNode )
        {
            pthread_mutex_unlock(&m_mutex_h);
            return i;
        }
        i++;
    }
    pthread_mutex_unlock(&m_mutex_h);
    return -1;
}
