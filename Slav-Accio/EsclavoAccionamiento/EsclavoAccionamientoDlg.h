
// EsclavoAccionamientoDlg.h : header file
//

#pragma once
#include "CMySocket.h"

// CEsclavoAccionamientoDlg dialog
class CEsclavoAccionamientoDlg : public CDialogEx
{
// Construction
public:
	CEsclavoAccionamientoDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ESCLAVOACCIONAMIENTO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int m_port;
	afx_msg void OnBnClickedStart();
	CString m_msg;
	BOOL m_fren;
	BOOL m_izq;
	BOOL m_der;
};
