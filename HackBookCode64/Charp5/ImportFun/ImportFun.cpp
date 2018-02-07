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

	//如果虚拟地址小于文件对其的最小值，则说明，在加载到内存中的偏移地址和
	//文件偏移地址是一一对应的
	FileAlignment = ntHeader->OptionalHeader.FileAlignment;

	if (FileAlignment > VirtualAddress){
		return VirtualAddress;
	}else{
		return 0;
	}
}

int main(int argc, char* argv[])
{
	//打开文件
	argv[1] = "D:\\software\\ver1.3.exe";
	HANDLE hFile=CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile Failed\n");
		return 0;
	}
	//创建内存映射文件的内核对象
	HANDLE hMap=CreateFileMapping(hFile,NULL,PAGE_READONLY,NULL,NULL,NULL);
	if(hMap==INVALID_HANDLE_VALUE)
	{
		printf("CreateFileMapping Failed\n");
		return 0;
	}
	//把文件映射入内存
	LPVOID lpBase=MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
	if(lpBase==NULL)
	{
		printf("MapViewOfFile Failed\n");
		return 0;	
	}
	IMAGE_DOS_HEADER *dosHeader;
	IMAGE_NT_HEADERS *ntHeader;
	IMAGE_IMPORT_BY_NAME *ImportName;
	//lpBase由MapViewOfFile函数返回
	dosHeader=(IMAGE_DOS_HEADER*)lpBase;

	//检测是否是有效的PE文件
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
	//定位到导入表

	//todo 把前面的内容在消化一下，理解一下为什么在这个程序中，文件中的偏移和内存中的偏移相同？或者说文件中的偏移和内存中的不同？
	//既然程序还没有加载到内存，为什么在文件里面，这些值已经写死了（包括对齐值，内存加载的位置）？理解一下是否文件加载到内存的
	//时候，内存位置，为其方式，应该加载的位置都已经计算好了。最后只用按照PE文件指示直接加载就行了，而不是临时决定的。
	//理解这段话“PE 文件头里边的SectionAligment 定义了内存中区块的对齐值。PE 文件被映射到内存中时，区块总是至少从一个页边界开始。
	//一般在X86 系列的CPU 中，页是按4KB（1000h）来排列的；在IA-64 上，是按8KB（2000h）来排列的。所以在X86 系统中，
	//PE文件区块的内存对齐值一般等于 1000h，每个区块按1000h 的倍数在内存中存放。”
	//实验一下，是否x86体系中的应用程序都是从400000h的地方开始加载的，是否也是提前定死的？是否和对齐方式一样，写在PE文件中？还是说
	//系统默认执行时决定的？


	IMAGE_IMPORT_DESCRIPTOR *ImportDec=(IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)lpBase+RVAToOffset(lpBase,ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));
	while(ImportDec->FirstThunk)
	{
		//得到DLL文件名
		char *pDllName=(char*)((BYTE*)lpBase+RVAToOffset(lpBase,ImportDec->Name));
		printf("\nDLL文件名：%s\n",pDllName);
		//通过OriginalFirstThunk定位到PIMAGE_THUNK_DATA结构数组(INT)
		PIMAGE_THUNK_DATA pThunk=(PIMAGE_THUNK_DATA)((BYTE*)lpBase+RVAToOffset(lpBase,ImportDec->OriginalFirstThunk));
		while(pThunk->u1.Function)
		{
			//判断函数是用函数名导入的还是序号导入的
			if(pThunk->u1.Ordinal& IMAGE_ORDINAL_FLAG32)
			{
				//输出序号
				printf("从此DLL模块导出的函数的序号：%x\n",pThunk->u1.Ordinal&0xFFFF);
			}
			else
			{
				//得到IMAGE_IMPORT_BY_NAME结构中的函数名
				ImportName=(IMAGE_IMPORT_BY_NAME*)((BYTE*)lpBase+RVAToOffset(lpBase,(DWORD)pThunk->u1.AddressOfData));
				printf("从此DLL模块导出的函数的函数名：%s\n",ImportName->Name);
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
