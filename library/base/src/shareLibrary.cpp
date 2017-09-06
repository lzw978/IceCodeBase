/********************************************
 **  模块名称：shareLibrary.cpp
 **  模块功能:
 **  模块简述: 加载共享库实现
 **  编 写 人:
 **  日    期: 2017.08.08
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include "shareLibrary.h"

using namespace commbase;
//构造
SharedLibrary::SharedLibrary()
{
    m_hLoad = NULL;
    m_iErrCode = 0;
    memset(m_szErrDesc, 0, sizeof(m_szErrDesc));
    m_mapHandle.clear();
}
// 析构
SharedLibrary::~SharedLibrary()
{
    m_mapHandle.clear();
}
// 获取错误码
int SharedLibrary::GetErrCode()
{
    return m_iErrCode;
}
// 获取错误描述
char* SharedLibrary::GetErrDesc()
{
    return m_szErrDesc;
}
// 加载共享库
int SharedLibrary::Load(const char* pLibPath, ldmode_t mode)
{
    int iFlag = -1;
    iFlag = RTLD_NOW;

    // 查找是否在动态库列表中是否已存在
    MAPHANDLE::iterator it = m_mapHandle.find(pLibPath);
    if (it != m_mapHandle.end())
    {
        m_hLoad = (void*)it->second;
        return 0;
    }

    // 新加载
    m_hLoad = dlopen(pLibPath, iFlag);
    if (NULL == m_hLoad)
    {
        m_iErrCode = errno;
        strncpy(m_szErrDesc, dlerror(), sizeof(m_szErrDesc)-1);
        return -1;
    }
    // 插入到加载列表中
    m_mapHandle[pLibPath] = (void*)m_hLoad;
    return 0;
}
// 获取共享库函数地址
void* SharedLibrary::GetAddr(const char *pSymbol)
{
    if (NULL == m_hLoad)
    {
        m_iErrCode = -1;
        strncpy(m_szErrDesc, "Get lib addr m_hLoad is NULL", sizeof(m_szErrDesc)-1);
        return NULL;
    }

    return dlsym(m_hLoad, pSymbol);
}
// 释放加载连接
void SharedLibrary::FreeH()
{
    if (NULL != m_hLoad)
    {
        dlclose(m_hLoad);
        m_hLoad = NULL;
    }
}