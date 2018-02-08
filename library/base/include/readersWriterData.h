/********************************************
 **  模块名称：readersWriterData.h
 **  模块功能: 双buff类，多读一写数据的封装
 **  模块简述: 用于数据量比较小的字典数据、配置数据等
 **  编 写 人: lzw978
 **  日    期: 2018.02.06
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
 *********************************************/

#ifndef __READERS_WRITER_DATA_H__
#define __READERS_WRITER_DATA_H__

namespace commbase
{

template<typename T>
class TReadersWriterData
{
public:
    TReadersWriterData() : reader_using_inst0(true)
    {
    }
    virtual ~TReadersWriterData()
    {
    }

    // 获取读取的数据
    virtual T& getReaderData()
    {
        if (reader_using_inst0)
        {
            return inst0;
        }
        else
        {
            return inst1;
        }
    }

    // 获取可写的数据
    virtual T& getWriterData()
    {
        if (reader_using_inst0)
        {
            return inst1;
        }
        else
        {
            return inst0;
        }
    }

    // 写完数据后，将读写数据交换
    virtual void swap()
    {
        reader_using_inst0 = !reader_using_inst0;
    }

private:
    T inst0;
    T inst1;
    bool reader_using_inst0;
};

}

#endif
