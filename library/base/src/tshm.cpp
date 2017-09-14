/********************************************
 **  模块名称：tshm.cpp
 **  模块功能:
 **  模块简述: 共享内存封装实现
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <cassert>
#include <cerrno>
#include "tshm.h"
#include "exception.h"

using namespace commbase;

// 构造函数
TShm::TShm(size_t iShmSize, key_t iKey, bool bOwner)
{
    init(iShmSize, iKey, bOwner);
}
// 析构函数
TShm::~TShm()
{
    if( m_bOwner)
    {
        detach();
    }
}

// 初始化
void TShm::init(size_t iShmSize, key_t iKey, bool bOwner)
{
    assert(m_pShm == NULL);

    m_bOwner = bOwner;
    // 创建共享内存
    if ((m_iShmId = shmget(iKey, iShmSize, IPC_CREAT | IPC_EXCL | 0666)) < 0)
    {
        m_bCreate = false;
        // 如果已存在，则进行连接
        if ((m_iShmId = shmget(iKey, iShmSize, 0666)) < 0)
        {
            throw Exception(Z_SOURCEINFO, errno, "shmget error");
        }
    }
    else
    {
        m_bCreate = true;
    }

    // 访问共享内存
    if ((m_pShm = shmat(m_iShmId, NULL, 0)) == (char *)-1)
    {
        throw Exception(Z_SOURCEINFO, errno, "shmat error");
    }

    m_shmSize = iShmSize;
    m_shmKey  = iKey;
}

// 解除共享内存
int TShm::detach()
{
    int iRet = 0;
    if (m_pShm != NULL)
    {
        iRet = shmdt(m_pShm);
        m_pShm = NULL;
    }
}
// 删除共享内存
int TShm::del()
{
    int iRet = 0;
    if (m_pShm != NULL)
    {
        iRet   = shmctl(m_iShmId, IPC_RMID, 0);
        m_pShm = NULL;
    }
    return iRet;
}
