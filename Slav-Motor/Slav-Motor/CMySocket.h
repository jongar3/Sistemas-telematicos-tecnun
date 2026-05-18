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
    volatile bool m_bStop;

    CMySocket(CSlavMotorDlg* pDlg) : pDlg(pDlg), m_bStop(false) {}
    virtual void OnAccept(int err);

};


