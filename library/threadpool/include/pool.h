/********************************************
 **  模块名称：pool.h
 **  模块功能: 消息池实现
 **  模块简述:
 **  编 写 人:
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#ifndef __LIB_MSG_POOL_H__
#define __LIB_MSG_POOL_H__

#include <assert.h>
#include <list>
#include <deque>
#include <algorithm>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>


using namespace std;


template<bool isPtr=false>
class Bool2Type
{
    enum{eType = isPtr};
};

template<typename T, bool isPtr=false, typename storage= deque<T > >
class Pool
{
public:

    typedef void (*FUNC)(T &t);
    Pool()
    {
        m_nListMaxSize = 3000;
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_cond, NULL);
    }

    virtual ~Pool()
    {
        desDel(Bool2Type<isPtr>() );

        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
    }

    void SetMaxSize(unsigned int nMaxSize)
    {
        if (nMaxSize < 100 || nMaxSize > 1000000)
        {
            return;
        }
        m_nListMaxSize = nMaxSize;
    }

    void desDel(Bool2Type<false>)
    {
    }

    void desDel(Bool2Type<true>)
    {
        pthread_mutex_lock(&m_mutex);
        typename storage::iterator iter;
        for ( iter = m_list.begin(); iter !=m_list.end(); iter++ )
        {
            if(NULL != *iter)
            delete ( *iter );
        }
        pthread_mutex_unlock(&m_mutex);
    }

    void insert(int index,  const T &pNode)
    {
        pthread_mutex_lock(&m_mutex);
        typename storage::iterator iter = m_list.begin();
        advance( iter, index);
        m_list.insert( iter, pNode);
        pthread_mutex_unlock(&m_mutex);
    }

    bool append(const T &pNode, int nMaxWaitSeconds = 1)
    {
        bool bRet = false;
        int nSeconds = 0;
        while (!bRet)
        {
            nSeconds++;
            pthread_mutex_lock(&m_mutex);
            if (m_list.size() < 5000)
            {
                m_list.push_back( pNode);
                bRet = true;
            }
            pthread_mutex_unlock(&m_mutex);

            if (!bRet)
            {
                if (nSeconds > nMaxWaitSeconds) break;
                sleep(1);
            }
        }
        return bRet;
    }

    //单位微妙
    int waitToNotEmpty(int timeout=5000000)
    {
        int nRet = 0;

        while (timeout > 0)
        {
            pthread_mutex_lock(&m_mutex);
            if (m_list.empty())
            {
                pthread_mutex_unlock(&m_mutex);
                nRet = -1;
                usleep(1000);
                timeout -= 100;
            }
            else
            {
                pthread_mutex_unlock(&m_mutex);
                break;
            }
        }

        return nRet;
    }

    //从队列中删除元素单位微秒1us=1/1000000s
    bool pop(T &t, int timeout_us=5000) //单位微秒
    {
        bool bRet = false;
        while(!bRet)
        {
            pthread_mutex_lock(&m_mutex);
            if (!m_list.empty())
            {
                t = m_list.front();
                m_list.pop_front();
                bRet = true;
            }
            pthread_mutex_unlock(&m_mutex);

            if (bRet)
            {
                break;
            }
            else
            {
                usleep(1000);
                timeout_us -= 100;
                if (timeout_us < 0)
                {
                    break;
                }
            }
        }

        return bRet;
    }

    //timeout -- 毫秒
    bool get(T &t, int timeout=5000, int index=0)
    {
        pthread_mutex_lock(&m_mutex);

        if(index <0 || index >= (int)m_list.size() )
        {
            pthread_mutex_unlock(&m_mutex);
            return false;
        }

        while (timeout > 0 && m_list.empty())
        {
            pthread_mutex_unlock(&m_mutex);
            usleep(1000);
            timeout -= 1;
            pthread_mutex_lock(&m_mutex);
        }

        if(m_list.empty())
        {
            pthread_mutex_unlock(&m_mutex);
            return false;
        }

        typename storage::iterator iter = m_list.begin();
        advance( iter, index);
        t = *iter;
        pthread_mutex_unlock(&m_mutex);
        return true;
    }

    bool erase(int index)
    {
        pthread_mutex_lock(&m_mutex);
        if(index <0 || index >= (int)m_list.size() )
        {
            pthread_mutex_unlock(&m_mutex);
            return false;
        }


        if(m_list.empty())
        {
            pthread_mutex_unlock(&m_mutex);
            return false;
        }

        typename storage::iterator iter = m_list.begin();
        advance( iter, index);

        m_list.erase(iter);
        pthread_mutex_unlock(&m_mutex);
        return true;
    }


    bool erase(T &t, int timeout=5000, int index=0)
    {
        pthread_mutex_lock(&m_mutex);
        if(index <0 || index >= m_list.size() )
        {
            pthread_mutex_unlock(&m_mutex);
            return false;
        }

        typename storage::iterator iter = m_list.begin();
        advance( iter, index);
        t = *iter;

        m_list.erase(iter);
        pthread_mutex_unlock(&m_mutex);
        return true;
    }

    int size()
    {
        int nSize = 0;
        pthread_mutex_lock(&m_mutex);
        nSize = m_list.size();
        pthread_mutex_unlock(&m_mutex);
        return nSize;
    }

    void each(FUNC func)
    {
        pthread_mutex_lock(&m_mutex);

        typename storage::iterator iter = m_list.begin();
        for(; iter != m_list.end(); iter++)
        {
            (*func)(*iter);
        }
        pthread_mutex_unlock(&m_mutex);
        return ;
    }

protected:
    //微秒1s=1000ms=1000000us=1000000000ns
    void maketime(struct timespec *abstime, int timeout)
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        abstime->tv_sec = now.tv_sec + timeout/1000000;
        abstime->tv_nsec = now.tv_usec*1000 + (timeout%1000000)*1000;
        return;
    }
    storage  m_list;                               // 存放数据的列表
    typedef typename storage::iterator NODE_ITEM;  // 元素指针
    //线程锁
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_cond;
    bool m_bInit;
    unsigned int m_nListMaxSize;
};



#endif



