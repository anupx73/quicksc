// CaptureSave.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "quickSC.h"
#include "quickSCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCaptureSaveApp

BEGIN_MESSAGE_MAP(CCaptureSaveApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCaptureSaveApp construction

CCaptureSaveApp::CCaptureSaveApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CCaptureSaveApp object

CCaptureSaveApp theApp;


// CCaptureSaveApp initialization

BOOL CCaptureSaveApp::InitInstance()
{
	CWinApp::InitInstance();

	RegistrySettings();

	CCaptureSaveDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}

void CCaptureSaveApp::RegistrySettings()
{
	HKEY hKey;
	RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\QuickSC", &hKey);
	
	if (RegQueryValueEx(hKey, L"NoDesktopSave", 0, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		wchar_t szDesktopPath[100];
		SHGetSpecialFolderPathW(0, szDesktopPath, CSIDL_DESKTOPDIRECTORY, FALSE);
		RegSetValueExW(hKey, L"JpgPath", 0, REG_SZ, (LPBYTE)szDesktopPath, ((DWORD)(wcslen(szDesktopPath) + 1))*sizeof(wchar_t));
	}
	

	if (RegQueryValueEx(hKey, L"NoAutoStart", 0, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		wchar_t pPath[MAX_PATH];
		HKEY hOpened;
		RegOpenKeyEx (HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hOpened);
		GetModuleFileName (0, pPath, MAX_PATH);
		RegSetValueExW (hOpened, L"quickSC", 0, REG_SZ, (LPBYTE)pPath, sizeof (pPath)/sizeof (wchar_t));
		RegCloseKey(hOpened);
	}

	if (RegQueryValueEx(hKey, L"NoJpgNameStyle", 0, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		RegSetValueExW(hKey, L"JpgName", 0, REG_SZ, (LPBYTE)L"Shot", ((DWORD)(wcslen(L"Shot") + 1))*sizeof(wchar_t));		
	}
	RegCloseKey(hKey);
}