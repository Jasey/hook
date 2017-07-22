// stdafx.cpp : source file that includes just the standard includes
//	NirvanaSys.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"



void WriteLog(char *LogStr)
{
	FILE *stream;
    int   numwritten;
	if( (stream = fopen( "D:\\N_Logdllp.txt", "a" )) != NULL ){
		numwritten = fwrite(LogStr, sizeof(char), strlen(LogStr), stream);
		fclose( stream );
	}
}
