
// Master-centralita.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMastercentralitaApp:
// See Master-centralita.cpp for the implementation of this class
//

class CMastercentralitaApp : public CWinApp
{
public:
	CMastercentralitaApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMastercentralitaApp theApp;
