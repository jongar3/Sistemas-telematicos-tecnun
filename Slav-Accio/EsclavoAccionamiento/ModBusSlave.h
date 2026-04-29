#pragma once
#include <winsock2.h>

#pragma pack(push, 1)
struct ModbusTCPHeader {
    unsigned short transactionId;
    unsigned short protocolId;
    unsigned short length;
    unsigned char  unitId;
};
#pragma pack(pop)

class CModbusSlave {
public:
    static int BuildResponse(
        unsigned char* inBuf, int inLen, unsigned char* outBuf,
        bool m_fren, bool m_izq, bool m_der,
        unsigned char myID = 1,
        unsigned short regBase = 0)   // <-- NEW: base address, default 0
    {
        if (inLen < 12) return 0;

        ModbusTCPHeader* reqHeader = (ModbusTCPHeader*)inBuf;

        // Uncomment to enforce slave ID matching:
        // if (reqHeader->unitId != myID && reqHeader->unitId != 0xFF) return 0;

        unsigned char functionCode = inBuf[7];
        unsigned short startAddr   = ntohs(*(unsigned short*)&inBuf[8]);
        unsigned short quantity    = ntohs(*(unsigned short*)&inBuf[10]);

        if (functionCode == 0x03)
        {
            // Guard: quantity must be 1–125 per Modbus spec
            if (quantity == 0 || quantity > 125) return 0;

            // Check requested range falls within [regBase, regBase+3)
            if (startAddr >= regBase && (startAddr + quantity) <= (regBase + 3))
            {
                unsigned char byteCount = (unsigned char)(quantity * 2);

                // Build MBAP response header
                ModbusTCPHeader* resHeader = (ModbusTCPHeader*)outBuf;
                resHeader->transactionId = reqHeader->transactionId; // echo back
                resHeader->protocolId    = 0;
                // Length = UnitID(1) + FuncCode(1) + ByteCount(1) + Data(byteCount)
                resHeader->length = htons(3 + byteCount);
                resHeader->unitId = reqHeader->unitId;

                outBuf[7] = 0x03;       // Function code
                outBuf[8] = byteCount;  // Byte count

                for (int i = 0; i < (int)quantity; i++)
                {
                    unsigned short currentAddr = startAddr + i;
                    unsigned short regValue    = 0;

                    // Map address offset from base to your three booleans
                    unsigned short offset = currentAddr - regBase;
                    if      (offset == 0) regValue = m_fren ? 1 : 0;
                    else if (offset == 1) regValue = m_izq  ? 1 : 0;
                    else if (offset == 2) regValue = m_der  ? 1 : 0;

                    unsigned short netValue = htons(regValue);
                    memcpy(&outBuf[9 + (i * 2)], &netValue, 2);
                }

                // Total = MBAP header(7) + FuncCode(1) + ByteCount(1) + Data
                return 7 + 1 + 1 + byteCount;
            }

            // Address out of range — return Modbus Exception 02 (Illegal Data Address)
            ModbusTCPHeader* resHeader = (ModbusTCPHeader*)outBuf;
            resHeader->transactionId = reqHeader->transactionId;
            resHeader->protocolId    = 0;
            resHeader->length        = htons(3); // UnitID(1)+FuncCode(1)+ExcCode(1)
            resHeader->unitId        = reqHeader->unitId;
            outBuf[7] = 0x83; // FC03 + 0x80 = exception
            outBuf[8] = 0x02; // Illegal Data Address
            return 9;
        }

        // Unsupported function code — Exception 01 (Illegal Function)
        ModbusTCPHeader* resHeader = (ModbusTCPHeader*)outBuf;
        resHeader->transactionId = reqHeader->transactionId;
        resHeader->protocolId    = 0;
        resHeader->length        = htons(3);
        resHeader->unitId        = reqHeader->unitId;
        outBuf[7] = functionCode | 0x80;
        outBuf[8] = 0x01; // Illegal Function
        return 9;
    }
};