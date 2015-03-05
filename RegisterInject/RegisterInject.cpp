// RegisterInject.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RegisterInject.h"

#define MAX_LOADSTRING 100

// Global Variables:
TCHAR szDll[MAX_LOADSTRING] = _T("D:\\TextDll.dll");					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	HKEY hKey = NULL;
	LONG err = 0;
	if ((err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows"),0, KEY_ALL_ACCESS, &hKey)) != ERROR_SUCCESS)
	{
		
		MessageBox(NULL, _T("Can't open register"), _T("Error"), MB_OK | MB_ICONERROR);
		return 0;
	};
	err = RegSetValueEx(hKey, _T("AppInit_Dlls"),0, REG_SZ, (BYTE*)szDll, _tcslen(szDll)+1);
	return (int)0;
}


