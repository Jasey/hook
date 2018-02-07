// ImportFun.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>

DWORD RVAToOffset(LPVOID lpBase,DWORD VirtualAddress)
{
	IMAGE_DOS_HEADER *dosHeader;
	IMAGE_NT_HEADERS *ntHeader;
	IMAGE_SECTION_HEADER *SectionHeader;
	int NumOfSections;
	dosHeader=(IMAGE_DOS_HEADER*)lpBase;
	DWORD FileAlignment;

	ntHeader=(IMAGE_NT_HEADERS*)((BYTE*)lpBase+dosHeader->e_lfanew);
	NumOfSections=ntHeader->FileHeader.NumberOfSections;
	for (int i=0;i<NumOfSections;i++)
	{
		DWORD NT_HEADERS_SIZE = sizeof(IMAGE_NT_HEADERS);
		SectionHeader=(IMAGE_SECTION_HEADER*)((BYTE*)lpBase+dosHeader->e_lfanew+sizeof(IMAGE_NT_HEADERS))+i;
		if(VirtualAddress>=SectionHeader->VirtualAddress&&VirtualAddress<SectionHeader->VirtualAddress+SectionHeader->SizeOfRawData)
		{
			DWORD AposRAV=VirtualAddress-SectionHeader->VirtualAddress;
			DWORD Offset=SectionHeader->PointerToRawData+AposRAV;
			return Offset;
		}
	}

	//��������ַС���ļ��������Сֵ����˵�����ڼ��ص��ڴ��е�ƫ�Ƶ�ַ��
	//�ļ�ƫ�Ƶ�ַ��һһ��Ӧ��
	FileAlignment = ntHeader->OptionalHeader.FileAlignment;

	if (FileAlignment > VirtualAddress){
		return VirtualAddress;
	}else{
		return 0;
	}
}

int main(int argc, char* argv[])
{
	//���ļ�
	argv[1] = "D:\\software\\ver1.3.exe";
	HANDLE hFile=CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile Failed\n");
		return 0;
	}
	//�����ڴ�ӳ���ļ����ں˶���
	HANDLE hMap=CreateFileMapping(hFile,NULL,PAGE_READONLY,NULL,NULL,NULL);
	if(hMap==INVALID_HANDLE_VALUE)
	{
		printf("CreateFileMapping Failed\n");
		return 0;
	}
	//���ļ�ӳ�����ڴ�
	LPVOID lpBase=MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
	if(lpBase==NULL)
	{
		printf("MapViewOfFile Failed\n");
		return 0;	
	}
	IMAGE_DOS_HEADER *dosHeader;
	IMAGE_NT_HEADERS *ntHeader;
	IMAGE_IMPORT_BY_NAME *ImportName;
	//lpBase��MapViewOfFile��������
	dosHeader=(IMAGE_DOS_HEADER*)lpBase;

	//����Ƿ�����Ч��PE�ļ�
	if (dosHeader->e_magic!=IMAGE_DOS_SIGNATURE)
	{
		printf("This is not a windows file\n");
		return 0;
	}

	ntHeader = (IMAGE_NT_HEADERS*)((BYTE*)lpBase + dosHeader->e_lfanew);
	if(ntHeader->Signature!=IMAGE_NT_SIGNATURE)
	{
		printf("This is not a win32 file\n");
		return 0;
	}
	//��λ�������

	//todo ��ǰ�������������һ�£����һ��Ϊʲô����������У��ļ��е�ƫ�ƺ��ڴ��е�ƫ����ͬ������˵�ļ��е�ƫ�ƺ��ڴ��еĲ�ͬ��
	//��Ȼ����û�м��ص��ڴ棬Ϊʲô���ļ����棬��Щֵ�Ѿ�д���ˣ���������ֵ���ڴ���ص�λ�ã������һ���Ƿ��ļ����ص��ڴ��
	//ʱ���ڴ�λ�ã�Ϊ�䷽ʽ��Ӧ�ü��ص�λ�ö��Ѿ�������ˡ����ֻ�ð���PE�ļ�ָʾֱ�Ӽ��ؾ����ˣ���������ʱ�����ġ�
	//�����λ���PE �ļ�ͷ��ߵ�SectionAligment �������ڴ�������Ķ���ֵ��PE �ļ���ӳ�䵽�ڴ���ʱ�������������ٴ�һ��ҳ�߽翪ʼ��
	//һ����X86 ϵ�е�CPU �У�ҳ�ǰ�4KB��1000h�������еģ���IA-64 �ϣ��ǰ�8KB��2000h�������еġ�������X86 ϵͳ�У�
	//PE�ļ�������ڴ����ֵһ����� 1000h��ÿ�����鰴1000h �ı������ڴ��д�š���
	//ʵ��һ�£��Ƿ�x86��ϵ�е�Ӧ�ó����Ǵ�400000h�ĵط���ʼ���صģ��Ƿ�Ҳ����ǰ�����ģ��Ƿ�Ͷ��뷽ʽһ����д��PE�ļ��У�����˵
	//ϵͳĬ��ִ��ʱ�����ģ�


	IMAGE_IMPORT_DESCRIPTOR *ImportDec=(IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)lpBase+RVAToOffset(lpBase,ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));
	while(ImportDec->FirstThunk)
	{
		//�õ�DLL�ļ���
		char *pDllName=(char*)((BYTE*)lpBase+RVAToOffset(lpBase,ImportDec->Name));
		printf("\nDLL�ļ�����%s\n",pDllName);
		//ͨ��OriginalFirstThunk��λ��PIMAGE_THUNK_DATA�ṹ����(INT)
		PIMAGE_THUNK_DATA pThunk=(PIMAGE_THUNK_DATA)((BYTE*)lpBase+RVAToOffset(lpBase,ImportDec->OriginalFirstThunk));
		while(pThunk->u1.Function)
		{
			//�жϺ������ú���������Ļ�����ŵ����
			if(pThunk->u1.Ordinal& IMAGE_ORDINAL_FLAG32)
			{
				//������
				printf("�Ӵ�DLLģ�鵼���ĺ�������ţ�%x\n",pThunk->u1.Ordinal&0xFFFF);
			}
			else
			{
				//�õ�IMAGE_IMPORT_BY_NAME�ṹ�еĺ�����
				ImportName=(IMAGE_IMPORT_BY_NAME*)((BYTE*)lpBase+RVAToOffset(lpBase,(DWORD)pThunk->u1.AddressOfData));
				printf("�Ӵ�DLLģ�鵼���ĺ����ĺ�������%s\n",ImportName->Name);
			}
			pThunk++;
		}
		ImportDec++;
	}
	UnmapViewOfFile(lpBase);
	CloseHandle(hMap);
	CloseHandle(hFile);
	return 0;
}
