# Análisis Dinámico y de Memoria - Team 02

## 1. Instrumentación del Binario y Configuración de Breakpoints
Para comprender el comportamiento del binario educativo `team_payload.exe` en tiempo de ejecución, se procedió a realizar una detonación controlada dentro de un entorno seguro (Máquina Virtual Windows 11) utilizando el depurador de espacio de usuario **x64dbg**.

Con el fin de interceptar las técnicas ofensivas simuladas de forma segura antes de que interactuaran con el sistema operativo, se identificaron las llamadas intermodulares clave y se establecieron **breakpoints de software (INT3)** en las siguientes APIs sensibles:
* `WININET.dll!InternetOpenA` (Punto de inicio de las capacidades de red)
* `WININET.dll!InternetOpenUrlA` (Punto de conexión al servidor de Comando y Control)
* `KERNEL32.dll!WinExec` (Función utilizada para la ejecución del payload de PowerShell)

---

## 2. Descifrado de Cadenas en Memoria (Técnica 1)
El binario implementa una rutina de descifrado de cadenas en tiempo de ejecución utilizando el operador lógico **XOR** con la clave estática `0x05`. 

### Hallazgos del Analista:
* Durante el análisis estático inicial, la palabra clave indicadora de compromiso (IoC) no era visible en la sección `.rdata`.
* Al ejecutar el binario paso a paso en x64dbg e interceptar el flujo en el primer breakpoint de red (`InternetOpenA`), se inspeccionó el estado de la pila (**Stack**).
* Se detectó que la rutina de descifrado ya se había ejecutado de manera secuencial en memoria, revelando en texto plano la cadena `"Malware"` alojada en la dirección del puntero relativo `[rsp+50]`. Esto demuestra con éxito la efectividad de la técnica para evadir firmas estáticas simples y su posterior exposición inevitable en memoria dinámica.

**[INSERTAR AQUÍ: Captura de pantalla 1 - Intercepción en InternetOpenA donde se aprecia [rsp+50]: "Malware" en los comentarios/pila]**

---

## 3. Identificación de Regiones de Memoria y Permisos RWX
Como parte de los objetivos defensivos de la Fase IV, se monitoreó el mapa de memoria dinámica del proceso (`Memory Map` en x64dbg) para buscar la asignación de regiones anómalas con permisos de **Lectura, Escritura y Ejecución (RWX)**, comúnmente vinculadas con la inyección de shellcode (vía `VirtualAlloc` o `VirtualAllocEx`).

### Hallazgos del Analista:
* El binario se ejecutó enteramente bajo las regiones asignadas por defecto por el cargador del sistema operativo Windows.
* La sección ejecutable principal `.text` mantuvo sus permisos restrictivos de **Lectura y Ejecución (R-X)**, sin habilitar la capacidad de Escritura (W), descartando la automodificación de código en caliente.
* Las variables y buffers temporales utilizados para almacenar las cadenas descifradas se mantuvieron dentro de la pila (`Stack`) y la sección `.data`, las cuales operan bajo permisos de **Lectura y Escritura (RW-)** sin capacidades de ejecución. Esto confirma que la simulación es completamente benigna y controlada, evitando la ejecución de código arbitrario no autorizado en páginas de memoria RWX.

---

## 4. Análisis del Comportamiento del Stager y C2 (Técnica 2)
El binario simula un mecanismo de persistencia/comunicación ofensiva mediante un *Stager* controlado que busca establecer contacto con una infraestructura local.

### Hallazgos del Analista:
* Al continuar la ejecución del binario, el flujo se detuvo de manera exacta en la API sensible `InternetOpenUrlA`.
* La instrumentación de registros permitió auditar el contexto del procesador en el momento preciso del llamado: el registro de propósito general **RDX** actuaba como contenedor del puntero de memoria hacia la cadena de caracteres `"http://127.0.0.1:8080/ping"`.
* Este comportamiento confirma un patrón clásico de *Beaconing* simulado, diseñado para consultar periódicamente un servidor C2 local en busca de instrucciones o payloads secundarios.

**[INSERTAR AQUÍ: Captura de pantalla 2 - Detención en InternetOpenUrlA mostrando el registro RDX apuntando a la URL local]**

---

## 5. Análisis del Payload Fileless mediante PowerShell (Técnica 3)
La última etapa del binario ofensivo-educativo ejecuta un payload de tipo *Fileless* (sin archivos en disco) utilizando utilidades legítimas del sistema operativo (Living off the Land).

### Hallazgos del Analista:
* El depurador interceptó de manera exitosa el flujo del programa en la API sensible `KERNEL32.dll!WinExec`.
* Al analizar la memoria asociada a los argumentos de la función, se identificó que el registro **RCX** (utilizado para pasar el primer parámetro bajo la convención de llamadas de x64 en Windows) apuntaba directamente a la cadena:
    `"powershell.exe -WindowStyle Hidden -EncodedCommand YwBhAGwAYwAuAGUAeABlAA=="`
* **Análisis de evasión:** El uso del flag `-WindowStyle Hidden` indica un intento deliberado de ocultar la ventana de la consola al usuario final. El uso del parámetro `-EncodedCommand` seguido de un string codificado en Base64 con relleno estándar (`==`) representa un patrón clásico de evasión de defensas perimetrales e inspección de línea de comandos. 
* **Decodificación del Payload:** Al realizar la ingeniería inversa de la cadena Base64 en formato UTF-16LE, se traduce directamente al comando ejecutable benigno `calc.exe`. Esto confirma que el binario simula con precisión una técnica de ejecución fileless sin poner en riesgo la integridad de la máquina host o el entorno de análisis.

**[INSERTAR AQUÍ: Captura de pantalla 3 - Intercepción en la función WinExec con el registro RCX conteniendo el payload fileless de PowerShell]**
