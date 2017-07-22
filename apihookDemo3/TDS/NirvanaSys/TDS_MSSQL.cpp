// TDS_MSSQL.cpp: implementation of the CTDS_MSSQL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NirvanaSys.h"
#include "TDS_MSSQL.h"
#include <windows.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDS_MSSQL::CTDS_MSSQL()
{

}

CTDS_MSSQL::~CTDS_MSSQL()
{

}

int CTDS_MSSQL::TDSDecodeSqlACK(u_char* data,int len)
{
	int i;
	/*if ((data[0]==0x81) || (data[0]==0xa0)){
		for (i=0; i<len; i++) {
			printf("%02X ", data[i]);
		}
	}*/

    return 0;
}

u_char* replace(u_char *p,int len)
{
    u_char *psz=(u_char *)malloc(sizeof(u_char)*(len+1));
    char *rtn=(char *)psz;
	int i=0;
    while(i<len+4)
    {
        if(*p!=0x00)
            *psz++=*p++;
        else
            *p++;
		i++;
    }
	*psz=0x00;
	
    //return (u_char *)strlwr(rtn);
	return (u_char *)rtn;
}


int CTDS_MSSQL::TDSDecodeSqlBatch(u_char* data,int len)
{
	COPYDATASTRUCT cpd;
	/*FILE *stream;
    int   numwritten;

	if(memcmp(data,"I",1)==0 || memcmp(data,"U",1)==0 || memcmp(data,"i",1)==0 || memcmp(data,"u",1)==0){
		if( (stream = fopen( "D:\\fread.txt", "a" )) != NULL )
		{
		numwritten = fwrite(data, sizeof(char), len, stream);
		fclose( stream );
		}
	}*/
	u_char* tempData;
	tempData=replace(data,len);
	cpd.dwData = 0;
	cpd.cbData = len;
	cpd.lpData = tempData;
	::SendMessage(hWndtest,WM_COPYDATA,NULL,(LPARAM)&cpd);//发送！
    return 0;
}

CString CTDS_MSSQL::TDSPrintPasswd(u_char* data,int len)
{
    u_char p, p1, p2;
	CString passBuf="";
    int i;
    for(i=0; i < len; i++) {
        p = data[i*2] ^ '\xA5';
        p1 = p << 4; //upper half byte, most significant. 1a (hex), we are tackling 1
        p2 = p >> 4;
        printf("%c",p1|p2);
		passBuf+=(p1|p2);
    }
    return passBuf;
}

CString CTDS_MSSQL::TDSPrintString(u_char* data,int len)
{
    int i;
	CString strBuf="";
    for(i=0;i<len;i++){
        printf("%c",data[i*2]);
		strBuf+=data[i*2];
    }
    return strBuf;
}

int CTDS_MSSQL::TDSDecodeLoginPacket(u_char* data,int len)
{
    tds_login_packet_t p = (tds_login_packet_t)data;
    int uo = (p->user.offset),
        ul = (p->user.length),
        po = (p->pass.offset),
        pl = (p->pass.length),
        so = (p->server.offset),
        sl = (p->server.length),
		dno = (p->database.offset),
		dnl = (p->database.length);
    //printf("uo=%d, ul=%d, po=%d, pl=%d\n",uo,ul,po,pl);
    //printf("TDSLogin: user=");
    dataInfo.user=TDSPrintString(data+uo,ul);
    //printf(" pass=");
    dataInfo.pass=TDSPrintPasswd(data+po,pl);
    //printf(" server=");
    dataInfo.server=TDSPrintString(data+so,sl);
	dataInfo.dbname=TDSPrintString(data+dno,dnl);
    //printf("\n"); 
	//printf("LOGIN \n");
	OutputDebugString("user="+dataInfo.user+" pass="+dataInfo.pass+" server="+dataInfo.server+" dbname="+dataInfo.dbname);
    return 0;
}


int CTDS_MSSQL::TDSDecodePreLoginPacket(u_char* data, int len)
{
	/*for (i=0; i<len; i++) {
		printf("%02X ", data[i]);
	}
	printf("\n ");*/
	printf("PRELOGIN \n");
	//预登陆27为加密标记
	if(data[27]==0x01){
		printf("TDSPreLogin: ssl 加密传输！ \n");
	}
  return 0;
}

void CTDS_MSSQL::TDSDecode(u_char *data, int len,int type)
{
	softType=type;
    tds_header_t header = (tds_header_t)data;
    switch(header->type)
    {
		case 0x12: //PreLogin
			//TDSDecodePreLoginPacket(data+8,len-sizeof(struct tds_header_s));
			break;
		//case 0x02://Login
		case 0x10:
			//TDSDecodeLoginPacket(data+8,len-sizeof(struct tds_header_s));
			TDSDecodeLoginPacket(data+8,len);
			break;
		case 0x01://sqlbatch
			TDSDecodeSqlBatch(data+8,len);
			//捕获打印机调用
			//TDSDecodeSqlBatch_printer(data+8,len);
			break;
		case 0x04://ACK
			//TDSDecodeSqlACK(data+8,len-sizeof(struct tds_header_s));
			break;
    }
}