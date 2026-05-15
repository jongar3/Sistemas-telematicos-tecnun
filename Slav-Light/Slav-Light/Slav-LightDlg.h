
// Slav-LightDlg.h : header file
//

#pragma once


// CSlavLightDlg dialog
class CSlavLightDlg : public CDialogEx
{
// Construction
public:
	CSlavLightDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SLAVLIGHT_DIALOG };
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
	CStatic m_izqdel;
	CStatic m_derdel;
	CStatic m_izqtra;
	CStatic m_dertra;
	CString m_msg;
	int m_v500;
	int m_v501;
	int m_v502;
	int m_v503;
	int m_v504;
	CStatic m_status;
};
