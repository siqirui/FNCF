/*************************************************************************
    * File Name: ncf_setproctitle.cpp
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ncf_global.h"
using namespace std;


/*******************************************
 * 函数功能：为进程相关的系统变量重新分配内存
 * 参数：void
 * return：void
 * 备注：止可在进程开始时调用，不可放入子线程
 * *******************************************/
void ncf_init_setproctitle()
{
    int i = 0;

    for (i = 0; environ[i]; i++)
    {
        g_environlen += strlen(environ[i]) + 1;
    }
    
    gp_envmem = new char[g_environlen];
    memset(gp_envmem,0,g_environlen);;


/*    memcpy(gp_envmem,environ,g_environlen);

    environ = (char **)gp_envmem;

    for ( i = 0; environ[i]; i++)
    {
        printf("%s\n",environ[i]);
    }
    
*/
    char * tmp = gp_envmem;

    for ( i = 0; environ[i]; i++)
    {
        size_t len = strlen(environ[i]);
        strcpy(tmp,environ[i]);
        environ[i] = tmp;
        tmp += len + 1;
    }

    return;    
}
/**********************************************************
 * 函数功能：设置进程名称（格式 程序名： 进程名称 启动方式）
 * 参数：title为要设置的进程名称
 * return：void
 * 备注：止可在进程开始时调用，不可放入子线程
 * *******************************************************/
void ncf_setproctitle(const char * title)
{
    size_t titlelen = strlen(title);
    size_t firstargvlen = strlen(g_os_argv[0]) +1;
    char tmptitle[1024];
    memset(tmptitle,0,1024);
    memcpy(tmptitle,g_os_argv[0]+2, firstargvlen -3 );
    strncat(tmptitle,": ",strlen(" : "));
    strncat(tmptitle,title,titlelen);
    strncat(tmptitle," ",strlen(" "));
    strncat(tmptitle,g_os_argv[0],firstargvlen);
    titlelen = strlen(tmptitle);
    size_t e_argvlen = 0;
    for (int i = 0; g_os_argv[i]; i++)
    {
        e_argvlen += strlen(g_os_argv[i]) +1;
    }

    size_t sumlen = e_argvlen + g_environlen;

    if (sumlen <= titlelen)
    {
        return ;
    }
    
    g_os_argv[1] = nullptr;

    char * tmp = g_os_argv[0] ;//+ firstargvlen;
    strcpy(tmp ,tmptitle);
    tmp += titlelen;

    size_t surpluslen = sumlen - titlelen;

    memset(tmp , 0 ,surpluslen);
    
}
