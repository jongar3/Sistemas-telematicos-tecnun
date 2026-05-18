#pragma once
#include <afxsock.h>
#include "EsclavoAccionamientoDlg.h"
#include "pch.h"
#include "framework.h"
#include "EsclavoAccionamiento.h"
#include "afxdialogex.h"

class CEsclavoAccionamientoDlg;

class CMySocket :
    public CSocket
{
    CEsclavoAccionamientoDlg *pDlg;

public:
    volatile bool m_bStop;

    CMySocket(CEsclavoAccionamientoDlg* pDlg) : pDlg(pDlg), m_bStop(false) {}
    virtual void OnAccept(int err);

};


