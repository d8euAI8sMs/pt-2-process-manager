
// ProcessManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessManager.h"
#include "ProcessManagerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProcessManagerDlg dialog



CProcessManagerDlg::CProcessManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcessManagerDlg::IDD, pParent)
    , mWorkingDirectory(_T("C:\\Windows"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EXE_LIST, mExeList);
    DDX_Control(pDX, IDC_PROCESS_LIST, mProcessList);
    DDX_Text(pDX, IDC_EDIT1, mWorkingDirectory);
}

BEGIN_MESSAGE_MAP(CProcessManagerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON3, &CProcessManagerDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON1, &CProcessManagerDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CProcessManagerDlg::OnBnClickedButton2)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON4, &CProcessManagerDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CProcessManagerDlg message handlers

BOOL CProcessManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    

    mProcessList.InsertColumn(0, _T("PID"), LVCFMT_LEFT, 50);
    mProcessList.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 200);
    mProcessList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    RebuildExecutableList();

    SetTimer(123456, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessManagerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProcessManagerDlg::RebuildExecutableList()
{
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    CString dir = mWorkingDirectory + "\\*";

    hFind = FindFirstFile(dir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) 
    {
        ShowErrorMessage(_T("Invalid path specified"), GetLastError());
        return;
    }

    mExeList.DeleteAllItems();

    int i = 0;
    do
    {
        CString s1 = CString(ffd.cFileName);
        CString s2 = CString(ffd.cFileName).Right(4);
        if (CString(ffd.cFileName).Right(4) == CString(".exe"))
        {
            mExeList.InsertItem(i++, ffd.cFileName, 0);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    if (GetLastError() != ERROR_NO_MORE_FILES)
    {
        ShowErrorMessage(_T("Error while iterating through directory"), GetLastError());
    }

    FindClose(hFind);
}


void CProcessManagerDlg::OnBnClickedButton3()
{
    HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (!SUCCEEDED(result))
    {
        AfxMessageBox(_T("Cannot initialize COM"));
        return;
    }

    BROWSEINFO bi = {};
    bi.hwndOwner = this->GetSafeHwnd();
    bi.lpszTitle = _T("Select...");
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
    PIDLIST_ABSOLUTE pidl = SHBrowseForFolder(&bi);

    WCHAR path[MAX_PATH];
    if (pidl != NULL) // not canceled
    {
        if (!SHGetPathFromIDList(pidl, path))
        {
            AfxMessageBox(_T("The folder is not allowed"));
            CoTaskMemFree(pidl);
            return;
        }
        mWorkingDirectory = CString(path);
        CoTaskMemFree(pidl);
    }

    UpdateData(FALSE);

    RebuildExecutableList();
}


void CProcessManagerDlg::OnBnClickedButton1()
{
    int selection;
    if ((selection = mExeList.GetSelectionMark()) != -1)
    {
        CString file = mWorkingDirectory + "\\" + mExeList.GetItemText(selection, 0);
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION *pi = new PROCESS_INFORMATION();
        if (!CreateProcess(file, NULL, NULL, NULL, FALSE, 0, NULL, mWorkingDirectory, &si, pi))
        {
            ShowErrorMessage(_T("Cannot create process"), GetLastError());
            return;
        }
        CString pid; pid.Format(_T("%d"), pi->dwProcessId);
        mProcessList.InsertItem(mProcessList.GetItemCount(), pid);
        mProcessList.SetItemText(mProcessList.GetItemCount() - 1, 1, mExeList.GetItemText(selection, 0));
        mProcessList.SetItemData(mProcessList.GetItemCount() - 1, (DWORD_PTR)pi);
    }
}


void CProcessManagerDlg::OnBnClickedButton2()
{
    int selection;
    if ((selection = mProcessList.GetSelectionMark()) != -1)
    {
        PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)mProcessList.GetItemData(selection);
        
        DWORD exitCode;
        if (!GetExitCodeProcess(pi->hProcess, &exitCode))
        {
            ShowErrorMessage(_T("Cannot get process exit code"), GetLastError());
        }

        if (exitCode == STILL_ACTIVE)
        {
            if (!TerminateProcess(pi->hProcess, 0))
            {
                ShowErrorMessage(_T("Cannot terminate process"), GetLastError());
                return;
            }
        }
        CloseHandle(pi->hThread);
        CloseHandle(pi->hProcess);
        delete pi;
        mProcessList.DeleteItem(selection);
    }
}


void CProcessManagerDlg::ShowErrorMessage(LPCTSTR pszMessage, DWORD dwErrorCode)
{
    LPTSTR messageBuffer = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&messageBuffer, 0, NULL);
    CString str; str.Format(_T("%s\r\n(%d) %s"), pszMessage, dwErrorCode, messageBuffer);
    AfxMessageBox(str);
	LocalFree(messageBuffer);
}


void CProcessManagerDlg::OnTimer(UINT_PTR nIDEvent)
{
    for (int i = mProcessList.GetItemCount() - 1; i >= 0; --i)
    {
        PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)mProcessList.GetItemData(i);
        DWORD exitCode;
        if (GetExitCodeProcess(pi->hProcess, &exitCode) && (exitCode != STILL_ACTIVE))
        {
            CloseHandle(pi->hThread);
            CloseHandle(pi->hProcess);
            delete pi;
            mProcessList.DeleteItem(i);
        }
    }

    CDialogEx::OnTimer(nIDEvent);
}


void CProcessManagerDlg::OnBnClickedButton4()
{
    HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (!SUCCEEDED(result))
    {
        AfxMessageBox(_T("Cannot initialize COM"));
        return;
    }
    int selection;
    if ((selection = mExeList.GetSelectionMark()) != -1)
    {
        CString file = mWorkingDirectory + "\\" + mExeList.GetItemText(selection, 0);
        
        SHELLEXECUTEINFO shi = { sizeof(shi) };
        shi.lpVerb = _T("open");
        shi.lpFile = file;
        shi.nShow = SW_NORMAL;
        shi.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
        if (!ShellExecuteEx(&shi) || (shi.hProcess == NULL))
        {
            ShowErrorMessage(_T("Cannot create process"), GetLastError());
            return;
        }

        PROCESS_INFORMATION *pi = new PROCESS_INFORMATION();
        pi->dwProcessId = GetProcessId(shi.hProcess);
        pi->hProcess = shi.hProcess;
        pi->dwThreadId = 0;
        pi->hThread = INVALID_HANDLE_VALUE;

        CString pid; pid.Format(_T("%d"), pi->dwProcessId);
        mProcessList.InsertItem(mProcessList.GetItemCount(), pid);
        mProcessList.SetItemText(mProcessList.GetItemCount() - 1, 1, mExeList.GetItemText(selection, 0));
        mProcessList.SetItemData(mProcessList.GetItemCount() - 1, (DWORD_PTR)pi);
    }
}
