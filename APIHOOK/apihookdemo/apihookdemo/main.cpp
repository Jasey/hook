
using namespace std;
#include <iostream>
#include <Windows.h>


void Inject();
void HookOn();
void HookOff();
int WINAPI Myadd(int a,int b);

typedef int (WINAPI*AddProc)(int a,int b);
HINSTANCE hAddDll = NULL;
AddProc add;

FARPROC pfadd;  //指向add函数的远指针

BYTE OldCode[5]; //老的系统API入口代码
BYTE NewCode[5]; //要跳转的API代码 (jmp xxxx)
HANDLE hProcess=NULL; //所处进程的句柄

int main(){
	

	if (hAddDll == NULL){
		hAddDll=::LoadLibrary(L"add.dll");
	}

	if (hAddDll == NULL){
		cout << "add.dll load failed" << endl;
	}

	add=(AddProc)::GetProcAddress(hAddDll,"add");

	int a=1;
	int b=2;


	int c=add(a,b);
	

	cout << "add : " << a << " + " << b << " = " << c << endl;

	getchar();

	cout << "inter hook api " << endl;

	//获取所属进程id和句柄
	DWORD dwPid=::GetCurrentProcessId();
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,0,dwPid); 

	Inject();
	cout << "after api hook, add : " << a << " + " << b << " = " << add(a,b) << endl;
	getchar();
	HookOff();
	cout << "unintall hook, add : " << a << " + " << b << " = " << add(a,b) << endl;
	getchar();

	//inject 
}

void Inject()
{


		//读取add.dll并查找到add()函数
		HMODULE hmod=::LoadLibrary(L"add.dll");
		add=(AddProc)::GetProcAddress(hmod,"add");
		pfadd=(FARPROC)add;

		if (pfadd==NULL)
		{
			cout << "cannot locate add()" << endl;
		}

		// 将add()的入口代码保存到OldCode里
		_asm 
		{ 
			lea edi,OldCode 
				mov esi,pfadd 
				cld 
				movsd 
				movsb 
		}

		NewCode[0]=0xe9;//第一个字节0xe9相当于jmp指令
		//获取Myadd()的相对地址
		_asm 
		{ 
			lea eax,Myadd
				mov ebx,pfadd 
				sub eax,ebx 
				sub eax,5 
				mov dword ptr [NewCode+1],eax 
		} 
		//填充完毕，现在NewCode[]里面就相当于指令 jmp Myadd
		HookOn();
}

int WINAPI Myadd(int a,int b)
{
	//截获了对add()的调用，我们给a,b都加1
	a=a+1;
	b=b+1;
	cout << "fuck" << endl;
	return a + b;
}

void HookOn() 
{ 
	if (hProcess==NULL){
		cout << "process is null" << endl;
	}

	DWORD dwTemp=0;
	DWORD dwOldProtect;

	//将内存保护模式改为可写,老模式保存入dwOldProtect
	VirtualProtectEx(hProcess,pfadd,5,PAGE_READWRITE,&dwOldProtect); 
	//将所属进程中add的前5个字节改为Jmp Myadd 
	WriteProcessMemory(hProcess,pfadd,NewCode,5,0);
	//将内存保护模式改回为dwOldProtect
	VirtualProtectEx(hProcess,pfadd,5,dwOldProtect,&dwTemp);
}

void HookOff()//将所属进程中add()的入口代码恢复
{ 
	if (hProcess==NULL){
		cout << "process is null";
	}

	DWORD dwTemp=0;
	DWORD dwOldProtect;

	VirtualProtectEx(hProcess,pfadd,5,PAGE_READWRITE,&dwOldProtect); 
	WriteProcessMemory(hProcess,pfadd,OldCode,5,0); 
	VirtualProtectEx(hProcess,pfadd,5,dwOldProtect,&dwTemp); 
}
