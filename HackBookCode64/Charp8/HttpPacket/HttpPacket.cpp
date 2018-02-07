// HttpPacket.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>

void HttpPostPacket(char *packet,char *url,char *host,char *data)
{
	//ͨ����ʽ���ַ�����������Ϣ���ϵ�HTTP���ݰ���
	wsprintf(packet,"POST %s HTTP/1.1\r\n"
		"Accept: */*\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Host: %s\r\n"
		"Content-Length: %d\r\n"
		"Connection: Keep-Alive\r\n"
		"Cookie: ASPSESSIONIDSCDCQCTD=DCOJKBECOHEDIJJGNAPJGOKO\r\n\r\n%s",
		url,host,strlen(data),data);
}

void HttpGetPacket(char *packet,char *url,char *host,char *data)
{
	//ͨ����ʽ���ַ�����������Ϣ���ϵ�HTTP���ݰ���
	wsprintf(packet,"GET %s?%s HTTP/1.1\r\n"
		"Accept: */*\r\n"
		"Accept-Language: zh-cn\r\n"
        "Host: %s\r\n"
		"Connection: Keep-Alive\r\n"
		"Cookie: ASPSESSIONIDQQTDTQDB=BKNHHKEDEDPADFIEMLCFAKAO\r\n\r\n",
		url,data,host);	
}

int main(int argc, char* argv[])
{
	//�������ݰ��������
	char packet[1024];
	//�ύҳ��
	char *url="/bbs/index.asp";
	//����������
	char *host="www.nohack.cn";
	//�ύ����
	char data[80]="username=nohack";
	//����POST��
	HttpPostPacket(packet,url,host,data);
	printf("%s\n",packet);
	//����GET��
	HttpGetPacket(packet,url,host,data);
	printf("%s\n",packet);
	getchar();

	
	
	return 0;
}
