/*************************************************************************
    * File Name: ncf_memory.h
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/

#ifndef __NCF_MEMORY_H__
#define __NFC_MEMORY_H__


//内存相关类
class CMemory 
{
private:
	CMemory() {}  //单例模式

public:
	~CMemory(){};

private:
	static CMemory *m_instance;

public:	
	static CMemory* GetInstance() //单例
	{			
		if(m_instance == NULL)
		{
			//锁
			if(m_instance == NULL)
			{				
				m_instance = new CMemory(); //第一次调用放在主进程中
				static CGarhuishou cl; 
			}
			//放锁
		}
		return m_instance;
	}	
	class CGarhuishou 
	{
	public:				
		~CGarhuishou()
		{
			if (CMemory::m_instance)
			{						
				delete CMemory::m_instance; //系统退出时调用
				CMemory::m_instance = NULL;				
			}
		}
	};

public:
	void *AllocMemory(int memCount,bool ifmemset = true);
	void FreeMemory(void *point);
	
};


#endif

