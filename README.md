# PAC Fase 4: Análisis y Detección de Técnicas Ofensivas en un Binario

**Universidad Autónoma de Nuevo León (UANL)** **Facultad de Ciencias Físico Matemáticas (FCFM)** Este repositorio contiene los entregables correspondientes a la Fase 4 del reto académico de ciberseguridad. El objetivo de este proyecto es demostrar la comprensión práctica de las técnicas ofensivas mediante el desarrollo de un simulador de malware benigno, seguido de un análisis defensivo completo (estático, dinámico, de red y creación de reglas de detección).

## Equipo 02
* Jonathan Emir Jacobo Martinez
* Jose Miguel Castellanos Martinez
* Brandon Yahir Flores Garcia
* Juan Carlos Fernandez Flores

---

## Estructura del Repositorio

El proyecto se divide en el código fuente del simulador y los reportes técnicos de análisis:

### 1. Código Fuente (Ofensivo / Simulación)
* `team_payload.cpp`: Código fuente en C++ del binario simulado. Implementa técnicas de evasión (descifrado XOR en memoria), un Stager (conexión HTTP) y un payload Fileless (ejecución de PowerShell oculto).
* `team_payload.exe`: Binario compilado (x64) utilizado para los análisis.
* `server.py`: Script en Python que simula el servidor de Comando y Control (C2) escuchando en el puerto 8080.

### 2. Análisis Defensivo (Reportes)
* `static_analysis.md`: Documentación del análisis estático realizado con Ghidra. Incluye extracción de cadenas, revisión de la IAT (APIs sensibles como `WinExec` e `InternetOpenUrlA`) y análisis de cabeceras PE.
* `dynamic_memory_analysis.md`: Documentación del análisis dinámico realizado con x64dbg. Muestra la interrupción de APIs clave, extracción de cadenas descifradas en la pila (Stack) y validación de permisos de memoria.
* `network_behavior.md`: Análisis de tráfico de red capturado con Wireshark, evidenciando el comportamiento de *beaconing* hacia el C2 local.

### 3. Detección Automatizada
* `team_rule.yar`: Regla YARA personalizada diseñada para detectar las firmas estáticas y los IoCs específicos de nuestro binario.
* `capa_report.txt`: Salida cruda de la herramienta FLARE CAPA mostrando el mapeo de capacidades del binario contra el framework MITRE ATT&CK.
* `detection_summary.md`: Resumen técnico de la efectividad de las herramientas YARA y CAPA sobre el payload.

### 4. Reporte Final
* `team_report.pdf`: Documento ejecutivo consolidando todos los hallazgos de las fases anteriores (Estático, Dinámico, Red y Detección).

---

## Aviso Legal
Este proyecto fue desarrollado **estrictamente con fines académicos y educativos**. El código compilado no realiza modificaciones destructivas, no exfiltra información real y las conexiones de red están configuradas para apuntar exclusivamente al entorno local (`127.0.0.1`). 

## Instrucciones de Ejecución (Entorno Aislado)
Si se desea replicar el análisis:
1. Levantar el servidor C2 ejecutando `python server.py`.
2. Ejecutar `team_payload.exe` en una máquina virtual Windows con las defensas desactivadas (para evitar bloqueos por falsos positivos durante la simulación).
3. Monitorear con Wireshark (interfaz loopback) o depurar con x64dbg.
