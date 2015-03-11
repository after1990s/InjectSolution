// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
//The Target Process
TCHAR g_szInjectProcessName[PROCESSSIZE] = _T("notepad.exe");
//The Injection Dll
TCHAR g_szInjectDll[DLLPATHSIZE] = _T("D:\\InjectionDll.dll");

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
		PVOID p = VirtualAllocEx(hInjectProcess, NULL, PROCESSSIZE, MEM_COMMIT, PAGE_READWRITE);
		if (p == NULL)
			throw 3;
		SIZE_T dwWriteSize = 0;
		if (!WriteProcessMemory(hInjectProcess, p, g_szInjectDll, (_tcslen(g_szInjectDll) + 1) * 2, &dwWriteSize))
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
unsigned int _stdcall WorkThread(void *p)
{
	//open pipe , get process name and dll path.
	try{
	const DWORD dwProcessSize = PROCESSSIZE;
	const DWORD dwDllPath = DLLPATHSIZE;
	TCHAR tszProcess[dwProcessSize] = { 0 };
	TCHAR tszDllPath[dwDllPath] = { 0 };
	HANDLE hReadPipe = NULL;
	do{
		hReadPipe = CreateFileW(PIPENAME, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		Sleep(1000);
	} while (hReadPipe == INVALID_HANDLE_VALUE);
	DWORD dwRead = 0;
	ReadFile(hReadPipe, tszProcess, dwProcessSize, &dwRead, NULL);
	if (dwRead != dwProcessSize)
		throw 3;
	ReadFile(hReadPipe, tszDllPath, dwDllPath, &dwRead, NULL);
	CloseHandle(hReadPipe);
	if (dwRead != dwDllPath)
		throw 3;
	MoveMemory(g_szInjectDll, tszDllPath, dwDllPath);
	MoveMemory(g_szInjectProcessName, tszProcess, dwProcessSize);
	//inject dll to process.
	InjectMain();
	}
	catch(int e){

	}
	return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		_beginthreadex(NULL, 0, WorkThread, NULL, 0, NULL);
#ifdef _DEBUG
		::MessageBox(NULL, _T("Reliable dll Inject."), _T("Noetice"), MB_OK);
#endif
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

