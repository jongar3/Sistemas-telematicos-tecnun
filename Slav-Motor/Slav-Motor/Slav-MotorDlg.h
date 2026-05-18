
// Slav-MotorDlg.h : header file
//

#pragma once
class CMySocket;


// CSlavMotorDlg dialog
class CSlavMotorDlg : public CDialogEx
{
// Construction
public:
	CSlavMotorDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SLAVMOTOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CMySocket* misoc;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int m_port;
	int m_temp;
	int m_rpm;
	bool m_bRunning;
	afx_msg void OnBnClickedStart();
	afx_msg void OnReleasedcaptureTemp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureRpm(NMHDR* pNMHDR, LRESULT* pResult);
	CString m_msg;
};
