/*************************************************************************
    * File Name: ncf_tools.h
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
    * file function: 零散的功能函数声明
 ************************************************************************/

#ifndef __NCF_TOOLS_H__
#define __NCF_TOOLS_H__//字符串相关函数



//字符串处理  
void Rtrim(char *string);
void Ltrim(char *string);



//设置可执行程序相关函数
void ncf_init_setproctitle();
void ncf_setproctitle(const char * title);

//log相关
//和日志，打印输出有关
void   nfc_log_init();
void   nfc_log_stderr(int err, const char *fmt, ...);
void   nfc_log_error_core(int level,  int err, const char *fmt, ...);

u_char *nfc_log_errno(u_char *buf, u_char *last, int err);
u_char *nfc_slprintf(u_char *buf, u_char *last, const char *fmt, ...);
u_char *ncf_snprintf(u_char *buf, size_t max, const char *fmt, ...);
u_char *nfc_vslprintf(u_char *buf, u_char *last,const char *fmt,va_list args);

//和信号/主流程相关相关
int    ncf_init_signals();
void   ncf_master_process_cycle();
int    ncf_daemon();
void   ncf_process_events_and_timers();


#endif

