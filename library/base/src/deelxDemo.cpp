/*
 * =====================================================================================
 *
 *       Filename:  deelxDemo.cpp
 *
 *    Description:  正则表达式引擎测试程序
 *
 *        Version:  1.0
 *        Created:  08/09/17 02:11:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  来源于官方网站
 *   Organization:
 *   编译src目录下执行：
 * g++ -g -o deelxDemo -I../include -L../ -lCommBase -lrt deelxDemo.cpp
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "deelx.h"

int test_email(const char * string)
{
    // declare
    static CRegexpT <char> regexp("^([0-9a-zA-Z]([-.\\w]*[0-9a-zA-Z])*@(([0-9a-zA-Z])+([-\\w]*[0-9a-zA-Z])*\\.)+[a-zA-Z]{2,9})$");

    // test
    MatchResult result = regexp.MatchExact(string);

    // matched or not
    return result.IsMatched();
}

int main(int argc, char * argv[])
{
    char * str1 = "bob@smith.com";
    char * str2 = "bob@.com";

    printf("'%s' => %s\n", str1, (test_email(str1) ? "yes" : "no"));
    printf("'%s' => %s\n", str2, (test_email(str2) ? "yes" : "no"));

    return 0;
}


