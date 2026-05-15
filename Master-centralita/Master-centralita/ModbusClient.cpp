#include "pch.h"
#include "ModbusClient.h"

// Transaction ID global, se incrementa en cada peticion
WORD CModbusClient::m_transactionId = 0;


// ------------------------------------------------------------
// BuildRequest
// Construye una trama Modbus TCP completa (MBAP Header + PDU)
//
// Estructura MBAP Header (6 bytes):
//   [0-1] Transaction Identifier  (2B)
//   [2-3] Protocol Identifier = 0 (2B)
//   [4-5] Length (bytes que siguen)(2B)
//   [6]   Unit Identifier          (1B)
//
// PDU (Function Code + Data):
//   FC03 Read Holding Registers:
//     [7]   Function Code = 0x03
//     [8-9] Starting Address
//     [10-11] Quantity of Registers = 1
//
//   FC06 Write Single Register:
//     [7]   Function Code = 0x06
//     [8-9] Register Address
//     [10-11] Register Value
// ------------------------------------------------------------


CModbusClient::CModbusClient() : m_bConectado(FALSE)
{
}

CModbusClient::~CModbusClient()
{
    Desconectar();
}

// Implementación de conexión siguiendo el patrón de TCP_IP) 
BOOL CModbusClient::Conectar(CString ip, int puerto)
{
    if (m_bConectado) Desconectar();

    // 1. Crear el socket 
    if (!m_socket.Create()) {
        //aquí se podría usar MessageBox("Error al crear socket") 
        return FALSE;
    }

    // 2. Conectar al servidor 
    if (!m_socket.Connect(ip, puerto)) {
        m_socket.Close();
        return FALSE;
    }

    m_bConectado = TRUE;
    return TRUE;
}

void CModbusClient::Desconectar()
{
    if (m_bConectado) {
        m_socket.Close();
        m_bConectado = FALSE;
    }
}

BOOL CModbusClient::LeerRegistro(BYTE unitId, WORD direccion, short& valor)
{
    if (!m_bConectado) return FALSE;

    BYTE sendBuf[12];
    BYTE recvBuf[64];
    WORD transId = ++m_transactionId;

    // Construir trama para FC03 (Read Holding Registers)
    int sendLen = ConstruirPeticion(sendBuf, transId, unitId, 0x03, direccion, 1);

    // Enviar y recibir usando el socket abierto 
    if (m_socket.Send(sendBuf, sendLen) == SOCKET_ERROR) return FALSE;

    int recvLen = m_socket.Receive(recvBuf, sizeof(recvBuf));
    if (recvLen < 11 || (recvBuf[7] & 0x80)) return FALSE; // Error o trama incompleta

    // Extraer valor (Big-endian)
    valor = (short)((recvBuf[9] << 8) | recvBuf[10]);
    return TRUE;
}

BOOL CModbusClient::EscribirRegistro(BYTE unitId, WORD direccion, short valor)
{
    if (!m_bConectado) return FALSE;

    BYTE sendBuf[12];
    BYTE recvBuf[64];
    WORD transId = ++m_transactionId;

    // FC06 (Write Single Register)
    int sendLen = ConstruirPeticion(sendBuf, transId, unitId, 0x06, direccion, (WORD)valor);

    if (m_socket.Send(sendBuf, sendLen) == SOCKET_ERROR) return FALSE;

    int recvLen = m_socket.Receive(recvBuf, sizeof(recvBuf));
    return (recvLen >= 9 && !(recvBuf[7] & 0x80));
}

int CModbusClient::ConstruirPeticion(BYTE* buf, WORD transId, BYTE unitId, BYTE funcCode, WORD addr, WORD data)
{
    // MBAP Header (6 bytes)
    buf[0] = (BYTE)(transId >> 8);
    buf[1] = (BYTE)(transId & 0xFF);
    buf[2] = 0x00; // Protocol ID
    buf[3] = 0x00;
    buf[4] = 0x00; // Longitud (UnitID + PDU)
    buf[5] = 0x06;
    buf[6] = unitId;

    // PDU
    buf[7] = funcCode;
    buf[8] = (BYTE)(addr >> 8);
    buf[9] = (BYTE)(addr & 0xFF);
    buf[10] = (BYTE)(data >> 8);
    buf[11] = (BYTE)(data & 0xFF);

    return 12;
}