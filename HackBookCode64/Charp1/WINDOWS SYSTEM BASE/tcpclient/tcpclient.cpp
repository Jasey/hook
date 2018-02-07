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
	//����winsock��
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// �����׽���
    SOCKET sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sClient == INVALID_SOCKET)
	{
		printf("socket error\n");
		return 0;
	}	
	// ��sockaddr_in�ṹ��װ��������˵�ַ��Ϣ
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(4500);// htons���� ���������޷��Ŷ�������ת��������
	                                //�ֽ�˳��,4500�˿�ΪҪ���ӷ������˵Ķ˿�
	servAddr.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");//��������ip
	//���ӷ�������
	if(connect(sClient,(sockaddr*)&servAddr,sizeof(servAddr))==SOCKET_ERROR)
	{
		printf("connect error\n");
		closesocket(sClient);
		return 0;
	}
	char *buff="\r\n�ڿͱ�̣�������\r\n";
    //��������
	send(sClient,buff,strlen(buff),0);
	char revData[255];
	//ֱ���յ���Ч����ʱ�Ŵ�ӡ����
	int ret=recv(sClient,revData,255,0);
	if(ret>0)
	{
		//Ϊ�˷�ֹ��ӡ�������ַ�����β���0x00
		revData[ret]=0x00;
		printf(revData);
	}
	closesocket(sClient);
	WSACleanup();
	return 0;
}
