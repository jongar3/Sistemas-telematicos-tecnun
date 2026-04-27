
// Master-centralitaDlg.h : header file
//

#pragma once
#include "ModbusClient.h"

// CMastercentralitaDlg dialog
class CMastercentralitaDlg : public CDialogEx
{
// Construction
public:
	CMastercentralitaDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MASTERCENTRALITA_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	// Variables de estado para los LEDs
	BOOL m_statusBrake;
	BOOL m_statusLeft;
	BOOL m_statusRight;

	// Brochas para los colores
	CBrush m_brushRed;    // Para el Freno
	CBrush m_brushYellow; // Para Intermitentes
	CBrush m_brushGray;   // Para estado OFF

	CModbusClient m_modbusMotor;          // Para el puerto 502
	CModbusClient m_modbusAccionamientos; // Para el puerto 503 en realidad son variables... pero weno
	CModbusClient m_modbusLuces;          // Para el puerto 504
	HICON m_hIcon;
	void actualizarInterfazAccionamientos(short brake, short left, short right);
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedLedAccionamientos();
	CString m_ipAccionamientos;
	int m_portAccionamientos;
	CStatic m_led2;
	CStatic m_led2i;
	CStatic m_led2_d;
	CStatic m_led2f;
	afx_msg void OnBnClickedOk();
	int m_pollingMs;
	afx_msg void OnEnChangePolling();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CListBox m_log;
};
