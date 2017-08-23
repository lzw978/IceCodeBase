/********************************************
 **  模块名称：tsmm.h
 **  模块功能:
 **  模块简述: 信号量+共享内存+消息队列
 **  编 写 人:
 **  日    期: 2017.08.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include "tsmm.h"

typedef struct {
    long mtype;
    char mtext[9];
}TSMM_MSG;

CCommSMM::CCommSMM()
{
    m_iFdLck     = 0;
    m_iShmId     = 0;
    m_iMsgId     = 0;
    m_iErrCode   = 0;
    m_iBuffLen   = 0;
    m_iMaxMsgLen = 0;
    m_iMaxMsgNum = 0;

    memset(m_szPath   , 0, sizeof(m_szPath));
    memset(m_szMsgId  , 0, sizeof(m_szMsgId));
    memset(m_szMsgFile, 0, sizeof(m_szMsgFile));
    memset(m_szSemFile, 0, sizeof(m_szSemFile));
    memset(m_szErrDesc, 0, sizeof(m_szErrDesc));

    m_pBuffer = NULL;
}

CCommSMM::~CCommSMM()
{
    if (NULL != m_pBuffer)
    {
        delete m_pBuffer;
        m_pBuffer = NULL;
    }
    if (m_iFdLck >= 0)
    {
        close(m_iFdLck);
    }
}

// 创建资源
int CCommSMM::OpenSMM()
{
    key_t key = -1;
    int msgid = -1;

    //  文件锁
    m_iFdLck = open(m_szMsgFile, O_EXCL|O_RDWR, 0600);
    if (m_iFdLck < 0)
    {
        m_iFdLck = open(m_szMsgFile, O_CREAT|O_RDWR, 0600);
    }
    // 创建失败返回
    if (m_iFdLck < 0)
    {
        m_iErrCode = errno;
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "create file lock [%s] error:[%d][%s]", m_szMsgFile, errno, strerror(errno));
        return m_iFdLck;
    }

    // 键值
    key = ftok(m_szMsgFile, 'a');
    if ( -1 == key)
    {
        m_iErrCode = errno;
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "create msgqueue key [%s] error:[%d][%s]", m_szMsgFile, errno, strerror(errno));
        return key;
    }
    // 共享内存
    msgid = shmget(key, m_iMaxMsgLen*m_iMaxMsgNum, IPC_EXCL);
    if (-1 == msgid)
    {
        msgid = shmget(key, m_iMaxMsgLen*m_iMaxMsgNum, IPC_CREAT | 0666);
    }
    if (-1 == msgid)
    {
        m_iErrCode = errno;
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "create shm msgid [%s] error:[%d][%s]", m_szMsgFile, errno, strerror(errno));
        return msgid;
    }
    m_iShmId = msgid;

    // 消息队列
    msgid = msgget(key, IPC_EXCL);
    if (-1 == msgid)
    {
        msgid = msgget(key, IPC_CREAT | 0666);
    }
    if (-1 == msgid)
    {
        m_iErrCode = errno;
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "create msgqueue msgid [%s] error:[%d][%s]", m_szMsgFile, errno, strerror(errno));
        return msgid;
    }
    m_iMsgId = msgid;

    return msgid;
}

// 初始化
int CCommSMM::InitSMM(const char* lpMsgFile, const char* lpPath, int iMaxMsgLen, int iMaxMsgNum)
{
    char szEtc[256+1] = {0};
    char szTmp[256+1] = {0};
    char *pTmp = NULL;
    // 获取配置文件路径
    char *pEtc = getenv("SCPS_ETC");
    if (NULL == pEtc)
    {
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "getenv %s error", "SCPS_ETC");
        return -1;
    }
    // 信号量标识符增加用户名，支持同一台机器部署多套应用
    char *pUser = getenv("USER");
    if (NULL == pUser)
    {
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "getenv %s error", "USER");
        return -1;
    }
    // 用户名转换为大写
    char *pszTmp = pUser;
    while (*pszTmp != 0)
    {
        if (*pszTmp>='a' && *pszTmp<='z')
            *pszTmp = *pszTmp + 'A' - 'a';
        pszTmp++;
    }

    // 从 配置文件路径 中获取 配置文件名称
    while (1)
    {
        pTmp = strstr(pEtc, "/");
        if (NULL != pTmp)
        {
            memset(szTmp, 0, sizeof(szTmp));
            memcpy(szTmp, pEtc, pTmp-pEtc);
            strcat(szEtc, szTmp);
            strcat(szEtc, "/");
            pEtc = pTmp + 1;
        }
        else
        {
            break;
        }
    }
    // 组装消息队列以及共享内存文件名
    strncpy(m_szMsgFile, szEtc, sizeof(m_szMsgFile)-1);
    strcat(m_szMsgFile, ".");
    strcat(m_szMsgFile, lpMsgFile);
    strcat(m_szMsgFile, ".kid");
    // 信号量文件名
    strncpy(m_szSemFile, pUser, sizeof(m_szSemFile)-1);
    strcat(m_szSemFile, lpMsgFile);
    strcat(m_szSemFile, ".kid");
    // 大报文缓存路径
    strncpy(m_szPath, lpPath, sizeof(m_szPath)-1);
    // 设置报文长度(至少为最小长度)
    m_iMaxMsgLen = iMaxMsgLen;
    if (m_iMaxMsgLen < MIN_MSG_LEN)
    {
        m_iMaxMsgLen = MIN_MSG_LEN;
    }
    // 设置报文个数(至少为最小个数)
    m_iMaxMsgNum = iMaxMsgNum;
    if (m_iMaxMsgNum < MIN_MSG_NUM)
    {
        m_iMaxMsgNum = MIN_MSG_NUM;
    }
    // 申请缓冲区
    m_iBuffLen = iMaxMsgLen + 1;
    m_pBuffer  = new char[iMaxMsgLen+1];
    if (NULL == m_pBuffer)
    {
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "malloc [%d] error[%d][%s]", iMaxMsgLen+1, errno, strerror(errno));
        return -3;
    }

    return OpenSMM();
}
// 获取消息 iWaitFlag：0-阻塞 1-不阻塞 iWaitTime：等待时间(秒) lpMsgId:同步等待关联ID
int CCommSMM::GetMsg(string& strMsg, iWaitFlag, int iWaitTime, const char* lpMsgId)
{

}
// 发送消息
int CCommSMM::PutMsg(const char* lpMsg, int iLen, const char* lpMsgId)
{

}
// 获取当前时间
char* CCommSMM::GetSMMCurrTime()
{
    static char szCurrTime[32] = {0};
    struct timeval tp;
    struct tm *p;
    time_t timep;

    time(&timep);
    p = localtime(&timep);
    gettimeofday(&tp, NULL);

    sprintf(szCurrTime, "%04d%02d%02d%02d%02d%02d%06d",
            1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, (int)tp.tv_usec);
    return szCurrTime;
}

// 获取错误信息
char* CCommSMM::GetErrorDesc()
{
    return m_szErrDesc;
}