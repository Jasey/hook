#include <Windows.h>
#include <stdio.h>

using namespace std;

LPCSTR WindowsName = "收银系统"; 

int main(){
	char pwd[1024] = {0};

	//主系统
	HWND hwnd = FindWindow(NULL,WindowsName); 

	if (hwnd == NULL){
		return 0;
	}

	//第一级子窗口
	HWND parent = hwnd;
	BOOL bFind = FALSE;
	HWND startChildHwnd = NULL;
	while (!bFind)
	{
		hwnd = FindWindowEx(parent, startChildHwnd,"WindowsForms10.Window.8.app.0.33c0d9d", NULL);
		//是否有子窗口
		HWND hFoundHwnd = FindWindowEx(hwnd, NULL,"WindowsForms10.Window.8.app.0.33c0d9d", "餐饮版");
		if (hFoundHwnd != NULL){
			bFind = TRUE;
		}
		startChildHwnd = hwnd;

	}


	if (hwnd == NULL){
		return 0;
	}

	//第二级子窗口
	hwnd = FindWindowEx(hwnd, NULL, "WindowsForms10.Window.8.app.0.33c0d9d", "");
	if (hwnd == NULL){
		return 0;
	}

	//第三级子窗口
	hwnd = FindWindowEx(hwnd, NULL, "WindowsForms10.Window.8.app.0.33c0d9d", "");

	if (hwnd == NULL){
		return 0;
	}

	//密码级子窗口
	hwnd = FindWindowEx(hwnd, NULL, "WindowsForms10.Window.8.app.0.33c0d9d", "密码：");
	if (hwnd == NULL){
		return 0;
	}


	//密码文本窗口
	hwnd = FindWindowEx(hwnd, NULL, "WindowsForms10.EDIT.app.0.33c0d9d", NULL);
	if (hwnd == NULL){
		return 0;
	}


	SendMessage(hwnd, WM_GETTEXT, 1024, (LPARAM)pwd);

	printf("pwssword : %s \n", pwd);

	getchar();

}
