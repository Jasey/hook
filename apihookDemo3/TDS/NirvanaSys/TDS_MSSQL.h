// TDS_MSSQL.h: interface for the CTDS_MSSQL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TDS_MSSQL_H__460DFA27_6F56_4283_9D04_DC51BF795418__INCLUDED_)
#define AFX_TDS_MSSQL_H__460DFA27_6F56_4283_9D04_DC51BF795418__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>

/* TDS Ê×²¿*/ 
typedef struct tds_header_s{
    u_char type;
    u_char status;
    u_int size;
    u_int channel;
    u_char packet;
    u_char window;
    u_char data[0];
}*tds_header_t;

typedef struct offset_and_length_s{
    u_short offset;
    u_short length;
}offlen_t;

typedef struct tds_login_packet_s{
    u_long length;
    u_long version;
    u_long packet_size;
    u_long client_version;
    u_long client_pid;
    u_long connection_id;
    u_long flags;
    u_long time_zone;
    u_long lcid;
    offlen_t client;
    offlen_t user;
    offlen_t pass;
    offlen_t app;
    offlen_t server;
    offlen_t _unused1;
    offlen_t library;
    offlen_t locale;
    offlen_t database;
    u_char  client_id[6];
    offlen_t sspi;
    offlen_t attch_db;
    union{
        struct{
            offlen_t change_pwd;
            u_long sspi;
            u_char  data[0];
        }tds_72;
        u_char data[0];
    } data;
}*tds_login_packet_t;

typedef struct dbinfo{
    CString user;
    CString pass;
    CString server;
	CString dbname;
}dbinfo_t;



class CTDS_MSSQL  
{
public:
	CTDS_MSSQL();
	virtual ~CTDS_MSSQL();
	void TDSDecode(u_char *data, int len,int type);
	int TDSDecodePreLoginPacket(u_char* data, int len);
	int TDSDecodeLoginPacket(u_char* data,int len);
	int TDSDecodeSqlBatch(u_char* data,int len);
	int TDSDecodeSqlACK(u_char* data,int len);
	int TDSDecodeSqlBatch_printer(u_char* data,int len);
	CString TDSPrintPasswd(u_char* data,int len);
	CString TDSPrintString(u_char* data,int len);
	int softType;
	dbinfo_t dataInfo;
	HWND hWndtest;
};

#endif // !defined(AFX_TDS_MSSQL_H__460DFA27_6F56_4283_9D04_DC51BF795418__INCLUDED_)
