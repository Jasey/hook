// add.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "windows.h"
#include "add.h"



BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}


extern "C" _declspec(dllexport)int WINAPI add(int a, int b) 
{ 
    return a+b; 
}