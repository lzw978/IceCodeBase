/********************************************
 **  模块名称：file.cpp
 **  模块功能:
 **  模块简述: 目录/文件通用操作封装
 **  编 写 人:
 **  日    期: 2017.08.21
 **  修 改 人:
 **  修改日期:
 **  修改目的：
 **  特别说明:
 **  问    题：
*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "file.h"
#include "strHelper.h"
#include "exception.h"

using namespace std;
using namespace commbase;

// 文件信息
FileInfo::FileInfo()
{
    m_strDirSepChar = "/";
}
FileInfo::FileInfo(const string& strFullPath)
{
    try
    {
        // 替换文件路径中的'\'为'/'
        string tmpFullPath = strFullPath;
        if ( tmpFullPath.find('/') != string::npos && tmpFullPath.find('\\') != string::npos)
        {
            tmpFullPath = StrHelper::replace(tmpFullPath, "\\", "/");
        }
        // 获取文件信息
        getFileInfo(tmpFullPath);
    }
    catch(...)
    {
    }
}
FileInfo::~FileInfo()
{
}

// 获取文件信息
void FileInfo::getFileInfo(string strFullPath) throw(Exception)
{
    if (strFullPath.length() <= 0)
        return ;
    // 删除路径最后一个'/'或'\'
    if (strFullPath.at(strFullPath.length()-1) == '\\' || strFullPath.at(strFullPath.length()-1) == '/')
        strFullPath.erase(strFullPath.length()-1, 1);

    // 查询最后一个路径分隔符
    string::size_type namepos = strFullPath.find_last_of("\\");
    if (namepos == string::npos)
    {
        namepos = strFullPath.find_last_of("/");
        m_strDirSepChar = "/";
    }
    else
    {
        m_strDirSepChar = "\\";
    }
    // 判断查询结果
    if ( namepos == string::npos)
    {
        m_strPath = "";
        m_strName = strFullPath;
    }
    else
    {
        m_strPath = strFullPath.substr(0, namepos); // 路径等于开始到分隔符
        m_strName = strFullPath.substr(namepos+1);  // 文件名等于分隔符之后到结尾
    }
    // 使用stat()获取文件信息
    struct stat stbuf;
    if ( 0 != stat(strFullPath.c_str(), &stbuf))
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "stat() file fail");
    }
    m_lSize = stbuf.st_size;
    // 获取文件创建/更新/访问时间
    struct tm *pTime = localtime(&stbuf.st_atime);
    m_atime = Date(pTime);

    pTime = localtime(&stbuf.st_ctime);
    m_ctime = Date(pTime);

    pTime = localtime(&stbuf.st_mtime);
    m_mtime = Date(pTime);

    // 获取文件类型
    m_eType = eFileTypeUnknown;
    if (S_ISDIR(stbuf.st_mode))
    {
        m_eType = eFileTypeDirectory;
        if (S_ISLNK(stbuf.st_mode))
        {
            m_eType = (eFileType) (m_eType | eFileTypeLink);
        }
    }
    else
    {
        if (S_ISREG(stbuf.st_mode))
            m_eType = eFileTypeFile;
        else if (S_ISCHR(stbuf.st_mode))
            m_eType = eFileTypeCharDevice;
        else if (S_ISBLK(stbuf.st_mode))
            m_eType = eFileTypeBlockDevice;
        else if (S_ISFIFO(stbuf.st_mode))
            m_eType = eFileTypeFifo;
        else if (S_ISSOCK(stbuf.st_mode))
            m_eType = eFileTypeSock;

        if (S_ISLNK(stbuf.st_mode))
            m_eType = (eFileType) (m_eType | eFileTypeLink);
    }
}
// 纯名字
const string& FileInfo::name() const
{
    return m_strName;
}
// 纯名字(无扩展名)
const string FileInfo::pureName()
{
    string::size_type pos = m_strName.find_last_of(".");
    if (pos != string::npos)
        return m_strName.substr(0, pos);
    return m_strName;
}
// 设置名称
void FileInfo::setName(string strName)
{
    m_strName = strName;
}
// 返回扩展名(不含.号)
string FileInfo::suffix()
{
    return getSuffix(m_strName);
}
// 返回扩展名(不含.号)
string FileInfo::getSuffix(string strPath)
{
    string suffix = "";
    string::size_type pos = strPath.find_last_of(".");
    if (pos != string::npos)
        suffix = strPath.substr(pos+1);
    return suffix;
}
// 修改扩展名
string FileInfo::changeSuffix(string strPath, string newSuffix)
{
    string oldSuffix = "";
    string::size_type pos = strPath.find_last_of(".");
    if (pos != string::npos)
        strPath = strPath.substr(0, pos+1) + newSuffix;
    else
        strPath = strPath + "." + newSuffix;
    return strPath;
}
// 修改扩展名
FileInfo& FileInfo::changeSuffix(string newSuffix)
{
    m_strName = changeSuffix(m_strName, newSuffix);
    return *this;
}
// 修改路径中分隔符
string FileInfo::setDirSepStyle(string strOrigPath, eDirSepStyle toStyle)
{
    // 修改为UNIX风格,替换‘\'和'//' 为'/'
    if (toStyle == eDirSepStyleUNIX)
    {
        string::size_type pos = strOrigPath.find_last_of("\\");
        if (pos != string::npos)
            strOrigPath = StrHelper::replace(strOrigPath, "\\", "/");

        pos = strOrigPath.find_last_of("//");
        if (pos != string::npos)
            strOrigPath = StrHelper::replace(strOrigPath, "//", "/");
    }
    // 修改为Windows风格,替换'/'和'\\'为'\'
    else if (toStyle == eDirSepStyleWIN)
    {
        string::size_type pos = strOrigPath.find_last_of("/");
        if (pos != string::npos)
            strOrigPath = StrHelper::replace(strOrigPath, "/", "\\");

        pos = strOrigPath.find_last_of("\\\\");
        if (pos != string::npos)
            strOrigPath = StrHelper::replace(strOrigPath, "\\\\", "\\");
    }
    return strOrigPath;
}
// 不含名字的路径
const string FileInfo::path(eDirSepStyle toStyle) const
{
    return setDirSepStyle(m_strPath, toStyle);
}
// 设置路径
void FileInfo::setPath(string strPath)
{
    m_strPath = strPath;
}
// 返回含文件名的完整路径
string FileInfo::getFullPath(eDirSepStyle style) const
{
    string sep = m_strDirSepChar;
    if (style = eDirSepStyleUNIX)
        sep = "\\";
    else if (style == eDirSepStyleUNIX)
        sep = "/";

    if( m_strPath == "/" || m_strPath == "\\" || m_strPath == "")
        return m_strPath + m_strName;
    else
        return m_strPath + "/" + m_strName;
}
// 文件大小
TInt64 FileInfo::size64() const
{
    return m_lSize;
}
// 文件大小
off_t FileInfo::size() const
{
    return (off_t)m_lSize;
}
// 设置文件大小
void FileInfo::setSize(TInt64 lSize)
{
    m_lSize = lSize;
}
// 文件类型
eFileType FileInfo::type() const
{
    return m_eType;
}
// 设置文件类型
void FileInfo::setType(eFileType eType)
{
    m_eType = eType;
}
// 创建时间
const Datetime& FileInfo::ctime() const
{
    return m_ctime;
}
// 设置创建时间
void FileInfo::setCtime(Datetime &cTime)
{
    m_ctime = cTime;
}
// 修改时间
const Datetime& FileInfo::mtime() const
{
    return m_mtime;
}
// 设置修改时间
void FileInfo::setMtime(Datetime &mTime)
{
    m_mtime = mTime;
}
// 访问时间
const Datetime& FileInfo::atime() const
{
    return m_atime;
}
// 设置访问时间
void FileInfo::setAtime(Datetime &aTime)
{
    m_atime = aTime;
}

// 文件类型判断
bool FileInfo::isVirtual() const
{
    return (m_eType & eFileTypeUnknown) == eFileTypeUnknown;
}
bool FileInfo::isFile() const
{
    return (m_eType & eFileTypeFile) == eFileTypeFile;
}
bool FileInfo::isDirectory() const
{
    return (m_eType & eFileTypeDirectory) == eFileTypeDirectory;
}
bool FileInfo::isCharDevice() const
{
    return (m_eType & eFileTypeCharDevice) == eFileTypeCharDevice;
}
bool FileInfo::isBlockDevice() const
{
    return (m_eType & eFileTypeBlockDevice) == eFileTypeBlockDevice;
}
bool FileInfo::isLink() const
{
    return (m_eType & eFileTypeLink) == eFileTypeLink;
}
bool FileInfo::isFifo() const
{
    return (m_eType & eFileTypeFifo) == eFileTypeFifo;
}
bool FileInfo::isSock() const
{
    return (m_eType & eFileTypeSock) == eFileTypeSock;
}
bool FileInfo::isPipe() const
{
    return (m_eType & eFileTypePipe) == eFileTypePipe;
}
// 重置
void FileInfo::reset()
{
    m_lSize   = 0;
    m_strPath = "";
    m_strName = "";
    m_eType   = eFileTypeUnknown;
    m_strDirSepChar = "/";
}


// 文件操作
File::File() : Stream(), m_fp(NULL)
{
}
File::File(const string &strPath, const string &strMode) throw(Exception) : m_fp(NULL)
{
    open(strPath, strMode);
}
File::~File()
{
    try
    {
        close();
    }
    catch(...)
    {}
}
// 打开文件
void File::open(const string &strPath, const string &strMode) throw(Exception)
{
    m_strPath = strPath;
    m_strOpenMode = strMode;

    close();

    m_fp = fopen(m_strPath.c_str(), m_strOpenMode.c_str());
    if (m_fp == NULL)
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "open file error!");
    }
}
// 关闭文件
void File::close()
{
    if (m_fp != NULL)
    {
        fclose(m_fp);
        m_fp = NULL;
    }
}
// 写数据
size_t File::write(const char* buffer, size_t nCount)
{
    if( !isOpened())
        throw Exception(Z_SOURCEINFO, -1, "file not opened");

    size_t nwritten = 0;
    size_t nleft = nCount;

    while (nleft > 0)
    {
        if ((nwritten = fwrite(buffer, 1, nleft, m_fp)) <= 0)
        {
            if ( EINTR == errno) // 阻塞
            {
                continue;
            }
            else
            {
                int r = errno;
                throw Exception(Z_SOURCEINFO, r, "write file error");
            }
        }
        nleft  -= nwritten;
        buffer += nwritten;
    }
    return nCount - nleft;
}
// 读数据
size_t File::read(char* buffer, size_t nCount)
{
    if( !isOpened())
        throw Exception(Z_SOURCEINFO, -1, "file not opened");

    size_t nread = 0;
    size_t nleft = nCount;

    while (nleft > 0)
    {
        if ( (nread = fread(buffer, 1, nleft, m_fp)) < 0)
        {
            if ( EINTR == errno) // 阻塞
            {
                continue;
            }
            else
            {
                int r = errno;
                throw Exception(Z_SOURCEINFO, r, "read file error");
            }
        }
        else if (nread == 0)
        {
            break;
        }
        nleft  -= nread;
        buffer += nread;
    }
    return nCount - nleft;
}
// 移动读写指针
bool File::seek(off_t offset, int iMode)
{
    if( !isOpened())
        throw Exception(Z_SOURCEINFO, -1, "file not opened");
    return fseek(m_fp, offset, iMode) == 0;
}
// 文件是否打开
bool File::isOpened()
{
    return m_fp != NULL;
}
// 改变文件大小
void File::truncate(TInt64 offset)
{
    int ret = ::ftruncate(fileno(m_fp), offset);
    int r = errno;
    if (ret == 0)
    {
        seek(offset, SEEK_SET);
        return ;
    }
    throw Exception(Z_SOURCEINFO, r, "ftruncate file error");
}
// 文件大小
TInt64 File::size()
{
    struct stat stb;
    int ret = fstat(fileno(m_fp), &stb);
    int r = errno;
    if (ret == 0)
        return stb.st_size;
    throw Exception(Z_SOURCEINFO, r, "get file size error");
}
// 重新打开
void File::reopen() throw(Exception)
{
    close();
    open(m_strPath, m_strOpenMode);
}
// 路径
const string& File::path() const
{
    return m_strPath;
}
// 删除
void File::unlink() throw(Exception)
{
    close();
    File::unlink(m_strPath);
}
// 静态方法
// 删除文件
void File::unlink(const string &strPath) throw(Exception)
{
    if (::unlink(strPath.c_str()) == -1)
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "unlink file error");
    }
}
// 是否存在
bool File::exists(const string &strPath)
{
    return ::access(strPath.c_str(), F_OK) ? false : true;
}
// 移动文件
bool File::move(const string &strPathFrom, const string &strPathTo)
{
    return 0 == rename(strPathFrom.c_str(), strPathTo.c_str());
}
// 是否是文件
bool File::isFile(const string &strFullPath)
{
    bool ret = false;
    try
    {
        FileInfo v;
        v.getFileInfo(strFullPath);
        ret = v.isFile();
    }
    catch(...)
    {
    }

    return ret;
}
// 获取文件信息
FileInfo File::getFileInfo(const string &strPath) throw(Exception)
{
    FileInfo info;
    info.getFileInfo(strPath);
    return info;
}
// 创建临时文件供读写
void File::mktemp(const string &strPerfix, File& file) throw(Exception)
{
    int len = strPerfix.length();
    char* tempname = new char[len+10];
    memset(tempname, 0, len+10);

    strcpy(tempname, strPerfix.c_str());
    strcpy(tempname+len, "XXXXXX");

    int handle = ::mkstemp(tempname);
    int r = errno;
    string path = tempname;
    delete[] tempname;

    if (handle == -1)
        throw Exception(Z_SOURCEINFO, r, "make temp file error");

    file.open(path, "wb");
}

// 目录操作
struct Directory::dir_handle_t
{
    DIR* handle;
    dirent* current;
    bool first;
    bool eof;

    dir_handle_t()
    {
        handle  = NULL;
        current = NULL;
        first = false;
        eof   = false;
    }
};

Directory::Directory(string strDirPath) : m_DirInfo(NULL), m_Handle(NULL), m_Separator("\\")
{
    DIR* handle = opendir(strDirPath.c_str());
    int r = errno;
    if (!handle)
        throw Exception(Z_SOURCEINFO, r, "opendir() fail");

    m_Handle = new dir_handle_t;
    m_Handle->handle  = handle;
    m_Handle->current = readdir(m_Handle->handle);
    if (m_Handle->current)
    {
        m_Handle->first = true;
        m_Handle->eof   = false;
    }
    else
    {
        m_Handle->first = false;
        m_Handle->eof   = true;
    }
    m_DirInfo = new FileInfo(strDirPath);
}
Directory::~Directory()
{
    if (m_Handle != NULL)
    {
        closedir(m_Handle->handle);
        delete m_Handle;
        m_Handle = NULL;
    }
    if ( m_DirInfo != NULL)
    {
        delete m_DirInfo;
        m_DirInfo = NULL;
    }
}
// 重设dir目录流目前读取位置为原来开头的读取位置
void Directory::rewind() throw(Exception)
{
    rewinddir(m_Handle->handle);
    m_Handle->current = readdir(m_Handle->handle);
    if (m_Handle->current)
    {
        m_Handle->first = true;
        m_Handle->eof   = false;
    }
    else
    {
        m_Handle->first = false;
        m_Handle->eof   = true;
    }
}

// 获取当前目录文件
bool Directory::getCurFile(FileInfo &fileInfo) throw(Exception)
{
    bool r = false;
    fileInfo.reset();
    if ( !m_Handle->eof && m_Handle->current != NULL)
    {
        string fullpath = m_DirInfo->getFullPath() + "/" + m_Handle->current->d_name;
        try
        {
            fileInfo.getFileInfo(fullpath);
            r = true;
        }
        catch(...)
        {
        }
    }
    return r;
}
// 获取下一文件
bool Directory::getNextFile(FileInfo &fileInfo) throw(Exception)
{
    fileInfo.reset();

    if (m_Handle->first)
    {
        m_Handle->first = false;
        return getCurFile(fileInfo);
    }

    int r = errno;
    m_Handle->current = readdir(m_Handle->handle);
    if (!m_Handle->current)
    {
        int r1 = errno;
        if ( r1 == r)
            m_Handle->eof = true;
        else
            throw Exception(Z_SOURCEINFO, r1, "can't read file info");
    }
    return getCurFile(fileInfo);
}
// 设置分隔符
void Directory::setSeparator(char separator)
{
    if (separator != '\\' && separator != '/')
        separator = '/';
    m_Separator = separator;
}
// 返回分隔符
string Directory::separator(eDirSepStyle style)
{
    return m_Separator;
}

// 静态方法
// 获取目录下第一层文件(files:文件存储列表 bIncludeDot:结果是否包含. ..)
bool Directory::getSubFiles(string strDirPath, vector<FileInfo> &files, bool bIncludeDot) throw(Exception)
{
    if (strDirPath == "")
        return false;

    try
    {
        Directory dir(strDirPath);
        FileInfo file;
        for( ;dir.getNextFile(file); )
        {
            if (!bIncludeDot && (file.name()=="." || file.name()==".."))
                continue;

            files.push_back(file);
        }
    }
    catch(...)
    {
        return false;
    }
    return true;
}
// 获取目录下所有文件(含子目录中)
bool Directory::getAllSubFiles(string strDirPath, vector<FileInfo> &files, bool bIncludeDot)
{
    stack<string> dirsContainer;
    dirsContainer.push(strDirPath);

    // 检测当前目录是否存在或者可读
    try
    {
        Directory dir(strDirPath);
    }
    catch(...)
    {
        return false;
    }

    for (; dirsContainer.size()>0; )
    {
        string path = dirsContainer.top();
        dirsContainer.pop();

        try
        {
            Directory dir(path);

            FileInfo file;
            for (;dir.getNextFile(file); )
            {
                if (!bIncludeDot && (file.name() == "." || file.name() == ".."))
                    continue;

                if (file.isDirectory() && (file.name() != "." && file.name() != ".."))
                {
                    dirsContainer.push(file.getFullPath());
                    files.push_back(file);
                }
                else
                {
                    files.push_back(file);
                }
            }
        }
        catch(...)
        {
            return false;
        }
    }
    return true;
}
// 创建目录/路径
bool Directory::mkdir(string strFullPath)
{
    strFullPath = StrHelper::replace(strFullPath, "\\", "/");
    strFullPath = StrHelper::replaceLoop(strFullPath, "//", "/");

    vector<string> dirs;
    dirs = StrHelper::split(strFullPath, '/');

    vector<string> paths;
    paths.push_back(strFullPath);

    for (size_t i=dirs.size()-1; i>0; i--)
    {
        string tmpPath;
        for ( size_t j=0; j<i; j++)
        {
            tmpPath += dirs[j];
            if (j < (i-1))
                tmpPath += '/';
        }
        if (tmpPath != "")
            paths.push_back(tmpPath);
    }

    for (int i=paths.size()-1; i>=0; i--)
    {
        int err = 0;
        if (!isDirectory(paths[i]))
        {
            err = ::mkdir(paths[i].c_str(), 0766);
        }

        if (!isDirectory(paths[i]))
            return false;
    }
    return true;
}
// 删除目录/路径
int Directory::rmdir(string strFullPath, bool bFailWhenNoEmpty)
{
    strFullPath = StrHelper::replace(strFullPath, "\\", "/");
    strFullPath = StrHelper::replaceLoop(strFullPath, "//", "/");

    vector<FileInfo> files;
    Directory::getSubFiles(strFullPath, files, false);

    vector<FileInfo>::iterator it = files.begin();
    for (; it!=files.end(); ++it)
    {
        if (it->isDirectory())
        {
            rmdir(it->getFullPath().c_str(), bFailWhenNoEmpty);
        }
        else
        {
            chmod(it->getFullPath().c_str(), 0777);
            int r = unlink(it->getFullPath().c_str());
            int iErrno = errno;
            if (r!=0 || File::isFile(it->getFullPath()))
            {
                throw Exception(Z_SOURCEINFO, iErrno, "delete file failed!");
            }
        }
    }

    int r = ::rmdir(strFullPath.c_str());
    int iErrno = errno;
    if (r!=0 || Directory::isDirectory(strFullPath))
    {
        throw Exception(Z_SOURCEINFO, iErrno, "delete file failed!");
    }
    return r;
}
// 判断是否为目录
bool Directory::isDirectory(string strFullPath)
{
    bool ret = false;
    try
    {
        FileInfo fileInfo;
        fileInfo.getFileInfo(strFullPath);
        ret = fileInfo.isDirectory();
    }
    catch(...)
    {
    }
    return ret;
}