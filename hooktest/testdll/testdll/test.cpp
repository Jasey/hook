
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

int main()
{
      HMODULE hModule = NULL;
	  typedef int (WINAPI *AddProc)(int a,int b);//add.dll�е�add��������
 
      // ��̬���� DLL �ļ�
      hModule = LoadLibrary(_TEXT("..//..//myproject//add//Debug//add.dll" ));
 
      // ��ȡ add ������ַ
      AddProc fAdd = (AddProc)GetProcAddress(hModule, "add" );
 
      // ʹ�ú���ָ��
      printf("%d\n" , fAdd(5, 2));
 
      // ���ǵ�Ҫ�ͷ�ָ��
      FreeLibrary(hModule);
 
      return 0;
}
