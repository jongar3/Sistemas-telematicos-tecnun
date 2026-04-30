#include "pch.h"
#include "CMySocket.h"
#include "EsclavoAccionamientoDlg.h"
#include "ModBusSlave.h"

struct ClientContext {
    SOCKET rawSocket;
    CEsclavoAccionamientoDlg* pDlg;
};

static UINT ClientThreadProc(LPVOID pParam)
{
    ClientContext* ctx = (ClientContext*)pParam;
    CEsclavoAccionamientoDlg* pDlg = ctx->pDlg;
    SOCKET rawSock = ctx->rawSocket;  // save before delete
    delete ctx;

    // Re-attach the socket in this new thread (MFC requirement)
    CSocket client;
    client.Attach(rawSock);

    // ── Your original working logic, untouched ──────────────────
    CString cs, cs1;
    UINT port;
    client.GetSockName(cs, port);
    cs1.Format("%d conectado", port);
    pDlg->SetDlgItemText(IDC_MSG, cs1);

    unsigned char buf[300];
    unsigned char response[300];

    while (true)
    {
        int len = client.Receive(buf, sizeof(buf));

        if (len == SOCKET_ERROR)
        {
            pDlg->SetDlgItemText(IDC_MSG, "Conex. ERROR");
            break;
        }
        if (len == 0)
        {
            break;  // graceful disconnect
        }

        int resLen = CModbusSlave::BuildResponse(
            buf, len, response,
            (pDlg->m_fren != 0),
            (pDlg->m_izq  != 0),
            (pDlg->m_der  != 0),
            1,    // Slave ID
            400   // regBase
        );

        if (resLen > 0)
            client.Send(response, resLen);
    }

    client.Close();
    pDlg->SetDlgItemText(IDC_MSG, "Conex. Terminada");
    // ── End of original logic ───────────────────────────────────

    return 0;
}

void CMySocket::OnAccept(int err)
{
    if (err != 0) return;

    CSocket tempClient;
    if (!Accept(tempClient))
    {
        pDlg->SetDlgItemText(IDC_MSG, "Accept() failed");
        return;
    }

    // Detach the handle and pass it to the worker thread
    ClientContext* ctx = new ClientContext();
    ctx->rawSocket = tempClient.Detach();
    ctx->pDlg      = pDlg;

    AfxBeginThread(ClientThreadProc, ctx);
    // OnAccept returns immediately — UI stays responsive
}