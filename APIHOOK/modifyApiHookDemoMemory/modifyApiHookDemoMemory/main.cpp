#include <iostream>
#include <Windows.h>
#include <string>
using namespace std;

/**
1、直接获取需要修改的远程进程句柄，在远程进程中修改他的虚拟地址空间的值
	具体如下：
	1、根据进程PID使用 hProcHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwProcId ); 获取进程句柄
	2、找到进程的基地址，然后定位到需要修改代码的地址（基地址加偏移量，基地址可能因重启程序，或者不同电脑而不同，但是偏移量一般都是相同的）
	3、使用WriteProcessMemory( hProcHandle, (BYTE*)addressToWrite, &value, sizeof(value), NULL);方法在远程进程中写入需要的数据
	（不能直接在本地进程中写的原因：每个进程的虚拟地址空间都是独立的，本地进程无法直接访问远程虚拟空间地址）
**/
int main(){
	// Get Process Handle 
	HANDLE hProcHandle = NULL;
	DWORD dwProcId = NULL;
	DWORD addressToWrite = NULL;
	BYTE value[] = {0x55,0x22,0x00,0xFF}; 

	cout << "please enter the process's PID : " << endl;
	cin >> dwProcId;
	cout << "please enter the vm address of the process" <<endl;
	cin >> addressToWrite;
	hProcHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwProcId ); 
	if( hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL ) 
	{ 
		cout << "Failed to open process for valid handle" << endl; 
	} 
	else
	{
		printf("0x%08X ~ 0x%08X's value hacked to be : 55 22 00 FF\n",addressToWrite, addressToWrite + 3, value);
		if (WriteProcessMemory( hProcHandle, (BYTE*)addressToWrite, &value, sizeof(value), NULL)){
			cout << "modified success"<< endl;
		}else
		{
			cout << "failed modified" << endl;
		}

	}

	getchar();
	getchar();
}
