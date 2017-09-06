/********************************************
 **  模块名称：shareLibrary.h
 **  模块功能: 动态库管理类
 **  模块简述: 代码移植自 Portable C++ Application Framework
 **  编 写 人: Christian Prochnow <cproch@seculogix.de>
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
    用于动态加载dll/so并获取函数地址的类, 示例:
    @code
        CSharedLibrary shl("libm.so");
        double (*pCosine)(double) = NULL;
        pCosine = shl.getAddr("cos");
        pCosine(2.0);
    @endcode
 **  问    题：
*********************************************/
#ifndef __LIB_SHARELIBRARY_H__
#define __LIB_SHARELIBRARY_H__

#include <string>
#include <map>

namespace commbase
{
    using namespace std;

    typedef map <string, void*> MAPHANDLE;

    class SharedLibrary
    {
    public:
        enum ldmode_t
        {
            bindNow,  // 立即加载
            bindLazy  // 延迟加载
        };

        SharedLibrary();
        ~SharedLibrary();
        // 加载
        int Load(const char* pLibPath, ldmode_t mode = bindNow);
        // 从动态库中获取函数地址
        void* GetAddr(const char* pSymbol);
        // 关闭动态库
        void FreeH();

        // 获取错误码
        int GetErrCode();
        // 获取错误信息
        char* GetErrDesc();
    private:
        void* m_hLoad;            // 动态库句柄
        int  m_iErrCode;          // 错误码
        char m_szErrDesc[1024+1]; // 错误描述

        MAPHANDLE m_mapHandle; // 存储已加载动态库列表
    };
}


#endif