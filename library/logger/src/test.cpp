/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  mytest
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "../include/logger.h"


int main(void)
{
    char test[]  = "my test is ok ";
    int code = 1024;

    SetLogMaxSize(1000000);
    SetLogLevel(4);
    SetLogPath(".");
    SetErrLogFile("./ERRORLOG");
    SetLogFileName("MYTEST");
    
    Trace(L_ERROR, NULL,  "%s|%d", test, code); 

    return 0;
}

