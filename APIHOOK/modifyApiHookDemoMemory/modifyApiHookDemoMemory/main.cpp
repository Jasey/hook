#include <iostream>
#include <Windows.h>
#include <string>
using namespace std;

/**
1��ֱ�ӻ�ȡ��Ҫ�޸ĵ�Զ�̽��̾������Զ�̽������޸����������ַ�ռ��ֵ
	�������£�
	1�����ݽ���PIDʹ�� hProcHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwProcId ); ��ȡ���̾��
	2���ҵ����̵Ļ���ַ��Ȼ��λ����Ҫ�޸Ĵ���ĵ�ַ������ַ��ƫ����������ַ�������������򣬻��߲�ͬ���Զ���ͬ������ƫ����һ�㶼����ͬ�ģ�
	3��ʹ��WriteProcessMemory( hProcHandle, (BYTE*)addressToWrite, &value, sizeof(value), NULL);������Զ�̽�����д����Ҫ������
	������ֱ���ڱ��ؽ�����д��ԭ��ÿ�����̵������ַ�ռ䶼�Ƕ����ģ����ؽ����޷�ֱ�ӷ���Զ������ռ��ַ��
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
