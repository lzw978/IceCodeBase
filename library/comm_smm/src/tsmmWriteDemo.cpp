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
 * g++ -g -o WriteDemo -I../include -L../ -lCommSmm tsmmWriteDemo.cpp
 * =====================================================================================
 */

#include "tsmm.h"

int main(int argc, char* argv[])
{
    CCommSMM oCommSmm;

    string strSmmMsgFile = "LZWTEST";
    string strSmmPath    = "./";
    int iSmmMsgLen = 1024;
    int iSmmMsgNum = 1;

    string strMsg = "LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11"
    "LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11"
    "LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11"
    "LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11"
    "LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11LargeMsgToTestQueueIsOk11";

    // 初始化写共享内存
    int iRet = oCommSmm.InitSMM(strSmmMsgFile.c_str(), strSmmPath.c_str(), iSmmMsgLen, iSmmMsgNum);
    if( iRet < 0)
    {
        printf("SMM init error[%d][%s]\n", iRet, oCommSmm.GetErrorDesc());
        return -1;
    }

    // 发送消息
    iRet = oCommSmm.PutMsg(strMsg.c_str(), strMsg.length());
    if ( iRet < 0)
    {
        printf("SMM send Msg error[%d][%s]\n", iRet, oCommSmm.GetErrorDesc());
        return -1;
    }
    else
    {
        printf("send msg=[%s] length=[%d]\n", strMsg.c_str(), strMsg.length());
    }

    return 0;
}