# Análisis Estático Defensivo - Team 02

## 1. Información General y Secciones PE
Como parte del análisis estático, procedimos a revisar los timestamps y las secciones PE del binario.
* **Nombre del archivo:** `team_payload.exe`
* **Arquitectura:** x64 (compilado con G++ / MinGW)
* **Análisis de Secciones:** Al revisar el mapa de memoria en Ghidra, identificamos las secciones estándar de un ejecutable no empaquetado:
    * `.text`: Contiene el código ejecutable.
    * `.rdata`: Contiene datos de solo lectura (como nuestras cadenas visibles).
    * `.data` / `.bss`: Variables globales e inicializadas.
* **Conclusión PE:** No se detectaron secciones anómalas (como `.upx`), lo que indica que el binario no hace uso de packers para ocultar su código fuente.
<img width="905" height="355" alt="imagen" src="https://github.com/user-attachments/assets/8746c1ba-6b46-4dc1-9e29-0983ff964388" />

<img width="903" height="359" alt="imagen" src="https://github.com/user-attachments/assets/5bba1641-7001-4f01-88e8-64e702eb57bc" />

---

## 2. Extracción de Cadenas (Strings)
Durante la inspección del binario, logramos extraer cadenas estáticas e identificar cadenas cifradas.
* **Cadenas estáticas relevantes descubiertas:**
    * Se identificó un comando de sistema para ejecución oculta: `powershell.exe -WindowStyle Hidden -EncodedCommand YwBhAGwAYwAuAGUAeABlAA==`
    * Se identificó una URL sospechosa apuntando a un servidor local, indicio de un posible C2: `http://127.0.0.1:8080/ping`

<img width="993" height="109" alt="imagen" src="https://github.com/user-attachments/assets/54550604-3dac-415b-ad48-4817242aeb0e" />

* **Cadenas cifradas / Ofuscación:**
    * A pesar de que el código ejecuta una rutina de impresión, la palabra objetivo ("Malware") no se encuentra presente en la lista de *Defined Strings* extraídas estáticamente. Esto confirma la presencia de una técnica de ofuscación de cadenas (XOR) en tiempo de ejecución.

---

## 3. Tabla de Importaciones (IAT) y APIs Sensibles
Al revisar las import tables (IAT), logramos detectar APIs sensibles que revelan las capacidades potenciales del binario.
* **Librería `KERNEL32.DLL`:**
    * **API Detectada:** `WinExec`
    * **Impacto:** Esta es una API altamente sensible que permite la ejecución de procesos externos. Las referencias cruzadas (XREF) en Ghidra confirman que es utilizada por la función `ejecutarFilelessPayload`.

<img width="979" height="254" alt="imagen" src="https://github.com/user-attachments/assets/84e2b582-3543-45f9-b491-fe91dcf6910a" />

* **Librería `WININET.DLL`:**
    * **APIs Detectadas:** `InternetOpenA` e `InternetOpenUrlA`
    * **Impacto:** Indican fuertemente capacidades de red (Beaconing/Stager). Las referencias cruzadas apuntan a la función `ejecutarStager`.

<img width="982" height="438" alt="imagen" src="https://github.com/user-attachments/assets/cb25a3cc-e525-4efd-8b40-a4eadbc696d9" />

---

## 4. Estructuras o Patrones Sospechosos
Al cruzar los datos obtenidos, pudimos identificar estructuras o patrones sospechosos.
* **Patrón de ataque Fileless:** La cadena de PowerShell detectada hace uso del parámetro `-EncodedCommand` seguido de una cadena que termina con el relleno clásico de Base64 (`==`). Al decodificar `YwBhAGwAYwAuAGUAeABlAA==` (en formato UTF-16LE, requerido por PowerShell), se revela la instrucción `calc.exe`. Este es un patrón clásico para evadir la detección de comandos maliciosos por línea de comandos.
