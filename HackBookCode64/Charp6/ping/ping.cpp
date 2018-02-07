// ping.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include <string.h>
#include "Ws2tcpip.h"
typedef struct icmp_hdr
{
    unsigned char   icmp_type;		// ����
    unsigned char   icmp_code;		// ����
    unsigned short  icmp_checksum;	// У���
    unsigned short  icmp_id;		// ��ʶ��
    unsigned short  icmp_sequence;	// ���к�
    //������ѡ������
    unsigned long   icmp_timestamp; // ʱ���
} ICMP_HDR, *PICMP_HDR;

typedef struct _IPHeader		// 20�ֽڵ�IPͷ
{
    UCHAR     iphVerLen;      // �汾�ź�ͷ���ȣ���ռ4λ��
    UCHAR     ipTOS;          // �������� 
    USHORT    ipLength;       // ����ܳ��ȣ�������IP���ĳ���
    USHORT    ipID;			  // �����ʶ��Ωһ��ʶ���͵�ÿһ�����ݱ�
    USHORT    ipFlags;	      // ��־
    UCHAR     ipTTL;	      // ����ʱ�䣬����TTL
    UCHAR     ipProtocol;     // Э�飬������TCP��UDP��ICMP��
    USHORT    ipChecksum;     // У���
    ULONG     ipSource;       // ԴIP��ַ
    ULONG     ipDestination;  // Ŀ��IP��ַ
} IPHeader, *PIPHeader; 

USHORT checksum(USHORT* buff, int size)
{
	unsigned long cksum = 0;
	while(size>1)
	{
		cksum += *buff++;
		size -= sizeof(USHORT);
	}
	// ������
	if(size)
	{
		cksum += *(UCHAR*)buff;
	}
	// ��32λ��chsum��16λ�͵�16λ��ӣ�Ȼ��ȡ��
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);		
	return (USHORT)(~cksum);
}



int main(int argc, char* argv[])
{
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//����winsock��
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	//����ԭʼ�׽���
	SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	//��ʱʱ��
	int OutTime=100;
	//���ó�ʱ
	setsockopt(sRaw, IPPROTO_IP, IP_TTL, (char*)&OutTime, sizeof(OutTime));
	SOCKADDR_IN dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons(0);
	//��дping������ַ
	dest.sin_addr.S_un.S_addr = inet_addr(argv[1]);
	char buff[sizeof(ICMP_HDR)];
	ICMP_HDR* pIcmp = (ICMP_HDR*)buff;
	char recvBuf[1024];
	SOCKADDR_IN from;
	int nLen = sizeof(from);
	while(TRUE)
	{
		static int nCount = 0;
		int nRet;
		if(nCount++ == 4)
			break;
		//����8
		pIcmp->icmp_type = 8;
		//����0
		pIcmp->icmp_code = 0;
		//��ʶ��Ϊ������id
		pIcmp->icmp_id = (USHORT)GetCurrentProcessId();
		//����������Ϊ0
		pIcmp->icmp_checksum = 0;
		//ʱ���Ϊϵͳ����ʱ��
		pIcmp->icmp_timestamp = GetTickCount();
		//��������
		pIcmp->icmp_checksum = checksum((USHORT*)buff, sizeof(ICMP_HDR) );
		//����ICMP�����������ݱ�
		nRet = sendto(sRaw, buff, sizeof(ICMP_HDR) , 0, (SOCKADDR *)&dest, sizeof(dest));
		//���ʹ�����ʾ���˳�����
		if(nRet == SOCKET_ERROR)
		{
			printf(" sendto failed\n");
			return 0;
		}
		nRet = recvfrom(sRaw, recvBuf, 1024, 0, (sockaddr*)&from, &nLen);
		if(nRet == SOCKET_ERROR)
		{
			if(WSAGetLastError() == WSAETIMEDOUT)
			{
				printf(" timed out\n");
				continue;
			}
			
			printf(" recvfrom failed \n");
			return 0;
		}
		//�õ���ǰϵͳ����ʱ��
		int nTick = GetTickCount();
		//�õ�IP�ײ�
		IPHeader *Iphdr=(IPHeader*)recvBuf;
		//�ж�������Ƿ���ICMP����
		if(Iphdr->ipProtocol!=0x01)
		{
			printf("this is not a icmp packet\n");
			return 0;
		}
		//��λ��ICMP�����ײ�
		ICMP_HDR* pRecvIcmp = (ICMP_HDR*)(recvBuf + 20);
		//�ж��յ��ı����Ƿ�����Ƿ��͵ı������Ӧ
		if(pRecvIcmp->icmp_id != GetCurrentProcessId())
		{
			printf("this is another icmp packet\n");
			return 0;
		}
		//���ping������ip
		printf("Reply from %s: ",inet_ntoa(from.sin_addr));
		//����յ����ֽ���
        printf("bytes=%d ",nRet);
		//����ӷ������ݱ����յ����ݱ���������ʱ��
		printf("time=%dms\n",nTick - pRecvIcmp->icmp_timestamp);
		Sleep(100);
	}
	return 0;
}
