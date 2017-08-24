/*
 * =====================================================================================
 *
 *       Filename:   tsmmWriteDemo.cpp
 *
 *    Description:   内部消息队列Demo程序 发送消息
 *
 *        Version:  1.0
 *        Created:  08/24/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lzw978
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o ReadDemo -I../include -L../ -lCommSmm tsmmReadDemo.cpp
 * =====================================================================================
 */

#include "tsmm.h"

int main(int argc, char* argv[])
{
    CCommSMM oCommSmm;

    string strSmmMsgFile = "LZWTEST";
    string strSmmPath    = "."; // 设置为当前目录
    string strMsg = "";
    int iSmmMsgLen = 1024;
    int iSmmMsgNum = 1;

    // 初始化写共享内存
    int iRet = oCommSmm.InitSMM(strSmmMsgFile.c_str(), strSmmPath.c_str(), iSmmMsgLen, iSmmMsgNum);
    if( iRet < 0)
    {
        printf("SMM init error[%d][%s]\n", iRet, oCommSmm.GetErrorDesc());
        return -1;
    }

    // 读取消息
    while(1)
    {
        string strRet = "";
        iRet = oCommSmm.GetMsg(strRet);
        if (iRet > 0)
        {
            printf("read msg=[%s] length=[%d]\n", strRet.c_str(), strRet.length());
        }
        else if( iRet == 0)
        {
            printf("recv no msg\n");
            sleep(2);
        }
        else
        {
            printf("SMM get Msg error[%d][%s]\n", iRet, oCommSmm.GetErrorDesc());
            return -1;
        }
    }

    return 0;
}