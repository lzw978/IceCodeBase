/********************************************
 **  模块名称：tmmap.h
 **  模块功能:
 **  模块简述: 共享内存mmap封装
 **  编 写 人:
 **  日    期: 2017.09.11
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 *   1: fd可以指定为-1，此时须指定flags参数中的MAP_ANONYMOUS
 *      表明进行的是匿名映射,不涉及具体的文件名,避免了文件的创建及打开,很显然只能用于具有亲缘关系的进程间通信
 *   2: 创建map时,文件一定需要有length的长度, 否则可能导致越界
 *   3: 2中说的情况一般通过产生空洞文件避免,int mmap(const char *file, size_t length);
 *      实现了类似的封装, 推荐直接使用
 **  问    题：
*********************************************/

#ifndef __LIB_TMMAP_H__
#define __LIB_TMMAP_H__

#include <sys/mman.h>
#include <string>

namespace commbase
{
    class TMMap
    {
    public:
        // 构造函数
        TMMap(bool bOwner = true);
        // 析构函数
        ~TMMap();
        // 映射到内存空间
        /* length: 映射到进程地址空间的字节数,从offset个字节算起
            prot : 指定共享内存的访问权限,可取值为(几个值的或):
                    PROT_READ: 可读
                    PROT_WRITE: 可写
                    PROT_EXEC : 可执行
                    PROT_NONE: 不可访问
            flags : MAP_SHARED,MAP_PRIVATE,MAP_FIXED:一般取:MAP_SHARED
            fd    : 进程空间中的文件描述符
            offset: 文件的偏移地址, 一般为0 */
        void mmap(size_t length, int prot, int flags, int fd, off_t offset = 0);
        // 映射到进程空间，采用 PROT_READ|PROT_WRITE,MAP_SHARED方式
        // 注意的文件大小会比length大一个字节(初始化时生成空洞文件的原因)
        void mmap(const char *file, size_t length);
        // 解除映射关系，解除后不能继续访问该空间
        void munmap();
        // 把共享内存的改变写回磁盘中 bSync true=同步写回 false=异步写回
        void msync(bool bSync = false);
        // 获取映射到指针地址
        void *getPointer() const
        {
            return m_pAddr;
        }
        // 获取映射空间的大小
        size_t getSize() const
        {
            return m_iLength;
        }
        // 是否创建出来的
        bool isCreate() const
        {
            return m_bCreate;
        }
        // 设置是否拥有
        void setOwner(bool bOwner)
        {
            m_bOwner = bOwner;
        }
    protected:
        bool    m_bOwner;  // 是否拥有
        bool    m_bCreate; //是否创建出来的
        size_t  m_iLength; // 映射的空间大小
        void    *m_pAddr;  //映射到进程空间的地址
    };
}

#endif