/*************************************************************************
    * File Name: ncf_global.h
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
    * file function: 声明全局数据、通用数据
 ************************************************************************/

#ifndef __NCF_GLOBAL_H__
#define __NCF_GLOBAL_H__

#include <signal.h>
#inlcude "ncf_socket.h"
typedef struct
{
	char ItemName[50];
	char ItemContent[500];
}SConfItem,*PSConfItem;

typedef struct 
{
    int log_level;
    int fd;
}ncf_log_t;


//外部全局量声明
extern char  **g_os_argv;
extern char  *gp_envmem; 
extern int   g_environlen; 

extern size_t        g_argvneedmem;
extern size_t        g_envneedmem; 
extern int           g_os_argc; 
extern int           g_daemonized;
extern CSocekt       g_socket;  

extern pid_t         ncf_pid;
extern pid_t         ncf_parent;
extern ngx_log_t     ncf_log;
extern int           ncf_process;   
extern sig_atomic_t  ncf_reap; 



#define     CONFITEM_MAXLEN        512

#endif
