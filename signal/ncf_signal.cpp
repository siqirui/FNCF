/*************************************************************************
    * File Name: ncf_signal.cpp
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
    * Remark : 信号处理
 ************************************************************************/
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h> 

#include "nfc_global.h"
#include "ncf_macro.h"
#include "ncf_tools.h" 



typedef struct 
{
    int           signo;
    const  char   *signame;
   
    void  (*handler)(int signo, siginfo_t *siginfo, void *ucontext); 
} ncf_signal_t;


static void ncf_signal_handler(int signo, siginfo_t *siginfo, void *ucontext); 
static void ncf_process_get_status(void);

ncf_signal_t  signals[] = {
    // signo      signame             handler
    { SIGHUP,    "SIGHUP",           ncf_signal_handler },        //终端断开信号，对于守护进程常用于reload重载配置文件通知--标识1
    { SIGINT,    "SIGINT",           ncf_signal_handler },        //标识2   
	{ SIGTERM,   "SIGTERM",          ncf_signal_handler },        //标识15
    { SIGCHLD,   "SIGCHLD",          ncf_signal_handler },        //子进程退出时，父进程会收到这个信号--标识17
    { SIGQUIT,   "SIGQUIT",          ncf_signal_handler },        //标识3
    { SIGIO,     "SIGIO",            ncf_signal_handler },        //指示一个异步I/O事件【通用异步I/O信号】
    { SIGSYS,    "SIGSYS, SIG_IGN",  NULL               },        //忽略这个信号，SIGSYS表示收到了一个无效系统调用31
                                                                 
    { 0,         NULL,               NULL               }         
};

int ncf_init_signals()
{
    ncf_signal_t      *sig; 
    struct sigaction   sa;

    for (sig = signals; sig->signo != 0; sig++)
    {        
        memset(&sa,0,sizeof(struct sigaction));

        if (sig->handler)
        {
            sa.sa_sigaction = sig->handler;
            sa.sa_flags = SA_SIGINFO;
        }
        else
        {
            sa.sa_handler = SIG_IGN;                             
        } 

        sigemptyset(&sa.sa_mask)
        if (sigaction(sig->signo, &sa, NULL) == -1)
        {   
            ncf_log_error_core(ncf_LOG_EMERG,errno,"sigaction(%s) failed",sig->signame);
            return -1;
        }	
        else
        {            
            ncf_log_error_core(ncf_LOG_EMERG,errno,"sigaction(%s) succed!",sig->signame);
            ncf_log_stderr(0,"sigaction(%s) succed!",sig->signame);
        }
    } 
    return 0;   
}

//信号处理函数
//siginfo：这个系统定义的结构中包含了信号产生原因的有关信息
static void ncf_signal_handler(int signo, siginfo_t *siginfo, void *ucontext)
{    
    printf("太阳啊 来信号了 是不是最帅的司琪瑞发的呢\n");    
    ncf_signal_t    *sig;
    char            *action;
    
    for (sig = signals; sig->signo != 0; sig++) //遍历信号数组    
    {         
        if (sig->signo == signo) 
        { 
            break;
        }
    }

    action = (char *)"";

    if(ncf_process == ncf_PROCESS_MASTER)      //master进程
    {
        switch (signo)
        {
        case SIGCHLD:
            ncf_reap = 1;
            break;

        //TODO其他信号处理

        default:
            break;
        }
    }
    else if(ncf_process == ncf_PROCESS_WORKER) //worker进程
    {
        //TODO......以后再增加
    }
    else
    {

    }


    if(siginfo && siginfo->si_pid)
    {
        ncf_log_error_core(ncf_LOG_NOTICE,0,"signal %d (%s) received from %P%s", signo, sig->signame, siginfo->si_pid, action); 
    }
    else
    {
        ncf_log_error_core(ncf_LOG_NOTICE,0,"signal %d (%s) received %s",signo, sig->signame, action);
    }

    if (signo == SIGCHLD) 
    {
        ncf_process_get_status();
    }

    return;
}

//获取子进程的结束状态，防止单独kill子进程时子进程变成僵尸进程
static void ncf_process_get_status(void)
{
    pid_t            pid;
    int              status;
    int              err;
    int              one=0;

    for ( ;; ) 
    {

        pid = waitpid(-1, &status, WNOHANG);
        if(pid == 0)
        {
            return;
        }
        if(pid == -1)
        {
           
            err = errno;
            if(err == EINTR)
            {
                continue;
            }

            if(err == ECHILD  && one)
            {
                return;
            }

            if (err == ECHILD)
            {
                ncf_log_error_core(ncf_LOG_INFO,err,"waitpid() failed!");
                return;
            }
            ncf_log_error_core(ncf_LOG_ALERT,err,"waitpid() failed!");
            return;
        }
        one = 1;
        if(WTERMSIG(status))  //获取使子进程终止的信号编号
        {
            ncf_log_error_core(ncf_LOG_ALERT,0,"pid = %P exited on signal %d!",pid,WTERMSIG(status)); //获取使子进程终止的信号编号
        }
        else
        {
            ncf_log_error_core(ncf_LOG_NOTICE,0,"pid = %P exited with code %d!",pid,WEXITSTATUS(status)); //WEXITSTATUS()获取子进程传递给exit或者_exit参数的低八位
        }
    }
    return;
}
