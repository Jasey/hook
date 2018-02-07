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
	//����winsock��
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// �����׽���
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sListen == INVALID_SOCKET)
	{
		printf("socket error\n");
		return 0;
	}	
	// ��sockaddr_in�ṹ��װ���ַ��Ϣ
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);	// htons���� ���������޷��Ŷ�������ת��������
	//�ֽ�˳��
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	// ʹ�׽��ֺͱ��ص�ַ��
	if(bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf(" bind error \n");
		closesocket(sListen);
		return 0;
	}
	// �����׽��ֽ������ģʽ
	if(listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("listen error\n");
		closesocket(sListen);
		return 0;
	}
	// ѭ�����ܿͻ�����������
	sockaddr_in remoteAddr; 
	SOCKET sClient;
    int nAddrLen = sizeof(remoteAddr);
	char revData[255];
	while(TRUE)
	{
		// ����һ��������
		sClient = accept(sListen, (SOCKADDR*)&remoteAddr, &nAddrLen);
		//accept��������ʧ��������ȴ����ӡ�
		if(sClient == INVALID_SOCKET)
		{
			printf("accept() error");
			continue;
		}
		//��ӡ�������ߵ�ip
		printf(" ���ܵ�һ�����ӣ�%s \r\n", inet_ntoa(remoteAddr.sin_addr));
		//ֱ���յ���Ч����ʱ�Ŵ�ӡ����
		int ret=recv(sClient,revData,255,0);
        if(ret>0)
		{
			//Ϊ�˷�ֹ��ӡ�������ַ�����β���0x00
			revData[ret]=0x00;
			printf(revData);
		}
		char *buff="\r\n�ڿͱ�̣�������\r\n";
        //��������
		send(sClient,buff,strlen(buff),0);
		// �ر��׽��־���������Ự
		closesocket(sClient);
	}
	closesocket(sListen);
	WSACleanup();
	return 0;
}
