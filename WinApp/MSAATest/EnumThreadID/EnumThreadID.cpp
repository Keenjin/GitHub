// EnumThreadID.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <TlHelp32.h>
#include <iostream>
#include <string>

using namespace std;


#include <windows.h>
#include <malloc.h>
#include <stdio.h>

// Structure to be used for a list item; the first member is the 
// SLIST_ENTRY structure, and additional members are used for data.
// Here, the data is simply a signature for testing purposes. 


typedef struct _PROGRAM_ITEM {
	SLIST_ENTRY ItemEntry;
	ULONG Signature;
} PROGRAM_ITEM, *PPROGRAM_ITEM;

int main()
{
	ULONG Count;
	PSLIST_ENTRY pFirstEntry, pListEntry;
	PSLIST_HEADER pListHead;
	PPROGRAM_ITEM pProgramItem;

	// Initialize the list header.
	pListHead = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER),
		MEMORY_ALLOCATION_ALIGNMENT);
	if (NULL == pListHead)
	{
		printf("Memory allocation failed.\n");
		return -1;
	}
	InitializeSListHead(pListHead);

	// Insert 10 items into the list.
	for (Count = 1; Count <= 10; Count += 1)
	{
		pProgramItem = (PPROGRAM_ITEM)_aligned_malloc(sizeof(PROGRAM_ITEM),
			MEMORY_ALLOCATION_ALIGNMENT);
		if (NULL == pProgramItem)
		{
			printf("Memory allocation failed.\n");
			return -1;
		}
		pProgramItem->Signature = Count;
		pFirstEntry = InterlockedPushEntrySList(pListHead,
			&(pProgramItem->ItemEntry));
	}

	// Remove 10 items from the list and display the signature.
	for (Count = 10; Count >= 1; Count -= 1)
	{
		pListEntry = InterlockedPopEntrySList(pListHead);

		if (NULL == pListEntry)
		{
			printf("List is empty.\n");
			return -1;
		}

		pProgramItem = (PPROGRAM_ITEM)pListEntry;
		printf("Signature is %d\n", pProgramItem->Signature);

		// This example assumes that the SLIST_ENTRY structure is the 
		// first member of the structure. If your structure does not 
		// follow this convention, you must compute the starting address 
		// of the structure before calling the free function.

		_aligned_free(pListEntry);
	}

	// Flush the list and verify that the items are gone.
	pListEntry = InterlockedFlushSList(pListHead);
	pFirstEntry = InterlockedPopEntrySList(pListHead);
	if (pFirstEntry != NULL)
	{
		printf("Error: List is not empty.\n");
	}
	return 1;
}





//// 旋转锁范例
//BOOL g_fResourceInUse = FALSE;
//void Func1()
//{
//	// 等待可以访问资源
//	while (InterlockedExchange(&g_fResourceInUse, TRUE) == TRUE)
//	{
//		Sleep(0);
//	}
//
//	// 访问资源
//	...
//
//	InterlockedExchange(&g_fResourceInUse, FALSE);
//}


int _tmain(int argc, _TCHAR* argv[])
{
	cout << "枚举线程所属的进程ID" << endl;

	while (true)
	{
		cout << "待枚举的线程ID:";
		DWORD dwThreadID = 0;
		cin >> dwThreadID;
		if (dwThreadID == 0)
		{
			break;
		}

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32);
		THREADENTRY32 te32;
		te32.dwSize = sizeof(te32);
		HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);

		do
		{
			if (INVALID_HANDLE_VALUE == hSnap)
			{
				break;
			}

			BOOL bFind = FALSE;

			BOOL bMore = ::Thread32First(hSnap, &te32);
			while (bMore)
			{
				if (dwThreadID == te32.th32ThreadID)
				{
					bFind = TRUE;
					CAtlString strProcessName;

					BOOL bPMore = ::Process32First(hSnap, &pe32);
					while (bPMore)
					{
						if (pe32.th32ProcessID == te32.th32OwnerProcessID)
						{
							strProcessName = pe32.szExeFile;
						}
						bPMore = ::Process32Next(hSnap, &pe32);
					}
					cout << "对应进程ID：" << te32.th32OwnerProcessID << " 进程名:" << CW2A(strProcessName.GetString(), CP_UTF8) << endl;
					break;
				}
				bMore = ::Thread32Next(hSnap, &te32);
			}

			if (!bFind)
			{
				cout << "线程句柄未找到" << endl;
			}
		} while (false);

		if (hSnap)
		{
			::CloseHandle(hSnap);
			hSnap = NULL;
		}
	}

	return 0;
}

