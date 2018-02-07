

#include <stdio.h>


void HttpPostPacket(char *packet,char *url,char *host,char *data)
{
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
	wsprintf(packet,"GET %s?%s HTTP/1.1\r\n"
		"Accept: */*\r\n"
		"Accept-Language: zh-cn\r\n"

        "Host: %s\r\n"
		"Connection: Keep-Alive\r\n"
		"Cookie: ASPSESSIONIDQQTDTQDB=BKNHHKEDEDPADFIEMLCFAKAO\r\n\r\n",
		url,data,host);
	
	
	
}