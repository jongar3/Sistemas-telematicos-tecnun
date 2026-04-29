#pragma once
#include <winsock2.h>

// Ensure the struct is packed byte-by-byte (no padding)
#pragma pack(push, 1)
struct ModbusTCPHeader {
    unsigned short transactionId; // Set by client, must be returned
    unsigned short protocolId;    // Always 0 for Modbus
    unsigned short length;        // Remaining bytes (UnitID + PDU)
    unsigned char  unitId;        // Slave ID
};
#pragma pack(pop)

class CModbusSlave {
public:
    // Process a Modbus TCP Request
    // Returns the length of the response written to outBuf
    static int BuildResponse(unsigned char* inBuf, int inLen, unsigned char* outBuf,
        bool m_fren, bool m_izq, bool m_der, unsigned char myID = 1)
    {
        // 1. Basic validation (Header is 7 bytes, Read Request is 12 bytes total)
        if (inLen < 12) return 0;

        ModbusTCPHeader* reqHeader = (ModbusTCPHeader*)inBuf;

        // 2. Check Slave ID (Unit Identifier)
        //if (reqHeader->unitId != myID && reqHeader->unitId != 0xFF) return 0;

        unsigned char functionCode = inBuf[7];
        // Address and Quantity are in Big-Endian (Network Byte Order)
        unsigned short startAddr = ntohs(*(unsigned short*)&inBuf[8]);
        unsigned short quantity = ntohs(*(unsigned short*)&inBuf[10]);

        // 3. Handle Function Code 03 (Read Holding Registers)
        if (functionCode == 0x03) {
            // Check if they are asking for our range (400-402)
            if (startAddr >= 400 && (startAddr + quantity) <= 403) {

                unsigned char byteCount = (unsigned char)(quantity * 2);

                // Build MBAP Header for Response
                ModbusTCPHeader* resHeader = (ModbusTCPHeader*)outBuf;
                resHeader->transactionId = reqHeader->transactionId;
                resHeader->protocolId = 0;
                resHeader->length = htons(3 + byteCount); // UnitID(1) + Func(1) + Count(1) + Data
                resHeader->unitId = reqHeader->unitId;

                outBuf[7] = 0x03; // Function Code
                outBuf[8] = byteCount;

                // Fill registers based on the offset from 400
                for (int i = 0; i < quantity; i++) {
                    int currentAddr = startAddr + i;
                    unsigned short regValue = 0;

                    if (currentAddr == 400) regValue = m_fren ? 1 : 0;
                    else if (currentAddr == 401) regValue = m_izq ? 1 : 0;
                    else if (currentAddr == 402) regValue = m_der ? 1 : 0;

                    // Modbus registers are 16-bit Big-Endian
                    unsigned short netValue = htons(regValue);
                    memcpy(&outBuf[9 + (i * 2)], &netValue, 2);
                }

                return 7 + 2 + byteCount; // Header(7) + Func(1) + Count(1) + Data
            }
        }

        // Error or unsupported address: return 0 (or you could implement Modbus Exceptions)
        return 0;
    }
};