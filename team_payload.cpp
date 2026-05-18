#include <iostream>
#include <windows.h>
#include <wininet.h>
#include <string>
#pragma comment(lib, "wininet.lib")

// ===TÉCNICA 1: Descifrado de cadenas en memoria===
void descifrarCadena(char* str, int len, char key) {
    for (int i = 0; i < len; i++) {
        str[i] ^= key;
    }
}

// ===TÉCNICA 2: Stager controlado (Servidor Local)===
void ejecutarStager() {
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet) {
        HINTERNET hConnect = InternetOpenUrlA(hInternet, "http://127.0.0.1:8080/ping", NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (hConnect) {
            std::cout << "[+] Stager: Conexion exitosa al servidor local (Beaconing simulado).\n";
            InternetCloseHandle(hConnect);
        } else {
            std::cout << "[-] Stager: No se detecto el servidor. (Levanta el servidor local en el puerto 8080).\n";
        }
        InternetCloseHandle(hInternet);
    }
}

// ===TÉCNICA 3: Payload Fileless y Función Sensible===
void ejecutarFilelessPayload() {
    std::string psCommand = "powershell.exe -WindowStyle Hidden -EncodedCommand YwBhAGwAYwAuAGUAeABlAA==";
    std::cout << "[*] Ejecutando payload fileless mediante API sensible (WinExec)...\n";
    std::cout << "[+] Accion: Abriendo la calculadora de forma segura.\n";
    WinExec(psCommand.c_str(), SW_HIDE);
}

int main() {
    std::cout << "=== Inicio de Binario Educativo PAC ===\n\n";
    std::cout << "[*] 1. Extrayendo y descifrando indicador de compromiso...\n";
    char palabraOculta[] = { 'M'^0x05, 'a'^0x05, 'l'^0x05, 'w'^0x05, 'a'^0x05, 'r'^0x05, 'e'^0x05, '\0' };
    descifrarCadena(palabraOculta, 7, 0x05);
    std::cout << "[+] Cadena revelada en memoria: " << palabraOculta << "\n\n";
    std::cout << "[*] 2. Ejecutando modulo de C2 (Stager)...\n";
    ejecutarStager();
    std::cout << "\n";
    std::cout << "[*] 3. Inyectando comando en memoria (Fileless)...\n";
    ejecutarFilelessPayload();
    std::cout << "\n";
    std::cout << "=== Ejecucion Finalizada ===\n";
    system("pause");
    return 0;
}