/*************************************************************************
    * File Name: ncf_conf.h
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
    * file function: 配置文件加载头文件
 ************************************************************************/
#ifndef __NCF_CONF_H__
#define __NCF_CONF_H__

#include <vector>
#include "ncf_global.h"

//配置文件只需要在启动的时候加载一个全局量，所以在此使用线程安全的单例模式
//目前未做线程安全改造，切勿重复加载
class SConfig
{
private:
    SConfig();
public:
    ~SConfig();
private:
    static SConfig * GetInstance()
    {
        if(m_instance == nullptr)
        {
            if(m_instance == nullptr)
            {
                m_instance = new SConfig();
                static  CGarhuishou C1;
            }
        }
    }

    class CGarhuishou
    {
        public:
            ~CGarhuishou()
            {
                if(SConfig::m_instance)
                {
                    delete SConfig::m_instance;
                    SConfig::m_instance = nullptr;
                }
            }
    };
public:
    bool Load(const char * pconfName);
    const char *GetString(const char * p_itemname);
    int GetIntDefault(const char *p_itemname,const int def);
public:
    std::vector<PSConfItem> m_ConfigItemList;//存储配置信息列表
private:
    static SConfig *m_instance;

};

#endif
