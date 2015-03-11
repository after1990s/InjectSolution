// RemoteThreadInject.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RemoteThreadInject.h"

#define MAX_LOADSTRING 100
//The Target Process
TCHAR g_szInjectProcessName[MAXPATH] = _T("notepad.exe");
//The Injection Dll
TCHAR g_szInjectDll[MAXPATH] = _T("D:\\InjectionDll.dll");

void SuccessfulNotify()
{                   
	//Add successful notify by yourself.
}
void FailNotify()
{
	//Add fail notify by yourself.
}
void InjectProcessToDll(DWORD dwPID)
{
	try{
		HANDLE hInjectProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPID);
		if (hInjectProcess == INVALID_HANDLE_VALUE)
			throw 3;
		PVOID p = VirtualAllocEx(hInjectProcess, NULL, MAXPATH, MEM_COMMIT, PAGE_READWRITE);
		if (p == NULL)
			throw 3;
		SIZE_T dwWriteSize = 0;
     		if (!WriteProcessMemory(hInjectProcess, p, g_szInjectDll, (_tcslen(g_szInjectDll) + 1)*2, &dwWriteSize))
		{
			throw 3;
		}
		LPTHREAD_START_ROUTINE lpRemoteRoutine = (LPTHREAD_START_ROUTINE)::GetProcAddress(::LoadLibrary(_T("Kernel32.dll")), "LoadLibraryW");
		SECURITY_ATTRIBUTES sa = { 0 };
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		HANDLE hRemoteThread = CreateRemoteThread( 
			hInjectProcess,
			&sa,
			0,
			lpRemoteRoutine,
			p,
			0,
			NULL);
		if (hRemoteThread == NULL)
			throw 3;
		CloseHandle(hRemoteThread);
		CloseHandle(hInjectProcess);
	}
	catch (int &e)
	{
		return;
	}
	SuccessfulNotify();
	
	return;
}
void InjectMain(void)
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (h == INVALID_HANDLE_VALUE)
		throw 3;
	PROCESSENTRY32 pEntry = { 0 };
	pEntry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(h, &pEntry) == false)
		throw 3;
	do {
		if (_tccmp(pEntry.szExeFile, g_szInjectProcessName) == 0)
		{
			InjectProcessToDll(pEntry.th32ProcessID);
		}
	} while (Process32Next(h, &pEntry));

}
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	InjectMain();
	return (int)0;
}


