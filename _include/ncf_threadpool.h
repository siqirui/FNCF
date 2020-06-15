/**************************************************************************
# File Name : ncf_threadpool.h
# Author: 司琪瑞
# mail : siqiruipcz@126.com
# Create Time: Mon 15 Jun 2020 08:32:22 AM CST
# Remark : 线程池
***************************************************************************/
#ifndef __NCF_THREADPOOL_H__
#define __NCF_THREADPOOL_H__

#include <vector>
#include <pthread.h>
#include <atomic> 

//线程池
class CThreadPool
{
public:
    CThreadPool();               
    ~CThreadPool();                           

public:
    bool Create(int threadNum);                     //threadNum 线程池中所有线程数量
    void StopAll();

    void inMsgRecvQueueAndSignal(char *buf);        //入消息队列，并触信号
    void Call();                                    //线程快来干活  
    int  getRecvMsgQueueCount(){return m_iRecvMsgQueueCount;} //获取接收消息队列大小

private:
    static void* ThreadFunc(void *threadData);      //新线程的线程回调函数    
    void clearMsgRecvQueue();                       //清理接收消息队列

private:  
    struct ThreadItem   
    {
        pthread_t   _Handle;
        CThreadPool *_pThis;
        bool        isrunning;                      //是否启动

        ThreadItem(CThreadPool *pthis):_pThis(pthis),ifrunning(false){}                             
        ~ThreadItem(){}        
    };

private:
    static pthread_mutex_t     m_pthreadMutex;
    static pthread_cond_t      m_pthreadCond;       //条件变量
    static bool                m_is_shutdown;       //线程退出标志出

    int                        m_iThreadNum;        //要创建的线程数量
	
    std::atomic<int>           m_iRunningThreadNum; //运行中的线程数，原子操作
    time_t                     m_iLastEmgTime;      //上次发生线程不够用【紧急事件】的时间,防止日志报的太频繁


    std::vector<ThreadItem *>  m_threadVector;      //线程容器 

    //接收消息队列相关
    std::list<char *>          m_MsgRecvQueue;
	int                        m_iRecvMsgQueueCount;
};

#endif
