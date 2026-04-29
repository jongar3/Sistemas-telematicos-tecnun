#include "pch.h"
#include "CMySocket.h"
#include "EsclavoACcionamientoDlg.h"
#include "ModBusSlave.h"

void CMySocket::OnAccept(int err)
{
	CString cs, cs1; UINT port;
	static bool col = true;
	CSocket client;
	Accept(client);
	client.GetSockName(cs, port);
	cs1.Format("%d conectado", port);
	cs += cs1;
	pDlg->SetDlgItemText(IDC_MSG, cs1);

    // Modbus TCP max packet is 260 bytes
    unsigned char buf[300];
    unsigned char response[300];

    while (true) {
        // Receive the raw binary Modbus request
        int len = client.Receive(buf, 300);

        if (len <= 0) break; pDlg->SetDlgItemText(IDC_MSG, "Conex. ERROR"); ; // Connection closed or error

        // Use the helper class to parse and generate response
        // We pass the booleans and a Slave ID (here set to 1)
        int resLen = CModbusSlave::BuildResponse(
            buf,
            len,
            response,
            (pDlg->m_fren != 0),
            (pDlg->m_izq != 0),
            (pDlg->m_der != 0),
            1
        );

        if (resLen > 0) {
            client.Send(response, resLen);
        }
    }


	client.Close();
	pDlg->SetDlgItemText(IDC_MSG, "Conex. Terminada");
}
