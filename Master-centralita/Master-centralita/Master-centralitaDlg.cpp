
// Master-centralitaDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Master-centralita.h"
#include "Master-centralitaDlg.h"
#include "afxdialogex.h"
#define _USE_MATH_DEFINES
#include <cmath>

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
	, m_IP_3(_T(""))
	, m_port_3(0)
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
	DDX_Text(pDX, IDC_IP_3, m_IP_3);
	DDX_Text(pDX, IDC_PORT_4, m_port_3);
	DDX_Control(pDX, IDC_STATIC_temp, m_temp_pic);
	DDX_Control(pDX, IDC_STATIC_rpm, m_rpm_pic);
}

BEGIN_MESSAGE_MAP(CMastercentralitaDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMastercentralitaDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_MESSAGE(WM_POLLING_RESULT, &CMastercentralitaDlg::OnPollingResult)
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

	// ── Default connection parameters ──────────────────────────
	m_ip_1 = _T("127.0.0.1");   m_port_1 = 3502;
	m_ipAccionamientos = _T("127.0.0.1");   m_portAccionamientos = 3503;
	m_IP_3 = _T("127.0.0.1");   m_port_3 = 3504;
	m_pollingMs = 500;

	m_bThreadRunning = false;
	m_pPollingThread = nullptr;

	UpdateData(FALSE);

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


void CMastercentralitaDlg::OnBnClickedOk() {
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	BOOL c2 = m_modbusAccionamientos.Conectar(m_ipAccionamientos, m_portAccionamientos);
	BOOL c1 = m_modbusMotor.Conectar(m_ip_1, m_port_1);
	BOOL c3 = m_modbusLuces.Conectar(m_IP_3, m_port_3);
	CTime time = CTime::GetCurrentTime();
	CString strLog;
	if (!is_running) {

		if (m_pollingMs <= 0) return;

		// Build the parameter block — thread owns and deletes it
		PollingThreadParam* p = new PollingThreadParam();
		p->ipAccion = m_ipAccionamientos;  p->portAccion = m_portAccionamientos;
		p->ipMotor = m_ip_1;              p->portMotor = m_port_1;
		p->ipLuces = m_IP_3;             p->portLuces = m_port_3;
		p->pollMs = (DWORD)m_pollingMs;
		p->hWnd = GetSafeHwnd();
		p->pRun = &m_bThreadRunning;

		m_bThreadRunning = true;
		m_pPollingThread = AfxBeginThread(PollingThreadProc, p);

		is_running = true;
		// parpadeo intermitentes ID 2
		SetTimer(2, 500, NULL);
		CTime t = CTime::GetCurrentTime();
		CString s;
		s.Format(_T("[%02d:%02d:%02d] Polling thread iniciado"), t.GetHour(), t.GetMinute(), t.GetSecond());
		m_log.AddString(s);

	}
	else {
		m_bThreadRunning = false;

		// Wait up to 3 s so the thread can finish its current poll + Sleep
		if (m_pPollingThread)
		{
			WaitForSingleObject(m_pPollingThread->m_hThread, 3000);
			m_pPollingThread = nullptr;
		}

		KillTimer(2);
		is_running = false;

		CTime t = CTime::GetCurrentTime();
		CString s;
		s.Format(_T("[%02d:%02d:%02d] Sistema detenido."), t.GetHour(), t.GetMinute(), t.GetSecond());
		m_log.AddString(s);

		// Refresh connection LEDs
		GetDlgItem(IDC_LED_ACCIONAMIENTOS6)->Invalidate();
		GetDlgItem(IDC_LED_ACCIONAMIENTOS6)->UpdateWindow();
		GetDlgItem(IDC_LED_ACCIONAMIENTOS7)->Invalidate();
		GetDlgItem(IDC_LED_ACCIONAMIENTOS7)->UpdateWindow();
		GetDlgItem(IDC_LED_LUCES)->Invalidate();
		GetDlgItem(IDC_LED_LUCES)->UpdateWindow();
	}
}

void CMastercentralitaDlg::DibujarTacometro(CStatic& control, int value, int maxValue, COLORREF needleColor)
{
	// 1. Safety check and Clamp
	if (maxValue <= 0) maxValue = 100;
	value = max(0, min(value, maxValue));

	CRect rect;
	control.GetClientRect(&rect);
	CDC* pDC = control.GetDC();
	if (!pDC) return;

	// --- Double buffer ---
	CDC memDC;
	CBitmap bmp;
	memDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&bmp);

	memDC.FillSolidRect(&rect, RGB(255, 255, 255));

	// --- Metrics ---
	int cx = rect.Width() / 2;
	int cy = rect.Height() - 15;
	int r = min(cx, cy) - 10;


	CPen blackpen(PS_SOLID, 4, RGB(80, 80, 80)); 
	CPen* pOldPen = memDC.SelectObject(&blackpen);
	// Draw arc from 180 to 0 degrees
	memDC.Arc(cx - r, cy - r, cx + r, cy + r,
		cx + r, cy,   // Start (Right)
		cx - r, cy);  // End (Left)

	// --- Needle Logic ---
	// Make sure we use (double) to avoid integer division issues
	double ratio = (double)value / (double)maxValue;
	double needleAngle = (180.0 - (ratio * 180.0)) * M_PI / 180.0;

	int tipX = cx + (int)((r - 5) * cos(needleAngle));
	int tipY = cy - (int)((r - 5) * sin(needleAngle));

	CPen needlePen(PS_SOLID, 3, needleColor);
	memDC.SelectObject(&needlePen);
	memDC.MoveTo(cx, cy);
	memDC.LineTo(tipX, tipY);

	// Center Hub
	CBrush centerBrush(needleColor);
	CBrush* pOldBrush = memDC.SelectObject(&centerBrush);
	memDC.SelectStockObject(NULL_PEN); // No border for the hub
	memDC.Ellipse(cx - 6, cy - 6, cx + 6, cy + 6);

	// --- Clean up and Swap ---
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldPen);
	memDC.SelectObject(pOldBrush);
	memDC.SelectObject(pOldBmp);
	control.ReleaseDC(pDC);
}

void CMastercentralitaDlg::ActualizarTacometros()
{
	DibujarTacometro(m_temp_pic, m_temp, 200, RGB(255, 0, 0));  // 0–200 °C, aguja roja
	DibujarTacometro(m_rpm_pic, m_rev, 7000, RGB(200, 0, 0));  // 0–7000 rpm, aguja roja
}

void CMastercentralitaDlg::OnTimer(UINT_PTR nIDEvent)
{
		
	if (nIDEvent == 1) // Si es nuestro timer de polling
	{
	//YA NO SE USA ON TIMER PARA POLLING
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
	// LED de CONEXIÓN Luces
	if (controlID == IDC_LED_LUCES) {
		if (m_modbusLuces.EstaConectado()) {
			pDC->SetBkColor(RGB(0, 255, 0));
			return m_brushGreen;
		}
		return m_brushGray;
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

//HILOOO PARA comunicar SIN BLOQUEAR

UINT CMastercentralitaDlg::PollingThreadProc(LPVOID pParam)
{
	PollingThreadParam* p = (PollingThreadParam*)pParam;

	// Local clients — created here, live here, die here
	CModbusClient clientAccion, clientMotor, clientLuces;

	clientAccion.Conectar(p->ipAccion, p->portAccion);
	clientMotor.Conectar(p->ipMotor, p->portMotor);
	clientLuces.Conectar(p->ipLuces, p->portLuces);

	HWND hWnd = p->hWnd;
	DWORD pollMs = p->pollMs;
	volatile bool* pRun = p->pRun;
	delete p;  

	while (*pRun)
	{
		PollingResult* pRes = new PollingResult();
		pRes->brakeVal = pRes->leftVal = pRes->rightVal = 0;
		pRes->tempVal = pRes->revVal = 0;

		// ── Read Accionamientos ───────────────────────────────
		pRes->accionOk =
			clientAccion.LeerRegistro(0x01, 400, pRes->brakeVal) &&
			clientAccion.LeerRegistro(0x01, 401, pRes->leftVal) &&
			clientAccion.LeerRegistro(0x01, 402, pRes->rightVal);

		// ── Read Motor ────────────────────────────────────────
		pRes->motorOk =
			clientMotor.LeerRegistro(0x01, 400, pRes->tempVal) &&
			clientMotor.LeerRegistro(0x01, 401, pRes->revVal);

		// ── Write Luces (only if we have valid accion data) ───
		pRes->lucesOk = FALSE;
		if (pRes->accionOk)
		{
			pRes->lucesOk =
				clientLuces.EscribirRegistro(0x01, 500, pRes->brakeVal) &&
				clientLuces.EscribirRegistro(0x01, 501, pRes->leftVal) &&
				clientLuces.EscribirRegistro(0x01, 503, pRes->leftVal) &&
				clientLuces.EscribirRegistro(0x01, 502, pRes->rightVal) &&
				clientLuces.EscribirRegistro(0x01, 504, pRes->rightVal);
		}

		// ── Send result to UI thread — non-blocking ───────────
		// pRes is heap-allocated; OnPollingResult deletes it.
		::PostMessage(hWnd, WM_POLLING_RESULT, 0, (LPARAM)pRes);
		Sleep(pollMs);
	}

	clientAccion.Desconectar();
	clientMotor.Desconectar();
	clientLuces.Desconectar();

	return 0;
}

// ── UI thread receives result, updates controls — never blocks ────────────────
LRESULT CMastercentralitaDlg::OnPollingResult(WPARAM /*wParam*/, LPARAM lParam)
{
	PollingResult* pRes = reinterpret_cast<PollingResult*>(lParam);
	if (!pRes) return 0;

	CTime t = CTime::GetCurrentTime();
	CString s;

	if (pRes->accionOk)
	{
		actualizarInterfazAccionamientos(pRes->brakeVal, pRes->leftVal, pRes->rightVal);
		s.Format(_T("[%02d:%02d:%02d] Brake=%d  Left=%d  Right=%d"),
			t.GetHour(), t.GetMinute(), t.GetSecond(),
			pRes->brakeVal, pRes->leftVal, pRes->rightVal);
		m_log.SetCurSel(m_log.AddString(s));
	}
	else
	{
		s.Format(_T("[%02d:%02d:%02d] Error lectura Accionamientos"),
			t.GetHour(), t.GetMinute(), t.GetSecond());
		m_log.AddString(s);
	}

	if (pRes->motorOk)
	{
		m_temp = pRes->tempVal * 3;
		m_rev = pRes->revVal * 70;
		UpdateData(FALSE);
		ActualizarTacometros();
		s.Format(_T("[%02d:%02d:%02d] Motor - Temp=%d°C  Rev=%d rpm"),
			t.GetHour(), t.GetMinute(), t.GetSecond(), m_temp, m_rev);
		m_log.SetCurSel(m_log.AddString(s));
	}
	else
	{
		s.Format(_T("[%02d:%02d:%02d] Error lectura Motor"),
			t.GetHour(), t.GetMinute(), t.GetSecond());
		m_log.AddString(s);
	}

	if (pRes->accionOk && !pRes->lucesOk)
	{
		s.Format(_T("[%02d:%02d:%02d] Error escritura Luces"),
			t.GetHour(), t.GetMinute(), t.GetSecond());
		m_log.AddString(s);
	}

	delete pRes;   
	return 0;
}
