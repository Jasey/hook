
/**
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include "add.h"

// �Ȱ� lib ���ӽ���
#pragma comment(lib,"add")
 
 
// �ⲿ������ add ����
extern "C" _declspec (dllimport )
      int add(int a, int b);
 
int main()
{
      // ֱ�ӵ��� add ����
      printf("%d\n" , add(5, 2));
 
      return 0;
}
**/