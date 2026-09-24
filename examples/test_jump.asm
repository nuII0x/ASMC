start:
    ; colocar 0xFF em um operando
    la 0x00
    ld 0xFF
    ; colocar 0x01 no outro operando
    lm 0x01
    ; agora sim, adiciona os dois operandos na memória RAM (A*) e no registrador D
    add

    ; testar Carry
    la carry_ok
    jc_a

    ; falhou
    la 0x20
    m 0xFF
    halt

carry_ok:
    la 0x20
    m 0x01
    halt