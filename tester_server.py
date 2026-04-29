import asyncio
from pymodbus.server import StartAsyncTcpServer
from pymodbus.datastore import ModbusSequentialDataBlock, ModbusSlaveContext, ModbusServerContext

PORT = 3503
IP = "127.0.0.1"
async def run_modbus_server():
    block = ModbusSequentialDataBlock(0, [0] * 500)
    block.setValues(401, [1, 0, 1])
    
    store = ModbusSlaveContext(hr=block)
    context = ModbusServerContext(slaves=store, single=True)
    
    print(f"Servidor Modbus listo en {IP}:{PORT}")
    
    await StartAsyncTcpServer(
        context=context, 
        address=(IP, PORT)
    )

if __name__ == "__main__":
    try:
        asyncio.run(run_modbus_server())
    except KeyboardInterrupt:
        print("\nServidor detenido.")