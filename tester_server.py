import asyncio
from pymodbus.server import StartAsyncTcpServer
from pymodbus.datastore import ModbusSequentialDataBlock, ModbusSlaveContext, ModbusServerContext

IP = "127.0.0.1"

# --- Bloque con callback en escritura ---
class DebugDataBlock(ModbusSequentialDataBlock):
    def __init__(self, name, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.name = name

    def setValues(self, address, values):
        super().setValues(address, values)
        # El índice interno de pymodbus está desplazado +1
        real_reg = address - 1
        for i, v in enumerate(values):
            print(f"[{self.name}] WRITE reg {real_reg + i} = {v}")

async def run_server_accionamientos():
    """Puerto 3503 - Freno=1, Izq=0, Der=1"""
    block = ModbusSequentialDataBlock(0, [0] * 500)
    block.setValues(401, [1, 0, 1])  # reg400=Freno, reg401=Izq, reg402=Der

    store = ModbusSlaveContext(hr=block)
    context = ModbusServerContext(slaves=store, single=True)

    print(f"[3503] Accionamientos listo en {IP}:3503  (Freno=ON, Izq=OFF, Der=ON)")
    await StartAsyncTcpServer(context=context, address=(IP, 3503))

async def run_server_motor():
    """Puerto 3502 - Temp raw=37 → 111°C  |  Rev raw=45 → 3150 rpm"""
    block = ModbusSequentialDataBlock(0, [0] * 500)
    block.setValues(401, [37, 45])  # reg400=Temp(37*3=111°C), reg401=Rev(45*70=3150rpm)

    store = ModbusSlaveContext(hr=block)
    context = ModbusServerContext(slaves=store, single=True)

    print(f"[3502] Motor listo en {IP}:3502  (Temp=37→111°C, Rev=45→3150rpm)")
    await StartAsyncTcpServer(context=context, address=(IP, 3502))

async def run_server_luces():
    """Puerto 3504 - recibe escrituras del maestro en registros 500-504"""
    block = DebugDataBlock("LUCES", 0, [0] * 600)

    store = ModbusSlaveContext(hr=block)
    context = ModbusServerContext(slaves=store, single=True)

    print(f"[3504] Luces listo en {IP}:3504  (esperando escrituras...)")
    await StartAsyncTcpServer(context=context, address=(IP, 3504))

async def main():
    await asyncio.gather(
        run_server_accionamientos(),
        run_server_motor(),
        run_server_luces(),
    )

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nServidor detenido.")