/********************************************
 **  模块名称：file.h
 **  模块功能:
 **  模块简述: 文件通用操作封装
 **  编 写 人:
 **  日    期: 2017.08.21
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#ifndef __LIB_FILE_H__
#define __LIB_FILE_H__

#include <string>
#include <vector>

#include "datetime.h"
#include "stream.h"

#ifndef _WIN32
#include <dirent.h>
#endif

#ifdef _WIN32
    typedef __int64 TInt64;
    typedef unsigned __int64 TUInt64;
#else
    typedef long long TInt64;
    typedef unsigned long long TUInt64;
#endif

namespace commbase
{
    using namespace std;

    // 文件类型
    enum eFileType
    {
        eFileTypeUnknown     =  0, // 未知/虚拟
        eFileTypeFile        =  1, // 普通文件
        eFileTypeDirectory   =  2, // 目录
        eFileTypeCharDevice  =  4, // 字符设备
        eFileTypeBlockDevice =  8, // 块设备
        eFileTypeLink        = 16, // 软连接
        eFileTypePipe        = 32  // 命名/匿名管道
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
        static string changeSuffix(string strPath, string strSuffix);
        // 修改路径中分隔符
        static string setDirSepStyle(string strOrigPath, eDirSepStyle toStyle);
        // 修改扩展名
        FileInfo& changeSuffix(string strSuffix);
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
        bool isPipe() const;
        // 重置
        void reset();
    private:
        string strPath;       // 路径
        string strName;       // 文件名
        string strDirSepChar; // 分隔符
        TUInt64 lSize;        // 文件大小
        eFileType eType;      // 文件类型
        Datetime  ctime;      // 创建时间
        Datetime  mtime;      // 修改时间
        Datetime  atime;      // 访问时间
        void setValues(const FileInfo &other);
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
        virtual size_t write(const char* buffer, size_t count);
        // 读数据
        virtual size_t read(char* buffer, size_t count);
        // 移动读写指针
        virtual bool seek(off_t offset, int iMode);
        // 文件是否打开
        bool isOpened();
        // 截断/扩展文件
        void truncate(TInt64 lSize);
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
        static bool isFile(const string &strPath);
        // 获取文件信息
        static FileInfo getFileInfo(const string &strPath) throw(Exception);
        // 创建临时文件供读写
        static void mktemp(const string &strPerfix, File& file) throw(Exception);
    private:
        FILE* fp;
        string strPath;
        string strOpenMode;
    };

    // 目录操作
    class Directory
    {
    public:
        Directory(string strDirPath);
        virtual ~Directory();

        void rewind() throw(Exception);
        bool getCurFile(FileInfo &fileInfo) throw(Exception);
        bool getNextFile(FileInfo &fileInfo) throw(Exception);
        void setSeparator(char separator);
        string separator(eDirSepStyle style=eDirSepStyleWIN);
        FileInfo getDirInfo();

        // 静态方法
        // 获取目录下第一层文件(files:文件存储列表 bIncludeDot:结果是否包含. ..)
        static bool getSubFile(string strDirPath, vector<FileInfo> &files, bool bIncludeDot=false) throw(Exception);
        // 获取目录下所有文件(含子目录中)
        static bool getAllSubFiles(string strDirPath, vector<FileInfo> &files, bool bIncludeDot=false);
        // 创建目录/路径
        static int mkdir(string strFullPath, bool bCreateFullPath=true);
        // 删除目录/路径
        static int rmdir(string strFullPath, bool bFailWhenNoEmpty=true);
        // 判断是否为目录
        static bool isDirectory(string strFullPath);
    private:
        FileInfo *m_DirInfo;
        struct dir_handle_t;
        dir_handle_t m_Handle;
        string m_Separator;
    };
}

#endif