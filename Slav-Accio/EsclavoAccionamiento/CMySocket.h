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

    CMySocket(CEsclavoAccionamientoDlg* pD) { pDlg = pD; }
    virtual void OnAccept(int err);

};


