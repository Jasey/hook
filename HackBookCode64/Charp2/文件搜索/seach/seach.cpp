// seach.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

struct DirList{
	char table[256];
	DirList *pNext;
};

DirList *first,*newlist,*last;

//加入文件夹链表
void AddList(char *list)
{
	newlist=new DirList;
	strcpy(newlist->table,list);
	newlist->pNext=NULL;
	//假如文件链表为空，那么第一个和最后一个节点都指向新节点
	if(first==NULL)
	{
		first=newlist;
		last=newlist;
	}
	//不为空，则原来最后一个节点指向新节点
	else
	{
		last->pNext=newlist;
		last=newlist;
	}	
}

//查找文件，并把找到的文件夹加入文件夹链表
void FindFile(char *pRoad,char *pFile)
{
	char FileRoad[256]={0};
	char DirRoad[256]={0};
	char FindedFile[256]={0};
	char FindedDir[256]={0};		
	strcpy(FileRoad,pRoad);
	strcpy(DirRoad,pRoad);
	strcat(DirRoad,"\\*.*");
	WIN32_FIND_DATA findData;
	HANDLE hFindFile;
	hFindFile=FindFirstFile(DirRoad,&findData);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		do
		{
			if(findData.cFileName[0]=='.')
				continue;
			//假如是文件夹，则假如文件夹列表
			if(findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcpy(FindedDir,pRoad);	
				strcat(FindedDir,"\\");
				strcat(FindedDir,findData.cFileName);
				//加入文件夹列表
				AddList(FindedDir);
				memset(FindedDir,0x00,256);
			}
			//继续查找
		}while(FindNextFile(hFindFile,&findData));
	}
	
	strcat(FileRoad,"\\");
	strcat(FileRoad,pFile);
	//查找要查找的文件
	hFindFile=FindFirstFile(FileRoad,&findData);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		do
		{ 
			strcpy(FindedFile,pRoad);
			strcat(FindedFile,"\\");
			strcat(FindedFile,findData.cFileName);
			//输出查找到的文件
			printf("%s\n",FindedFile);
			memset(FindedFile,0x00,256);
		}while(FindNextFile(hFindFile,&findData));
	}	
}

int SeachFile(char *Directory,char *SeachFile)
{
	DirList NewList;	
	strcpy(NewList.table,Directory);
	NewList.pNext=NULL;
	//初始化第一个和最后一个节点
	last=&NewList;
	first=&NewList; 
	while(true)
	{	
		DirList *Find;
		//假如链表不为空，提取链表中的第一个节点，并把第一个节点指向原来第二个
		if(first!=NULL)
		{
			//提取节点
			Find=first;
			//并把第一个节点指向原来第二个
			first=first->pNext;
			//在提取的节点的目录下查找文件
			FindFile(Find->table,SeachFile);		
		}
		//为空则停止查找
		else
		{
			printf("文件搜索完毕\n");
			return 0;		
		}		
	}
	return 0;
	
}

int main(int argc, char* argv[])
{
	SeachFile(argv[1],argv[2]);
	return 0;
}
