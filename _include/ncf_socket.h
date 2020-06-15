/*************************************************************************
    * File Name: ncf_socket.h
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/

#ifndef __NCF_SOCKET_H__
#define __NCF_SOCKET_H__

#include "ncf_comm.h"

#define NGX_LISTEN_BACKLOG  511    //已完成连接队列，nginx给511，我们也先按照这个来
#define NGX_MAX_EVENTS      512    //epoll_wait一次最多接收这么多个事件，nginx中缺省是512，我们这里固定给成512就行，没太大必要修改

typedef struct ncf_listening_s   ncf_listening_t, *lpncf_listening_t;
typedef struct ncf_connection_s  ncf_connection_t,*lpncf_connection_t;
typedef class  CSocekt           CSocekt;

typedef void (CSocekt::*ncf_event_handler_pt)(lpncf_connection_t c); //定义成员函数指针

//一些专用结构定义放在这里，暂时不考虑放ncf_global.h里了
struct ncf_listening_s  //和监听端口有关的结构
{
	int                       port;        //监听的端口号
	int                       fd;          //套接字句柄socket
	lpncf_connection_t        connection;  //连接池中的一个连接，注意这是个指针 
};

//该结构表示一个TCP连接【客户端主动发起的,服务器被动接受的TCP连接】
struct ncf_connection_s
{
	
	int                       fd;             //套接字句柄socket
	lpncf_listening_t         listening;      //监听套接字		

	unsigned                  instance:1;     //【位域】失效标志位：0：有效，1：失效 
	uint64_t                  iCurrsequence;  //我引入的一个序号，每次分配出去时+1
	struct sockaddr           s_sockaddr;     //保存client地址信息用的
	char                      addr_text[64]; //地址的文本信息

	//uint8_t                   r_ready;        //读准备好标记
	uint8_t                   w_ready;        //写准备好标记

	ncf_event_handler_pt      rhandler;       //读事件的相关处理方法
	ncf_event_handler_pt      whandler;       //写事件的相关处理方法
	
	//和收包有关
	unsigned char             curStat;                        //当前收包的状态
	char                      dataHeadInfo[_DATA_BUFSIZE_];   //用于保存收到的数据的包头信息			
	char                      *precvbuf;                      //接收数据的缓冲区的头指针
	unsigned int              irecvlen;                       //要收到多少数据，由这个变量指定，和precvbuf配套使用，看具体应用的代码

	bool                      ifnewrecvMem;                   //是否分配内存给包头+消息头+包体内容
	char                      *pnewMemPointer;                //new出来的用于收包的内存首地址，和ifnewrecvMem配对使用

	lpncf_connection_t        data;
};

//消息头，引入的目的是当收到数据包时，额外记录一些内容以备将来使用
typedef struct _STRUC_MSG_HEADER
{
	lpncf_connection_t pConn;
	uint64_t           iCurrsequence; //收到数据包时记录对应连接的序号，将来能用于比较是否连接已经作废用	
}STRUC_MSG_HEADER,*LPSTRUC_MSG_HEADER;

//------------------------------------
//socket相关类
class CSocekt
{
public:
	CSocekt();                                                         //构造函数
	virtual ~CSocekt();                                                //释放函数
public:    
    virtual bool Initialize();                                         //初始化函数

public:	
	int  ncf_epoll_init();                                             //epoll功能初始化
	//void ncf_epoll_listenportstart();                                  //监听端口开始工作 
	int  ncf_epoll_add_event(int fd,int readevent,int writeevent,uint32_t otherflag,uint32_t eventtype,lpngx_connection_t c);     
	                                                                   //epoll增加事件
	int  ncf_epoll_process_events(int timer);                          //epoll等待接收和处理事件

private:	
	void ReadConf();                                                   //专门用于读各种配置项	
	bool ncf_open_listening_sockets();                                 //监听必须的端口【支持多个端口】
	void ncf_close_listening_sockets();                                //关闭监听套接字
	bool setnonblocking(int sockfd);                                   //设置非阻塞套接字	


	void ncf_event_accept(lpncf_connection_t oldc);                    //建立新连接
	void ncf_wait_request_handler(lpncf_connection_t c);               //设置数据来时的读处理函数
	void ncf_close_connection(lpncf_connection_t c);                   //通用连接关闭函数，资源用这个函数释放【因为这里涉及到好几个要释放的资源，所以写成函数】

	ssize_t recvproc(lpncf_connection_t c,char *buff,ssize_t buflen);  //接收从客户端来的数据专用函数
	void ncf_wait_request_handler_proc_p1(lpncf_connection_t c);       //包头收完整后的处理	                                                                   
	void ncf_wait_request_handler_proc_plast(lpncf_connection_t c);    //收到一个完整包后的处理
	void inMsgRecvQueue(char *buf);                                    //收到一个完整消息后，入队列
	void tmpoutMsgRecvQueue(); //临时清除对列中消息函数，测试函数
	void clearMsgRecvQueue();                                          //清理接收消息队列

                                             
	size_t ncf_sock_ntop(struct sockaddr *sa,int port,u_char *text,size_t len);  //根据参数1给定的信息，获取地址端口字符串，返回这个字符串的长度

	//连接池 或 连接 相关
	lpncf_connection_t ncf_get_connection(int isock);                  //从连接池中获取一个空闲连接
	void ncf_free_connection(lpncf_connection_t c);                    //归还参数c所代表的连接到到连接池中	

private:
	int                            m_worker_connections;               //epoll连接的最大项数
	int                            m_ListenPortCount;                  //所监听的端口数量
	int                            m_epollhandle;                      //epoll_create返回的句柄

	//和连接池有关的
	lpncf_connection_t             m_pconnections;                     //注意这里可是个指针，其实这是个连接池的首地址
	lpncf_connection_t             m_pfree_connections;                //空闲连接链表头，连接池中总是有某些连接被占用，为了快速在池中找到一个空闲的连接，我把空闲的连接专门用该成员记录;
	                                                                        //【串成一串，其实这里指向的都是m_pconnections连接池里的没有被使用的成员】
	//lpncf_event_t                  m_pread_events;                     //指针，读事件数组
	//lpncf_event_t                  m_pwrite_events;                    //指针，写事件数组
	int                            m_connection_n;                     //当前进程中所有连接对象的总数【连接池大小】
	int                            m_free_connection_n;                //连接池中可用连接总数
	std::vector<lpncf_listening_t> m_ListenSocketList;                 //监听套接字队列
	struct epoll_event             m_events[NGX_MAX_EVENTS];           //用于在epoll_wait()中承载返回的所发生的事件

	//一些和网络通讯有关的成员变量
	size_t                         m_iLenPkgHeader;                    //sizeof(COMM_PKG_HEADER);		
	size_t                         m_iLenMsgHeader;                    //sizeof(STRUC_MSG_HEADER);
	//消息队列
	std::list<char *>              m_MsgRecvQueue;                     //接收数据消息队列 

};

#endif

