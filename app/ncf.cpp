/*************************************************************************
    * File Name: ncf_setproctitle.cpp
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h> 
#include <errno.h>

#include "ncf_macro.h"
#include "ncf_tools.h"
#include "ncf_conf.h"
#include "ncf_socket.h"
#include "ncf_memory.h"

static void freeresource();
//进程名称相关字段
size_t  g_argvneedmem=0;        //保存下这些argv参数所需要的内存大小
size_t  g_envneedmem=0;         //环境变量所占内存大小
int     g_os_argc;              //参数个数 
char    **g_os_argv;            //原始命令行参数数组
char    *gp_envmem=NULL;        //指向自己分配的env环境变量的内存
int     g_daemonized=0;         //守护进程标记，标记是否启用了守护进程模式，0：未启用，1：启用了

//socket相关
CSocekt g_socket;               //socket全局对象

//和进程本身有关的全局量
pid_t   ncf_pid;                //当前进程的pid
pid_t   ncf_parent;             //父进程的pid
int     ncf_process;            //进程类型，比如master,worker进程等
sig_atomic_t  ncf_reap;//标记子进程状态变化
int main(int argc, char *const *argv)
{       
    int exitcode = 0;           
    int i;                    
    CMemory *p_memory;
  
    ncf_pid    = getpid();      //进程pid
    ncf_parent = getppid();     //父进程的id 
    //统计argv所占的内存
    g_argvneedmem = 0;
    for(i = 0; i < argc; i++) 
    {
        g_argvneedmem += strlen(argv[i]) + 1;
    } 
    //统计环境变量所占的内存
    for(i = 0; environ[i]; i++) 
    {
        g_envneedmem += strlen(environ[i]) + 1; 
    }

    g_os_argc = argc;           
    g_os_argv = (char **) argv; 

    ncf_log.fd = -1; 
    ncf_process = NCF_PROCESS_MASTER; 
    ncf_reap = 0;
   
    //初始化配置文件
    CConfig *p_config = CConfig::GetInstance(); //单例类
    if(p_config->Load("nginx.conf") == false) //把配置文件内容载入到内存            
    {   
        ncf_log_init();    //初始化日志
        ncf_log_stderr(0,"配置文件[%s]载入失败，退出!","nginx.conf");
        exitcode = 2; //标记找不到文件
        goto lblexit;
    }
    
    CMemory::GetInstance();	

    ncf_log_init();             //日志初始化(创建/打开日志文件)，这个需要配置项，所以必须放配置文件载入的后边；      
    if(ncf_init_signals() != 0) //信号初始化
    {
        exitcode = 1;
        goto lblexit;
    }        
    if(g_socket.Initialize() == false)//初始化socket
    {
        exitcode = 1;
        goto lblexit;
    }

    ncf_init_setproctitle();    //把环境变量搬家

    //创建守护进程
    if(p_config->GetIntDefault("Daemon",0) == 1) //读配置文件，拿到配置文件中是否按守护进程方式启动的选项
    {
        //1：按守护进程方式运行
        int cdaemonresult = ncf_daemon();
        if(cdaemonresult == -1) //fork()失败
        {
            exitcode = 1;    //标记失败
            goto lblexit;
        }
        if(cdaemonresult == 1)
        {
            //这是原始的父进程
            freeresource();   
            exitcode = 0;
            return exitcode;  
        }
        g_daemonized = 1;//守护进程标志位
    }

    ncf_master_process_cycle(); //不管父进程还是子进程，正常工作期间都在这个函数里循环；

lblexit:
    //该释放的资源要释放掉
    ncf_log_stderr(0,"程序退出，再见了!");
    freeresource();   
    return exitcode;
}

//专门在程序执行末尾释放资源的函数
void freeresource()
{
    if(gp_envmem)
    {
        delete []gp_envmem;
        gp_envmem = NULL;
    }

    //关闭日志文件
    if(ncf_log.fd != STDERR_FILENO && ncf_log.fd != -1)  
    {        
        close(ncf_log.fd); 
        ncf_log.fd = -1;   
    }
}
