/*
 * =====================================================================================
 *
 *       Filename:  shareMemDemo1.cpp
 *
 *    Description:  基础类库测试Demo程序
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (lzw978),
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o ReadDemo -I../include -L../ -lCommBase -lrt shareMemReadDemo.cpp
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ShareMemory.h"

typedef struct
{
    int iType;
    char szText[8+1];
} TEST_MSG;


int main(int argc, char** argv)
{
    TEST_MSG *pMapSHM = NULL;

    CShareMemory MyMemory;
    // 生成共享内存
    int iSHMID = MyMemory.Create("TESTFILE", 1024);
    // 获取共享内存地址
    pMapSHM = (TEST_MSG*)MyMemory.GetMemoryAddress();
    if( pMapSHM == NULL)
    {
        printf("Get Address Error!\n");
        return -1;
    }
    printf("handle=[%d] pMapSHM_iType=[%d] szText=[%s]\n", MyMemory.m_hShareMemHandle, pMapSHM->iType, pMapSHM->szText);

    // 定义消息结构体
    TEST_MSG MyMsg;
    memset(&MyMsg, 0, sizeof(TEST_MSG));
    MyMsg.iType = pMapSHM->iType;
    strcpy(MyMsg.szText, pMapSHM->szText);

    printf("DEBUG read: iType=[%d] Text=[%s]\n", MyMsg.iType, MyMsg.szText);


    return 0;
}
