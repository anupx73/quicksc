#pragma once
#include "afxwin.h"


// CSettingsDialog dialog

class CSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDialog)

public:
	CSettingsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDialog();

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSaveClose();
	virtual BOOL OnInitDialog();
	HKEY hOpened, h_CU_SW;
	CButton m_chk;
	CEdit m_jpgPath;
	afx_msg void OnBnClickedBrowse();
	CEdit m_jpgName;
	CButton m_resetChk;
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnClose();
};
