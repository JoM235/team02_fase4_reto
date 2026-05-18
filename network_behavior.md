# Análisis de Red y Comunicación (Stager) - Team 02

## 1. Configuración del Entorno de Intercepción
Para analizar las capacidades de comunicación del binario `team_payload.exe`, se configuró un entorno de red controlado simulando una infraestructura de Comando y Control (C2). 
* **Servidor C2:** Se levantó un servidor HTTP local en Python escuchando en el puerto TCP 8080.
* **Herramienta de captura:** Se utilizó Wireshark para monitorear la interfaz de *loopback* (`127.0.0.1`), filtrando exclusivamente el tráfico HTTP.

## 2. Análisis del Tráfico y Patrones de Beaconing
Tras la ejecución del binario en el entorno aislado, se detectó actividad de red inmediata confirmando la técnica de *Stager controlado*.

### Hallazgos de red:
* **Protocolo y Puerto:** La comunicación se realizó a través de HTTP estándar sin cifrar (puerto 8080).
* **Método y URI:** El binario ejecutó una petición `GET` hacia el endpoint `/ping`.
* **Periodicidad:** Se observó una única petición inicial, diseñada para validar la conectividad con el C2 antes de proceder con el resto del flujo de ejecución.
* **Indicadores de Compromiso (IoCs) de Red:** * Dirección IP de destino: `127.0.0.1`
  * Puerto de destino: `8080`
  * Cadena de User-Agent: `Mozilla/5.0` (Hardcodeada en el binario para intentar camuflarse como tráfico de navegación web legítimo).

<img width="1917" height="932" alt="imagen" src="https://github.com/user-attachments/assets/b8288d2c-b9b5-4823-9f55-e2e42e43aa6b" />


## 3. Conclusión de Red
El análisis de paquetes confirma que el binario posee la capacidad de realizar *beaconing* hacia una infraestructura externa. Al utilizar un User-Agent común y un protocolo estándar, el binario intenta evadir la detección de firewalls perimetrales de capa 7, aunque la falta de cifrado (HTTPS) permite que herramientas de inspección profunda de paquetes (DPI) identifiquen la anomalía fácilmente.
