
/**
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include "add.h"

// 先把 lib 链接进来
#pragma comment(lib,"add")
 
 
// 外部声明的 add 函数
extern "C" _declspec (dllimport )
      int add(int a, int b);
 
int main()
{
      // 直接调用 add 函数
      printf("%d\n" , add(5, 2));
 
      return 0;
}
**/