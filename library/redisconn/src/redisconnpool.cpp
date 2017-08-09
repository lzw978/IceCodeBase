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

#include "redisconnpool.h"

RedisConnPool::RedisConnPool()
{
    m_nListMaxSize = 3000;
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
}

RedisConnPool::~RedisConnPool()
    {
        desDel(Bool2Type<isPtr>() );

        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
    }
    
void RedisConnPool::SetMaxSize(unsigned int nMaxSize)
    {
        if (nMaxSize < 100 || nMaxSize > 1000000)
        {
            return;
        }
        m_nListMaxSize = nMaxSize;
    }
void RedisConnPool::desDel(Bool2Type<false>)
    {

    }
void RedisConnPool::desDel(Bool2Type<true>)
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
void RedisConnPool::insert(int index,  const RedisClient & pNode)
    {
        pthread_mutex_lock(&m_mutex);
        typename storage::iterator iter = m_list.begin();
        advance( iter, index);
        m_list.insert( iter, pNode);
        pthread_mutex_unlock(&m_mutex);

    }

bool RedisConnPool::append(const RedisClient &pNode, int nMaxWaitSeconds = 1)
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
    int RedisConnPool::waitToNotEmpty(int timeout=5000000)
    {
        int nRet = 0;

        while (timeout > 0)
        {
            pthread_mutex_lock(&m_mutex);
            if (m_list.empty())
            {
                pthread_mutex_unlock(&m_mutex);
                nRet = -1;
                usleep(100);
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
    bool RedisConnPool::pop(RedisClient & t, int timeout_us=5000) //单位微秒
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
                usleep(100);
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
    bool RedisConnPool::get(RedisClient &t, int timeout=5000, int index=0)
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

    bool RedisConnPool::erase(int index)
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


    bool RedisConnPool::erase(RedisClient & t, int timeout=5000, int index=0)
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

    int RedisConnPool::size()
    {
        int nSize = 0;
        pthread_mutex_lock(&m_mutex);
        nSize = m_list.size();
        pthread_mutex_unlock(&m_mutex);
        return nSize;
    }

    void RedisConnPool::each(FUNC func)
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
    
    void RedisConnPool::maketime(struct timespec *abstime, int timeout)
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        abstime->tv_sec = now.tv_sec + timeout/1000000;
        abstime->tv_nsec = now.tv_usec*1000 + (timeout%1000000)*1000;
        return;
    }


