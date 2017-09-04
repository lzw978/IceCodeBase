/********************************************
 **  模块名称：sha1.h
 **  模块功能: sha1摘要算法
 **  模块简述:
 **  编 写 人:
 **  日    期: 2017.09.04
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_SHA1_H__
#define __LIB_SHA1_H__

#include <string>
#include "commdef.h"
#include "byteorder.h"
#include "digest.h"

namespace commbase
{
    class SHA1Digest : public Digest
    {
    public:
        enum
        {
            DIGEST_SIZE = 20,
            BLOCK_SIZE  = 64
        };

        SHA1Digest();
        SHA1Digest(const SHA1Digest& dig);
        ~SHA1Digest();

        virtual void update(const char* buff, size_t len);
        virtual std::string digest();
        virtual void clear();

        SHA1Digest& operator=(const SHA1Digest& dig);

    private:
        void init();
        void transform(const unsigned char* buffer);

        TUInt64  mByteCount;
        unsigned int  mState[5];
        unsigned char mBlock[64];
    };
}

#endif