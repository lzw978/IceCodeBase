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
#include <stdlib.h>
#include <string>
#include "file.h"

using namespace std;
using namespace commbase;
int main(int argc, char* argv[])
{
    // 带路径的文件名(全路径)
    string strFileName = "/home/linzhenwei/IceCode/library/base/src/logDemo.cpp";
    // 获取文件信息
    FileInfo cFileInfo(strFileName);

    return 0;
}

