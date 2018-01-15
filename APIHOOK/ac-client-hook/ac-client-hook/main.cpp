#include <iostream>
#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#include <tchar.h> 
using namespace std;

//变量定义
string GameName = "AssaultCube";
LPCSTR LGameWindow = "AssaultCube";
char* GameMoudle = "ac_client.exe";
//函数申明
void WriteToMemory(HANDLE hProcHandle);
DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress);

BOOL ListProcessModules( DWORD dwPID, DWORD * moudleStartAddr ) ;
void printError( TCHAR* msg );



int main(){
	HWND hGameWindow = NULL; //窗口名称
	DWORD dwProcId = NULL; // 进程ID
	HANDLE hProcHandle = NULL; //模块句柄
	//找到应用程序
	hGameWindow = FindWindow( NULL, LGameWindow);
	if (hGameWindow){
		GetWindowThreadProcessId( hGameWindow, &dwProcId ); 
		if (dwProcId != 0){
			cout << "找到了进程， id ： " << dwProcId << endl;
			hProcHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwProcId ); 
			if(hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL){
				cout << "无法获取模块句柄" << endl;
			}else
			{
				cout << "获取到了模块句柄 handle : " << hProcHandle << endl;
				MODULEENTRY32 moudleAddr;
				moudleAddr.dwSize = sizeof(MODULEENTRY32);
				DWORD  moudleStartAddr = 0;

				//这个地方找PID填进去
				BOOL success = ListProcessModules((DWORD)8436, &moudleStartAddr);
				
				if (success){
					_tprintf("ac_client 的起始地址为 : 0x%08X \n", moudleStartAddr);
				}


				WriteToMemory(hProcHandle);
			}

		}

	}else{
		cout << "没有找到" << GameName << "的进程" << endl;
	}

	getchar();

}

BOOL ListProcessModules( DWORD dwPID, DWORD * moudleStartAddr) 
{ 
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE; 
	MODULEENTRY32 me32; 
	FILE *fp;
	fp = fopen("D:\\moudle.log", "w");

	//  Take a snapshot of all modules in the specified process. 
	hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID ); 
	if( hModuleSnap == INVALID_HANDLE_VALUE ) 
	{ 
		printError( TEXT("CreateToolhelp32Snapshot (of modules)") ); 
		return( FALSE ); 
	} 

	//  Set the size of the structure before using it. 
	me32.dwSize = sizeof( MODULEENTRY32 ); 

	//  Retrieve information about the first module, 
	//  and exit if unsuccessful 
	if( !Module32First( hModuleSnap, &me32 ) ) 
	{ 
		printError( TEXT("Module32First") );  // Show cause of failure 
		CloseHandle( hModuleSnap );     // Must clean up the snapshot object! 
		return( FALSE ); 
	} 

	*moudleStartAddr = (DWORD)me32.modBaseAddr;
	

	//  Now walk the module list of the process, 
	//  and display information about each module 
	do 
	{ 		
		fprintf(fp,  TEXT("\n\n     MODULE NAME:     %s"),             me32.szModule ); 
		fprintf(fp, TEXT("\n     executable     = %s"),             me32.szExePath ); 
		fprintf(fp, TEXT("\n     process ID     = 0x%08X"),         me32.th32ProcessID ); 
		fprintf(fp, TEXT("\n     ref count (g)  =     0x%04X"),     me32.GlblcntUsage ); 
		fprintf(fp, TEXT("\n     ref count (p)  =     0x%04X"),     me32.ProccntUsage ); 
		fprintf(fp, TEXT("\n     base address   = 0x%08X"), (DWORD) me32.modBaseAddr ); 
		fprintf(fp, TEXT("\n     base size      = %d"),             me32.modBaseSize ); 

	} while( Module32Next( hModuleSnap, &me32 ) ); 

	fprintf(fp, TEXT("\n"));

	//  Do not forget to clean up the snapshot object. 
	CloseHandle( hModuleSnap ); 
	fclose(fp);
	return( TRUE ); 
} 


void printError( TCHAR* msg )
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError( );
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 256, NULL );

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while( ( *p > 31 ) || ( *p == 9 ) )
		++p;
	do { *p-- = 0; } while( ( p >= sysMsg ) &&
		( ( *p == '.' ) || ( *p < 33 ) ) );

	// Display the message
	_tprintf( TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg );
}


/**
void WriteToMemory(HANDLE hProcHandle)
{

DWORD AmmoAddressToWrite = FindDmaAddy(3, hProcHandle, AmmoOffsets, AmmoBaseAddress);
WriteProcessMemory( hProcHandle, (BYTE*)AmmoAddressToWrite, &AmmoValue, sizeof(AmmoValue), NULL);


}**/

DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress)
{
	//DECLARE BASE ADDRESS
	DWORD pointer = BaseAddress;             // Declare a pointer of DWORD
	//USED TO output the contents in the pointer
	DWORD pTemp;

	DWORD pointerAddr;
	for(int i = 0; i < PointerLevel; i ++)
	{
		if(i == 0)
		{
			ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, 4, NULL);
		}
		//add first offset to that address
		pointerAddr = pTemp + Offsets[i];   // Set p1 to content of p + offset

		//Read memory one more time and exit the loop
		ReadProcessMemory(hProcHandle, (LPCVOID)pointerAddr, &pTemp, 4, NULL);
	}
	return pointerAddr;
}
