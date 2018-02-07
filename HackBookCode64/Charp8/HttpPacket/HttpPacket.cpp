// HttpPacket.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>

void HttpPostPacket(char *packet,char *url,char *host,char *data)
{
	//通过格式化字符串函数把信息整合到HTTP数据包中
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
	//通过格式化字符串函数把信息整合到HTTP数据包中
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
	//用于数据包数据输出
	char packet[1024];
	//提交页面
	char *url="/bbs/index.asp";
	//服务器域名
	char *host="www.nohack.cn";
	//提交数据
	char data[80]="username=nohack";
	//够着POST包
	HttpPostPacket(packet,url,host,data);
	printf("%s\n",packet);
	//构造GET包
	HttpGetPacket(packet,url,host,data);
	printf("%s\n",packet);
	getchar();

	
	
	return 0;
}
