/********************************************
 **  模块名称：tmmap.h
 **  模块功能:
 **  模块简述: 共享内存mmap封装实现
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <cerrno>
#include <cassert>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "tmmap.h"
#include "exception.h"

using namespace commbase;

// 构造函数
TMMap::TMMap(bool bOwner) : m_bOwner(bOwner), m_iLength(0), m_bCreate(false), m_pAddr(NULL)
{
}
// 析构函数
TMMap::~TMMap()
{
    if (m_bOwner)
    {
        munmap();
    }
}
// 映射到内存空间
void TMMap::mmap(size_t length, int prot, int flags, int fd, off_t offset)
{
    if (m_bOwner)
    {
        munmap();
    }

    m_pAddr = ::mmap(NULL, length, prot, flags, fd, offset);
    if (m_pAddr == (void*)-1)
    {
        m_pAddr = NULL;
        throw Exception(Z_SOURCEINFO, errno, "mmap error");
    }
    m_iLength = length;
    m_bCreate = false;
}
// 映射到进程空间，采用 PROT_READ|PROT_WRITE,MAP_SHARED方式
// 注意的文件大小会比length大一个字节(初始化时生成空洞文件的原因)
void TMMap::mmap(const char *file, size_t length)
{
    assert(length > 0);
    if (m_bOwner)
    {
        munmap();
    }
    int fd = open(file, O_CREAT|O_EXCL|O_RDWR, 0666);
    if ( fd == -1)
    {
        if (errno != EEXIST)
        {
            throw Exception(Z_SOURCEINFO, errno, "open file error " + string(file));
        }
        else
        {
            fd = open(file, O_CREAT|O_RDWR, 0666);
            if (fd == -1)
            {
                throw Exception(Z_SOURCEINFO, errno, "open file error " + string(file));
            }
            m_bCreate = false;
        }
    }
    else
    {
        m_bCreate = true;
    }

    lseek(fd, length-1, SEEK_SET);
    write(fd, "\0", 1);

    m_pAddr = ::mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (m_pAddr == (void*)-1)
    {
        m_pAddr = NULL;
        close(fd);
        throw Exception(Z_SOURCEINFO, errno, "mmap file error " + string(file));
    }
    m_iLength = length;
    if (fd != -1)
    {
        close(fd);
    }
}
// 解除映射关系，解除后不能继续访问该空间
void TMMap::munmap()
{
    if (m_pAddr == NULL)
    {
        return ;
    }

    int iRet = ::munmap(m_pAddr, m_iLength);
    if (iRet == -1)
    {
        throw Exception(Z_SOURCEINFO, errno, "munmap file error");
    }

    m_pAddr   = NULL;
    m_iLength = 0;
    m_bCreate = false;
}
// 把共享内存的改变写回磁盘中 bSync true=同步写回 false=异步写回
void TMMap::msync(bool bSync)
{
    int iRet = 0;
    if (bSync)
    {
        iRet = ::msync(m_pAddr, m_iLength, MS_SYNC | MS_INVALIDATE);
    }
    else
    {
        iRet = ::msync(m_pAddr, m_iLength, MS_ASYNC | MS_INVALIDATE);
    }

    if (iRet != 0)
    {
        throw Exception(Z_SOURCEINFO, errno, "msync error");
    }
}