/********************************************
 **  模块名称：tsocketclient.h
 **  模块功能:
 **  模块简述: socket客户端类
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_TSOCKETCLIENT_H__
#define __LIB_TSOCKETCLIENT_H__

#include <stdint.h>
#include <sstream>
#include "tsocket.h"

namespace commbase
{
    using namespace std;

    class TEndpoint
    {
    public:
        // 构造
        TEndpoint();
        // 构造
        TEndpoint(const string& host, int port, int timeout, int istcp = true, int grid = 0, int qos = 0, int weight = -1, unsigned int weighttype = 0)
        {
            init(host, port, timeout, istcp, grid, qos, weight, weighttype);
        }
        // 用字符串构造
        TEndpoint(const string& strDesc)
        {
            parse(strDesc);
        }
        // 拷贝构造
        TEndpoint(const TEndpoint& t)
        {
            m_strHost     = t.m_strHost;
            m_iPort       = t.m_iPort;
            m_iTimeout    = t.m_iTimeout;
            m_isTcp       = t.m_isTcp;
            m_iGrid       = t.m_iGrid;
            m_iQos        = t.m_iQos;
            m_iWeight     = t.m_iWeight;
            m_iWeightType = t.m_iWeightType;
        }
        // 赋值函数
        TEndpoint& operator = (const TEndpoint& t)
        {
            if (this != &t)
            {
                m_strHost     = t.m_strHost;
                m_iPort       = t.m_iPort;
                m_iTimeout    = t.m_iTimeout;
                m_isTcp       = t.m_isTcp;
                m_iGrid       = t.m_iGrid;
                m_iQos        = t.m_iQos;
                m_iWeight     = t.m_iWeight;
                m_iWeightType = t.m_iWeightType;
            }
            return *this;
        }
        // 相等判断
        bool operator == (const TEndpoint& t)
        {
            return (m_strHost == t.m_strHost && m_iPort == t.m_iPort && m_iTimeout == t.m_iTimeout && m_isTcp == t.m_isTcp &&
                    m_iGrid == t.m_iGrid     && m_iQos == t.m_iQos   && m_iWeight == t.m_iWeight   && m_iWeightType == t.m_iWeightType);
        }
        // 设置ip
        void setHost(const string& host)
        {
            m_strHost = host;
        }
        // 获取ip
        string getHost() const
        {
            return m_strHost;
        }
        // 设置端口
        void setPort(int port)
        {
            m_iPort = port;
        }
        // 获取端口
        int getPort() const
        {
            return m_iPort;
        }
        // 设置超时时间
        void setTimeout(int timeout)
        {
            m_iTimeout = timeout;
        }
        // 获取超时时间
        int getTimeout() const
        {
            return m_iTimeout;
        }
        // 设置为tcp或udp
        void setTcp(bool bTcp)
        {
            m_isTcp = bTcp;
        }
        // 判断是为tcp
        bool isTcp() const
        {
            return m_isTcp;
        }
        // 设置路由状态
        void setGrid(int grid)
        {
            m_iGrid = grid;
        }
        // 获取路由状态
        int getGrid() const
        {
            return m_iGrid;
        }
        // 设置dscp值
        void setQos(int qos)
        {
            m_iQos = qos;
        }
        // 获取dscp值
        int getQos() const
        {
            return m_iQos;
        }
        // 设置节点的静态权重值
        void setWeight(int weight)
        {
            m_iWeight = weight;
        }
        // 获取节点的静态权重值
        int getWeight() const
        {
            return m_iWeight;
        }
        // 设置节点的权重使用方式
        void setWeightType(unsigned int weighttype)
        {
            m_iWeightType = weighttype;
        }
        // 获取节点的权重使用方式
        unsigned int getWeightType() const
        {
            return m_iWeightType;
        }
        // 是否本地套接字
        bool isUnixLocal() const
        {
            return m_iPort == 0;
        }
        // 字符串描述
        string toString()
        {
            ostringstream os;
            os << (isTcp()?"tcp" : "udp") << " -h " << m_strHost << " -p " << m_iPort << " -t " << m_iTimeout;
            if (m_iGrid != 0) os << " -g " << m_iGrid;
            if (m_iQos != 0) os << " -q " << m_iQos;
            if (m_iWeight != -1) os << " -w " << m_iWeight;
            if (m_iWeightType != 0) os << " -v " << m_iWeightType;
            return os.str();
        }
        // 解析字符串形式的端口
        void parse(const string &strDesc);
    private:
        void init(const string& host, int port, int timeout, int istcp, int grid, int qos, int weight, unsigned int weighttype);

    private:
        string m_strHost;           // ip
        int m_iPort;                // 端口
        int m_iTimeout;             // 超时时间
        int m_isTcp;                // 类型(TCP/UDP)
        int m_iGrid;                // 路由状态
        int m_iQos;                 // 网络Qos中的dscp值
        int m_iWeight;              // 节点的静态权重值
        unsigned int m_iWeightType; // 节点的权重使用方式
    };

    // 客户端socket相关操作基类
    class TSocketClient
    {
    public:
        // 定义发送到错误
        enum
        {
            EM_SUCCESS  =  0,     // EM_SUCCESS: 发送成功
            EM_SEND     = -1,     // EM_SEND: 发送错误
            EM_SELECT   = -2,     // EM_SELECT: select 错误
            EM_TIMEOUT  = -3,     // EM_TIMEOUT: select超时
            EM_RECV     = -4,     // EM_RECV: 接受错误
            EM_CLOSE    = -5,     // EM_CLOSE: 服务器主动关闭
            EM_CONNECT  = -6,     // EM_CONNECT: 服务器连接失败
            EM_SOCKET   = -7      // EM_SOCKET: SOCKET初始化失败
        };

        // 构造函数
        TSocketClient() : m_iPort(0), m_iTimeout(3000) {}
        // 构造
        TSocketClient(const string &sIp, int iPort, int iTimeout)
        {
            init(sIp, iPort, iTimeout);
        }
        // 析构函数
        virtual ~TSocketClient() {}
        // 初始化
        void init(const string &sIp, int iPort, int iTimeout)
        {
            m_socket.close();
            m_strIp = sIp;
            m_iPort = iPort;
            m_iTimeout = iTimeout;
        }
        // 发送到服务器
        virtual int send(const char *sSendBuffer, size_t iSendLen) = 0;
        // 从服务器返回不超过iRecvLen的字节
        virtual int recv(char *sRecvBuffer, size_t &iRecvLen) = 0;
    protected:
        TSocket m_socket;   // 套接字句柄
        string  m_strIp;    // ip或文件路径
        int     m_iPort;    // 端口或-1:标示本地套接字
        int     m_iTimeout; // 超时时间
    };

    // tcp客户端socket,多线程使用时,不同时send/recv, 可能串包
    class CTcpClient : public TSocketClient
    {
    public:
        // 构造
        CTcpClient(){}
        // 构造
        CTcpClient(const string &sIp, int iPort, int iTimeout) : TSocketClient(sIp, iPort, iTimeout){}
        // 发送到服务器
        int send(const char *sSendBuffer, size_t iSendLen);
        // 从服务器返回不超过iRecvLen的字节
        int recv(char *sRecvBuffer, size_t &iRecvLen);
        // 接收服务器直到结束符,只能同步调用
        int recvBySep(string &sRecvBuffer, const string &sSep);
        // 接收直到服务器关闭连接为止
        int recvAll(string &sRecvBuffer);
        // 从服务器返回iRecvLen的字节
        int recvLength(char *sRecvBuffer, size_t iRecvLen);
        // 发送到服务器，从服务器返回不超过iRecvLen的字节
        int sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen);
        // 发送到服务器，并等待服务器直到结尾字符
        int sendRecvBySep(const char *sSendBuffer, size_t iSendLen, string &sRecvBuffer, const string &sSep);
        // 发送到服务器，并等待服务器直到结尾字符(\r\n)
        int sendRecvLine(const char *sSendBuffer, size_t iSendLen, string &sRecvBuffer);
        // 发送到服务器，接收直到服务器关闭连接为止
        int sendRecvAll(const char *sSendBuffer, size_t iSendLen, string &sRecvBuffer);
    protected:
        // 校验socket
        int checkSocket();
    };


    // udp客户端socket,多线程使用时,不同时send/recv, 可能串包
    class CUdpClient : public TSocketClient
    {
    public:
        // 构造
        CUdpClient(){};
        // 析构
        CUdpClient(const string &sIp, int iPort, int iTimeout) : TSocketClient(sIp, iPort, iTimeout){}
        // 发送数据
        int send(const char *sSendBuffer, size_t iSendLen);
        // 接收数据
        int recv(char *sRecvBuffer, size_t &iRecvLen);
        // 接收数据，并返回远程端口和ip
        int recv(char *sRecvBuffer, size_t &iRecvLen, string &sRemoteIp, uint16_t &iRemotePort);
        // 发送并接收数据
        int sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen);
        // 发送并接收数据，返回远程ip和端口
        int sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen, string &sRemoteIp, uint16_t &iRemotePort);
    protected:
        // 检测socket有效性
        int checkSocket();
    };
}

#endif