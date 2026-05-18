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
    volatile bool m_bStop;
    CMySocket(CSlavLightDlg* pDlg) : pDlg(pDlg), m_bStop(false) {}

    //CMySocket(CSlavLightDlg* pD) { pDlg = pD; }
    virtual void OnAccept(int err);

};


