#include "stdafx.h"
#include "info.h"

RcLogInfo::RcLogInfo(void)
{
    m_pfLogFile = NULL;
    memset(m_cInfo,NULL,sizeof(m_cInfo));
}

RcLogInfo::~RcLogInfo(void)
{
    if (NULL != m_pfLogFile)
    {
        fclose(m_pfLogFile);
        m_pfLogFile = NULL;
    }
}

int RcLogInfo::SetLogFile(FILE *pfLogFile)
{
    m_pfLogFile=pfLogFile;    
    return 0;
}

int RcLogInfo::WriteLogInfo(const char *pInfo)
{
    if(NULL != m_pfLogFile)
    {
        fprintf(m_pfLogFile,"%s",pInfo);
        fflush(m_pfLogFile);
        return 0;
    }
    return 1;

    
}

bool RcLogInfo::openFile(const char * filename){

	//获取宿主进程所在路径
	char filepath[MAX_PATH];
	GetModuleFileName(NULL,filepath,MAX_PATH);
	*strrchr(filepath,'\\') = '\0';
	strcat(filepath,filename);
	//初始化日志文件
    FILE *m_file = fopen(filepath,"at+");	
    if(NULL == m_file)
    {
        return false;
    }
    m_pfLogFile = m_file;

}	
bool RcLogInfo::closeFile(){
	return fclose(m_pfLogFile);
}