#pragma once
#include <afxsock.h>

// CModbusClient - Basado en la lógica de conexión de TCP_IP
class CModbusClient
{
public:
    CModbusClient();
    virtual ~CModbusClient();

    // Métodos de conexión  
    BOOL Conectar(CString ip, int puerto);
    void Desconectar();

    // Operaciones Modbus (FC03 y FC06)
    BOOL LeerRegistro(BYTE unitId, WORD direccion, short& valor);
    BOOL EscribirRegistro(BYTE unitId, WORD direccion, short valor);
    BOOL EstaConectado() const { return m_bConectado; }

private:
    CSocket m_socket; // Socket persistente 
    BOOL m_bConectado;
    static WORD m_transactionId;

    // Auxiliar para construir la trama (MBAP + PDU)
    int ConstruirPeticion(BYTE* buf, WORD transId, BYTE unitId, BYTE funcCode, WORD addr, WORD data);
};