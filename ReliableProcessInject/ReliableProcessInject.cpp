// ReliableProcessInject.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ReliableProcessInject.h"
//settings
TCHAR tszTargetProcess[PROCESSSIZE] = _T("notepad.exe"); //target process
TCHAR tszTargetDll[DLLPATHSIZE] = _T("D:\\用户目录\\Documents\\GitHub\\InjectSolution\\x64\\Debug\\InjectionDll.dll");
TCHAR tszReliableProcess[] = _T("services.exe"); // mid process, suggest use services.exe.
TCHAR g_szInjectDll[DLLPATHSIZE] = _T("D:\\用户目录\\Documents\\GitHub\\InjectSolution\\x64\\Debug\\ReliableProcessInjectDll.dll");
//end of settings.
TCHAR g_szInjectProcessName[PROCESSSIZE] = { 0 };
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
		PVOID p = VirtualAllocEx(hInjectProcess, NULL, DLLPATHSIZE, MEM_COMMIT, PAGE_READWRITE);
		if (p == NULL)
			throw 3;
		SIZE_T dwWriteSize = 0;
		if (!WriteProcessMemory(hInjectProcess, p, g_szInjectDll, DLLPATHSIZE, &dwWriteSize))
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
		if (wcscmp(pEntry.szExeFile, g_szInjectProcessName) == 0)
		{
			InjectProcessToDll(pEntry.th32ProcessID);
		}
	} while (Process32Next(h, &pEntry));

}
void SetSelfDebugPrivilege()
{
	TOKEN_PRIVILEGES tp = { 0 };
	LUID luid = { 0 };
	TOKEN_PRIVILEGES tpPrevious = { 0 };

}
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MoveMemory(g_szInjectProcessName, tszReliableProcess, PROCESSSIZE);
	SetSelfDebugPrivilege();
	InjectMain();
	HANDLE hPipe = CreateNamedPipe(PIPENAME, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 2, 1025, 1025, INFINITE, NULL);
	{
		ConnectNamedPipe(hPipe, NULL);
		DWORD dwWrite;
		WriteFile(hPipe, tszTargetProcess, PROCESSSIZE, &dwWrite, NULL);
		WriteFile(hPipe, tszTargetDll, DLLPATHSIZE, &dwWrite, NULL);
	}
	return 0;
}
