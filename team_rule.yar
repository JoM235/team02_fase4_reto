rule Detect_Team02_PAC_Payload {
    meta:
        description = "Detecta el binario educativo ofensivo-controlado del Team 02"
        author = "Analista de Ciberseguridad"
        date = "2026-05-18"
    strings:
        // Firma del Stager C2
        $c2_url = "http://127.0.0.1:8080/ping" ascii wide
        
        // Firma del Payload Fileless
        $ps_cmd = "powershell.exe -WindowStyle Hidden -EncodedCommand" ascii wide
        $ps_b64 = "YwBhAGwAYwAuAGUAeABlAA==" ascii wide
        
        // Firma de ofuscación de consola
        $stager_msg = "[+] Stager: Conexion exitosa al servidor local" ascii wide

    condition:
        uint16(0) == 0x5A4D and // Verifica que sea un ejecutable de Windows (MZ)
        2 of them // Si detecta al menos 2 de los strings maliciosos, dispara la alerta
}
