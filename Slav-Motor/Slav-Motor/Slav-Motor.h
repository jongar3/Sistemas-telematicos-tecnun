
// Slav-Motor.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSlavMotorApp:
// See Slav-Motor.cpp for the implementation of this class
//

class CSlavMotorApp : public CWinApp
{
public:
	CSlavMotorApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSlavMotorApp theApp;
