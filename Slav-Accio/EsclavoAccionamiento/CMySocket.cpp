#include "pch.h"
#include "CMySocket.h"
#include "EsclavoACcionamientoDlg.h"
#include "ModBusSlave.h"

void CMySocket::OnAccept(int err)
{
    if (err != 0) return;

    CString cs, cs1;
    UINT port;
    CSocket client;

    if (!Accept(client))
    {
        pDlg->SetDlgItemText(IDC_MSG, "Accept() failed");
        return;
    }

    client.GetSockName(cs, port);
    cs1.Format("%d conectado", port);
    pDlg->SetDlgItemText(IDC_MSG, cs1);

    unsigned char buf[300];
    unsigned char response[300];

    while (true)
    {
        int len = client.Receive(buf, sizeof(buf));

        // Fixed: handle error and closed connection separately, with proper messages
        if (len == SOCKET_ERROR)
        {
            pDlg->SetDlgItemText(IDC_MSG, "Conex. ERROR");
            break;
        }
        if (len == 0)
        {
            // Graceful disconnect by master
            break;
        }

        // Pass regBase=0 if Modbus Doctor sends 0-based addresses,
        // or regBase=400 if it sends the raw holding register number
        int resLen = CModbusSlave::BuildResponse(
            buf, len, response,
            (pDlg->m_fren != 0),
            (pDlg->m_izq  != 0),
            (pDlg->m_der  != 0),
            1,    // Slave ID
            400     // <-- Try 0 first; change to 400 if still not working
        );

        if (resLen > 0)
        {
            client.Send(response, resLen);
        }
    }

    client.Close();
    pDlg->SetDlgItemText(IDC_MSG, "Conex. Terminada");
}