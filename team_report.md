
**Universidad Autónoma de Nuevo León (UANL)**
**Facultad de Ciencias Físico Matemáticas (FCFM)**
**Proyecto:** PAC Fase 4 - Análisis y Detección de Técnicas Ofensivas en un Binario
**Equipo 02:** Jonathan, Jose, Brandon, Carlos

---

## Resumen Ejecutivo
El presente documento consolida los hallazgos del análisis defensivo integral realizado sobre el binario `team_payload.exe` (arquitectura x64, compilado vía MinGW). El análisis se estructuró en cuatro fases metodológicas: Análisis Estático, Análisis Dinámico/Memoria, Análisis de Tráfico de Red y Detección Automatizada. 

El binario fue clasificado como un artefacto con capacidades de *Stager* y ejecución *Fileless*, implementando técnicas de evasión básicas como la ofuscación de cadenas y la ejecución de comandos codificados en Base64. A lo largo del reporte se detallan los Indicadores de Compromiso (IoCs) descubiertos y las reglas de detección formuladas.

---

## 1. Metodología y Entorno de Análisis
Para garantizar la integridad del análisis y aislar el comportamiento del artefacto, se desplegó un entorno de laboratorio seguro (Sandbox):
* **Sistema Operativo Host (Virtualizado):** Windows 11 (Aislado sin conexión a internet externa).
* **Herramientas de Análisis Estático:** Ghidra, Detect It Easy (DIE).
* **Herramientas de Análisis Dinámico:** x64dbg (Depurador de espacio de usuario).
* **Análisis de Red:** Wireshark, Script en Python (simulador de servidor C2 en localhost).
* **Detección Automatizada:** FLARE CAPA, YARA.

---

## 2. Análisis Estático Defensivo
El análisis estático sin ejecución permitió perfilar la estructura del binario y extraer las primeras alertas tempranas.

### 2.1. Análisis de Cabeceras PE y Secciones
La inspección del mapa de memoria en Ghidra reveló una estructura Portable Executable (PE) estándar. Se confirmaron las secciones `.text` (código con permisos R-X), `.rdata` (cadenas de solo lectura) y `.data` (variables inicializadas). No se detectaron anomalías en la entropía general ni la presencia de secciones inusuales (como `.upx`), descartando el uso de *packers* para ocultar el código fuente.

### 2.2. Importaciones Críticas (IAT Hooking)
El análisis de la Tabla de Direcciones de Importación (IAT) expuso dependencias a librerías del sistema altamente monitoreadas por las soluciones de ciberseguridad:
* **`KERNEL32.dll`:** Se detectó el import de la API sensible `WinExec`, utilizada típicamente para la creación de procesos hijos.
* **`WININET.dll`:** Se detectaron las APIs `InternetOpenA` e `InternetOpenUrlA`, revelando capacidades latentes de comunicación HTTP/FTP nativa.

### 2.3. Extracción de Cadenas (Strings) y Patrones
La extracción de cadenas de texto plano arrojó dos IoCs críticos:
1.  **URL sospechosa:** `http://127.0.0.1:8080/ping` (Indicio de infraestructura local C2).
2.  **Comando de ejecución:** `powershell.exe -WindowStyle Hidden -EncodedCommand YwBhAGwAYwAuAGUAeABlAA==`. Este patrón es una firma clásica de ejecución *Fileless*. Al decodificar el payload Base64 bajo el estándar UTF-16LE, se expuso la instrucción original: `calc.exe`.

Notablemente, la palabra clave del indicador de compromiso ("Malware") no figuraba en el volcado de cadenas, sugiriendo la aplicación de técnicas de ofuscación o cifrado estático.

---

## 3. Análisis Dinámico y de Memoria
Para comprender el comportamiento real y evadir la ofuscación, se procedió a la detonación controlada del binario interceptando el flujo de ejecución con x64dbg.

### 3.1. Evasión de Defensas y Descifrado en Memoria
Se colocaron *breakpoints* de software (INT3) en las APIs identificadas previamente. Al avanzar la ejecución hasta interceptar la llamada a `InternetOpenUrlA`, se inspeccionó la pila de memoria (Stack). En el puntero relativo `[rsp+50]`, el depurador reveló la cadena `"Malware"` en texto plano. Esto demostró que el binario ejecutó una rutina de descifrado dinámica (XOR) en memoria antes de su primera conexión, eludiendo la detección estática tradicional.

### 3.2. Mapeo de Regiones RWX
Durante la ejecución, la validación cruzada de los permisos de memoria confirmó que no hubo asignaciones dinámicas anómalas de lectura, escritura y ejecución simultáneas (RWX). Esto corroboró que el programa no inyecta *shellcode* de manera dinámica.

---

## 4. Análisis de Tráfico de Red (Stager / C2)
El binario simulado contiene un módulo de *Stager*, cuya función es establecer comunicación ("Beaconing") con el servidor C2, simulado mediante un script en Python (`server.py`) a la escucha en el puerto 8080.

### 4.1. Intercepción de Paquetes
Mediante Wireshark anclado a la interfaz de loopback (`127.0.0.1`), se capturó la detonación del binario. El análisis de la captura (PCAP) arrojó lo siguiente:
* **Petición inicial:** El malware ejecutó una solicitud HTTP `GET` estándar hacia el endpoint `/ping`.
* **Camuflaje de tráfico:** Se detectó el uso de una cabecera HTTP falsificada con el campo `User-Agent: Mozilla/5.0`, intentando mimetizar la conexión con el tráfico de un navegador web legítimo.
* **Vulnerabilidad del Artefacto:** La comunicación no utiliza encriptación (TLS/SSL), lo que permite que el tráfico sea fácilmente inspeccionado por herramientas de DPI (Deep Packet Inspection) en texto plano.

---

## 5. Detección Automatizada e Ingeniería de Reglas
Para garantizar la respuesta a incidentes en equipos comprometidos por variantes similares, se implementaron soluciones automatizadas.

### 5.1. Regla YARA de Detección
Basado en los artefactos estáticos descubiertos, se redactó una regla YARA (`team_rule.yar`). La regla mapea el comportamiento malicioso buscando la conjunción de las firmas del C2 y los argumentos codificados de PowerShell, logrando un 100% de efectividad en la detección del binario en disco sin arrojar falsos positivos en el sistema operativo host.

### 5.2. Evaluación con FLARE CAPA
La ejecución de la herramienta analítica de Mandiant, CAPA, corroboró de forma automática los hallazgos de ingeniería inversa manual. El reporte emitido mapeó exitosamente las capacidades del binario (ejecución de procesos, ofuscación de línea de comandos y resolución/conexión de red) hacia las tácticas del framework MITRE ATT&CK.

---

## 6. Conclusión y Recomendaciones de Seguridad
El binario `team_payload.exe` demuestra con éxito un ataque segmentado, utilizando un Stager inicial para conexión remota y un payload local ofuscado para evadir los antivirus basados en firmas estáticas. 

**Contramedidas recomendadas:**
1.  **Monitoreo EDR/AMSI:** Desplegar soluciones EDR que aprovechen la Antimalware Scan Interface (AMSI) de Windows, capaz de interceptar y decodificar el tráfico de PowerShell justo antes de su ejecución, mitigando las técnicas de *EncodedCommand*.
2.  **Inspección Perimetral:** Implementar listas de denegación en Firewalls de Próxima Generación (NGFW) bloqueando las conexiones salientes de procesos no firmados en puertos inusuales, o requiriendo intercepción TLS para evitar comunicaciones C2 no cifradas.
3.  **Restricción de APIs:** Habilitar directivas de mitigación de reducción de superficie de ataque (ASR) para impedir que aplicaciones no estandarizadas utilicen llamadas nativas como `WinExec` para instanciar el intérprete de comandos.
