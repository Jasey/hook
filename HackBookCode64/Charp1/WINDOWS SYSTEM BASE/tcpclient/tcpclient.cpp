// tcpclient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>

int main(int argc, char* argv[])
{
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//加载winsock库
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// 创建套节字
    SOCKET sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sClient == INVALID_SOCKET)
	{
		printf("socket error\n");
		return 0;
	}	
	// 在sockaddr_in结构中装入服务器端地址信息
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(4500);// htons函数 将主机的无符号短整形数转换成网络
	                                //字节顺序,4500端口为要连接服务器端的端口
	servAddr.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");//服务器端ip
	//连接服务器端
	if(connect(sClient,(sockaddr*)&servAddr,sizeof(servAddr))==SOCKET_ERROR)
	{
		printf("connect error\n");
		closesocket(sClient);
		return 0;
	}
	char *buff="\r\n黑客编程，我来了\r\n";
    //发送数据
	send(sClient,buff,strlen(buff),0);
	char revData[255];
	//直到收到有效数据时才打印出来
	int ret=recv(sClient,revData,255,0);
	if(ret>0)
	{
		//为了防止打印出错，把字符串结尾设成0x00
		revData[ret]=0x00;
		printf(revData);
	}
	closesocket(sClient);
	WSACleanup();
	return 0;
}
