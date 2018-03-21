/*
* @file        memBuffer.h
* @brief       ԭʼ�ڴ��򵥷�װ
*/
#ifndef __LIB_MEMBUFFER_H__
#define __LIB_MEMBUFFER_H__

#include <cstring>
#include <string>
#include <malloc.h>

namespace commbase
{
    using namespace std;

    //! �������ڴ������
    class MemBuffer
    {
    public:
        MemBuffer();
        MemBuffer(const MemBuffer&buf);
        MemBuffer& operator=(const MemBuffer&buf);
        ~MemBuffer();

        //! ���ؿ����ڴ���(ĩβ��0)
        char * getBuffer();

        //! ���ؿ����ڴ���
        void * getRawBuffer() const;

        //! �������ݴ�С
        const unsigned int& size() const ;

        //! �ܹ������ڴ��С
        unsigned int totalMemory();

        //! ��֤��len����
        int more(unsigned int len);

        //! �������
        int append(const void *data, unsigned int datlen);

        //! �������
        int append(MemBuffer&buf);

        //! �������
        int append(const std::string&str);

        //! �����ܼ�size�ڴ�
        void reserve(unsigned int size_);

        //! ���ò���ʼ���ڴ�Ϊ'\0'
        void reset();

        //! �ض�������len
        void truncate( unsigned int len );

        //! ��ӡ������
        int format( const char *fmt, ... );

        //! ���浽�ļ�
        int saveToFile(const string filePath, bool bAppend=false) const;

        //! ���ļ�����
        int loadFromFile(const string filePath, unsigned int nOffset=0, unsigned int nCount=0);

        //! ������Ϣ
        void dumpInfo(unsigned int dumpCount=96);

    private:
        void _init(unsigned int preAlloc);

        // ��Ч�ڴ��Ƶ�ͷ��
        void align();

        char *pBuffer_;
        char *pOrigBuffer_;

        unsigned int nMisalign_;
        unsigned int nTotalLength_;
        unsigned int nOff_;
    };

}

#endif
