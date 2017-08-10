/********************************************
 **  模块名称：ShareMemory.cpp
 **  模块功能:
 **  模块简述: 共享内存类实现
 **  编 写 人: lzw978
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include "ShareMemory.h"

// 构造函数
CShareMemory::CShareMemory(void)
{
    m_bExistFlag = false;
    m_pMemory = NULL;
    m_nSize = 0;
    m_hShareMemHandle = INVALID_SHAREMEM_HANDLE;
    memset(m_szShareMemName, 0, sizeof(m_szShareMemName));
}
// 构造函数(同时创建共享内存)
CShareMemory::CShareMemory(const char* pszName, int nSize)
{
    m_bExistFlag = false;
    m_pMemory = NULL;
    m_nSize = 0;
    m_hShareMemHandle = INVALID_SHAREMEM_HANDLE;
    Create(pszName, nSize);
}
// 析构函数
CShareMemory::~CShareMemory()
{
    if( GetMemoryAddress() != NULL)
    {
        Destroy();
    }
}

// 创建共享内存
int CShareMemory::Create(const char* pszName, int nSize)
{
    memset(m_szShareMemName, 0, sizeof(m_szShareMemName));
    m_szShareMemName[0] = '/'; // 共享内存名称前面必须加上字符'/'
    strncpy(&m_szShareMemName[1], pszName, sizeof(m_szShareMemName)-2);

    // 替换共享内存名称中的'/'为'_'
    for(int i=0; m_szShareMemName[i]!=0; ++i)
    {
        if(m_szShareMemName[i] == '/')
        {
            m_szShareMemName[i] = '_';
        }
    }

    // 尝试创建新共享内存
    m_hShareMemHandle = shm_open(m_szShareMemName, O_RDWR | O_CREAT | O_EXCL, 0777);
    if( m_hShareMemHandle == INVALID_SHAREMEM_HANDLE)
    {
        // 已经被创建
        if( errno == EEXIST)
        {
            // 共享内存已存在，打开内存
            m_hShareMemHandle = shm_open(m_szShareMemName, O_RDWR, 0777);
            if( m_hShareMemHandle == INVALID_SHAREMEM_HANDLE)
            {
                // 打开失败
                m_szShareMemName[0] = 0;
                printf("Open shareMemory error\n");
                return -1;
            }
        }
        else
        {
            // 共享内存不存在，创建失败
            m_szShareMemName[0] = 0;
            return -1;
        }
    }
    else
    {
        // 共享内存创建成功
        m_bExistFlag = false;
    }

    // 设置共享内存大小
    if( ftruncate(m_hShareMemHandle, nSize) != 0)
    {
        // 设置共享内存大小失败
        close(m_hShareMemHandle);
        m_hShareMemHandle = INVALID_SHAREMEM_HANDLE;
        shm_unlink(m_szShareMemName);
        m_szShareMemName[0] = 0;
        return -1;
    }

    // 将哦你共享内存映射到进程空间
    m_pMemory = mmap(NULL, nSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_hShareMemHandle, 0);
    if( m_pMemory == MAP_FAILED)
    {
        // 映射到进程空间失败
        close(m_hShareMemHandle);
        m_hShareMemHandle = INVALID_SHAREMEM_HANDLE;
        shm_unlink(m_szShareMemName);
        m_pMemory = NULL;
        m_szShareMemName[0] = 0;
        return -1;
    }

    // 尝试进行写锁定
    if( LockShareMemory(true))
    {
        // 写锁定成功，说明没有存在实例
        m_bExistFlag = false;
        UnlockShareMemory();
    }
    else
    {
        // 写锁定失败，说明已经有实例存在
        m_bExistFlag = true;
    }

    // 对共享内存进行读锁定
    LockShareMemory(false);
    // 共享内存创建成功，保存大小
    m_nSize = nSize;

    return m_hShareMemHandle;
}

// 销毁已经创建的共享内存
void CShareMemory::Destroy(void)
{
    if( m_pMemory == NULL)
        return ;

    // 解除对共享内存文件的读锁定
    UnlockShareMemory();
    // 判断本对象是否该共享内存最后一个实例
    bool bLastInstance = false;
    if( LockShareMemory(true))
    {
        // 写锁定成功，说明是最后的实例
        bLastInstance = true;
        UnlockShareMemory();
    }

    if( m_hShareMemHandle != INVALID_SHAREMEM_HANDLE)
    {
        close(m_hShareMemHandle);
    }
    if( bLastInstance )
    {
        // 若最后一个共享内存实例被释放，删除共享内存
        shm_unlink(m_szShareMemName);
    }
    m_bExistFlag = false;
    m_nSize = 0;
    memset(m_szShareMemName, 0, sizeof(m_szShareMemName));
}
// 创建是否已创建的共享内存对象
bool CShareMemory::IsCreated(void)
{
    return (m_pMemory != NULL);
}
// 返回在本实例创建前，共享内存对象是否已存在
bool CShareMemory::IsExistedBeforeCreate(void)
{
    return m_bExistFlag;
}
// 取共享内存地址
void* CShareMemory::GetMemoryAddress(void)
{
    return m_pMemory;
}

// 加锁：锁定共享内存,禁止其它进程访问
// 参数: bWriteLock - [in]标识是否是写锁定(若为FALSE表示仅仅是读锁定)
// 返回: 若成功则返回TRUE, 否则返回FALSE
// 说明: 读锁定可以允许多个进程进行,但写锁定只能一个进程锁定.在已经有读锁定的
//      情况下进行写锁定将会返回失败
bool CShareMemory::LockShareMemory(bool bWriteLock)
{
    struct flock sLocking;
    if( bWriteLock)
    {
        sLocking.l_type = F_WRLCK;
    }
    else
    {
        sLocking.l_type = F_RDLCK;
    }
    sLocking.l_whence = SEEK_SET;
    sLocking.l_start  = 0;
    sLocking.l_len     = m_nSize;
    int nRet = fcntl(m_hShareMemHandle, F_SETLK, &sLocking);
    return (nRet == 0);
}

// 解锁：解除对共享内存的,允许其它进程访问
void CShareMemory::UnlockShareMemory(void)
{
    struct flock sUnlocking;
    sUnlocking.l_type   = F_UNLCK;
    sUnlocking.l_whence = SEEK_SET;
    sUnlocking.l_start  = 0;
    sUnlocking.l_len    = m_nSize;
    fcntl(m_hShareMemHandle, F_SETLKW, &sUnlocking);
}
