/********************************************
 **  模块名称：tepoller.h
 **  模块功能:
 **  模块简述: epoll操作封装类
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_TEPOLLER_H__
#define __LIB_TEPOLLER_H__

#include <sys/epoll.h>
#include <cassert>

namespace commbase
{
    class TEpoller
    {
    public:
        // 构造:默认ET模式，当状态发生变化的时候才获得通知
        TEpoller(bool bEt = true);
        // 析构
        ~TEpoller();
        // 生成epoll句柄
        void create(int iMaxConnections);
        // 添加监听句柄
        void add(int fd, long long data, __uint32_t event);
        // 修改句柄事件
        void mod(int fd, long long data, __uint32_t event);
        // 删除句柄事件
        void del(int fd, long long data, __uint32_t event);
        // 等待时间
        int wait(int iMillSecond);
        // 获取被触发的事件
        struct epoll_event& get(int i)
        {
            assert(m_pstPevs != 0);
            return m_pstPevs[i];
        }
    protected:
        // 控制epoll，将模式设置为边缘触发EPOLLET模式
        // op : EPOLL_CTL_ADD | EPOLL_CTL_MOD | EPOLL_CTL_DEL
        void ctrl(int fd, long long data, __uint32_t events, int op);
    protected:
        int m_iEpollfd;                // epoll句柄
        int m_iMaxConns;               // 最大连接数
        bool m_bET;                    // 是否为ET模式
        struct epoll_event *m_pstPevs; // 事件集
    };
}

#endif