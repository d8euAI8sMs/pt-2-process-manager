
// ProcessManager.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CProcessManagerApp:
// See ProcessManager.cpp for the implementation of this class
//

class CProcessManagerApp : public CWinApp
{
public:
	CProcessManagerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
//    void ShowErrorMessage();
};

extern CProcessManagerApp theApp;