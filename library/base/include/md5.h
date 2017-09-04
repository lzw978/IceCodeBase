/********************************************
 **  模块名称：md5.h
 **  模块功能: md5摘要算法
 **  模块简述:
 **  编 写 人:
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_MD5_H__
#define __LIB_MD5_H__

#include <string>
#include "digest.h"

struct MD5Context;

namespace commbase
{
    class MD5Digest : public Digest
    {
    public:
        MD5Digest();
        ~MD5Digest();

        void update(const char* buff, size_t len);
        string digest();
        void clear();
    private:
        MD5Context* mContext;
    };
}

#endif
