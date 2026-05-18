
// EsclavoAccionamientoDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "EsclavoAccionamiento.h"
#include "EsclavoAccionamientoDlg.h"
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


// CEsclavoAccionamientoDlg dialog

CEsclavoAccionamientoDlg::CEsclavoAccionamientoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ESCLAVOACCIONAMIENTO_DIALOG, pParent)
	, m_port(0)
	, m_msg(_T(""))
	, m_fren(FALSE)
	, m_izq(FALSE)
	, m_der(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEsclavoAccionamientoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_port, m_port);
	DDX_Text(pDX, IDC_MSG, m_msg);
	DDX_Check(pDX, IDC_FREN, m_fren);
	DDX_Check(pDX, IDC_IZQ, m_izq);
	DDX_Check(pDX, IDC_DER, m_der);
}

BEGIN_MESSAGE_MAP(CEsclavoAccionamientoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CEsclavoAccionamientoDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_FREN, &CEsclavoAccionamientoDlg::OnBnClickedFreno)
	ON_BN_CLICKED(IDC_IZQ, &CEsclavoAccionamientoDlg::OnBnClickedIzq)
	ON_BN_CLICKED(IDC_DER, &CEsclavoAccionamientoDlg::OnBnClickedDer)
END_MESSAGE_MAP()


// CEsclavoAccionamientoDlg message handlers

BOOL CEsclavoAccionamientoDlg::OnInitDialog()
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
	m_bRunning = false;

	UpdateData(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEsclavoAccionamientoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEsclavoAccionamientoDlg::OnPaint()
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
HCURSOR CEsclavoAccionamientoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEsclavoAccionamientoDlg::OnBnClickedStart()
{

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

		SetDlgItemText(IDC_MSG, "Esperando Conexion...");


	}
	else if (m_bRunning) {
		
		misoc->m_bStop = true;
		misoc->Close();
		delete misoc;
		misoc = nullptr;

		m_bRunning = false;
		SetDlgItemText(IDC_MSG, "Socket Detenido...");
	}
	

}



void CEsclavoAccionamientoDlg::OnBnClickedFreno()
{
	UpdateData(true);
	// TODO: Add your control notification handler code here
}

void CEsclavoAccionamientoDlg::OnBnClickedIzq()
{
	UpdateData(true);
	// TODO: Add your control notification handler code here
}



void CEsclavoAccionamientoDlg::OnBnClickedDer()
{
	UpdateData(true);
	// TODO: Add your control notification handler code here
}
