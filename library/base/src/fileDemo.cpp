/*
* =====================================================================================
 *
 *       Filename:  fileDemo.cpp
 *
 *    Description:  test file lib
 *
 *        Version:  1.0
 *        Created:  08/28/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *    g++ -g -o fileDemo -I../include -L../ -lCommBase -lrt fileDemo.cpp
 * =====================================================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include "file.h"

using namespace std;
using namespace commbase;
int main(int argc, char* argv[])
{
    string strOut = "";
    // 全路径的文件名
    string strFileName = "/home/user/IceCode/library/base/src/logDemo.cpp";
    // 目录名
    string strDirName = "/home/user/IceCode/library/base/src";
    // 文件测试

    // 获取文件信息
    FileInfo cFileInfo(strFileName);
    // 文件名
    strOut = cFileInfo.name();
    cout << "full name = [" << strOut  << "]" << endl;
    // 仅文件名
    strOut = cFileInfo.pureName();
    cout << "only name = [" << strOut  << "]" << endl;
    // 扩展名
    strOut = cFileInfo.suffix();
    cout << "only suffix = [" << strOut  << "]" << endl;
    // 返回路径格式
    strOut = cFileInfo.setDirSepStyle(strFileName, eDirSepStyleWIN);
    cout << "path style = [" << strOut  << "]" << endl;

    // 目录测试
    Directory cDirectory(strDirName);
    bool bTest = false;
    bTest = Directory::isDirectory(strDirName);
    cout << "Is dir? bTest=[" << bTest << "]" << endl;

    // 查询目录下所有文件(包含.和..)
    cout << "find sub file, only this dir" << endl;
    vector<FileInfo> vDirs;
    bTest = Directory::getSubFiles(strDirName, vDirs, true);
    vector<FileInfo>::iterator it = vDirs.begin();
    for(; it!=vDirs.end(); ++it)
    {
        cout << it->name()  << endl;
    }

    // 查询目录和子目录下所有文件(不包括.和..)
    cout << "find sub file, include sub dir" << endl;
    strDirName = "/home/user/IceCode/library/base";
    vector<FileInfo> vSubDirs;
    bTest = Directory::getAllSubFiles(strDirName, vSubDirs, false);
    it = vSubDirs.begin();
    for(; it!=vSubDirs.end(); ++it)
    {
        cout << it->name()  << endl;
    }

    // 创建目录
    string strMkDir = "/tmp/testmkdir";
    bTest = Directory::mkdir(strMkDir);
    if( bTest)
    {
        cout << "make dir success" << endl;
    }

    // 删除目录(删除上一步创建的目录)
    /*
    int iRet = Directory::rmdir(strMkDir, true);
    if( iRet == 0)
    {
        cout << "delete dir success" << endl;
    }
    */

    // 文件操作
    string strFilePath = "/tmp/testmkdir/mytest.txt";
    const char pMsg[24] = "LZW978 test write file\n";
    char readMsg[24+1] = {0};
    int iMsgLen = strlen(pMsg);
    File cFile(strFilePath, "a+");
    // 写入信息
    int iLen = cFile.write(pMsg, iMsgLen);
    cout << "write file len=[" << iLen << "]" << endl;
    // 重置文件指针
    cFile.seek(0, 0);
    // 读取文件
    cFile.read(readMsg, iMsgLen);
    printf("read file msg=[%s]\n", readMsg);
    cFile.close();


    return 0;
}

