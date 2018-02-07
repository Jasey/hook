// tcpserver.cpp : Defines the entry point for the console application.
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
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sListen == INVALID_SOCKET)
	{
		printf("socket error\n");
		return 0;
	}	
	// 在sockaddr_in结构中装入地址信息
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);	// htons函数 将主机的无符号短整形数转换成网络
	//字节顺序
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	// 使套接字和本地地址绑定
	if(bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf(" bind error \n");
		closesocket(sListen);
		return 0;
	}
	// 设置套接字进入监听模式
	if(listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("listen error\n");
		closesocket(sListen);
		return 0;
	}
	// 循环接受客户的连接请求
	sockaddr_in remoteAddr; 
	SOCKET sClient;
    int nAddrLen = sizeof(remoteAddr);
	char revData[255];
	while(TRUE)
	{
		// 接受一个新连接
		sClient = accept(sListen, (SOCKADDR*)&remoteAddr, &nAddrLen);
		//accept函数调用失败则继续等待连接。
		if(sClient == INVALID_SOCKET)
		{
			printf("accept() error");
			continue;
		}
		//打印出连接者的ip
		printf(" 接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
		//直到收到有效数据时才打印出来
		int ret=recv(sClient,revData,255,0);
        if(ret>0)
		{
			//为了防止打印出错，把字符串结尾设成0x00
			revData[ret]=0x00;
			printf(revData);
		}
		char *buff="\r\n黑客编程，我来了\r\n";
        //发送数据
		send(sClient,buff,strlen(buff),0);
		// 关闭套接字句柄，结束会话
		closesocket(sClient);
	}
	closesocket(sListen);
	WSACleanup();
	return 0;
}
