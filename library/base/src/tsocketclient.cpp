/********************************************
 **  模块名称：tsocketclient.cpp
 **  模块功能:
 **  模块简述: socket客户端基类实现
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <cerrno>
#include <iostream>
#include "tsocketclient.h"
#include "tepoller.h"
#include "exception.h"

using namespace commbase;

#define LEN_MAXRECV 8196 // 接收缓冲区长度

TEndpoint::TEndpoint()
{
    m_strHost     = "0.0.0.0";
    m_iPort       = 0;
    m_iTimeout    = 3000;
    m_iGrid       = 0;
    m_iQos        = 0;
    m_iWeight     = -1;
    m_iWeightType = 0;
    m_bIsTcp      = true;
}

void TEndpoint::init(const string& host, int port, int timeout, bool istcp, int grid, int qos, int weight, unsigned int weighttype)
{
    m_strHost     = host;
    m_iPort       = port;
    m_iTimeout    = timeout;
    m_bIsTcp      = istcp;
    m_iGrid       = grid;
    m_iQos        = qos;

    if (weighttype == 0)
    {
        m_iWeight = -1;
        m_iWeightType = 0;
    }
    else
    {
        if (weight == -1)
        {
            weight = 100;
        }
        m_iWeight = (weight > 100 ? 100 : weight);
        m_iWeightType = weighttype;
    }
}

void TEndpoint::parse(const string &strDesc)
{
    m_iGrid       = 0;
    m_iQos        = 0;
    m_iWeight     = -1;
    m_iWeightType = 0;

    const string delim = " \t\n\r";

    string::size_type beg;
    string::size_type end = 0;

    // 查询第一个非空字符
    beg = strDesc.find_first_not_of(delim, end);
    if (beg == string::npos)
    {
        throw Exception(Z_SOURCEINFO, -1, "parse error : " + strDesc);
    }

    end = strDesc.find_first_of(delim, beg);
    if (end == string::npos)
    {
        end = strDesc.length();
    }

    string desc = strDesc.substr(beg, end - beg);
    if (desc == "tcp")
    {
        m_bIsTcp = true;
    }
    else if (desc == "upd")
    {
        m_bIsTcp = false;
    }
    else
    {
        throw Exception(Z_SOURCEINFO, -1, "parse tcp or udp error : " + strDesc);
    }

    desc = strDesc.substr(end);
    end  = 0;
    while(true)
    {
        beg = desc.find_first_not_of(delim, end);
        if (beg == string::npos)
        {
            break;
        }

        end = desc.find_first_of(delim, beg);
        if (end == string::npos)
        {
            end = desc.length();
        }

        string option = desc.substr(beg, end - beg);
        if (option.length() != 2 || option[0] != '-')
        {
            throw Exception(Z_SOURCEINFO, -1, "parse error : " + strDesc);
        }

        string argument;
        string::size_type argumentBeg = desc.find_first_not_of(delim, end);
        if (argumentBeg != string::npos && desc[argumentBeg] != '-')
        {
            beg = argumentBeg;
            end = desc.find_first_of(delim, beg);
            if ( end == string::npos)
            {
                end = desc.length();
            }
            argument = desc.substr(beg, end - beg);
        }

        // 命令选项
        switch(option[1])
        {
            case 'h':
            {
                if (argument.empty())
                {
                    throw Exception(Z_SOURCEINFO, -1, "parse -h error: " + strDesc);
                }
                const_cast<string&>(m_strHost) = argument;
                break;
            }
            case 'p':
            {
                istringstream p(argument);
                if (!(p >> const_cast<int&>(m_iPort)) || !p.eof() || m_iPort < 0 || m_iPort > 65535)
                {
                    throw Exception(Z_SOURCEINFO, -1, "parse -p error: " + strDesc);
                }
                break;
            }
            case 't':
            {
                istringstream t(argument);
                if ( !(t >> const_cast<int&>(m_iTimeout)) || !t.eof())
                {
                    throw Exception(Z_SOURCEINFO, -1, "parse -t error: " + strDesc);
                }
                break;
            }
            case 'g':
            {
                istringstream g(argument);
                if ( !(g >> const_cast<int&>(m_iGrid)) || !g.eof())
                {
                    throw Exception(Z_SOURCEINFO, -1, "parse -g error: " + strDesc);
                }
                break;
            }
            case 'q':
            {
                istringstream q(argument);
                if ( !(q >> const_cast<int&>(m_iQos)) || !q.eof())
                {
                    throw Exception(Z_SOURCEINFO, -1, "parse -q error: " + strDesc);
                }
                break;
            }
            case 'w':
            {
                istringstream w(argument);
                if ( !(w >> const_cast<int&>(m_iWeight)) || !w.eof())
                {
                    throw Exception(Z_SOURCEINFO, -1, "parse -w error: " + strDesc);
                }
                break;
            }
            case 'v':
            {
                istringstream v(argument);
                if ( !(v >> const_cast<unsigned int&>(m_iWeightType)) || !v.eof())
                {
                    throw Exception(Z_SOURCEINFO, -1, "parse -v error: " + strDesc);
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }

    if (m_iWeightType != 0)
    {
        if (m_iWeight == -1)
        {
            m_iWeight = 100;
        }
        m_iWeight = (m_iWeight > 100 ? 100 : m_iWeight);
    }

    if (m_strHost.empty())
    {
        throw Exception(Z_SOURCEINFO, -1, "parse error: host must not be empty: " + strDesc);
    }
    else if (m_strHost == "*")
    {
        const_cast<string&>(m_strHost) = "0.0.0.0";
    }
}

//---------------------------------TCP_CLIENT----------------------------
// 校验socket
int CTcpClient::checkSocket()
{
    if (!m_socket.isValid())
    {
        try
        {
            if (m_iPort == 0)
            {
                m_socket.createSocket(SOCK_STREAM, AF_LOCAL);
            }
            else
            {
                m_socket.createSocket(SOCK_STREAM, AF_INET);
            }
            // 设置非阻塞模式
            m_socket.setblock(false);

            // 连接
            try
            {
                if (m_iPort == 0)
                {
                    m_socket.connect(m_strIp.c_str());
                }
                else
                {
                    m_socket.connect(m_strIp, m_iPort);
                }
            }
            catch(Exception &ex)
            {
                if (errno != EINPROGRESS)
                {
                    m_socket.close();
                    return EM_CONNECT;
                }
            }

            if (errno != EINPROGRESS)
            {
                if (errno != EINPROGRESS)
                {
                    m_socket.close();
                    return EM_CONNECT;
                }
            }

            TEpoller epoller(false);
            epoller.create(1);
            epoller.add(m_socket.getfd(), 0, EPOLLOUT);
            int iRetCode = epoller.wait(m_iTimeout);
            if (iRetCode < 0)
            {
                m_socket.close();
                return EM_SELECT;
            }
            else if (iRetCode == 0)
            {
                m_socket.close();
                return EM_TIMEOUT;
            }
            else
            {
                for (int i=0; i<iRetCode; ++i)
                {
                    const epoll_event& ev = epoller.get(i);
                    if (ev.events & EPOLLERR || ev.events & EPOLLHUP)
                    {
                        m_socket.close();
                        return EM_CONNECT;
                    }
                    else
                    {
                        int iVal = 0;
                        socklen_t iLen = static_cast<socklen_t>(sizeof(int));
                        if (::getsockopt(m_socket.getfd(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&iVal), &iLen) == -1 || iVal)
                        {
                            m_socket.close();
                            return EM_CONNECT;
                        }
                    }
                }
            }
            // 设置为阻塞模式
            m_socket.setblock(true);
        }
        catch(Exception &ex)
        {
            m_socket.close();
            return EM_SOCKET;
        }
    }
    return EM_SUCCESS;
}
// 发送到服务器
int CTcpClient::send(const char *sSendBuffer, size_t iSendLen)
{
    int iRet = checkSocket();
    if (iRet < 0)
    {
        return iRet;
    }

    iRet = m_socket.send(sSendBuffer, iSendLen);
    if (iRet < 0)
    {
        m_socket.close();
        return EM_SEND;
    }
    return EM_SUCCESS;
}
// 从服务器返回不超过iRecvLen的字节
int CTcpClient::recv(char *sRecvBuffer, size_t &iRecvLen)
{
    int iRet = checkSocket();
    if (iRet < 0)
    {
        return iRet;
    }

    TEpoller epoller(false);
    epoller.create(1);
    epoller.add(m_socket.getfd(), 0, EPOLLIN);

    int iRetCode = epoller.wait(m_iTimeout);
    if (iRetCode < 0)
    {
        m_socket.close();
        return EM_SELECT;
    }
    else if (iRetCode == 0)
    {
        m_socket.close();
        return EM_TIMEOUT;
    }

    epoll_event ev = epoller.get(0);
    if (ev.events & EPOLLIN)
    {
        int iLen = m_socket.recv((void*)sRecvBuffer, iRecvLen);
        if (iLen < 0)
        {
            m_socket.close();
            return EM_RECV;
        }
        else if (iLen == 0)
        {
            m_socket.close();
            return EM_CLOSE;
        }

        iRecvLen = iLen;
        return EM_SUCCESS;
    }
    else
    {
        m_socket.close();
    }
    return EM_SELECT;
}
// 接收服务器直到结束符,只能同步调用
int CTcpClient::recvBySep(string &sRecvBuffer, const string &sSep)
{
    sRecvBuffer.clear();

    int iRet = checkSocket();
    if (iRet < 0)
    {
        return iRet;
    }

    TEpoller epoller(false);
    epoller.create(1);
    epoller.add(m_socket.getfd(), 0, EPOLLIN);

    while(true)
    {
        int iRetCode = epoller.wait(m_iTimeout);
        if (iRetCode < 0)
        {
            m_socket.close();
            return EM_SELECT;
        }
        else if (iRetCode == 0)
        {
            m_socket.close();
            return EM_TIMEOUT;
        }

        epoll_event ev = epoller.get(0);
        if (ev.events & EPOLLIN)
        {
            char buffer[LEN_MAXRECV] = {0};
            int len = m_socket.recv((void*)&buffer, sizeof(buffer));
            if (len < 0)
            {
                m_socket.close();
                return EM_RECV;
            }
            else if (len == 0)
            {
                m_socket.close();
                return EM_CLOSE;
            }

            sRecvBuffer.append(buffer, len);
            if (sRecvBuffer.length() >= sSep.length() &&
                sRecvBuffer.compare(sRecvBuffer.length() - sSep.length(), sSep.length(), sSep) == 0)
            {
                break;
            }
        }
    }
    return EM_SUCCESS;
}
// 接收直到服务器关闭连接为止
int CTcpClient::recvAll(string &sRecvBuffer)
{
    sRecvBuffer.clear();
    int iRet = checkSocket();
    if (iRet < 0)
    {
        return iRet;
    }

    TEpoller epoller(false);
    epoller.create(1);
    epoller.add(m_socket.getfd(), 0, EPOLLIN);

    while(true)
    {
        int iRetCode = epoller.wait(m_iTimeout);
        if (iRetCode < 0)
        {
            m_socket.close();
            return EM_SELECT;
        }
        else if (iRetCode == 0)
        {
            m_socket.close();
            return EM_TIMEOUT;
        }

        epoll_event ev = epoller.get(0);
        if (ev.events & EPOLLIN)
        {
            char sTmpBuffer[LEN_MAXRECV] = {0};
            int len = m_socket.recv((void*)sTmpBuffer, LEN_MAXRECV);
            if (len < 0)
            {
                m_socket.close();
                return EM_RECV;
            }
            else if (len == 0)
            {
                m_socket.close();
                return EM_CLOSE;
            }

            sRecvBuffer.append(sTmpBuffer, len);
        }
        else
        {
            m_socket.close();
            return EM_SELECT;
        }
    }
    return EM_SUCCESS;
}
// 从服务器返回iRecvLen的字节
int CTcpClient::recvLength(char *sRecvBuffer, size_t iRecvLen)
{
    int iRet = checkSocket();
    if (iRet < 0)
    {
        return iRet;
    }

    size_t iRecvLeft = iRecvLen;
    iRecvLen = 0;

    TEpoller epoller(false);
    epoller.create(1);
    epoller.add(m_socket.getfd(), 0, EPOLLIN);

    while( iRecvLeft != 0)
    {
        int iRetCode = epoller.wait(m_iTimeout);
        if (iRetCode < 0)
        {
            m_socket.close();
            return EM_SELECT;
        }
        else if (iRetCode == 0)
        {
            m_socket.close();
            return EM_TIMEOUT;
        }

        epoll_event ev = epoller.get(0);
        if (ev.events & EPOLLIN)
        {
            int len = m_socket.recv((void*)(sRecvBuffer + iRecvLen), iRecvLeft);
            if (len < 0)
            {
                m_socket.close();
                return EM_RECV;
            }
            else if (len == 0)
            {
                m_socket.close();
                return EM_CLOSE;
            }
            iRecvLeft -= len;
            iRecvLen  += len;
        }
        else
        {
            m_socket.close();
            return EM_SELECT;
        }
    }
    return EM_SUCCESS;
}
// 发送到服务器，从服务器返回不超过iRecvLen的字节
int CTcpClient::sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen)
{
    int iRet = send(sSendBuffer, iSendLen);
    if (iRet != EM_SUCCESS)
    {
        return iRet;
    }
    return recv(sRecvBuffer, iRecvLen);
}
// 发送到服务器，并等待服务器直到结尾字符
int CTcpClient::sendRecvBySep(const char *sSendBuffer, size_t iSendLen, string &sRecvBuffer, const string &sSep)
{
    int iRet = send(sSendBuffer, iSendLen);
    if (iRet != EM_SUCCESS)
    {
        return iRet;
    }
    return recvBySep(sRecvBuffer, sSep);
}
// 发送到服务器，并等待服务器直到结尾字符(\r\n)
int CTcpClient::sendRecvLine(const char *sSendBuffer, size_t iSendLen, string &sRecvBuffer)
{
    return sendRecvBySep(sSendBuffer, iSendLen, sRecvBuffer, "\r\n");
}
// 发送到服务器，接收直到服务器关闭连接为止
int CTcpClient::sendRecvAll(const char *sSendBuffer, size_t iSendLen, string &sRecvBuffer)
{
    int iRet = send(sSendBuffer, iSendLen);
    if (iRet != EM_SUCCESS)
    {
        return iRet;
    }
    return recvAll(sRecvBuffer);
}

//---------------------------------UDP_CLIENT----------------------------
// 检测socket有效性
int CUdpClient::checkSocket()
{
    if (!m_socket.isValid())
    {
        try
        {
            if (m_iPort == 0)
            {
                m_socket.createSocket(SOCK_DGRAM, AF_LOCAL);
            }
            else
            {
                m_socket.createSocket(SOCK_DGRAM, AF_INET);
            }
        }
        catch(Exception &ex)
        {
            m_socket.close();
            return EM_SOCKET;
        }

        try
        {
            if (m_iPort == 0)
            {
                m_socket.connect(m_strIp.c_str());
                m_socket.bind(m_strIp.c_str());
            }
            else
            {
                m_socket.connect(m_strIp, m_iPort);
            }
        }
        catch(Exception &ex)
        {
            m_socket.close();
            return EM_CONNECT;
        }
    }
    return EM_SUCCESS;
}
// 发送数据
int CUdpClient::send(const char *sSendBuffer, size_t iSendLen)
{
    int iRet = checkSocket();
    if (iRet < 0)
    {
        return iRet;
    }
    iRet = m_socket.send(sSendBuffer, iSendLen);
    if (iRet < 0)
    {
        return EM_SEND;
    }
    return EM_SUCCESS;
}
// 接收数据
int CUdpClient::recv(char *sRecvBuffer, size_t &iRecvLen)
{
    string sTmpIp;
    uint16_t iTmpPort;
    return recv(sRecvBuffer, iRecvLen, sTmpIp, iTmpPort);
}
// 接收数据，并返回远程端口和ip
int CUdpClient::recv(char *sRecvBuffer, size_t &iRecvLen, string &sRemoteIp, uint16_t &iRemotePort)
{
    int iRet = checkSocket();
    if (iRet < 0)
    {
        return iRet;
    }

    TEpoller epoller(false);
    epoller.create(1);
    epoller.add(m_socket.getfd(), 0, EPOLLIN);

    int iRetCode = epoller.wait(m_iTimeout);
    if (iRetCode < 0)
    {
        return EM_SELECT;
    }
    else if (iRetCode == 0)
    {
        return EM_TIMEOUT;
    }

    epoll_event ev = epoller.get(0);
    if (ev.events & EPOLLIN)
    {
        iRet = m_socket.recvfrom(sRecvBuffer, iRecvLen, sRemoteIp, iRemotePort);
        if (iRet < 0)
        {
            return EM_SEND;
        }

        iRecvLen = iRet;
        return EM_SUCCESS;
    }
    return EM_SELECT;
}
// 发送并接收数据
int CUdpClient::sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen)
{
    int iRet = send(sSendBuffer, iSendLen);
    if (iRet != EM_SUCCESS)
    {
        return iRet;
    }
    return recv(sRecvBuffer, iRecvLen);
}
// 发送并接收数据，返回远程ip和端口
int CUdpClient::sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen, string &sRemoteIp, uint16_t &iRemotePort)
{
    int iRet = send(sSendBuffer, iSendLen);
    if (iRet != EM_SUCCESS)
    {
        return iRet;
    }
    return recv(sRecvBuffer, iRecvLen, sRemoteIp, iRemotePort);
}

