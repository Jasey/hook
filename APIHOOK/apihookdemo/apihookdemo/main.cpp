
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

FARPROC pfadd;  //ָ��add������Զָ��

BYTE OldCode[5]; //�ϵ�ϵͳAPI��ڴ���
BYTE NewCode[5]; //Ҫ��ת��API���� (jmp xxxx)
HANDLE hProcess=NULL; //�������̵ľ��

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

	//��ȡ��������id�;��
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


		//��ȡadd.dll�����ҵ�add()����
		HMODULE hmod=::LoadLibrary(L"add.dll");
		add=(AddProc)::GetProcAddress(hmod,"add");
		pfadd=(FARPROC)add;

		if (pfadd==NULL)
		{
			cout << "cannot locate add()" << endl;
		}

		// ��add()����ڴ��뱣�浽OldCode��
		_asm 
		{ 
			lea edi,OldCode 
				mov esi,pfadd 
				cld 
				movsd 
				movsb 
		}

		NewCode[0]=0xe9;//��һ���ֽ�0xe9�൱��jmpָ��
		//��ȡMyadd()����Ե�ַ
		_asm 
		{ 
			lea eax,Myadd
				mov ebx,pfadd 
				sub eax,ebx 
				sub eax,5 
				mov dword ptr [NewCode+1],eax 
		} 
		//�����ϣ�����NewCode[]������൱��ָ�� jmp Myadd
		HookOn();
}

int WINAPI Myadd(int a,int b)
{
	//�ػ��˶�add()�ĵ��ã����Ǹ�a,b����1
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

	//���ڴ汣��ģʽ��Ϊ��д,��ģʽ������dwOldProtect
	VirtualProtectEx(hProcess,pfadd,5,PAGE_READWRITE,&dwOldProtect); 
	//������������add��ǰ5���ֽڸ�ΪJmp Myadd 
	WriteProcessMemory(hProcess,pfadd,NewCode,5,0);
	//���ڴ汣��ģʽ�Ļ�ΪdwOldProtect
	VirtualProtectEx(hProcess,pfadd,5,dwOldProtect,&dwTemp);
}

void HookOff()//������������add()����ڴ���ָ�
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
