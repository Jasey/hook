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
    unsigned char   icmp_type;		// 类型
    unsigned char   icmp_code;		// 代码
    unsigned short  icmp_checksum;	// 校验和
    unsigned short  icmp_id;		// 标识符
    unsigned short  icmp_sequence;	// 序列号
    //下面是选项数据
    unsigned long   icmp_timestamp; // 时间戳
} ICMP_HDR, *PICMP_HDR;

typedef struct _IPHeader		// 20字节的IP头
{
    UCHAR     iphVerLen;      // 版本号和头长度（各占4位）
    UCHAR     ipTOS;          // 服务类型 
    USHORT    ipLength;       // 封包总长度，即整个IP报的长度
    USHORT    ipID;			  // 封包标识，惟一标识发送的每一个数据报
    USHORT    ipFlags;	      // 标志
    UCHAR     ipTTL;	      // 生存时间，就是TTL
    UCHAR     ipProtocol;     // 协议，可能是TCP、UDP、ICMP等
    USHORT    ipChecksum;     // 校验和
    ULONG     ipSource;       // 源IP地址
    ULONG     ipDestination;  // 目标IP地址
} IPHeader, *PIPHeader; 

USHORT checksum(USHORT* buff, int size)
{
	unsigned long cksum = 0;
	while(size>1)
	{
		cksum += *buff++;
		size -= sizeof(USHORT);
	}
	// 是奇数
	if(size)
	{
		cksum += *(UCHAR*)buff;
	}
	// 将32位的chsum高16位和低16位相加，然后取反
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);		
	return (USHORT)(~cksum);
}



int main(int argc, char* argv[])
{
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//加载winsock库
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	//建立原始套接字
	SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	//超时时间
	int OutTime=100;
	//设置超时
	setsockopt(sRaw, IPPROTO_IP, IP_TTL, (char*)&OutTime, sizeof(OutTime));
	SOCKADDR_IN dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons(0);
	//填写ping主机地址
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
		//类型8
		pIcmp->icmp_type = 8;
		//代码0
		pIcmp->icmp_code = 0;
		//标识符为本进程id
		pIcmp->icmp_id = (USHORT)GetCurrentProcessId();
		//检验和先填充为0
		pIcmp->icmp_checksum = 0;
		//时间戳为系统运行时间
		pIcmp->icmp_timestamp = GetTickCount();
		//计算检验和
		pIcmp->icmp_checksum = checksum((USHORT*)buff, sizeof(ICMP_HDR) );
		//发送ICMP回显请求数据报
		nRet = sendto(sRaw, buff, sizeof(ICMP_HDR) , 0, (SOCKADDR *)&dest, sizeof(dest));
		//发送错误，提示并退出程序
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
		//得到当前系统运行时间
		int nTick = GetTickCount();
		//得到IP首部
		IPHeader *Iphdr=(IPHeader*)recvBuf;
		//判断是其后是否是ICMP报文
		if(Iphdr->ipProtocol!=0x01)
		{
			printf("this is not a icmp packet\n");
			return 0;
		}
		//定位到ICMP报文首部
		ICMP_HDR* pRecvIcmp = (ICMP_HDR*)(recvBuf + 20);
		//判断收到的报文是否和我们发送的报文相对应
		if(pRecvIcmp->icmp_id != GetCurrentProcessId())
		{
			printf("this is another icmp packet\n");
			return 0;
		}
		//输出ping主机的ip
		printf("Reply from %s: ",inet_ntoa(from.sin_addr));
		//输出收到的字节数
        printf("bytes=%d ",nRet);
		//输出从发送数据报到收到数据报所经历的时间
		printf("time=%dms\n",nTick - pRecvIcmp->icmp_timestamp);
		Sleep(100);
	}
	return 0;
}
