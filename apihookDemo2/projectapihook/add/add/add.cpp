// add.cpp : 定义 DLL 应用程序的导出函数。
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