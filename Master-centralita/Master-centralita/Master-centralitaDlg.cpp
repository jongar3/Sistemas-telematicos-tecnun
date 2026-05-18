
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
volatile bool g_brake = false;
volatile bool g_left = false;
volatile bool g_right = false;
volatile bool g_dataReady = false;
volatile bool g_connAccion = false;
volatile bool g_connMotor = false;
volatile bool g_connLuces = false;
CMastercentralitaDlg* g_pMainDlg = nullptr;

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
	ON_MESSAGE(WM_CONN_CHANGED, &CMastercentralitaDlg::OnConnChanged)
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
	m_pReadThread = nullptr;
	m_pWriteThread = nullptr;

	UpdateData(FALSE);

	// ── Web server en puerto 8082 ──────────────────────
	if (m_webSocket.Create(8082, SOCK_STREAM))
	{
		m_webSocket.Listen();
		m_log.AddString(_T("Web server escuchando en http://127.0.0.1:8082"));
	}
	else
	{
		m_log.AddString(_T("ERROR: no se pudo arrancar el web server"));
	}

	g_pMainDlg = this;
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

	CTime time = CTime::GetCurrentTime();
	CString strLog;
	if (!is_running) {

		if (m_pollingMs <= 0) return;

		// Hilo de LECTURA
		ReadThreadParam* pR = new ReadThreadParam();
		pR->ipAccion = m_ipAccionamientos;  pR->portAccion = m_portAccionamientos;
		pR->ipMotor = m_ip_1;             pR->portMotor = m_port_1;
		pR->pollMs = (DWORD)m_pollingMs;
		pR->hWnd = GetSafeHwnd();
		pR->pRun = &m_bThreadRunning;

		// Hilo de ESCRITURA
		WriteThreadParam* pW = new WriteThreadParam();
		pW->ipLuces = m_IP_3;  pW->portLuces = m_port_3;
		pW->pollMs = (DWORD)m_pollingMs;
		pW->hWnd = GetSafeHwnd();
		pW->pRun = &m_bThreadRunning;

		g_connAccion = false;
		g_connMotor = false;
		g_connLuces = false;
		g_dataReady = false;  // reset por si se relanza
		m_bThreadRunning = true;

		m_pReadThread = AfxBeginThread(ReadThreadProc, pR);
		m_pWriteThread = AfxBeginThread(WriteThreadProc, pW);

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
		if (m_pReadThread)
		{
			WaitForSingleObject(m_pReadThread->m_hThread, 3000);
			m_pReadThread = nullptr;
		}
		if (m_pWriteThread)
		{
			WaitForSingleObject(m_pWriteThread->m_hThread, 3000);
			m_pWriteThread = nullptr;
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
	DibujarTacometro(m_temp_pic, m_temp, 300, RGB(255, 0, 0));  // 0–300 °C, aguja roja
	DibujarTacometro(m_rpm_pic, m_rev, 7000, RGB(255, 0, 0));  // 0–7000 rpm, aguja roja
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
		if (g_connAccion) {
			pDC->SetBkColor(RGB(0, 255, 0));
			return m_brushGreen;
		}
		return m_brushGray;
	}
	// LED de CONEXIÓN Motor
	if (controlID == IDC_LED_ACCIONAMIENTOS7) {
		if (g_connMotor) {
			pDC->SetBkColor(RGB(0, 255, 0));
			return m_brushGreen;
		}
		return m_brushGray;
	}
	// LED de CONEXIÓN Luces
	if (controlID == IDC_LED_LUCES) {
		if (g_connLuces) {
			pDC->SetBkColor(RGB(0, 255, 0));
			return m_brushGreen;
		}
		return m_brushGray;
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
LRESULT CMastercentralitaDlg::OnConnChanged(WPARAM, LPARAM)
{
	GetDlgItem(IDC_LED_ACCIONAMIENTOS6)->Invalidate();
	GetDlgItem(IDC_LED_ACCIONAMIENTOS6)->UpdateWindow();
	GetDlgItem(IDC_LED_ACCIONAMIENTOS7)->Invalidate();
	GetDlgItem(IDC_LED_ACCIONAMIENTOS7)->UpdateWindow();
	GetDlgItem(IDC_LED_LUCES)->Invalidate();
	GetDlgItem(IDC_LED_LUCES)->UpdateWindow();
	return 0;
}

LRESULT CMastercentralitaDlg::OnPollingResult(WPARAM wParam, LPARAM lParam)
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
		s.Format(_T("[%02d:%02d:%02d] Motor - Temp=%d C  Rev=%d rpm"),
			t.GetHour(), t.GetMinute(), t.GetSecond(), m_temp, m_rev);
		m_log.SetCurSel(m_log.AddString(s));
	}
	else
	{
		s.Format(_T("[%02d:%02d:%02d] Error lectura Motor"),
			t.GetHour(), t.GetMinute(), t.GetSecond());
		m_log.AddString(s);
	}

	delete pRes;
	return 0;
}


//HILOOO PARA comunicar SIN BLOQUEAR uno escritura otra lectura y el "main" para dibujar

UINT CMastercentralitaDlg::ReadThreadProc(LPVOID pParam)
{
	ReadThreadParam* p = (ReadThreadParam*)pParam;
	CModbusClient clientAccion, clientMotor;

	// Extraemos TODOS los parámetros antes de liberar la estructura de memoria
	HWND hWnd = p->hWnd;
	DWORD pollMs = p->pollMs;
	volatile bool* pRun = p->pRun;
	CString ipAccion = p->ipAccion;   int portAccion = p->portAccion;
	CString ipMotor = p->ipMotor;     int portMotor = p->portMotor;
	delete p;

	while (*pRun)
	{
		if (!clientAccion.EstaConectado())
		{
			clientAccion.Conectar(ipAccion, portAccion);
		}
		if (!clientMotor.EstaConectado())
		{
			clientMotor.Conectar(ipMotor, portMotor);
		}

		if (g_connAccion != clientAccion.EstaConectado() || g_connMotor != clientMotor.EstaConectado())
		{
			g_connAccion = clientAccion.EstaConectado();
			g_connMotor = clientMotor.EstaConectado();
			::PostMessage(hWnd, WM_CONN_CHANGED, 0, 0); // Notificar a la UI
		}

		PollingResult* pRes = new PollingResult();
		short brakeVal = 0, leftVal = 0, rightVal = 0;
		short tempVal = 0, revVal = 0;

		
		if (clientAccion.EstaConectado())
		{
			pRes->accionOk =
				clientAccion.LeerRegistro(0x01, 400, brakeVal) &&
				clientAccion.LeerRegistro(0x01, 401, leftVal) &&
				clientAccion.LeerRegistro(0x01, 402, rightVal);

			if (!pRes->accionOk)
			{
				// Si falla el Modbus, forzamos cierre para que en la prox. vuelta intente reconectar
				clientAccion.Desconectar();
			}
		}
		else
		{
			pRes->accionOk = FALSE;
		}

		if (clientMotor.EstaConectado())
		{
			pRes->motorOk =
				clientMotor.LeerRegistro(0x01, 400, tempVal) &&
				clientMotor.LeerRegistro(0x01, 401, revVal);

			if (!pRes->motorOk)
			{
				clientMotor.Desconectar();
			}
		}
		else
		{
			pRes->motorOk = FALSE;
		}

		if (pRes->accionOk)
		{
			g_brake = (brakeVal != 0);
			g_left = (leftVal != 0);
			g_right = (rightVal != 0);
			g_dataReady = true;
		}

		pRes->brakeVal = brakeVal;
		pRes->leftVal = leftVal;
		pRes->rightVal = rightVal;
		pRes->tempVal = tempVal;
		pRes->revVal = revVal;
		pRes->lucesOk = TRUE;

		::PostMessage(hWnd, WM_POLLING_RESULT, 0, (LPARAM)pRes);

		Sleep(pollMs);
	}
	
	clientAccion.Desconectar();
	clientMotor.Desconectar();
	g_connAccion = false;
	g_connMotor = false;
	::PostMessage(hWnd, WM_CONN_CHANGED, 0, 0);
	return 0;
}

UINT CMastercentralitaDlg::WriteThreadProc(LPVOID pParam)
{
	WriteThreadParam* p = (WriteThreadParam*)pParam;
	CModbusClient clientLuces;

	HWND hWnd = p->hWnd;
	DWORD pollMs = p->pollMs;
	volatile bool* pRun = p->pRun;
	CString ipLuces = p->ipLuces;     int portLuces = p->portLuces;
	delete p;

	while (*pRun)
	{

		if (!clientLuces.EstaConectado())
		{
			clientLuces.Conectar(ipLuces, portLuces);
		}

		if (g_connLuces != clientLuces.EstaConectado())
		{
			g_connLuces = clientLuces.EstaConectado();
			::PostMessage(hWnd, WM_CONN_CHANGED, 0, 0);
		}

		if (clientLuces.EstaConectado() && g_dataReady)
		{
			short brake = (short)g_brake;
			short left = (short)g_left;
			short right = (short)g_right;

			BOOL writeOk =
				clientLuces.EscribirRegistro(0x01, 500, brake) &&
				clientLuces.EscribirRegistro(0x01, 501, left) &&
				clientLuces.EscribirRegistro(0x01, 503, left) &&
				clientLuces.EscribirRegistro(0x01, 502, right) &&
				clientLuces.EscribirRegistro(0x01, 504, right);

			if (!writeOk)
			{
				
				clientLuces.Desconectar();
			}
		}

		Sleep(pollMs);
	}

	clientLuces.Desconectar();
	g_connLuces = false;
	::PostMessage(hWnd, WM_CONN_CHANGED, 0, 0);
	return 0;
}


//--------WEB SERVER DESDE UN NAVEGADOR (http://localhost:8080)------------------
void CWebSocket::OnAccept(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		CMastercentralitaDlg* pDlg = (CMastercentralitaDlg*)AfxGetMainWnd();
		pDlg->OnWebAccept();
	}
	CAsyncSocket::OnAccept(nErrorCode);
}

CString CMastercentralitaDlg::GetWebPage()
{
	// Auto-refresh cada 1 segundo
	CString page;
	page = _T("<html><head><title>Centralita</title>");
	page += _T("<meta http-equiv='refresh' content='1'>");
	page += _T("<style>");
	page += _T("body { font-family: Arial; background:#1a1a2e; color:#eee; text-align:center; }");
	page += _T("h1 { color:#00d4ff; }");
	page += _T(".card { display:inline-block; background:#16213e; border-radius:12px;");
	page += _T("        padding:20px 40px; margin:10px; min-width:150px; }");
	page += _T(".label { font-size:12px; color:#aaa; text-transform:uppercase; }");
	page += _T(".value { font-size:36px; font-weight:bold; margin-top:5px; }");
	page += _T(".on  { color:#00ff88; }");
	page += _T(".off { color:#555; }");
	page += _T(".red { color:#ff4444; }");
	page += _T(".blue{ color:#00d4ff; }");
	page += _T("</style></head><body>");
	page += _T("<h1>Centralita Modbus</h1>");

	// ── Accionamientos ────────────────────────────────────────
	page += _T("<h2>Accionamientos</h2>");

	// Freno
	CString val;
	val.Format(_T("<div class='card'><div class='label'>Freno</div>")
		_T("<div class='value %s'>%s</div></div>"),
		g_brake ? _T("red") : _T("off"),
		g_brake ? _T("ON") : _T("OFF"));
	page += val;

	// Intermitente izquierdo
	val.Format(_T("<div class='card'><div class='label'>Interm. Izq</div>")
		_T("<div class='value %s'>%s</div></div>"),
		g_left ? _T("on") : _T("off"),
		g_left ? _T("ON") : _T("OFF"));
	page += val;

	// Intermitente derecho
	val.Format(_T("<div class='card'><div class='label'>Interm. Der</div>")
		_T("<div class='value %s'>%s</div></div>"),
		g_right ? _T("on") : _T("off"),
		g_right ? _T("ON") : _T("OFF"));
	page += val;

	// ── Motor ─────────────────────────────────────────────────
	page += _T("<h2>Motor</h2>");

	val.Format(_T("<div class='card'><div class='label'>Temperatura</div>")
		_T("<div class='value red'>%d &deg;C</div></div>"),
		m_temp);
	page += val;

	val.Format(_T("<div class='card'><div class='label'>Revoluciones</div>")
		_T("<div class='value blue'>%d rpm</div></div>"),
		m_rev);
	page += val;

	// ── Estado conexiones ─────────────────────────────────────
	page += _T("<h2>Conexiones</h2>");

	val.Format(_T("<div class='card'><div class='label'>Accionamientos</div>")
		_T("<div class='value %s'>%s</div></div>"),
		g_connAccion ? _T("on") : _T("off"),
		g_connAccion ? _T("OK") : _T("--"));
	page += val;

	val.Format(_T("<div class='card'><div class='label'>Motor</div>")
		_T("<div class='value %s'>%s</div></div>"),
		g_connMotor ? _T("on") : _T("off"),
		g_connMotor ? _T("OK") : _T("--"));
	page += val;

	val.Format(_T("<div class='card'><div class='label'>Luces</div>")
		_T("<div class='value %s'>%s</div></div>"),
		g_connLuces ? _T("on") : _T("off"),
		g_connLuces ? _T("OK") : _T("--"));
	page += val;

	page += _T("</body></html>");
	return page;
}


void CMastercentralitaDlg::OnWebAccept()
{
	CAsyncSocket clientSocket;

	if (!m_webSocket.Accept(clientSocket))
		return;

	SOCKET s = clientSocket.Detach();

	AfxBeginThread(ClientThreadProc, (LPVOID)s);
}

UINT CMastercentralitaDlg::ClientThreadProc(LPVOID pParam)
{
	SOCKET s = (SOCKET)pParam;

	char buf[4096] = {};
	int len = recv(s, buf, sizeof(buf) - 1, 0);

	if (len <= 0)
	{
		closesocket(s);
		return 0;
	}

	buf[len] = 0;
	CString request(buf);

	if (request.Find(_T("GET /favicon.ico")) >= 0)
	{
		const char* response =
			"HTTP/1.1 404 Not Found\r\n"
			"Content-Length: 0\r\n"
			"Connection: close\r\n\r\n";

		send(s, response, (int)strlen(response), 0);
		closesocket(s);
		return 0;
	}
	
	// Para USAR el puntero de la UI desde un hilo. Lo hacemos GLOBAL
	if (g_pMainDlg == nullptr)
	{
		closesocket(s);
		return 0;
	}

	// Llamamos directamente a GetWebPage a través del puntero global "seguro"
	CString page = g_pMainDlg->GetWebPage();

	
	CStringA pageA = CW2A(page, CP_UTF8);
	CStringA headerA;

	headerA.Format(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Content-Length: %d\r\n"
		"Connection: close\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n",
		pageA.GetLength());

	send(s, headerA.GetString(), headerA.GetLength(), 0);
	send(s, pageA.GetString(), pageA.GetLength(), 0);

	
	shutdown(s, SD_BOTH);
	closesocket(s);

	return 0;
}