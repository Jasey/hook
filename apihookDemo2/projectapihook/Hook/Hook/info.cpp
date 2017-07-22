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