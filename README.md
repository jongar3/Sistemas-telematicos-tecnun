# Sistemas Telemáticos — TECNUN

Solución al proyecto final de la asignatura de Sistemas Telemáticos Tecnun 2026, hecho en MFC porque somos vintage.

Lo dejamos aquí por si a alguien le resulta útil.

El proyecto implementa un sistema de monitorización industrial basado en el protocolo **Modbus TCP/IP**, con arquitectura maestro-esclavo. Un maestro (la centralita) se comunica con tres esclavos que simulan subsistemas reales de un coche (creo): motor, accionamientos y luces. 

## Arquitectura

```
Centralita (Master) → (puerto 8082) → "Servidor web"
    ├── Slave - Motor          (puerto 3502) → temperatura y revoluciones
    ├── Slave - Accionamientos (puerto 3503) → freno, izquierda, derecha
    └── Slave - Luces          (puerto 3504) → salidas controladas por el maestro
```

El maestro lee registros Holding de los esclavos (temperatura, rpm, estado de frenos...), procesa los datos aplicando factores de escala y puede escribir en los esclavos para controlar actuadores como las luces.

## Interfaz

La centralita muestra en tiempo real los datos de los tres esclavos: velocidad y temperatura del motor con indicadores tipo velocímetro, estado de los accionamientos (freno, izq., der.) con indicadores de color, y el log de eventos con timestamp.

![Centralita master](img/screenshot_master.png)

Los esclavos tienen sus propias ventanas de configuración: el de motor expone sliders para simular temperatura y rpm, el de accionamientos permite toggling de freno e intermitentes, y el de luces recibe escrituras del maestro.

![Esclavos](img/screenshot_slaves.png)

## Estructura del repo

- `Master-centralita/` — Aplicación maestra con interfaz gráfica (MFC)
- `Slav-Motor/` — Esclavo que expone registros de temperatura y velocidad del motor
- `Slav-Accio/` — Esclavo de accionamientos (freno, dirección)
- `Slav-Light/` — Esclavo receptor de órdenes de iluminación
- `tester_server.py` — Servidor Modbus en Python para testear el maestro sin los esclavos ***OPCIONAL***

## Tecnologías

- **C/C++** — lógica principal de maestros y esclavos (~97% del código)
- **MFC (Microsoft Foundation Classes)** — para la interfaz gráfica. [Llaman de los 90s quieren su Framework de vuelta.](https://youtu.be/zZ5Yaowmm1A?si=7Z63pzitZsFZxYcr&t=78). 🥵
- **Python + pymodbus** — servidor de pruebas para simular los esclavos ***OPCIONAL***

## Web server 

La centralita incluye un servidor HTTP embebido en `localhost:8082` que sirve una página con auto-refresh cada segundo. Muestra el estado de los tres esclavos (temperatura, rpm, accionamientos y estado de conexión) con un dashboard en HTML/CSS. 

El dashboard se sirve mediante la sofisticada técnica de concatenar etiquetas HTML a mano dentro de un `CString`. Porque, ¿quién usaría frameworks modernos de servidor o un motor de plantillas HTML en pleno 2026?

## Cómo probar sin los esclavo ***OPCIONAL***

Levanta los tres esclavos simulados con:

```bash
pip install -r requirements.txt
python tester_server.py
```

Esto arranca tres servidores Modbus en localhost (puertos 3502, 3503 y 3504) con valores de prueba precargados. Después arranca el ejecutable del maestro y debería conectar sin problema.
> [!Windows]
> Si se esta en Windows se sustituye py por python. Con python se suele trabajar con [entornos virtuales](https://docs.python.org/3/library/venv.html).
## Nota

Proyecto académico. El código cumple su función, que era aprobar (espero). 

Cuidado con los offsets de registros Modbus (pymodbus tiene un desplazamiento +1 que puede dar más de un dolor de cabeza).

---
## Notas post corrección

Tras la corrección, el sistema funciona correctamente en general.

 Hay dos puntos que funcionan con matices.
 
### Hilo compartido de lectura
 
El sistema usa un hilo compartido para la lectura de los esclavos. El problema aparece cuando, por ejemplo, se apagan los "Accionamientos": el hilo intenta conectarse igualmente a "Accionamientos" y se queda bloqueado unos segundos en cada interacción, esto produce un retraso en la lectura de "Motores" y baja muchisimo la responsibidad de "Motores". Esto ocurre porque la clase utilizada para el socket es bloqueante (no asíncrona).
 
La solución es sencilla: usar **3 hilos + el hilo principal** en lugar de 2, dedicando un hilo por esclavo de lectura. El cambio afecta únicamente al `Master-centralita`, puesto que el resto del código no requiriría modificación. Me da pereza asi que queda al usuario :)
 
### Servidor web y parpadeo en Chrome
 
El servidor web presentaba parpadeo en Google Chrome (e Internet Explorer). En Firefox funciona sin problemas.
 
Una causa probable es que Chrome lanza peticiones adicionales automáticamente al cargar la página, como `GET /favicon.ico`, que el servidor no espera y que provoca un ciclo de respuesta no controlado. Aunque en teoria debería estar resuelto... según claude al parecer chrome gestiona <meta http-equiv='refresh' content='1'> de forma "más agresiva" y dice que se arreglaria cambiando esa linea por algo de javascript. Aunque discutiendo un poco más con nubecita, creo que es porque el servidor esta en el hilo principal. 

Ahora mismo el hilo principal es el que escucha y cuando hay una peticion se crea un hilo secundario que escribe el html. Se propone separarlos:

La arquitectura actual de hilos es:
 
- Hilo principal: UI + aceptar conexiones (`CAsyncSocket`)
- Hilo de lectura Modbus (se recomienda partirlo en dos, Accionamientos + Motores)
- Hilo de escritura Modbus
- Un hilo por petición HTTP (`ClientThreadProc`)
 
La posible solución es añadir un **hilo dedicado al servidor** sacando esa responsabilidad completamente del hilo principal. Quedaría así:
 
- Hilo principal: solo UI
- Hilo servidor: bucle dedicado, siempre escuchando
- Hilo de lectura Modbus (se recomienda partirlo en dos, Accionamientos + Motores)
- Hilo de escritura Modbus
- Un hilo por petición HTTP (`ClientThreadProc`)
El cambio afecta únicamente a `Master-centralita` y se deja al usuario.


Al parecer si google chrome manda la peticion y el hilo principal esta ocupadon, ej. pintando la ventana, no recibe respuesta y manda un error. Supongo que Firefox es más calmado en esta aspecto y tendrá "time-outs" distintos por lo que puede tener sentido. **Pero no lo he probado asi que suerte!**


EN CUALQUIER CASO SE LE PUDE HACER LA DEMOSTRACIÓN EN FIREFOX, SI SE INSTALA FIREFOX EN EL ORDENADOR DE LA DEMOSTRACIÓN ANTES. Y el servidor funcionaría. 

---
Licencia MIT — haz lo que quieras con esto.
