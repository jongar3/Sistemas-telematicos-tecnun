
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
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
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

	m_port = 3503;

	m_msg = "Socket sin crear";

	UpdateData(false);

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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

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
	// TODO: Add your control notification handler code here
	UpdateData(true);

	CMySocket* misoc;
	misoc = new CMySocket(this);
	int puerto = (int)m_port;
	bool ret = misoc->Create(puerto, SOCK_STREAM);

	if (!ret) MessageBox("Error al crear el socket");

	ret = misoc->Listen();

	if (!ret) MessageBox("Error al quedar a la escucha...");

	m_msg = "Esperando Conexion...";

	UpdateData(false);
}
