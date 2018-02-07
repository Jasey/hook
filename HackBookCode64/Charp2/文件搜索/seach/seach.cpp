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

//�����ļ�������
void AddList(char *list)
{
	newlist=new DirList;
	strcpy(newlist->table,list);
	newlist->pNext=NULL;
	//�����ļ�����Ϊ�գ���ô��һ�������һ���ڵ㶼ָ���½ڵ�
	if(first==NULL)
	{
		first=newlist;
		last=newlist;
	}
	//��Ϊ�գ���ԭ�����һ���ڵ�ָ���½ڵ�
	else
	{
		last->pNext=newlist;
		last=newlist;
	}	
}

//�����ļ��������ҵ����ļ��м����ļ�������
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
			//�������ļ��У�������ļ����б�
			if(findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcpy(FindedDir,pRoad);	
				strcat(FindedDir,"\\");
				strcat(FindedDir,findData.cFileName);
				//�����ļ����б�
				AddList(FindedDir);
				memset(FindedDir,0x00,256);
			}
			//��������
		}while(FindNextFile(hFindFile,&findData));
	}
	
	strcat(FileRoad,"\\");
	strcat(FileRoad,pFile);
	//����Ҫ���ҵ��ļ�
	hFindFile=FindFirstFile(FileRoad,&findData);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		do
		{ 
			strcpy(FindedFile,pRoad);
			strcat(FindedFile,"\\");
			strcat(FindedFile,findData.cFileName);
			//������ҵ����ļ�
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
	//��ʼ����һ�������һ���ڵ�
	last=&NewList;
	first=&NewList; 
	while(true)
	{	
		DirList *Find;
		//��������Ϊ�գ���ȡ�����еĵ�һ���ڵ㣬���ѵ�һ���ڵ�ָ��ԭ���ڶ���
		if(first!=NULL)
		{
			//��ȡ�ڵ�
			Find=first;
			//���ѵ�һ���ڵ�ָ��ԭ���ڶ���
			first=first->pNext;
			//����ȡ�Ľڵ��Ŀ¼�²����ļ�
			FindFile(Find->table,SeachFile);		
		}
		//Ϊ����ֹͣ����
		else
		{
			printf("�ļ��������\n");
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
