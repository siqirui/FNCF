/**************************************************************************
# File Name : ncf_threadpool.cpp
# Author: 司琪瑞
# mail : siqiruipcz@126.com
# Create Time: Mon 15 Jun 2020 08:48:58 AM CST
# Remark: 线程池
***************************************************************************/

#include <stdarg.h>
#include <unistd.h>

#include "ncf_global.h"
#include "ncf_func.h"
#include "ncf_c_threadpool.h"
#include "ncf_c_memory.h"
#include "ncf_macro.h"

pthread_mutex_t CThreadPool::m_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;  //#define PTHREAD_MUTEX_INITIALIZER ((pthread_mutex_t) -1)
pthread_cond_t CThreadPool::m_pthreadCond = PTHREAD_COND_INITIALIZER;     //#define PTHREAD_COND_INITIALIZER ((pthread_cond_t) -1)
bool CThreadPool::m_is_shutdown = false;    //默认不退出      

CThreadPool::CThreadPool()
{
    m_iRunningThreadNum = 0;
    m_iLastEmgTime = 0;
    m_iRecvMsgQueueCount = 0;
}


CThreadPool::~CThreadPool()
{    
    clearMsgRecvQueue();
}

//各种清理函数
void CThreadPool::clearMsgRecvQueue()
{
	char * sTmpMempoint;
	CMemory *p_memory = CMemory::GetInstance();

	while(!m_MsgRecvQueue.empty())
	{
		sTmpMempoint = m_MsgRecvQueue.front();		
		m_MsgRecvQueue.pop_front(); 
		p_memory->FreeMemory(sTmpMempoint);
	}	
}

bool CThreadPool::Create(int threadNum)
{    
    ThreadItem *pThread;
    int err;

    m_iThreadNum = threadNum; //保存要创建的线程数量    
    
    for(int i = 0; i < m_iThreadNum; ++i)
    {
        m_threadVector.push_back(pThread = new ThreadItem(this));       
        err = pthread_create(&pThread->_Handle, NULL, ThreadFunc, pThread);
        if(err != 0)
        {
            ncf_log_stderr(err,"CThreadPool::Create()创建线程%d失败，返回的错误码为%d!",i,err);
            return false;
        }
        else
        {
            ncf_log_stderr(0,"CThreadPool::Create()创建线程%d成功,线程id=%d",pNew->_Handle);
        }        
    }
    std::vector<ThreadItem*>::iterator iter;
lblfor:
    for(iter = m_threadVector.begin(); iter != m_threadVector.end(); iter++)
    {
        if( (*iter)->ifrunning == false)
        {
            //等待线程启动
            usleep(100 * 1000);
        }
    }
    return true;
}

//线程入口函数
void* CThreadPool::ThreadFunc(void* threadData)
{
    ThreadItem *pThread = static_cast<ThreadItem*>(threadData);
    CThreadPool *pThreadPoolObj = pThread->_pThis;
    
    CMemory *p_memory = CMemory::GetInstance();	    
    int err;

    pthread_t tid = pthread_self();
    while(true)
    {

        err = pthread_mutex_lock(&m_pthreadMutex);  
        if(err != 0) ncf_log_stderr(err,"CThreadPool::ThreadFunc()中pthread_mutex_lock()失败，返回的错误码为%d!",err);
        

        while ( (pThreadPoolObj->m_MsgRecvQueue.size() == 0) && m_shutdown == false)
        {

            if(pThread->ifrunning == false)            
                pThread->ifrunning = true;

            pthread_cond_wait(&m_pthreadCond, &m_pthreadMutex);
 
        }

        if(m_shutdown)
        {   
            pthread_mutex_unlock(&m_pthreadMutex); 
            break;                     
        }


        char *jobbuf = pThreadPoolObj->m_MsgRecvQueue.front();
        pThreadPoolObj->m_MsgRecvQueue.pop_front();
        --pThreadPoolObj->m_iRecvMsgQueueCount;
               

        err = pthread_mutex_unlock(&m_pthreadMutex); 
        if(err != 0)  ncf_log_stderr(err,"CThreadPool::ThreadFunc()中pthread_mutex_unlock()失败，返回的错误码为%d!",err);

        ++pThreadPoolObj->m_iRunningThreadNum;

        g_socket.threadRecvProcFunc(jobbuf);

        p_memory->FreeMemory(jobbuf);
        --pThreadPoolObj->m_iRunningThreadNum;

    }

    return (void*)0;
}

//停止所有线程
void CThreadPool::StopAll() 
{

    if(m_shutdown == true)
    {
        return;
    }
    m_shutdown = true;


    int err = pthread_cond_broadcast(&m_pthreadCond); 
    if(err != 0)
    {
        ncf_log_stderr(err,"CThreadPool::StopAll()中pthread_cond_broadcast()失败，返回的错误码为%d!",err);
        return;
    }   
    std::vector<ThreadItem*>::iterator iter;
	for(iter = m_threadVector.begin(); iter != m_threadVector.end(); iter++)
    {
        pthread_join((*iter)->_Handle, NULL);
    }

    pthread_mutex_destroy(&m_pthreadMutex);
    pthread_cond_destroy(&m_pthreadCond);    
    
	for(iter = m_threadVector.begin(); iter != m_threadVector.end(); iter++)
	{
		if(*iter)
			delete *iter;
	}
	m_threadVector.clear();

    ncf_log_stderr(0,"CThreadPool::StopAll()成功返回，线程池中线程全部正常结束!");
    return;    
}

//收到一个完整消息后，入消息队列，并触发线程池中线程来处理该消息
void CThreadPool::inMsgRecvQueueAndSignal(char *buf)
{
    //互斥
    int err = pthread_mutex_lock(&m_pthreadMutex);     
    if(err != 0)
    {
        ncf_log_stderr(err,"CThreadPool::inMsgRecvQueueAndSignal()pthread_mutex_lock()失败，返回的错误码为%d!",err);
    }
        
    m_MsgRecvQueue.push_back(buf);
    ++m_iRecvMsgQueueCount;

    //取消互斥
    err = pthread_mutex_unlock(&m_pthreadMutex);   
    if(err != 0)
    {
        ncf_log_stderr(err,"CThreadPool::inMsgRecvQueueAndSignal()pthread_mutex_unlock()失败，返回的错误码为%d!",err);
    }

    //可以激发一个线程来干活了
    Call();                                  
    return;
}

//来任务了，调一个线程池中的线程下来干活
void CThreadPool::Call()
{
    int err = pthread_cond_signal(&m_pthreadCond); //唤醒一个等待该条件的线程
    if(err != 0 )
    {
        ncf_log_stderr(err,"CThreadPool::Call()中pthread_cond_signal()失败，返回的错误码为%d!",err);
    }
 
    if(m_iThreadNum == m_iRunningThreadNum)
    {        
        //线程不够用了

        time_t currtime = time(NULL);
        if(currtime - m_iLastEmgTime > 10)
        {

            m_iLastEmgTime = currtime;  //更新时间
            ncf_log_stderr(0,"CThreadPool::Call()中发现线程池中当前空闲线程数量为0，要考虑扩容线程池了!");
        }
    } 


    return;
}
