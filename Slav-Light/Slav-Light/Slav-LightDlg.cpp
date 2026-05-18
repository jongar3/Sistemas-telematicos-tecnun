
// Slav-LightDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Slav-Light.h"
#include "Slav-LightDlg.h"
#include "afxdialogex.h"
#include "CMySocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSlavLightDlg dialog

CSlavLightDlg::CSlavLightDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SLAVLIGHT_DIALOG, pParent)
	, m_port(0)
	, m_msg(_T(""))
	, m_v500(0)
	, m_v501(0)
	, m_v502(0)
	, m_v503(0)
	, m_v504(0)
	, m_bRunning(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSlavLightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Control(pDX, IDC_IZQDEL, m_izqdel);
	DDX_Control(pDX, IDC_DERDEL, m_derdel);
	DDX_Control(pDX, IDC_IZQTRA, m_izqtra);
	DDX_Control(pDX, IDC_DERTRA, m_dertra);
	DDX_Text(pDX, IDC_MSG, m_msg);
	DDX_Text(pDX, IDC_TESTFRE, m_v500);
	DDX_Text(pDX, IDC_TESTFRE3, m_v501);
	DDX_Text(pDX, IDC_TESTFRE2, m_v502);
	DDX_Text(pDX, IDC_TESTFRE4, m_v503);
	DDX_Text(pDX, IDC_TESTFRE5, m_v504);
	DDX_Control(pDX, IDC_STATUS, m_status);
}

BEGIN_MESSAGE_MAP(CSlavLightDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CSlavLightDlg::OnBnClickedStart)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_UPDATE_MODBUS_DATA, &CSlavLightDlg::OnUpdateModbusData)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSlavLightDlg message handlers

BOOL CSlavLightDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_port = 3504;

	m_msg = "Socket sin crear";

	UpdateData(false);

	m_brushRed.CreateSolidBrush(RGB(255, 0, 0));
	m_brushYellow.CreateSolidBrush(RGB(255, 255, 0));
	m_brushWhite.CreateSolidBrush(RGB(160, 160, 160));

	m_blinkState = false;
	m_bRunning = false;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSlavLightDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

HBRUSH CSlavLightDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	COLORREF yellow = RGB(255, 255, 0);
	COLORREF white = RGB(160, 160, 160);
	COLORREF red = RGB(255, 0, 0);

	COLORREF color = white;

	bool isOurs = false;

	if (pWnd->GetSafeHwnd() == m_status.GetSafeHwnd())
	{
		color = (m_v500 != 0) ? red : white;
		isOurs = true;
	}
	else if (pWnd->GetSafeHwnd() == m_izqdel.GetSafeHwnd())
	{
		color = (m_v501 != 0 && m_blinkState) ? yellow : white;
		isOurs = true;
	}
	else if (pWnd->GetSafeHwnd() == m_derdel.GetSafeHwnd())
	{
		color = (m_v502 != 0 && m_blinkState) ? yellow : white;
		isOurs = true;
	}
	else if (pWnd->GetSafeHwnd() == m_dertra.GetSafeHwnd())
	{
		color = (m_v504 != 0 && m_blinkState) ? yellow : white;
		isOurs = true;
	}
	else if (pWnd->GetSafeHwnd() == m_izqtra.GetSafeHwnd())
	{
		color = (m_v503 != 0 && m_blinkState) ? yellow : white;
		isOurs = true;
	}

	if (!isOurs) return hbr;

	pDC->SetBkColor(color);
	pDC->SetTextColor(color);

	if (color == red)    return (HBRUSH)m_brushRed;
	if (color == yellow) return (HBRUSH)m_brushYellow;
	return (HBRUSH)m_brushWhite;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

LRESULT CSlavLightDlg::OnUpdateModbusData(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);

	Invalidate(FALSE);

	return 0;
}

void CSlavLightDlg::UpdateIndicator(CStatic& ctrl, bool active, bool blink)
{
	if (!ctrl.GetSafeHwnd()) return;

	COLORREF color;
	if (!active)
		color = RGB(160, 160, 160);       // off 
	else if (blink)
		color = RGB(255, 255, 0);           //on
	else
		color = RGB(160, 160, 160);       // on + off 

	CClientDC dc(&ctrl);
	CRect r;
	ctrl.GetClientRect(r);
	dc.FillSolidRect(r, color);
}

void CSlavLightDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		UpdateIndicator(m_status, m_v500 != 0, true);
		UpdateIndicator(m_izqdel, m_v501 != 0, m_blinkState);
		UpdateIndicator(m_derdel, m_v502 != 0, m_blinkState);
		UpdateIndicator(m_dertra, m_v504 != 0, m_blinkState);
		UpdateIndicator(m_izqtra, m_v503 != 0, m_blinkState);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSlavLightDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSlavLightDlg::OnBnClickedStart()
{
	UpdateData();
	
	if (m_bRunning == false) {

		UpdateData(true);
		int puerto = (int)m_port;

		misoc = new CMySocket(this);


		if (!misoc->Create(puerto, SOCK_STREAM)) {
			MessageBox("Error al crear el socket");
		} 

		if (!misoc->Listen()) {
			MessageBox("Error al quedar a la escucha");
		}

		m_bRunning = true;
		SetTimer(1, 300, NULL);


		SetDlgItemText(IDC_MSG, "Esperando Conexion...");
		

	}
	else if(m_bRunning) {

		misoc->m_bStop = true;
		misoc->Close();
		delete misoc;
		misoc = nullptr;

		KillTimer(1);
		m_bRunning = false;
		SetDlgItemText(IDC_MSG, "Socket Detenido...");

	}

}

void CSlavLightDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{

		m_blinkState = !m_blinkState;
		m_status.Invalidate(FALSE);
		m_izqdel.Invalidate(FALSE);
		m_derdel.Invalidate(FALSE);
		m_dertra.Invalidate(FALSE);
		m_izqtra.Invalidate(FALSE);
	}

	CDialogEx::OnTimer(nIDEvent);
}
