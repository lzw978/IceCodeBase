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
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
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
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "create msgq msgid [%s] error:[%d][%s]", m_szMsgFile, errno, strerror(errno));
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
    //char *pEtc = getenv("SCPS_ETC");
    char *pEtc = "/home/linzhenwei/IceCode/library/comm_smm"; //DEBUG
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
    printf("szEtc path=[%s]\n", szEtc); //DEBUG
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
    m_iBuffLen = m_iMaxMsgLen + 1;
    m_pBuffer  = new char[m_iMaxMsgLen+1];

    if (NULL == m_pBuffer)
    {
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "malloc [%d] error[%d][%s]", m_iMaxMsgLen+1, errno, strerror(errno));
        return -3;
    }

    printf("DEBUG: lpMsgFile=[%s] lpPath=[%s]\n", lpMsgFile, lpPath);
    printf("DEBUG m_iMaxMsgLen=[%d] m_iMsgMsgNum=[%d] bufferLen=[%d]\n", m_iMaxMsgLen, m_iMaxMsgNum, m_iBuffLen);
    return OpenSMM();
}
// 获取消息 iWaitFlag：0-阻塞 1-不阻塞 iWaitTime：等待时间(秒) lpMsgId:同步等待关联ID
int CCommSMM::GetMsg(string& strMsg, int iWaitFlag, int iWaitTime, const char* lpMsgId)
{
    int iRet        = -1;
    int iRecvLen    = -1;
    int iMsgFlag    = 0;
    long lMsgType   = 9;
    char* pRecvData = NULL;
    TSMM_MSG stuMsg;
    memset(&stuMsg, 0, sizeof(stuMsg));

    // 消息类型（大于0，则返回其类型为mtype的第一个消息）
    if (NULL != lpMsgId && strlen(lpMsgId) > 0)
    {
        lMsgType = atol(lpMsgId);
    }
    // 不阻塞
    if ( 1 == iWaitFlag)
    {
        iMsgFlag = IPC_NOWAIT;
    }
    // 接收数据
    for (int i=0; i<iWaitTime+1; ++i)
    {
        iRecvLen = msgrcv(m_iMsgId, &stuMsg, 8, lMsgType, iMsgFlag);
        if (iRecvLen < 0)
        {
            // 没有消息
            if ( IPC_NOWAIT == iMsgFlag && ENOMSG == errno)
            {
                // 如果不等待，没有消息则立即返回,否则休眠继续等待
                if ( 0 == iWaitTime)
                {
                    return 0;
                }
                sleep(1);
                continue;
            }
            // 其他错误返回错误码跟描述
            m_iErrCode = errno;
            snprintf(m_szErrDesc, sizeof(m_szErrDesc), "recv msg error[%d][%s]", errno, strerror(errno));
            return iRecvLen;
        }
        else
        {
            // 读取消息成功
            break;
        }
    }

    // 判断读取的数据
    if (iRecvLen < 0)
    {
        if ( IPC_NOWAIT == iMsgFlag && ENOMSG == errno)
        {
            snprintf(m_szErrDesc, sizeof(m_szErrDesc), "recv msg no data.");
            return 0;
        }
        m_iErrCode = errno;
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "recv msg error[%d][%s]", errno, strerror(errno));
        return iRecvLen;
    }

    // 记录偏移量
    int iPos = atoi(stuMsg.mtext);
    // 读取数据
    pRecvData = (char*)shmat(m_iShmId, NULL, 0);
    if (NULL == pRecvData || 0 == pRecvData[iPos*m_iMaxMsgLen+1])
    {
        iRecvLen = 1;
        strMsg = "";
    }
    else
    {
        memset(m_pBuffer, 0, m_iBuffLen);
        memcpy(m_pBuffer, pRecvData+iPos*m_iMaxMsgLen+1, m_iMaxMsgLen-1);
        strMsg = m_pBuffer + 1;
        iRecvLen = strMsg.length();
    }
    // 还原回“未使用”状态（此行代码别的进程就变为N）
    memcpy(pRecvData+iPos*m_iMaxMsgLen, "N", 1);
    // 此行代码不会影响别的进程的数据
    shmdt(pRecvData);

    // 文件存储
    if ('F' == m_pBuffer[0])
    {

        int  iFileLen = 0;
        char szFile[256+1]  = {0};
        char szFileLen[8+1] = {0};
        char *pFileBuff = NULL;

        memcpy(szFileLen, m_pBuffer+1, 8);
        memcpy(szFile   , m_pBuffer+9, 256);
        iFileLen = atoi(szFileLen);

        printf("DEBUG: read Largemsg=[%s]\n", szFile);

        // 打开文件
        FILE *fp = fopen(szFile, "rb");
        if ( NULL == fp)
        {
            snprintf(m_szErrDesc, sizeof(m_szErrDesc), "fopen [%s] error [%d][%s]", szFile, errno, strerror(errno));
            return -2;
        }
        // 申请缓冲区
        pFileBuff = new char[iFileLen+1];
        // 读取文件
        fread(pFileBuff, 1, iFileLen, fp);
        fclose(fp);
        // 防止结尾乱码
        pFileBuff[iFileLen] = 0;
        strMsg = pFileBuff;
        iRecvLen = strMsg.length();
        // 释放资源
        delete pFileBuff;
        remove(szFile);
    }
    return iRecvLen;
}
// 发送消息
int CCommSMM::PutMsg(const char* lpMsg, int iLen, const char* lpMsgId)
{
    int iRet = -1;
    int iSendLen = -1;
    char *pSendData = NULL;
    TSMM_MSG stuMsg;
    memset(&stuMsg, 0, sizeof(stuMsg));

    // 报文超过内存大小，转为文件存储(在锁外面完成，减少锁时间)
    char szFile[256+1] = {0};
    char szFileLen[8+1] = {0};
    if (iLen > (m_iMaxMsgLen-8))
    {
        // 构建报文文件名:示例:
        char szTmp[32] = {0};
        snprintf(szTmp, sizeof(szTmp), "%010d%s", getpid(), GetSMMCurrTime());
        strcat(szFile, m_szPath);
        strcat(szFile, "/SMM");
        strcat(szFile, szTmp);
        sprintf(szFileLen, "%08d", iLen);

        printf("DEBUG: write LargeMsg=[%s] szFileLen=[%s] iLen=[%d]\n", szFile, szFileLen, iLen);

        // 打开文件
        FILE *fp = fopen(szFile, "wb");
        if ( NULL == fp)
        {
            snprintf(m_szErrDesc, sizeof(m_szErrDesc), "fopen [%s] error [%d][%s]", szFile, errno, strerror(errno));
            return -2;
        }
        // 写入文件
        fwrite(lpMsg, 1, iLen, fp);
        fclose(fp);
    }

    // 加锁
    struct flock fLck;
    fLck.l_type   = F_WRLCK;
    fLck.l_whence = SEEK_SET;
    fLck.l_start  = 0;
    fLck.l_len    = 0;

    iRet = fcntl(m_iFdLck, F_SETLKW, &fLck);
    if (iRet < 0)
    {
        m_iErrCode = errno;
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "lock [%s][%d] error [%d][%s]", m_szMsgFile, m_iFdLck, errno, strerror(errno));
        return -1;
    }

    // 存储数据(获取共享内存中报文数量)
    pSendData = (char*)shmat(m_iShmId, NULL, 0);
    int i = 0;
    for (i=0; i<m_iMaxMsgNum; ++i)
    {
        if ( 'U' == pSendData[i*m_iMaxMsgLen])
        {
            continue;
        }
        else // "未使用"状态
        {
            // 报文超过内存大小，转为文件存储
            if (iLen > (m_iMaxMsgLen-8))
            {
                memset(pSendData+i*m_iMaxMsgLen  , 0, m_iMaxMsgLen);
                memcpy(pSendData+i*m_iMaxMsgLen  , "U", 1);
                memcpy(pSendData+i*m_iMaxMsgLen+1, "F", 1);
                memcpy(pSendData+i*m_iMaxMsgLen+2, szFileLen, 8);
                memcpy(pSendData+i*m_iMaxMsgLen+10, szFile, strlen(szFile));
            }
            else
            {
                memset(pSendData+i*m_iMaxMsgLen  , 0, m_iMaxMsgLen);
                memcpy(pSendData+i*m_iMaxMsgLen  , "U", 1);
                memcpy(pSendData+i*m_iMaxMsgLen+1, "N", 1);
                memcpy(pSendData+i*m_iMaxMsgLen+2, lpMsg, iLen);
            }
            break;
        }
    }
    shmdt(pSendData);

    // 解锁
    struct flock fUnLck;
    fUnLck.l_type   = F_UNLCK;
    fUnLck.l_whence = SEEK_SET;
    fUnLck.l_start  = 0;
    fUnLck.l_len    = 0;
    iRet = fcntl(m_iFdLck, F_SETLK, &fUnLck);
    if (iRet < 0)
    {
        m_iErrCode = errno;
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "unlock [%s][%d] error [%d][%s]", m_szMsgFile, m_iFdLck, errno, strerror(errno));
        return -1;
    }

    // 判断是否超过报文个数
    if ( i >= m_iMaxMsgNum)
    {
        m_iErrCode = -1;
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "shm catch full.[%d]", m_iMaxMsgNum);
        return -1;
    }

    // 消息类型
    stuMsg.mtype = 9;
    if (NULL!=lpMsgId && strlen(lpMsgId)>0)
    {
        stuMsg.mtype = atol(lpMsgId);
    }
    sprintf(stuMsg.mtext, "%08d", i);

    // 发送数据
    iSendLen = msgsnd(m_iMsgId, &stuMsg, 8, IPC_NOWAIT);
    if (iSendLen < 0)
    {
        m_iErrCode = errno;
        snprintf(m_szErrDesc, sizeof(m_szErrDesc), "send msg error...[%d][%s]", errno, strerror(errno));
        return iSendLen;
    }
    return iSendLen;
}
// 获取当前时间(20字节长度)
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