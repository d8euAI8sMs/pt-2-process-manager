
// ProcessManagerDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CProcessManagerDlg dialog
class CProcessManagerDlg : public CDialogEx
{
// Construction
public:
	CProcessManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROCESSMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CListCtrl mExeList;
    CListCtrl mProcessList;
    CString mWorkingDirectory;
    void RebuildExecutableList();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    void ShowErrorMessage(LPCTSTR pszMessage, DWORD dwErrorCode);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedButton4();
};
