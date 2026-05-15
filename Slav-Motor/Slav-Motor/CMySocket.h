#pragma once
#include <afxsock.h>
#include "Slav-MotorDlg.h"
#include "pch.h"
#include "framework.h"
#include "Slav-Motor.h"
#include "afxdialogex.h"

class CSlavMotorDlg;

class CMySocket :
    public CSocket
{
    CSlavMotorDlg* pDlg;

public:

    CMySocket(CSlavMotorDlg* pD) { pDlg = pD; }
    virtual void OnAccept(int err);

};


