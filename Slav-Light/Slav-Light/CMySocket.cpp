#include "pch.h"
#include "CMySocket.h"
#include "Slav-LightDlg.h"
#include "ModBusSlave.h"

struct ClientContext {
    SOCKET rawSocket;
    CSlavLightDlg* pDlg;
    volatile bool* pStop;
};

static UINT ClientThreadProc(LPVOID pParam)
{
    //Socket worker thread
    ClientContext* ctx = (ClientContext*)pParam;
    CSlavLightDlg* pDlg = ctx->pDlg;
    SOCKET rawSock = ctx->rawSocket;  // save before delete
    volatile bool* pStop = ctx->pStop;
    delete ctx;


    CSocket client;
    client.Attach(rawSock);


    CString cs, cs1;
    UINT port;
    client.GetSockName(cs, port);
    cs1.Format("%d conectado", port);
    pDlg->SetDlgItemText(IDC_MSG, cs1);

    unsigned char buf[300];
    unsigned char response[300];

    while (!(*pStop))
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
            pDlg->m_v500,
            pDlg->m_v501,
            pDlg->m_v502,
            pDlg->m_v503,
            pDlg->m_v504,
            1    // Slave ID
        );


        if (resLen > 0)
        {
            pDlg->PostMessage(WM_UPDATE_MODBUS_DATA, 0, 0);

            client.Send(response, resLen);
        }
    }

    client.Close();
    pDlg->SetDlgItemText(IDC_MSG, "Conex. Terminada");

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
    ctx->pDlg = pDlg;
    ctx->pStop = &m_bStop;

    AfxBeginThread(ClientThreadProc, ctx);
}