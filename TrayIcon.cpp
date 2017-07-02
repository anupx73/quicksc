#include "stdafx.h"
#include "resource.h"
#include "TrayIcon.h"

//#define COUNTOF(x)   (sizeof(x)/sizeof(x[0]))


CTrayIcon cTray;

CTrayIcon::CTrayIcon()
{
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	ZeroMemory(&vi, sizeof(OSVERSIONINFO));
}

CTrayIcon::~CTrayIcon()
{
	DelTrayIcon();
}

void CTrayIcon::SetTrayIcon (HWND hwnd, LPWSTR szTip)
{
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&vi);
	
	if (vi.dwMajorVersion >= 6)
	{
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.uVersion = NOTIFYICON_VERSION_4;
	}

	else if (vi.dwMajorVersion >= 5)
	{
		nid.cbSize = NOTIFYICONDATA_V2_SIZE;	
		nid.uVersion = NOTIFYICON_VERSION;
	}

	Shell_NotifyIcon (NIM_SETVERSION, &nid);

	nid.hWnd = hwnd;
	nid.uID = 0;
	nid.uCallbackMessage = NIF_MSG;
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	wcscpy_s(nid.szTip, _countof(nid.szTip), szTip);

	Shell_NotifyIcon (NIM_ADD, &nid);
}


void CTrayIcon::ShowBalloon (DWORD dwInfoFlag, LPWSTR szInfoTitle, LPWSTR szInfo)
{
	nid.uFlags = NIF_INFO;
	nid.dwInfoFlags = dwInfoFlag;

	wcscpy_s(nid.szInfoTitle, _countof(nid.szInfoTitle), szInfoTitle);
	wcscpy_s(nid.szInfo, _countof(nid.szInfo), szInfo);

	Shell_NotifyIcon(NIM_MODIFY, &nid);
}


void CTrayIcon::DelTrayIcon ()
{
	Shell_NotifyIcon(NIM_DELETE, &nid);
}