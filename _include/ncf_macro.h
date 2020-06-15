/*************************************************************************
    * File Name: ncf_macro.h
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/

#ifndef __NFC_MACRO_H__
#define __NFC_MACRO_H__

//配置文件相关宏定义
#define     CONFITEM_MAXLEN        512

//错误信息最大数字长度


#define NCF_MAX_ERROR_STR   2048   //显示的错误信息最大数组长度

//调用memcpy之后指针指向字符串之后一位
#define ncf_cpymem(dst, src, n)   (((u_char *) memcpy(dst, src, n)) + (n))  
#define ncf_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))              

//数字相关
#define NCF_MAX_UINT32_VALUE   (uint32_t) 0xffffffff              //最大的32位无符号数：十进制是‭4294967295‬
#define NCF_INT64_LEN          (sizeof("-9223372036854775808") - 1)     

//日志相关
#define NCF_LOG_STDERR            0    //控制台错误【stderr】：最高级别日志，日志的内容不再写入log参数指定的文件，而是会直接将日志输出到标准错误设备比如控制台屏幕
#define NCF_LOG_EMERG             1    //紧急 【emerg】
#define NCF_LOG_ALERT             2    //警戒 【alert】
#define NCF_LOG_CRIT              3    //严重 【crit】
#define NCF_LOG_ERR               4    //错误 【error】：属于常用级别
#define NCF_LOG_WARN              5    //警告 【warn】：属于常用级别
#define NCF_LOG_NOTICE            6    //注意 【notice】
#define NCF_LOG_INFO              7    //信息 【info】
#define NCF_LOG_DEBUG             8    //调试 【debug】：最低级别

#define NCF_ERROR_LOG_PATH       "logs/error1.log"   //定义日志存放的路径和文件名 

//进程相关
//标记当前进程类型
#define NCF_PROCESS_MASTER     0  //master进程，管理进程
#define NCF_PROCESS_WORKER     1  //worker进程，工作进程

#endif
