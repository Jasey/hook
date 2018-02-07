// MessageBox.cpp : Defines the entry point for the console application.
//
//vc自动添加的头文件
#include "stdafx.h"
//包含MessageBox函数申明的头文件
#include <windows.h>

int main(int argc, char* argv[])
{
	//调用MessageBox这个API函数
	MessageBox(NULL,"hello,hacker program","Test",0);
	return 0;
}
