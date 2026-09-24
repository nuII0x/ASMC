start:
    ; ==================================
    ; TESTE 1: SOMA DO BYTE BAIXO
    ; 0xFF + 0x01 = 0x00, Carry = 1
    ; ==================================

    la 0xFF
    ; Carregar o primeiro operando em A

    ; Carregar 0x01 no segundo operando
    ; Executar ADD

    ; Guardar resultado baixo (0x00)
    la 0x10
    m 0x00

    ; ==================================
    ; TESTE 2: SOMA DO BYTE ALTO
    ; 0x00 + 0x00 + Carry = 0x01
    ; ==================================

    ; Executar ADC com Carry-in armazenado

    ; Guardar resultado alto (0x01)
    la 0x11
    m 0x01

    ; ==================================
    ; INDICAR SUCESSO
    ; ==================================

    la 0x12
    m 0x01

    halt