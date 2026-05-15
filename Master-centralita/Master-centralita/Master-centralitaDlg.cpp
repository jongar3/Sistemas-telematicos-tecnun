
// Master-centralitaDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Master-centralita.h"
#include "Master-centralitaDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About
bool is_running = false;


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
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMastercentralitaDlg dialog



CMastercentralitaDlg::CMastercentralitaDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MASTERCENTRALITA_DIALOG, pParent)
	, m_ipAccionamientos(_T(""))
	, m_portAccionamientos(0)
	, m_pollingMs(0)
	, m_port_1(0)
	, m_ip_1(_T(""))
	, m_temp(0)
	, m_rev(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_statusBrake = m_statusLeft = m_statusRight = FALSE;
	m_brushRed.CreateSolidBrush(RGB(255, 0, 0));      // Rojo brillante
	m_brushYellow.CreateSolidBrush(RGB(255, 255, 0)); // Amarillo/Naranja
	m_brushGray.CreateSolidBrush(RGB(160, 160, 160)); // Gris oscuro
	m_brushGreen.CreateSolidBrush(RGB(0, 255, 0));
	m_blinkState = false;
	m_blinkTimer = 0;

}

void CMastercentralitaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IP_2, m_ipAccionamientos);
	DDX_Text(pDX, IDC_PORT_2, m_portAccionamientos);
	DDX_Control(pDX, IDC_LED_ACCIONAMIENTOS, m_led2);
	DDX_Control(pDX, IDC_LED_ACCIONAMIENTOS3, m_led2i);
	DDX_Control(pDX, IDC_LED_ACCIONAMIENTOS2, m_led2_d);
	DDX_Text(pDX, IDC_POLLING, m_pollingMs);
	DDX_Control(pDX, IDC_LOG, m_log);
	DDX_Text(pDX, IDC_PORT_1, m_port_1);
	DDX_Text(pDX, IDC_IP_1, m_ip_1);
	DDX_Text(pDX, IDC_TEMP, m_temp);
	DDX_Text(pDX, IDC_REV, m_rev);
}

BEGIN_MESSAGE_MAP(CMastercentralitaDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMastercentralitaDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMastercentralitaDlg message handlers

BOOL CMastercentralitaDlg::OnInitDialog()
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//ACTUALIZAR LOS LEDS DE ACCIONAMIENTOS


void CMastercentralitaDlg::actualizarInterfazAccionamientos(short brake, short left, short right)
{
	// Guardamos los estados que vienen de Modbus
	m_statusBrake = (brake > 0);
	m_statusLeft = (left > 0);
	m_statusRight = (right > 0);

	// Forzamos el repintado de los controles específicos para que salte OnCtlColor
	GetDlgItem(IDC_LED_ACCIONAMIENTOS)->Invalidate();
	GetDlgItem(IDC_LED_ACCIONAMIENTOS)->UpdateWindow();

	GetDlgItem(IDC_LED_ACCIONAMIENTOS2)->Invalidate();
	GetDlgItem(IDC_LED_ACCIONAMIENTOS2)->UpdateWindow();

	GetDlgItem(IDC_LED_ACCIONAMIENTOS3)->Invalidate();
	GetDlgItem(IDC_LED_ACCIONAMIENTOS3)->UpdateWindow();
}








void CMastercentralitaDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMastercentralitaDlg::OnPaint()
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
HCURSOR CMastercentralitaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMastercentralitaDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	BOOL c2 = m_modbusAccionamientos.Conectar(m_ipAccionamientos, m_portAccionamientos);
	BOOL c1 = m_modbusMotor.Conectar(m_ip_1, m_port_1);
	CTime time = CTime::GetCurrentTime();
	CString strLog;
	if (!is_running) {
		if (m_pollingMs > 0) {

			is_running = true;
			//escribimos los logs
			if (c2) {
				strLog.Format(_T("[%02d:%02d:%02d] Accionamientos conectado en %s:%d"), time.GetHour(), time.GetMinute(), time.GetSecond(),
				m_ipAccionamientos, m_portAccionamientos);
			}
			else {
				strLog.Format(_T("[%02d:%02d:%02d] ERROR: No se pudo conectar a Accionamientos"),
					time.GetHour(), time.GetMinute(), time.GetSecond());
			}
			m_log.AddString(strLog);
			
			if (c1) {
				strLog.Format(_T("[%02d:%02d:%02d] Motor conectado en %s:%d"),
					time.GetHour(), time.GetMinute(), time.GetSecond(),
					m_ip_1, m_port_1);
			}
			else {
				strLog.Format(_T("[%02d:%02d:%02d] ERROR: No se pudo conectar a Motor"),
					time.GetHour(), time.GetMinute(), time.GetSecond());
			}
			m_log.AddString(strLog);
			
			
			
			
			SetTimer(1, m_pollingMs, NULL); // Iniciamos el timer con ID 1
			SetTimer(2, 500, NULL);           // parpadeo intermitentes ID 2
		}
	}
	else {
		KillTimer(1);
		KillTimer(2);
	}
	GetDlgItem(IDC_LED_ACCIONAMIENTOS6)->Invalidate();
	GetDlgItem(IDC_LED_ACCIONAMIENTOS6)->UpdateWindow();
	GetDlgItem(IDC_LED_ACCIONAMIENTOS7)->Invalidate();
	GetDlgItem(IDC_LED_ACCIONAMIENTOS7)->UpdateWindow();
}


void CMastercentralitaDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) // Si es nuestro timer de polling
	{
		
		//---------------ACCIONAMIENTOS-------------
		short brakeVal = 0, leftVal = 0, rightVal = 0;

		// Leemos los 3 registros definidos en el PDF para Accionamientos
		// Registro 400: Freno
		// Registro 401: Intermitente Izquierdo
		// Registro 402: Intermitente Derecho
		BOOL res1 = m_modbusAccionamientos.LeerRegistro(0x01, 400, brakeVal);
		BOOL res2 = m_modbusAccionamientos.LeerRegistro(0x01, 401, leftVal);
		BOOL res3 = m_modbusAccionamientos.LeerRegistro(0x01, 402, rightVal);
		//-------------MOTOR--------------------
		short tempVal = 0, revVal = 0;
		// Registro 400: Temperatura (0-100, escalar x3)
		// Registro 401: Revoluciones (0-100, escalar x70)
		BOOL resT = m_modbusMotor.LeerRegistro(0x01, 400, tempVal);
		BOOL resR = m_modbusMotor.LeerRegistro(0x01, 401, revVal);

		if (res1 && res2 && res3)
		{
			// Lógica para actualizar los LEDs en la interfaz
			// Si el valor es 1 (On), mostramos color o activamos control
			actualizarInterfazAccionamientos(brakeVal, leftVal, rightVal);
			CTime time = CTime::GetCurrentTime();
			CString strLog;
			strLog.Format(_T("[%02d:%02d:%02d] Brake=%d  Left=%d  Right=%d"), time.GetHour(), time.GetMinute(), time.GetSecond(), brakeVal, leftVal, rightVal);
			int idx = m_log.AddString(strLog);
			m_log.SetCurSel(idx);

		}else{
			// Log de error si falla la comunicación en el timer
			CTime time = CTime::GetCurrentTime();
			CString strLog;
			strLog.Format(_T("[%02d:%02d:%02d] Error de lectura en Accionamientos"), time.GetHour(), time.GetMinute(), time.GetSecond());
			int idx = m_log.AddString(strLog);
			m_log.SetCurSel(idx);
		}

		if (resT && resR)
		{
			m_temp = tempVal * 3;
			m_rev = revVal * 70;
			UpdateData(FALSE);  // Vuelca m_temp y m_rev a los controles IDC_TEMP / IDC_REV

			CTime time2 = CTime::GetCurrentTime();
			CString strLog2;
			strLog2.Format(_T("[%02d:%02d:%02d] Motor - Temp=%d°C  Rev=%d rpm"),
				time2.GetHour(), time2.GetMinute(), time2.GetSecond(),
				m_temp, m_rev);
			int idx2 = m_log.AddString(strLog2);
			m_log.SetCurSel(idx2);
		}
		else
		{
			CTime time2 = CTime::GetCurrentTime();
			CString strLog2;
			strLog2.Format(_T("[%02d:%02d:%02d] Error de lectura en Motor"),
				time2.GetHour(), time2.GetMinute(), time2.GetSecond());
			int idx2 = m_log.AddString(strLog2);
			m_log.SetCurSel(idx2);
		}


	}
	else if (nIDEvent == 2) // Timer de parpadeo
	{
		m_blinkState = !m_blinkState;

		GetDlgItem(IDC_LED_ACCIONAMIENTOS2)->Invalidate();
		GetDlgItem(IDC_LED_ACCIONAMIENTOS2)->UpdateWindow();
		GetDlgItem(IDC_LED_ACCIONAMIENTOS3)->Invalidate();
		GetDlgItem(IDC_LED_ACCIONAMIENTOS3)->UpdateWindow();
	}

	CDialogEx::OnTimer(nIDEvent);
}

















//Mensaje de color ----- ME ENCANTA MFC :)
HBRUSH CMastercentralitaDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	int controlID = pWnd->GetDlgCtrlID();

	
	
	//-----ACCIONAMIENTOS-----

	// LED de FRENO
	if (controlID == IDC_LED_ACCIONAMIENTOS) {
		if (m_statusBrake) {
			pDC->SetBkColor(RGB(255, 0, 0));
			return m_brushRed;
		}
		return m_brushGray;
	}

	// LED de INTERMITENTE IZQ
	if (controlID == IDC_LED_ACCIONAMIENTOS3) {
		if (m_statusLeft && m_blinkState) {  
			pDC->SetBkColor(RGB(255, 255, 0));
			return m_brushYellow;
		}
		return m_brushGray;
	}

	// LED de INTERMITENTE DER
	if (controlID == IDC_LED_ACCIONAMIENTOS2) {
		if (m_statusRight && m_blinkState) { 
			pDC->SetBkColor(RGB(255, 255, 0));
			return m_brushYellow;
		}
		return m_brushGray;
	}
	// LED de CONEXIÓN
	if (controlID == IDC_LED_ACCIONAMIENTOS6) {
		if (m_modbusAccionamientos.EstaConectado()) {
			pDC->SetBkColor(RGB(0, 255, 0));
			return m_brushGreen;
		}
		return m_brushGray;
	}
	// LED de CONEXIÓN Motor
	if (controlID == IDC_LED_ACCIONAMIENTOS7) {
		if (m_modbusMotor.EstaConectado()) {
			pDC->SetBkColor(RGB(0, 255, 0));
			return m_brushGreen;
		}
		return m_brushGray;
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
