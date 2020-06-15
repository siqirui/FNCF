/*************************************************************************
    * File Name: ncf_comm.h
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/
#ifndef     __NCF_COMM_H__
#define     __NCF_COMM_H__

#include <stddef.h> 

#define _PKG_MAX_LENGTH     30960  //每个包的长度不得大于 _PKG_MAX_LENGTH - 1

//通信 收包状态定义
#define _PKG_HD_INIT         0  //初始状态，准备接收数据包头
#define _PKG_HD_RECVING      1  //接收包头中，包头不完整，继续接收中
#define _PKG_BD_INIT         2  //包头刚好收完，准备接收包体
#define _PKG_BD_RECVING      3  //接收包体中，包体不完整，继续接收中，处理后直接回到_PKG_HD_INIT状态
//#define _PKG_RV_FINISHED     4  //完整包收完

#define _DATA_BUFSIZE_       16  //包头缓冲区大小，应大于sizeof(_COMM_PKG_HEANDER)


#pragma pack (1) //对齐方式,1字节对齐

//网络通讯相关结构
//包头结构
typedef struct _COMM_PKG_HEADER
{
	unsigned short pkgLen;    //总体包长度  包头+数据段
	unsigned short msgCode;   //消息类型代码--2字节
	int            crc32;     //CRC32效验--4字节	
}COMM_PKG_HEADER,*LPCOMM_PKG_HEADER;


#pragma pack() //取消指定对齐，恢复缺省对齐

#endif