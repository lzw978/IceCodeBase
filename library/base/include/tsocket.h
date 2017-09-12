/********************************************
 **  模块名称：tsocket.h
 **  模块功能:
 **  模块简述: socket操作封装
 **  编 写 人:
 **  日    期: 2017.08.22
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_TSOCKET_LIB__
#define __LIB_TSOCKET_LIB__

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <vector>
#include <string>

namespace commbase
{
    using namespace std;

    class TSocket
    {
    public:
        // 构造函数
        TSocket();
        // 析构函数
        virtual ~TSocket();
        // 初始化
        void init(int fd, bool bOwner, int iDomain=AF_INET);
        // 设置是否拥有该fd
        void setOwner(bool bOwner)
        {
            m_bOwner = bOwner;
        }
        // 设置套接字类型
        void setDomain(int iDomain)
        {
            m_iDomain = iDomain;
        }
        // 创建socket，如果已存在，释放后生成新的
        void createSocket(int iSocketType = SOCK_STREAM, int iDomain = AF_INET);
        // 获取socket句柄
        int getfd() const
        {
            return m_iSock;
        }
        // 判断socket是否有效
        bool isValid() const
        {
            return m_iSock != INVALID_SOCKET;
        }
        // 关闭socket
        void close();
        // 获取对点的ip和端口(对AF_INET的socket有效)
        void getPeerName(string &sPeerAddress, uint16_t &iPeerPort);
        // 获取对点的ip和端口(对AF_LOCAL的socket有效)
        void getPeerName(string &sPathName);
        // 获取自身ip和端口
        void getSockName(string &sSockAddress, uint16_t &iSockPort);
        // 获取socket名称(对AF_LOCAL的socket有效)
        void getSockName(string &sPathName);
        // 修改socket选项
        int setSockOpt(int opt, const void *pvOptVal, socklen_t optLen, int level = SOL_SOCKET);
        // 获取socket选项值
        int getSockOpt(int opt, void *pvOptVal, socklen_t &optLen, int level = SOL_SOCKET);
        // accept
        int accept(TSocket &tcSock, struct sockaddr *pstSockAddr, socklen_t &iSockLen);
        // 绑定端口(对AF_INET的socket有效)
        void bind(const string &sServerAddr, int port);
        // 绑定端口(对AF_LOCAL的socket有效)
        void bind(const char *szPathName);
        // 绑定端口
        void bind(struct sockaddr *pstBindAddr, socklen_t iAddrLen);
        // 连接其他服务(同步连接)
        void connect(const string &sServerAddr, uint16_t port);
        // 连接本地套接字(同步连接)
        void connect(const char* szPathName);
        // 发起连接,连接失败返回异常
        int connectNoThrow(const string &sServerAddr, uint16_t port);
        int connectNoThrow(struct sockaddr* addr);
        int connectNoThrow(const char* szPathName);
        // 监听
        void listen(int iConnBackLog);
        // 接收数据(一般用于tcp)
        int recv(void* pvBuf, size_t iLen, int iFlag = 0);
        // 发送数据(一般用于tcp)
        int send(const void* pvBuf, size_t iLen, int iFlag = 0);
        // 接收数据(一般用于udp)
        int recvfrom(void* pvBuf, size_t iLen, string &sFromAddr, uint16_t &iFromPort, int iFlags = 0);
        // 接收数据(一般用于udp)
        int recvfrom(void* pvBuf, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags = 0);
        // 发送数据(一般用于udp)
        int sendto(const void *pvBuf, size_t iLen, const string &sToAddr, uint16_t iToPort, int iFlags = 0);
        // 发送数据(一般用于udp)
        int sendto(const void *pvBuf, size_t iLen, struct sockaddr* pstToAddr, socklen_t iToLen, int iFlags = 0);
        // 关闭: 关闭方式iHow:SHUT_RD|SHUT_WR|SHUT_RDWR
        void shutdown(int iHow);
        // 设置阻塞方式(默认不阻塞)
        void setblock(bool bBlock = false);
        // 设置非closewait状态
        void setNoCloseWait();
        // 设置为closewait状态，最长等待多久
        void setCloseWait(int delay = 30);
        // 设置closewait缺省状态，close以后马上返回并尽量把数据发送出去
        void setCloseWaitDefault();
        // 设置nodeley(只有打开keepalive才有效)
        void setTcpNoDelay();
        // 设置keeyalive
        void setKeepAlive();
        // 获取recv缓冲区大小
        int getRecvBufferSize();
        // 设置recv缓冲区大小
        void setRecvBufferSize(int iSize);
        // 获取send缓冲区大小
        int getSendBufferSize();
        // 设置发送缓冲区大小
        void setSendBufferSize(int iSize);

        // 静态方法
        // 获取本地所有ip
        static vector<string> getLocalHosts();
        // 设置socket阻塞方式
        static void setblock(int fd, bool bBlock);
        // 生成管道，抛出异常会关闭fd
        static void createPipe(int fds[2], bool bBlock);
        // 解析地址，从字符串(ip或域名),解析到in_addr结构
        static void parseAddr(const string &sAddr, struct in_addr &stSinAddr);

    protected:
        // 连接其他服务
        int connect(struct sockaddr *pstServerAddr, socklen_t serverLen);
        // 获取对点的地址
        void getPeerName(struct sockaddr *pstPeerAddr, socklen_t &iPeerLen);
        // 获取自己的ip和端口
        void getSockName(struct sockaddr *pstSockAddr, socklen_t &iSockLen);
    protected:
        static const int INVALID_SOCKET = -1;
        int m_iSock;       // socket句柄
        int m_iDomain;     // socket类型
        bool m_bOwner;     // 是否拥有socket
        string m_sErrDesc; // 错误描述
    };
}

#endif