/*************************************************************************
    * File Name: ncf_printf.cpp
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>   //类型相关头文件

#include "ncf_global.h"
#include "ncf_macro.h"
#include "ncf_tools.h"

static u_char *ncf_sprintf_num(u_char *buf,u_char *last,uint64_t ui64,u_char zero,uintptr_t hexadecimal,uintptr_t width);

u_char *nfc_slprintf(u_char *buf,u_char *last,const char *fmt, ...)
{
    va_list args;
    u_char *p = nullptr;

    va_start(args,fmt);
    p = ncf_vslprintf(buf,last,fmt,args);
    va_end(args);
    return p;
}


u_char *nfc_vslprintf(u_char * buf,u_char * last,const char *fmt,va_list args)
{
    u_char zero;

    uintptr_t width,sign,hex,frac_width,scale,n;
    int64_t i64 = 0;
    uint64_t ui64 = 0;
    u_char *p = nullptr;
    double f = 0.00;
    uint64_t frac = 0;

    while (*fmt && buf < last)
    {
        if(*fmt == '%')
        {
            zero = (u_char )((*++fmt == '0')?'0' : ' ');
            width = 0;
            sign = 1;
            hex = 0;
            frac_width = 0;
        }
        whild(*fmt >= '0'&& *fmt <= '9'
        width = width * 10 + (*fmt++ - '0'));
    }
    while(true)
    {
        switch(*fmt)
        {
            case 'u':
                sign = 0;
                fmt++;
                continue;
            case 'x':
                hex = 1;
                sign = 0;
                fmt++;
                continue;
            case 'X':
                hex = 2;
                sign = 0;
                fmt++;
                continue;
            case '.':
                fmt++;
                while(*fmt >= '0' && *fmt <= '9')
                {
                    frac_width = frac_width * 10 + (*fmt++ - '0');
                }
                break;
            default:
                break;
        }
        break;
    }
    
}

