#pragma once
#include <afxsock.h>
#include "Slav-LightDlg.h"
#include "pch.h"
#include "framework.h"
#include "Slav-Light.h"
#include "afxdialogex.h"

class CSlavLightDlg;

class CMySocket :
    public CSocket
{
    CSlavLightDlg* pDlg;

public:

    CMySocket(CSlavLightDlg* pD) { pDlg = pD; }
    virtual void OnAccept(int err);

};


