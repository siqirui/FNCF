/*************************************************************************
    * File Name: ncf_log.cpp
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>    //uintptr_t
#include <stdarg.h>    //va_start....
#include <unistd.h>    //STDERR_FILENO等
#include <sys/time.h>  //gettimeofday
#include <time.h>      //localtime_r
#include <fcntl.h>     //open
#include <errno.h>     //errno
#include <stdio.h>

#include "ncf_global.h"
#include "ncf_macro.h"
#include "ncf_tools.h"
#include "nfc_conf.h"


//错误等级
static u_char err_levels[][20]  = 
{
    {"stderr"},    //0：控制台错误
    {"emerg"},     //1：紧急
    {"alert"},     //2：警戒
    {"crit"},      //3：严重
    {"error"},     //4：错误
    {"warn"},      //5：警告
    {"notice"},    //6：注意
    {"info"},      //7：信息
    {"debug"}      //8：调试
};

ncf_log_t   ncf_log;

void ncf_log_stderr(int err,const char *fmt, ...)
{
    va_list args;
    u_char errstr[NCF_MAX_ERROR_STR +1] = {0};
    u_char *p = nullptr,*last = nullptr;

    memset(errstr,0,sizeof(errstr));

    last = errstr + NCF_MAX_ERROR_STR;

    p = ncf_cpymem(errstr,"ncf: ",strlen("ncf: "));
    va_start(args,fmt);
    p = nfc_vslprintf(p,list,fmt,args);
    va_end(args);

    if(err)
        p = nfc_log_errno(p,last,err);

    if (p >= (last -1))
        p = (last -1 ) - 1;

    *p++ = '\n';

    write(STDERR_FILENO,erstr,p - errstr)    ;
    return ;

}


u_char * ncf_log_errno(u_char *buf,u_char *last,int err)
{
    u_char *perrorinfo = strerror(err);
    size_t len = strlen(perorinfo);

    u_char leftstr[16] = {0};
    sprintf(leftstr,"(%d: ",err);

    u_char rightstr[] = ")";
    size_t rightlen = strlen(rightstr);

    size_t extranlen = leftlen + rightlen;

    if ((buf + len + extralen) < last)
    {
        buf = ncf_cpymem(buf,leftstr,leftlen);
        buf = ncf_cpymem(buf,perrorinfo,len);
        buf = ncf_cpymem(buf,rightstr,rightlen);
    }

    return 0;
    
}

void ncf_log_error_core(int level,int err,const char *fmt, ...)
{
    u_char *last = nullptr;
    u_char errstr[NCF_MAX_ERROR_STR + 1] = {0};
    memset(errstr,0,sizeof(errstr));
    last = errstr + NCF_MAX_ERROR_STR;

    struct timeval  tv;
    struct tm       tm;
    time_t          sec;
    u_char          *p = nullptr;
    va_list         args;

    memset(&tm,0,sizeof(struct timeval));
    memset(&tm,0,sizeof(struct tm));

    gettimeofdat(&tv,NULL);

    sec = tv.tv_sec;
    localtime_r(&sec,tm);
    tm.tm_mon++;
    tm.tm_year += 1900;

    u_char strcurrtime[64] = {0};
    ncf_slprintf(strcurrtime,(u_char *) -1,
    "%4d/%02d/%02d %02d:%02d:%02d",
    tm.tm_year,tm.tm_mon,tm.tm_mday,
    tm.tm_hour,tm.tm_min,tm.tm_sec);

    p = ncf_cpymem(errstr,strcurrtime,strlen((const char *)strcurrtime));
    p = ncf_slprintf(p,last," [%s] ",err_levels[level]);
    p = ncf_slprintf(p,last,"%P ",ncf_pid);

    va_start(args,fmt);
    p = ncf_vslprintf(p,last,fmt,args);
    va_end(args);

    if(err)
        p = ncf_log_errno(p,last,err);
    
    if(p >= (last -1))
        p = (last - 1) -1;
    *p++ = "\n";

    ssize_t n = 0;
    while (true)
    {
        if(level > ncf_log.log_level)
            break;
        
        n = write(ncf_log.fd,errstr,p - errstr);
        if (n == -1)
        {
            if(errno == ENOSPC)
            {}
            else
            {
                if (ncf_lig.fd != STDERR_FILENO)
                    n = write(STDERR_FILENO,ERRSTR,p - errstr);
                
            }
            
        }
        break;
    }
    return;
}


void ncf_log_init()
{
    u_char *plogname = nullptr;
    size_t nlen = 0;

    SConfig *p_config = SConifg::GetInstance();
    plogname = (u_char *)p_config->GetString("Log");
    if (plogname == nullptr)
        plogname = (u_char *)NCF_ERROR_LOG_PATH;
    
    ncf_log.log_level = p_config->GetIntDfault("LogLevel",NCF_LOG_NOTICE);
    if (ncf_lig.fd == -1)
    {
        ncf_log_stderr(errno,"[alert] could not open error log file: open()\"%s\" fail ",plogname);
        ncf_log.fd = STDERR_FILENO;
    }
    return;
    
    
}