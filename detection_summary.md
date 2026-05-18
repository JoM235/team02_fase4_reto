# Resumen de Detección (YARA y CAPA) - Team 02

## 1. Detección con YARA
Se desarrolló una regla YARA personalizada (`team_rule.yar`) enfocada en identificar los Indicadores de Compromiso (IoCs) estáticos del binario simulado.
* **Criterio de coincidencia:** La regla condiciona la alerta a la existencia de la cabecera PE (`MZ`) junto con al menos dos firmas estáticas clave.
* **Efectividad:** La regla detecta exitosamente el binario basándose en la URL hardcodeada del C2 (`http://127.0.0.1:8080/ping`) y las secuencias del payload fileless (`powershell.exe...`).

## 2. Detección con CAPA
Se ejecutó la herramienta FLARE CAPA sobre el archivo `team_payload.exe` para mapear sus comportamientos contra el framework MITRE ATT&CK. Los resultados detallados se encuentran en `capa_report.txt`.

### Análisis de las capacidades detectadas:
* **Ejecución y Evasión:** CAPA logró identificar exitosamente la capacidad del binario para crear nuevos procesos (debido al uso de `WinExec`) y el uso de técnicas de evasión por ofuscación de comandos.
* **Comunicaciones de Red:** La herramienta detectó capacidades relacionadas con la resolución de URLs y la inicialización de conexiones HTTP (gracias a las importaciones de `wininet.dll` detectadas estáticamente).
* **Conclusión:** CAPA detectó correctamente la intención subyacente de las técnicas simuladas, perfilando el binario como una amenaza con capacidades de Stager y ejecución de comandos, validando la efectividad del análisis automatizado sobre nuestro código.
