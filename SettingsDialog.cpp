// SettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "quickSC.h"
#include "SettingsDialog.h"


// CSettingsDialog dialog

IMPLEMENT_DYNAMIC(CSettingsDialog, CDialog)

CSettingsDialog::CSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDialog::IDD, pParent)
{
	RegOpenKeyEx (HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hOpened);
	RegOpenKeyEx (HKEY_CURRENT_USER, L"SOFTWARE\\QuickSC", 0, KEY_ALL_ACCESS, &h_CU_SW);
}

CSettingsDialog::~CSettingsDialog()
{
	RegCloseKey(hOpened);
	RegCloseKey(h_CU_SW);
}

void CSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK1, m_chk);
	DDX_Control(pDX, IDC_EDIT2, m_jpgPath);
	DDX_Control(pDX, IDC_EDIT3, m_jpgName);
	DDX_Control(pDX, IDC_CHECK2, m_resetChk);
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
	ON_BN_CLICKED(BTN_SAVE_CLOSE, &CSettingsDialog::OnBnClickedSaveClose)
	ON_BN_CLICKED(BTN_BROWSE, &CSettingsDialog::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_CHECK2, &CSettingsDialog::OnBnClickedCheck2)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSettingsDialog message handlers

void CSettingsDialog::OnBnClickedSaveClose()
{	
	const DWORD data = 1;
	
	///*Reset Check*/
	//if (m_resetChk.GetCheck() == BST_CHECKED)
	//	RegSetValueExW(h_CU_SW, L"AllDefault", 0, REG_DWORD, (BYTE*)&data, sizeof (DWORD));
	//else
	//	RegDeleteValueW (h_CU_SW, L"AllDefault");

	//m_chk.SetCheck(BST_CHECKED);
	//wchar_t szDesktopPath[100];
	//SHGetSpecialFolderPathW(0, szDesktopPath, CSIDL_DESKTOPDIRECTORY, FALSE);
	//m_jpgPath.SetWindowTextW(szDesktopPath);
	//m_jpgName.SetWindowTextW(L"Shot");
	///*----------*/


	/*Auto start*/
	if (m_chk.GetCheck() == BST_CHECKED)
	{
		wchar_t pPath[MAX_PATH];
		GetModuleFileName (0, pPath, MAX_PATH);
		RegSetValueExW (hOpened, L"quickSC", 0, REG_SZ, (LPBYTE)pPath, sizeof (pPath)/sizeof (wchar_t));
		RegDeleteValue(h_CU_SW, L"NoAutoStart");
	}

	else if (m_chk.GetCheck() == BST_UNCHECKED)
	{
		RegDeleteValueW (hOpened, L"quickSC");
		RegSetValueExW(h_CU_SW, L"NoAutoStart", 0, REG_DWORD, (BYTE*)&data, sizeof (DWORD));	
	}
	/*----------*/

	/*JpgPath*/
	wchar_t szChosenJpgPath[MAX_PATH];
	const wchar_t *szDesk = L"Desktop";

	m_jpgPath.GetWindowTextW(szChosenJpgPath, MAX_PATH);

	if (wcswcs(szChosenJpgPath, szDesk) ==0 )
		RegSetValueExW(h_CU_SW, L"NoDesktopSave", 0, REG_DWORD, (BYTE*)&data, sizeof (DWORD));
	else
		RegDeleteValueW (h_CU_SW, L"NoDesktopSave");

	RegSetValueExW(h_CU_SW, L"JpgPath", 0, REG_SZ, (const BYTE*)&szChosenJpgPath, sizeof(wchar_t) * ((DWORD)wcslen(szChosenJpgPath) + 1));	
	/*-------*/

	/*JpgName*/
	wchar_t szJpgName[100] = L"";
	m_jpgName.GetWindowTextW(szJpgName, 100);

	RegSetValueExW(h_CU_SW, L"JpgName", 0, REG_SZ, (const BYTE*)&szJpgName, sizeof(wchar_t) * ((DWORD)wcslen(szJpgName) + 1));	
	
	if (wcscmp(szJpgName, L"Shot")!=0)
		RegSetValueExW(h_CU_SW, L"NoJpgNameStyle", 0, REG_DWORD, (BYTE*)&data, sizeof (DWORD));
	else
		RegDeleteValueW (h_CU_SW, L"NoJpgNameStyle");
	/*-------*/
	
	if (m_resetChk.GetCheck() == BST_UNCHECKED)
		RegSetValueExW(h_CU_SW, L"NoDefault", 0, REG_DWORD, (BYTE*)&data, sizeof (DWORD));

	EndDialog(0);
}

BOOL CSettingsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	HKEY hKey_SW_Quck;
	wchar_t szJpgPath[MAX_PATH] = L"";
	wchar_t szJpgName[100] = L"";
	DWORD regType = REG_SZ;
	DWORD cbSizP = MAX_PATH;
	DWORD cbSizN = 100;

	if (RegQueryValueEx(hOpened, L"quickSC", 0, NULL, NULL, NULL) == ERROR_SUCCESS)
		m_chk.SetCheck(BST_CHECKED);

	RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\QuickSC", 0, KEY_ALL_ACCESS, &hKey_SW_Quck);
	RegQueryValueExW(hKey_SW_Quck, L"JpgPath", 0, &regType, (LPBYTE)&szJpgPath, &cbSizP);
	RegQueryValueExW(hKey_SW_Quck, L"JpgName", 0, &regType, (LPBYTE)&szJpgName, &cbSizN);
	RegCloseKey(hKey_SW_Quck);

	m_jpgPath.SetWindowTextW(szJpgPath);
	m_jpgName.SetWindowTextW(szJpgName);

	return TRUE;
}

void CSettingsDialog::OnBnClickedBrowse()
{
	CoInitialize(NULL);

	BROWSEINFO bfi;
	memset(&bfi, 0, sizeof(BROWSEINFO));
	bfi.hwndOwner = m_hWnd;

	LPITEMIDLIST pidl = SHBrowseForFolder((LPBROWSEINFO)&bfi);
	wchar_t szChosenPath[MAX_PATH];
	SHGetPathFromIDListW(pidl, szChosenPath);

	/*IMalloc* memAllocator = NULL;
	HRESULT hr = CoGetMalloc(1, &memAllocator);
	memAllocator->gt;
	Free((void*)pidl);*/
	CoUninitialize();

	m_jpgPath.SetWindowTextW(szChosenPath);
}

void CSettingsDialog::OnBnClickedCheck2()
{
	if (m_resetChk.GetCheck() == BST_CHECKED)
	{
		RegDeleteValueW (h_CU_SW, L"NoDefault");
		wchar_t szDesktopPath[100];

		SHGetSpecialFolderPathW(0, szDesktopPath, CSIDL_DESKTOPDIRECTORY, FALSE);
		m_chk.SetCheck(BST_CHECKED);
		m_jpgPath.SetWindowTextW(szDesktopPath);
		m_jpgName.SetWindowTextW(L"Shot");
	}
}

void CSettingsDialog::OnClose()
{
	if (m_resetChk.GetCheck() == BST_CHECKED)
	{
		if (AfxMessageBox(L"Restore default settings checked, Do you want to save changes? ", MB_YESNO) == IDYES)
			OnBnClickedSaveClose();
	}
	CDialog::OnClose();
}
