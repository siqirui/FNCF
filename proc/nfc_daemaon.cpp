/*************************************************************************
    * File Name: ncf_daemaon.cpp
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>     //errno
#include <sys/stat.h>
#include <fcntl.h>


#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_conf.h"

int ngx_daemon()
{
    
    switch (fork())  //生产进程
    {
    case -1:
        ncf_log_error_core(NCF_LOG_EMERG,errno, "ncf_daemon()中fork()失败!");
        return -1;
    case 0:
        break;
    default:
        return 1; 
    }

    //生产进程流程
    ncf_parent = ncf_pid;     //ncf_pid是原来父进程的id
    ncf_pid = getpid();       //当前子进程的id
    
    //脱离终端，终端关闭，将跟此子进程无关
    if (setsid() == -1)  
    {
        ncf_log_error_core(NCF_LOG_EMERG, errno,"ncf_daemon()中setsid()失败!");
        return -1;
    }

    umask(0); 

    //打开黑洞设备
    int fd = open("/dev/null", O_RDWR);
    if (fd == -1) 
    {
        ncf_log_error_core(NCF_LOG_EMERG,errno,"ncf_daemon()中open(\"/dev/null\")失败!");        
        return -1;
    }
    if (dup2(fd, STDIN_FILENO) == -1) //先关闭STDIN_FILENO
    {
        ncf_log_error_core(NCF_LOG_EMERG,errno,"ncf_daemon()中dup2(STDIN)失败!");        
        return -1;
    }
    if (dup2(fd, STDOUT_FILENO) == -1) //再关闭STDIN_FILENO
    {
        ncf_log_error_core(NCF_LOG_EMERG,errno,"ncf_daemon()中dup2(STDOUT)失败!");
        return -1;
    }
    if (fd > STDERR_FILENO) 
     {
        if (close(fd) == -1)  //释放资源
        {
            ncf_log_error_core(NCF_LOG_EMERG,errno, "ncf_daemon()中close(fd)失败!");
            return -1;
        }
    }
    return 0; //子进程返回0
}
