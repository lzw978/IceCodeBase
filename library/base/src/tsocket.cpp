/********************************************
 **  模块名称：tsocket.h
 **  模块功能:
 **  模块简述: socket操作封装实现
 **  编 写 人:
 **  日    期: 2017.08.22
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 根据 https://github.com/Tencent/Tars/tree/master/cpp 修改
 **  问    题：
*********************************************/

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <cassert>
#include <cerrno>
#include <stdio.h>
#include <stdlib.h>
#include "tsocket.h"
#include "exception.h"

using namespace commbase;

// 构造函数
TSocket::TSocket() : m_iSock(INVALID_SOCKET), m_iDomain(AF_INET), m_bOwner(true)
{
}
// 析构函数
TSocket::~TSocket()
{
    if (m_bOwner)
    {
        close();
    }
}
// 初始化
void TSocket::init(int fd, bool bOwner, int iDomain)
{
    if (m_bOwner)
    {
        close();
    }
    m_iSock   = fd;
    m_iDomain = iDomain;
    m_bOwner  = bOwner;
}
 // 创建socket，如果已存在，释放后生成新的
 void TSocket::createSocket(int iSocketType, int iDomain)
 {
    assert(iSocketType == SOCK_STREAM || iSocketType == SOCK_DGRAM);
    close();

    m_iDomain = iDomain;
    m_iSock   = socket(iDomain, iSocketType, 0);
    if (m_iSock < 0)
    {
        m_iSock = INVALID_SOCKET;
        m_sErrDesc = "create socket error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
 }
// 关闭socket
void TSocket::close()
{
    if (m_iSock != INVALID_SOCKET)
    {
        ::close(m_iSock);
        m_iSock = INVALID_SOCKET;
    }
}
// 获取对点的ip和端口(对AF_INET的socket有效)
void TSocket::getPeerName(string &sPeerAddress, uint16_t &iPeerPort)
{
    assert(m_iDomain == AF_INET);

    struct sockaddr stPeer;
    bzero(&stPeer, sizeof(struct sockaddr));
    socklen_t iPeerLen = sizeof(sockaddr);

    getPeerName(&stPeer, iPeerLen);

    char szAddr[INET_ADDRSTRLEN] = {0};
    struct sockaddr_in *p = (struct sockaddr_in *)&stPeer;
    // 网站字节序二进制值转换为点分十进制串
    inet_ntop(m_iDomain, &p->sin_addr, szAddr, sizeof(szAddr));
    sPeerAddress = szAddr;
    // 16位数由网络字节序转换为主机字节序
    iPeerPort    = ntohs(p->sin_port);
}
// 获取对点的ip和端口(对AF_LOCAL的socket有效)
void TSocket::getPeerName(string &sPathName)
{
    assert(m_iDomain == AF_LOCAL);

    struct sockaddr_un stSock;
    bzero(&stSock, sizeof(struct sockaddr_un));
    socklen_t iSockLen = sizeof(stSock);
    getPeerName((struct sockaddr *)&stSock, iSockLen);
    sPathName = stSock.sun_path;
}
// 获取对点的地址
void TSocket::getPeerName(struct sockaddr *pstPeerAddr, socklen_t &iPeerLen)
{
    if (getpeername(m_iSock, pstPeerAddr, &iPeerLen) < 0)
    {
        m_sErrDesc = "get peername error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 获取自身ip和端口
void TSocket::getSockName(string &sSockAddress, uint16_t &iSockPort)
{
    assert(m_iDomain == AF_INET);

    struct sockaddr stSock;
    bzero(&stSock, sizeof(struct sockaddr));
    socklen_t iSockLen = sizeof(sockaddr);

    getSockName(&stSock, iSockLen);

    char szAddr[INET_ADDRSTRLEN] = {0};
    struct sockaddr_in *p = (struct sockaddr_in *)&stSock;

    // 二进制整数转换为点分十进制
    inet_ntop(m_iDomain, &p->sin_addr, szAddr, sizeof(szAddr));
    sSockAddress = szAddr;
    // 16位数由网络字节顺序转换为主机字节顺序
    iSockPort = ntohs(p->sin_port);
}
// 获取socket名称(对AF_LOCAL的socket有效)
void TSocket::getSockName(string &sPathName)
{
    assert(m_iDomain == AF_LOCAL);

    struct sockaddr_un stSock;
    bzero(&stSock, sizeof(struct sockaddr_un));
    socklen_t iSockLen = sizeof(stSock);
    getSockName((struct sockaddr *)&stSock, iSockLen);

    sPathName = stSock.sun_path;
}
// 获取自己的ip和端口
void TSocket::getSockName(struct sockaddr *pstSockAddr, socklen_t &iSockLen)
{
    if (getsockname(m_iSock, pstSockAddr, &iSockLen) < 0)
    {
        m_sErrDesc = "getSockName error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 修改socket选项
int TSocket::setSockOpt(int opt, const void *pvOptVal, socklen_t optLen, int level)
{
    return setsockopt(m_iSock, level, opt, pvOptVal, optLen);
}
// 获取socket选项值
int TSocket::getSockOpt(int opt, void *pvOptVal, socklen_t &optLen, int level)
{
    return getsockopt(m_iSock, level, opt, pvOptVal, &optLen);
}
// accept
int TSocket::accept(TSocket &tcSock, struct sockaddr *pstSockAddr, socklen_t &iSockLen)
{
    assert(tcSock.m_iSock == INVALID_SOCKET);

    int iFd;

    while ((iFd = ::accept(m_iSock, pstSockAddr, &iSockLen)) < 0 && errno == EINTR);

    tcSock.m_iSock = iFd;
    tcSock.m_iDomain = m_iDomain;

    return tcSock.m_iSock;
}
// 绑定端口(对AF_INET的socket有效)
void TSocket::bind(const string &sServerAddr, int port)
{
    assert(m_iDomain == AF_INET);
    struct sockaddr_in bindAddr;
    bzero(&bindAddr, sizeof(bindAddr));

    bindAddr.sin_family = m_iDomain;
    bindAddr.sin_port   = htons(port);

    if (sServerAddr == "")
    {
        // 本机ip转换为网络协议规定的格式
        bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        parseAddr(sServerAddr, bindAddr.sin_addr);
    }

    try
    {
        bind((struct sockaddr *)(&bindAddr), sizeof(bindAddr));
    }
    catch(...)
    {
        m_sErrDesc = "bind error: [" + sServerAddr  + "] errno = " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 绑定端口(对AF_LOCAL的socket有效)
void TSocket::bind(const char *szPathName)
{
    assert(m_iDomain == AF_LOCAL);
    // 删除指定名字文件，清空资源
    unlink(szPathName);

    struct sockaddr_un stBindAddr;
    bzero(&stBindAddr, sizeof(struct sockaddr_un));
    stBindAddr.sun_family = m_iDomain;
    strncpy(stBindAddr.sun_path, szPathName, sizeof(stBindAddr.sun_path));

    try
    {
        bind((struct sockaddr *)&stBindAddr, sizeof(stBindAddr));
    }
    catch(...)
    {
        m_sErrDesc = "bind error: [" + string(szPathName) + "] errno = " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 绑定端口
void TSocket::bind(struct sockaddr *pstBindAddr, socklen_t iAddrLen)
{
    // 如果服务器终止后，服务器可以第二次快速启动而不用等待
    int iReuseAddr = 1;
    setSockOpt(SO_REUSEADDR, (const void *)&iReuseAddr, sizeof(int), SOL_SOCKET);

    if (::bind(m_iSock, pstBindAddr, iAddrLen) < 0)
    {
        m_sErrDesc = "bind error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 连接其他服务(同步连接)
void TSocket::connect(const string &sServerAddr, uint16_t port)
{
    int iRet = connectNoThrow(sServerAddr, port);
    if (iRet < 0)
    {
        m_sErrDesc = "connect error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 连接本地套接字(同步连接)
void TSocket::connect(const char* szPathName)
{
    int iRet = connectNoThrow(szPathName);
    if (iRet < 0)
    {
        m_sErrDesc = "connect error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 连接其他服务
int TSocket::connect(struct sockaddr *pstServerAddr, socklen_t serverLen)
{
    return ::connect(m_iSock, pstServerAddr, serverLen);
}
// 发起连接,连接失败返回异常
int TSocket::connectNoThrow(const string &sServerAddr, uint16_t port)
{
    assert(m_iDomain == AF_INET);
    if ( sServerAddr == "")
    {
        throw Exception(Z_SOURCEINFO, -1, "connect error: server address is empty!");
    }

    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = m_iDomain;
    parseAddr(sServerAddr, serverAddr.sin_addr);
    serverAddr.sin_port = htons(port);

    return connect((struct sockaddr *)(&serverAddr), sizeof(serverAddr));
}
int TSocket::connectNoThrow(struct sockaddr* addr)
{
    assert(m_iDomain == AF_INET);
    return connect(addr, sizeof(struct sockaddr));
}
int TSocket::connectNoThrow(const char* szPathName)
{
    int iRet = connectNoThrow(szPathName);
    if (iRet < 0)
    {
        m_sErrDesc = "connect error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 监听
void TSocket::listen(int iConnBackLog)
{
    if (::listen(m_iSock, iConnBackLog) < 0)
    {
        m_sErrDesc = "listen error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 接收数据(一般用于tcp)
int TSocket::recv(void* pvBuf, size_t iLen, int iFlag)
{
    return ::recv(m_iSock, pvBuf, iLen, iFlag);
}
// 发送数据(一般用于tcp)
int TSocket::send(const void* pvBuf, size_t iLen, int iFlag)
{
    return ::send(m_iSock, pvBuf, iLen, iFlag);
}
// 接收数据(一般用于udp)
int TSocket::recvfrom(void* pvBuf, size_t iLen, string &sFromAddr, uint16_t &iFromPort, int iFlags)
{
    struct sockaddr stFromAddr;
    socklen_t iFromLen = sizeof(struct sockaddr);
    struct sockaddr_in *p = (struct sockaddr_in *)&stFromAddr;

    bzero(&stFromAddr, sizeof(struct sockaddr));

    int iBytes = recvfrom(pvBuf, iLen, &stFromAddr, iFromLen, iFlags);
    if ( iBytes >= 0)
    {
        char szAddr[INET_ADDRSTRLEN] = {0};
        inet_ntop(m_iDomain, &p->sin_addr, szAddr, sizeof(szAddr));

        sFromAddr = szAddr;
        iFromPort = ntohs(p->sin_port);
    }
    return iBytes;
}
// 接收数据(一般用于udp)
int TSocket::recvfrom(void* pvBuf, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags)
{
    return ::recvfrom(m_iSock, pvBuf, iLen, iFlags, pstFromAddr, &iFromLen);
}
// 发送数据(一般用于udp)
int TSocket::sendto(const void *pvBuf, size_t iLen, const string &sToAddr, uint16_t iToPort, int iFlags)
{
    struct sockaddr_in toAddr;
    bzero(&toAddr, sizeof(struct sockaddr_in));
    toAddr.sin_family = m_iDomain;

    if (sToAddr == "")
    {
        toAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    }
    else
    {
        parseAddr(sToAddr, toAddr.sin_addr);
    }
    toAddr.sin_port = htons(iToPort);
    return sendto(pvBuf, iLen, (struct sockaddr *)(&toAddr), sizeof(toAddr), iFlags);
}
// 发送数据(一般用于udp)
int TSocket::sendto(const void *pvBuf, size_t iLen, struct sockaddr* pstToAddr, socklen_t iToLen, int iFlags)
{
    return ::sendto(m_iSock, pvBuf, iLen, iFlags, pstToAddr, iToLen);
}
// 关闭: 关闭方式iHow:SHUT_RD|SHUT_WR|SHUT_RDWR
void TSocket::shutdown(int iHow)
{
    if (::shutdown(m_iSock, iHow) < 0)
    {
        m_sErrDesc = "shutdown error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 设置阻塞方式(默认不阻塞)
void TSocket::setblock(bool bBlock)
{
    assert(m_iSock != INVALID_SOCKET);
    setblock(m_iSock, bBlock);
}
// 设置非closewait状态
void TSocket::setNoCloseWait()
{
    linger stLinger;
    stLinger.l_onoff  = 1; // 在close socket调用后，但是还有数据未发送完毕的时候容许逗留
    stLinger.l_linger = 0; // 容许逗留的时间为0秒

    if (setSockOpt(SO_LINGER, (const void*)&stLinger, sizeof(linger), SOL_SOCKET) == -1)
    {
        m_sErrDesc = "setNoCloseWait error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 设置为closewait状态，最长等待多久
void TSocket::setCloseWait(int delay)
{
    linger stLinger;
    stLinger.l_onoff  = 1;     // 在close socket调用后，但是还有数据未发送完毕的时候容许逗留
    stLinger.l_linger = delay; // 容许逗留的时间为 deley 秒,默认30

    if (setSockOpt(SO_LINGER, (const void*)&stLinger, sizeof(linger), SOL_SOCKET) == -1)
    {
        m_sErrDesc = "setNoCloseWait error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 设置closewait缺省状态，close以后马上返回并尽量把数据发送出去
void TSocket::setCloseWaitDefault()
{
    linger stLinger;
    stLinger.l_onoff  = 0;
    stLinger.l_linger = 0;

    if (setSockOpt(SO_LINGER, (const void*)&stLinger, sizeof(linger), SOL_SOCKET) == -1)
    {
        m_sErrDesc = "setNoCloseWait error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 设置nodeley(只有打开keepalive才有效)
void TSocket::setTcpNoDelay()
{
    int flag = 1;
    if (setSockOpt(TCP_NODELAY, (char*)&flag, int(sizeof(int)), IPPROTO_TCP) == -1)
    {
        m_sErrDesc = "setTcpNoDelay error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 设置keeyalive
void TSocket::setKeepAlive()
{
    int flag = 1;
    if (setSockOpt(SO_KEEPALIVE, (char*)&flag, int(sizeof(int)), SOL_SOCKET) == -1)
    {
        m_sErrDesc = "setKeepAlive error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 获取recv缓冲区大小
int TSocket::getRecvBufferSize()
{
    int iSize;
    socklen_t len = sizeof(iSize);
    if (getSockOpt(SO_RCVBUF, (void*)&iSize, len, SOL_SOCKET) == -1 || len != sizeof(iSize))
    {
        m_sErrDesc = "getRecvBufferSize error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
    return iSize;
}
// 设置recv缓冲区大小
void TSocket::setRecvBufferSize(int iSize)
{
    if ( setSockOpt(SO_RCVBUF, (char*)&iSize, int(sizeof(int)), SOL_SOCKET) == -1)
    {
        m_sErrDesc = "setRecvBufferSize error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}
// 获取send缓冲区大小
int TSocket::getSendBufferSize()
{
    int iSize;
    socklen_t len = sizeof(iSize);
    if (getSockOpt(SO_SNDBUF, (void*)&iSize, len, SOL_SOCKET) == -1 || len != sizeof(iSize))
    {
        m_sErrDesc = "getSendBufferSize error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
    return iSize;
}
// 设置发送缓冲区大小
void TSocket::setSendBufferSize(int iSize)
{
    if (setSockOpt(SO_SNDBUF, (char*)&iSize, int(sizeof(int)), SOL_SOCKET) == -1)
    {
        m_sErrDesc = "setSendBufferSize error: " + string(strerror(errno));
        throw Exception(Z_SOURCEINFO, errno, m_sErrDesc);
    }
}

// 静态方法
// 获取本地所有ip
vector<string> TSocket::getLocalHosts()
{
    vector<string> result;
    TSocket ts;
    ts.createSocket(SOCK_STREAM, AF_INET);

    int cmd = SIOCGIFCONF;
    int numaddrs = 10;
    int old_ifc_len  = 0;
    struct ifconf ifc;

    while(1)
    {
        int bufsize = numaddrs * static_cast<int>(sizeof(struct ifreq));
        ifc.ifc_len = bufsize;
        ifc.ifc_buf = (char*)malloc(bufsize);

        int rs = ioctl(ts.getfd(), cmd, &ifc);
        if (rs == -1)
        {
            free(ifc.ifc_buf);
            throw Exception(Z_SOURCEINFO, errno, "getLocalHosts error: " + string(strerror(errno)));
        }
        else if (ifc.ifc_len == old_ifc_len)
        {
            break;
        }
        else
        {
            old_ifc_len = ifc.ifc_len;
        }

        numaddrs += 10;
        free(ifc.ifc_buf);
    }

    numaddrs = ifc.ifc_len / static_cast<int>(sizeof(struct ifreq));
    struct ifreq* ifr = ifc.ifc_req;
    for (int i=0; i<numaddrs; ++i)
    {
        if (ifr[i].ifr_addr.sa_family == AF_INET)
        {
            struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(&ifr[i].ifr_addr);
            if (addr->sin_addr.s_addr != 0)
            {
                char szAddr[INET_ADDRSTRLEN] = {0};
                inet_ntop(AF_INET, &(*addr).sin_addr, szAddr, sizeof(szAddr));
                result.push_back(szAddr);
            }
        }
    }
    free(ifc.ifc_buf);
    return result;
}
// 设置socket阻塞方式
void TSocket::setblock(int fd, bool bBlock)
{
    int iVal = 0;
    if ((iVal = fcntl(fd, F_GETFL, 0)) == -1)
    {
        throw Exception(Z_SOURCEINFO, errno, "setblock error: " + string(strerror(errno)));
    }

    if (!bBlock)
    {
        iVal |= O_NONBLOCK;
    }
    else
    {
        iVal &= ~O_NONBLOCK;
    }

    if (fcntl(fd, F_SETFL, iVal) == -1)
    {
        throw Exception(Z_SOURCEINFO, errno, "setblock error: " + string(strerror(errno)));
    }
}
// 生成管道，抛出异常会关闭fd
void TSocket::createPipe(int fds[2], bool bBlock)
{
    if (::pipe(fds) != 0)
    {
        throw Exception(Z_SOURCEINFO, errno, "createPipe error: " + string(strerror(errno)));
    }

    try
    {
        setblock(fds[0], bBlock);
        setblock(fds[1], bBlock);
    }
    catch(...)
    {
        ::close(fds[0]);
        ::close(fds[1]);
        throw;
    }
}
// 解析地址，从字符串(ip或域名),解析到in_addr结构
void TSocket::parseAddr(const string &sAddr, struct in_addr &stSinAddr)
{
    int iRet = inet_pton(AF_INET, sAddr.c_str(), &stSinAddr);
    if (iRet < 0)
    {
        throw Exception(Z_SOURCEINFO, errno, "parseAddr error: " + string(strerror(errno)));
    }
    else if (iRet == 0)
    {
        struct hostent stHostent;
        struct hostent *pstHostent;
        char buf[2048+1] = {0};
        int iError = -1;

        gethostbyname_r(sAddr.c_str(), &stHostent, buf, sizeof(buf), &pstHostent, &iError);
        if (pstHostent == NULL)
        {
            throw Exception(Z_SOURCEINFO, errno, "parseAddr error: " + string(strerror(errno)));
        }
        else
        {
            stSinAddr = *(struct in_addr *) pstHostent->h_addr;
        }
    }
}
