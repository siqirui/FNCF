/*************************************************************************
    * File Name: ncf_conf.cpp
    * Author: siqirui
    * Mail: siqiruipcz@126.com
    * Created Time: 2019年05月06日 星期一 06时21分33秒
 ************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <map>

#include "ncf_tools.h"
#include "ncf_conf.h"
#include "ncf_global.h"

SConfig *SConfig::m_instance = nullptr;

SConfig::SConfig()
{
}
SConfig::~SConfig()
{
    for (auto pos = m_ConfigItemList.begin();pos != m_ConfigItemList.end();++pos)
    {
        delete(*pos) ; 
    }
    m_ConfigItemList.clear();
    return;
}

bool SConfig::Load(const char * pconfName)
{
    FILE *fp;
    fp = fopen(pconfName,"r");
    if (fp == nullptr)
        return false;
    
    char linebuf[CONFITEM_MAXLEN] = {};

    while (!feof(fp))
    {
        if (fgets(linebuf,CONFITEM_MAXLEN,fp) == NULL)
            continue;

        if(linebuf[0] == 0||*linebuf == ';'||*linebuf == ' '/*||*linebuf == '\t'*/||*linebuf == '\n')
            continue;
//截掉尾部的换行 空格 回车
lpprocstring:
        size_t linebuflen = strlen(linebuf);
        if (linebuflen > 0)
        {
           if(linebuf[linebuflen - 1] == 10 || linebuf[linebuflen -1] == 13 || 
           linebuf[linebuflen -1] == 32)
           {
               linebuf[linebuflen -1 ] = 0;
               goto lpprocstring;
           } 
        }
        if(linebuf[0] == 0 )
            continue;
        if (*linebuf == '[')//配置分组暂时不做处理
            continue;

        char *ptmp = strchr(linebuf,'=');

        if (ptmp != nullptr)
        {
            PSConfItem p_confitem = new SConfItem;
            memset(p_confitem,0,sizeof(SConfItem));
            strncpy(p_confitem->ItemName,linebuf,(int)(ptmp - linebuf));
            strcpy(p_confitem->ItemContent,ptmp + 1);

            Rtrim(p_confitem->ItemName);
            Ltrim(p_confitem->ItemName);
			Rtrim(p_confitem->ItemContent);
			Ltrim(p_confitem->ItemContent);

            m_ConfigItemList.push_back(p_confitem);

        }
        
    }
    fclose(fp);
    return true;

}

int SConfig::GetIntDefault(const char * p_itemname,const int def)
{
    const char * ptmp = GetString(p_itemname);
    if(ptmp != nullptr)
        return atoi(ptmp);
    return def;
}

const char * SConfig::GetString(const char * p_itemname)
{
    for (auto  pos = m_ConfigItemList.begin();pos != m_ConfigItemList.end();pos++)
    {
        if(strcasecmp((*pos)->ItemName,p_itemname) == 0)
            return (*pos)->ItemContent;
    }
    return nullptr;
}
