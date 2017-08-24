/********************************************
 **  模块名称：tsmm.h
 **  模块功能:
 **  模块简述: 信号量+共享内存+消息队列
 **  编 写 人:
 **  日    期: 2017.08.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:共享内存(第1字节表示是否使用，第2字节表示内部是否为文件存储)
             消息队列(一个消息8字节表示内存偏移)(单机使用)
             文件报文  ：UF+8位报文长度(报文长度不超过99999999字节,约95M)+报文内容
             非文件报文：UN+报文内容
             如果超过设置报文长度，报文将转换为文件进行存储，消息队列中存储
 **  问    题：
*********************************************/

#ifndef __LIB_TSMM_H__
#define __LIB_TSMM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>

using namespace std;

#define MIN_MSG_LEN 1024*2   // 最小报文长度(2K)
#define MIN_MSG_NUM 10       // 最小报文个数

class CCommSMM
{
public:
    CCommSMM();
    ~CCommSMM();

    // 创建资源
    int OpenSMM();
    // 初始化
    int InitSMM(const char* lpMsgFile, const char* lpPath, int iMaxMsgLen, int iMaxMsgNum);
    // 获取消息 iWaitFlag：0-阻塞 1-不阻塞 iWaitTime：等待时间(秒) lpMsgId:同步等待关联ID
    int GetMsg(string& strMsg, int iWaitFlag=1, int iWaitTime=0, const char* lpMsgId=NULL);
    // 发送消息
    int PutMsg(const char* lpMsg, int iLen, const char* lpMsgId=NULL);
    // 获取当前时间
    char* GetSMMCurrTime();
    // 获取错误信息
    char* GetErrorDesc();
private:
    int m_iFdLck;               // 文件锁识别码
    int m_iShmId;               // 共享内存识别码
    int m_iMsgId;               // 消息队列识别码
    int m_iMaxMsgLen;           // 报文长度
    int m_iMaxMsgNum;           // 报文个数
    int m_iErrCode;             // 错误码
    int m_iBuffLen;             // 缓冲区长度
    char m_szMsgFile[256+1];    // 消息队列及共享内存名
    char m_szSemFile[256+1];    // 信号量文件名
    char m_szPath[256+1];       // 大报文缓存路径
    char m_szMsgId[24+1];       // 同步等待关联ID
    char m_szErrDesc[1024+1];   // 错误描述
    char* m_pBuffer;            // 缓冲区指针
};

#endif