#pragma once
#include <afxsock.h>

// CModbusClient - Basado en la lógica de conexión de TCP_IP.pdf [cite: 1015]
class CModbusClient
{
public:
    CModbusClient();
    virtual ~CModbusClient();

    // Métodos de conexión basados en el ejemplo de Cliente TCP 
    BOOL Conectar(CString ip, int puerto);
    void Desconectar();

    // Operaciones Modbus (FC03 y FC06)
    BOOL LeerRegistro(BYTE unitId, WORD direccion, short& valor);
    BOOL EscribirRegistro(BYTE unitId, WORD direccion, short valor);

private:
    CSocket m_socket; // Socket persistente siguiendo el estilo de los ejercicios 
    BOOL m_bConectado;
    static WORD m_transactionId;

    // Auxiliar para construir la trama (MBAP + PDU)
    int ConstruirPeticion(BYTE* buf, WORD transId, BYTE unitId, BYTE funcCode, WORD addr, WORD data);
};