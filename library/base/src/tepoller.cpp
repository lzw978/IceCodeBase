/********************************************
 **  模块名称：tepoller.cpp
 **  模块功能:
 **  模块简述: epoll操作封装类实现
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <unistd.h>
#include "tepoller.h"
#include "exception.h"

using namespace commbase;

TEpoller::TEpoller(bool bET)
{
    m_iEpollfd  = -1;
    m_iMaxConns = 1024;
    m_bET       = bET;
    m_pstPevs   = NULL;

}
// 析构
TEpoller::~TEpoller()
{
    if (m_pstPevs != NULL)
    {
        delete[] m_pstPevs;
        m_pstPevs = NULL;
    }

    if (m_iEpollfd > 0)
    {
        close(m_iEpollfd);
    }
}
// 生成epoll句柄
void TEpoller::create(int iMaxConnections)
{
    m_iMaxConns = iMaxConnections;
    m_iEpollfd  = epoll_create(m_iMaxConns + 1);
    if (m_pstPevs != NULL)
    {
        delete[] m_pstPevs;
    }

    m_pstPevs = new epoll_event[m_iMaxConns + 1];
}
// 添加监听句柄
void TEpoller::add(int fd, long long data, __uint32_t event)
{
    ctrl(fd, data, event, EPOLL_CTL_ADD);
}
// 修改句柄事件
void TEpoller::mod(int fd, long long data, __uint32_t event)
{
    ctrl(fd, data, event, EPOLL_CTL_MOD);
}
// 删除句柄事件
void TEpoller::del(int fd, long long data, __uint32_t event)
{
    ctrl(fd, data, event, EPOLL_CTL_DEL);
}
// 等待时间
int TEpoller::wait(int iMillSecond)
{
    return epoll_wait(m_iEpollfd, m_pstPevs, m_iMaxConns + 1, iMillSecond);
}
// 控制epoll，将模式设置为边缘触发EPOLLET模式
void TEpoller::ctrl(int fd, long long data, __uint32_t events, int op)
{
    struct epoll_event ev;
    ev.data.u64 = data;
    if (m_bET)
    {
        ev.events = events | EPOLLET;
    }
    else
    {
        ev.events = events;
    }
    epoll_ctl(m_iEpollfd, op, fd, &ev);
}
