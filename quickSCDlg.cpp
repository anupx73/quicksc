// CaptureSaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickSC.h"
#include "quickSCDlg.h"
#include "TrayIcon.h"
#include "ScreenCaptureSave.h"
#include "SettingsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CCaptureSaveDlg dialog

CCaptureSaveDlg::CCaptureSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCaptureSaveDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_visible = FALSE;
}

void CCaptureSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCaptureSaveDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_CONTEXT_EXIT, &CCaptureSaveDlg::OnContextExit)
	ON_COMMAND(ID_CONTEXT_ABOUT, &CCaptureSaveDlg::OnContextAbout)
	ON_COMMAND(ID_CONTEXT_SETTINGS, &CCaptureSaveDlg::OnContextSettings)
	ON_MESSAGE(WM_HOTKEY,OnHotKey)
END_MESSAGE_MAP()


// CCaptureSaveDlg message handlers

BOOL CCaptureSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	cTray.SetTrayIcon(m_hWnd, L"quick screen clipper");
	cTray.ShowBalloon(NIIF_INFO, L"quickSC - quick screen clipper", L"Press (WinKey + A) to clip active window.\nPress (WinKey + S) to clip entire screen.");

	::RegisterHotKey(m_hWnd, 100, MOD_WIN, 0x41);		//WinKey + A
	::RegisterHotKey(m_hWnd, 200, MOD_WIN, 0x53);		//WinKey + S

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCaptureSaveDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCaptureSaveDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCaptureSaveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CCaptureSaveDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	CMenu PopupMenu;
	CPoint point;

	if (message == NIF_MSG)
	{
		switch (LOWORD(lParam))
		{
			case WM_LBUTTONUP:
			{
				CAboutDlg dlg;
				dlg.DoModal();

				break;
			}

			case WM_RBUTTONUP:
			{
				PopupMenu.LoadMenu(IDR_MENU1);
			
				CMenu *pPopupMenu = PopupMenu.GetSubMenu(0);
				::GetCursorPos(&point);
				SetForegroundWindow();
				pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this, NULL);
				PostMessage(WM_NULL);
			}break;
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


void CCaptureSaveDlg::OnDestroy()
{
	CDialog::OnDestroy();
	
	cTray.DelTrayIcon();
}


void CCaptureSaveDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if(!m_visible)
		lpwndpos->flags &= ~SWP_SHOWWINDOW;

	CDialog::OnWindowPosChanging(lpwndpos);
}

LRESULT CCaptureSaveDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case 100:
			{
				CRect rect;
				
				HWND hWnd = ::GetForegroundWindow();

				::GetWindowRect(hWnd, rect);

				cSC.ProcessCaptureSave(rect);   
			}
			//OnContextWindow();
			break;
		
		case 200:
			{
				CRect rect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
				cSC.ProcessCaptureSave(rect);
			}
			//OnContextScreen();
			break;
	}

	return 0;
}
void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	EndDialog(0);
	CDialog::OnLButtonDown(nFlags, point);
}

void CCaptureSaveDlg::OnContextAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CCaptureSaveDlg::OnContextExit()
{
	PostQuitMessage(0);
}

void CCaptureSaveDlg::OnContextSettings()
{
	CSettingsDialog sDlg;
	sDlg.DoModal();
}
