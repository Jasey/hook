#include <Windows.h>
#include <stdio.h>

using namespace std;

LPCSTR WindowsName = "����ϵͳ"; 

int main(){
	char pwd[1024] = {0};

	//��ϵͳ
	HWND hwnd = FindWindow(NULL,WindowsName); 

	if (hwnd == NULL){
		return 0;
	}

	//��һ���Ӵ���
	HWND parent = hwnd;
	BOOL bFind = FALSE;
	HWND startChildHwnd = NULL;
	while (!bFind)
	{
		hwnd = FindWindowEx(parent, startChildHwnd,"WindowsForms10.Window.8.app.0.33c0d9d", NULL);
		//�Ƿ����Ӵ���
		HWND hFoundHwnd = FindWindowEx(hwnd, NULL,"WindowsForms10.Window.8.app.0.33c0d9d", "������");
		if (hFoundHwnd != NULL){
			bFind = TRUE;
		}
		startChildHwnd = hwnd;

	}


	if (hwnd == NULL){
		return 0;
	}

	//�ڶ����Ӵ���
	hwnd = FindWindowEx(hwnd, NULL, "WindowsForms10.Window.8.app.0.33c0d9d", "");
	if (hwnd == NULL){
		return 0;
	}

	//�������Ӵ���
	hwnd = FindWindowEx(hwnd, NULL, "WindowsForms10.Window.8.app.0.33c0d9d", "");

	if (hwnd == NULL){
		return 0;
	}

	//���뼶�Ӵ���
	hwnd = FindWindowEx(hwnd, NULL, "WindowsForms10.Window.8.app.0.33c0d9d", "���룺");
	if (hwnd == NULL){
		return 0;
	}


	//�����ı�����
	hwnd = FindWindowEx(hwnd, NULL, "WindowsForms10.EDIT.app.0.33c0d9d", NULL);
	if (hwnd == NULL){
		return 0;
	}


	SendMessage(hwnd, WM_GETTEXT, 1024, (LPARAM)pwd);

	printf("pwssword : %s \n", pwd);

	getchar();

}
