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
        int& v500, int& v501, int& v502, int& v503, int& v504, // Passed by reference unsigned char myID = 1,
        unsigned short regBase = 0)
    {
        if (inLen < 12) return 0;

        ModbusTCPHeader* reqHeader = (ModbusTCPHeader*)inBuf;

        // Uncomment to enforce slave ID matching:
        // if (reqHeader->unitId != myID && reqHeader->unitId != 0xFF) return 0;

        unsigned char functionCode = inBuf[7];
        unsigned short address = ntohs(*(unsigned short*)&inBuf[8]);
        unsigned short value = ntohs(*(unsigned short*)&inBuf[10]);

        if (functionCode == 0x06)
        {
            bool validAddress = true;

            if (address == 500)      v500 = (short)value;
            else if (address == 501) v501 = (short)value;
            else if (address == 502) v502 = (short)value;
            else if (address == 503) v503 = (short)value;
            else if (address == 504) v504 = (short)value;
            else validAddress = false;

            if (validAddress) {
                // Success: Echo the 12-byte request back to the Master
                memcpy(outBuf, inBuf, 12);
                return 12;
            }
            else {
                // Error: Illegal Data Address
                return BuildException(reqHeader, outBuf, 0x86, 0x02);
            }
        }

        // If any other Function Code (like 03) is received, return Exception 01 (Illegal Function)
        return BuildException(reqHeader, outBuf, functionCode | 0x80, 0x01);
    }


private:
    static int BuildException(ModbusTCPHeader* req, unsigned char* outBuf, unsigned char fCode, unsigned char exCode) {
        ModbusTCPHeader* res = (ModbusTCPHeader*)outBuf;
        res->transactionId = req->transactionId;
        res->protocolId = 0;
        res->length = htons(3); // UnitID(1) + FuncCode(1) + ExceptionCode(1)
        res->unitId = req->unitId;

        outBuf[7] = fCode;
        outBuf[8] = exCode;
        return 9;
    }
};