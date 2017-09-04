/********************************************
 **  模块名称：file.h
 **  模块功能:
 **  模块简述: 目录/文件通用操作封装
 **  编 写 人:
 **  日    期: 2017.08.21
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
打开文件 strOpenMode支持如下:
r   以只读方式打开文件，该文件必须存在。
r+  以读/写方式打开文件，该文件必须存在。
rb+ 以读/写方式打开一个二进制文件，只允许读/写数据。
rt+ 以读/写方式打开一个文本文件，允许读和写。
w   打开只写文件，若文件存在则长度清为 0，即该文件内容消失，若不存在则创建该文件。
w+  打开可读/写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
a   以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留（EOF 符保留）。
a+  以附加方式打开可读/写的文件。若文件不存在，则会建立该文件，如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（原来的 EOF 符不保留）。
wb  以只写方式打开或新建一个二进制文件，只允许写数据。
wb+ 以读/写方式打开或建立一个二进制文件，允许读和写。
wt+ 以读/写方式打开或建立一个文本文件，允许读写。
at+ 以读/写方式打开一个文本文件，允许读或在文本末追加数据。
ab+ 以读/写方式打开一个二进制文件，允许读或在文件末追加数据。
 **  问    题：
*********************************************/

#ifndef __LIB_FILE_H__
#define __LIB_FILE_H__

#include <string>
#include <vector>
#include <stack>

#include "datetime.h"
#include "stream.h"
#include "commdef.h"

#ifndef _WIN32
#include <dirent.h>
#endif

namespace commbase
{
    using namespace std;

    // 文件类型
    enum eFileType
    {
        eFileTypeUnknown     = 0, // 未知/虚拟
        eFileTypeFile        = 1, // 普通文件
        eFileTypeDirectory   = 2, // 目录
        eFileTypeCharDevice  = 3, // 字符设备
        eFileTypeBlockDevice = 4, // 块设备
        eFileTypeLink        = 5, // 软连接
        eFileTypeFifo        = 6, // FIFO文件
        eFileTypeSock        = 7, // SOCKET文件
        eFileTypePipe        = 8  // 命名/匿名管道
    };
    // 目录类型
    enum eDirSepStyle
    {
        eDirSepStyleAuto = 0, // 自动
        eDirSepStyleWIN  = 1, // windows文件
        eDirSepStyleUNIX = 2  // unix文件
    };

    // 文件信息类(文件系统)
    class FileInfo
    {
    public:
        FileInfo();
        FileInfo(const string& strFullPath);
        ~FileInfo();

        // 获取文件信息
        void getFileInfo(string strFullPath) throw(Exception);
        // 纯名字
        const string& name() const;
        // 纯名字(无扩展名)
        const string pureName();
        // 设置名称
        void setName(string strName);
        // 返回扩展名(不含.号)
        string suffix();
        // 返回扩展名(不含.号)
        static string getSuffix(string strPath);
        // 修改扩展名
        static string changeSuffix(string strPath, string newSuffix);
        // 修改路径中分隔符
        static string setDirSepStyle(string strOrigPath, eDirSepStyle toStyle);
        // 修改扩展名
        FileInfo& changeSuffix(string newSuffix);
        // 不含名字的路径
        const string path(eDirSepStyle toStyle=eDirSepStyleAuto) const;
        // 设置路径
        void setPath(string strPath);
        // 返回含文件名的完整路径
        string getFullPath(eDirSepStyle style=eDirSepStyleAuto) const;
        // 文件大小
        TInt64 size64() const;
        // 文件大小
        off_t size() const;
        // 设置文件大小
        void setSize(TInt64 lSize);
        // 文件类型
        eFileType type() const;
        // 设置文件类型
        void setType(eFileType eType);
        // 创建时间
        const Datetime& ctime() const;
        // 设置创建时间
        void setCtime(Datetime &cTime);
        // 修改时间
        const Datetime& mtime() const;
        // 设置修改时间
        void setMtime(Datetime &mTime);
        // 访问时间
        const Datetime& atime() const;
        // 设置访问时间
        void setAtime(Datetime &aTime);

        // 文件类型判断
        bool isVirtual() const;
        bool isFile() const;
        bool isDirectory() const;
        bool isCharDevice() const;
        bool isBlockDevice() const;
        bool isLink() const;
        bool isFifo() const;
        bool isSock() const;
        bool isPipe() const;
        // 重置
        void reset();
    private:
        string    m_strDirSepChar; // 分隔符
        string    m_strPath;  // 路径
        string    m_strName;  // 文件名
        TUInt64   m_lSize;    // 文件大小
        eFileType m_eType;    // 文件类型
        Datetime  m_ctime;    // 创建时间
        Datetime  m_mtime;    // 修改时间
        Datetime  m_atime;    // 访问时间
    };

    // 文件读写类(文件系统)
    class File : public Stream
    {
    public:
        File();
        File(const string &strPath, const string &strMode) throw(Exception);
        ~File();

        // 打开文件
        void open(const string &strPath, const string &strMode) throw(Exception);
        // 关闭文件
        virtual void close();
        // 写数据
        virtual size_t write(const char* buffer, size_t nCount);
        // 读数据
        virtual size_t read(char* buffer, size_t nCount);
        // 移动读写指针
        virtual bool seek(off_t offset, int iMode);
        // 文件是否打开
        bool isOpened();
        // 改变文件大小
        void truncate(TInt64 offset);
        // 文件大小
        TInt64 size();
        // 重新打开
        void reopen() throw(Exception);
        // 路径
        const string& path() const;
        // 删除
        void unlink() throw(Exception);

        // 静态方法
        // 删除文件
        static void unlink(const string &strPath) throw(Exception);
        // 是否存在
        static bool exists(const string &strPath);
        // 移动文件
        static bool move(const string &strPathFrom, const string &strPathTo);
        // 是否是文件
        static bool isFile(const string &strFullPath);
        // 获取文件信息
        static FileInfo getFileInfo(const string &strPath) throw(Exception);
        // 创建临时文件供读写
        static void mktemp(const string &strPerfix, File& file) throw(Exception);
    private:
        FILE*  m_fp;
        string m_strPath;
        string m_strOpenMode;
    };

    // 目录操作
    class Directory
    {
    public:
        Directory(string strDirPath);
        virtual ~Directory();

        // 重设目录流读取位置
        void rewind() throw(Exception);
        // 获取当前文件
        bool getCurFile(FileInfo &fileInfo) throw(Exception);
        // 获取下一文件
        bool getNextFile(FileInfo &fileInfo) throw(Exception);
        // 设置分隔符
        void setSeparator(char separator);
        // 返回分隔符
        string separator(eDirSepStyle style=eDirSepStyleUNIX);

        // 静态方法
        // 获取目录下第一层文件(files:文件存储列表 bIncludeDot:为true结果则包含. ..)
        static bool getSubFiles(string strDirPath, vector<FileInfo> &files, bool bIncludeDot=false) throw(Exception);
        // 获取目录下所有文件(含子目录中文件)
        static bool getAllSubFiles(string strDirPath, vector<FileInfo> &files, bool bIncludeDot=false);
        // 创建目录/路径
        static bool mkdir(string strFullPath);
        // 删除目录/路径
        static int rmdir(string strFullPath, bool bFailWhenNoEmpty=true);
        // 判断是否为目录
        static bool isDirectory(string strFullPath);
    private:
        FileInfo *m_DirInfo;
        struct dir_handle_t;
        dir_handle_t *m_Handle;
        string m_Separator;
    };
}

#endif