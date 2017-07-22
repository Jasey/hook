
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

int main()
{
      HMODULE hModule = NULL;
	  typedef int (WINAPI *AddProc)(int a,int b);//add.dll中的add函数定义
 
      // 动态加载 DLL 文件
      hModule = LoadLibrary(_TEXT("..//..//myproject//add//Debug//add.dll" ));
 
      // 获取 add 函数地址
      AddProc fAdd = (AddProc)GetProcAddress(hModule, "add" );
 
      // 使用函数指针
      printf("%d\n" , fAdd(5, 2));
 
      // 最后记得要释放指针
      FreeLibrary(hModule);
 
      return 0;
}
