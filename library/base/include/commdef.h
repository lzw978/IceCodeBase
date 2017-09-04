/********************************************
 **  模块名称：commdef.h
 **  模块功能: 基础库公用的宏定义
 **  模块简述:
 **  编 写 人: lzw978
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_COMMDEF_H__
#define __LIB_COMMDEF_H__

#ifdef _MSC_VER
    #define Z_64BIT_CONSTANT(x)     x##i64
    #define Z_U64BIT_CONSTANT(x)    x##ui64
#else
    #define Z_64BIT_CONSTANT(x)     x##ll
    #define Z_U64BIT_CONSTANT(x)    x##ull
#endif


#ifndef Z_DEFINE_INT64
#define Z_DEFINE_INT64
    #if defined(_WIN32)
        typedef __int64 TInt64;
        typedef unsigned __int64 TUInt64;
    #else
        typedef long long TInt64;
        typedef unsigned long long TUInt64;
    #endif
#endif

typedef char TInt8;
typedef unsigned char TUInt8;

typedef short TInt16;
typedef unsigned short TUInt16;

typedef int TInt32;
typedef unsigned int TUInt32;

#endif