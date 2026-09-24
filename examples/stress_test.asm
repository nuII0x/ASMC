; =============================
; STRESS TEST DA CPU
; -----------------------------
; 0x00 - 0x0F
; Básico: aritmética e lógica
; -----------------------------

la 0x00
m 0x00
load_d
inc_d
dec_d
add_d
sub_d
and_d
or_d
xor_d
not_d
add_sw
sub_sw
and_sw
or_sw


; ------------------------------------------------------------
; 0x10 - 0x1F
; Padrão 0xFF
; ------------------------------------------------------------

la 0x10
m 0xFF
load_d
add_d
sub_d
inc_d
dec_d
not_d
and_d
or_d
xor_d
add_sw
sub_sw
or_sw
not_a


; ------------------------------------------------------------
; 0x20 - 0x2F
; Padrão 0xAA
; ------------------------------------------------------------

la 0x20
m 0xAA
load_d
add_d
sub_d
inc_d
dec_d
not_d
and_d
or_d
xor_d
add_sw
sub_sw
xor_sw
not_a


; ------------------------------------------------------------
; 0x30 - 0x3F
; Padrão 0x55
; ------------------------------------------------------------

la 0x30
m 0x55
load_d
add_d
sub_d
inc_d
dec_d
not_d
and_d
or_d
xor_d
add_sw
sub_sw
or_sw
xor_sw


; ------------------------------------------------------------
; 0x40 - 0x4F
; Limite positivo: 0x7F
; ------------------------------------------------------------

la 0x40
m 0x7F
load_d
inc_d
add_d
sub_d
dec_d
not_d
and_d
or_d
xor_d
add_sw
sub_sw
or_sw
not_a


; ------------------------------------------------------------
; 0x50 - 0x5F
; Limite negativo: 0x80
; ------------------------------------------------------------

la 0x50
m 0x80
load_d
inc_d
add_d
sub_d
dec_d
not_d
and_d
or_d
xor_d
add_sw
sub_sw
or_sw
not_a


; ------------------------------------------------------------
; 0x60 - 0x6F
; ZERO_LEFT + operações
; ------------------------------------------------------------

la 0x60
m 0xFF
load_d
add_zx_d
add_zxsw_d
sub_zx_d
sub_zxsw_d
inc_zx_d
dec_zx_d
and_zx_d
or_zx_d
xor_zx_d
add_d
sub_d
inc_d


; ------------------------------------------------------------
; 0x70 - 0x7F
; ZERO_LEFT + 0x00
; ------------------------------------------------------------

la 0x70
m 0x00
load_d
add_zx_d
add_zxsw_d
sub_zx_d
sub_zxsw_d
inc_zx_d
dec_zx_d
and_zx_d
or_zx_d
xor_zx_d
add_d
sub_d
inc_d


; ------------------------------------------------------------
; 0x80 - 0x8F
; Memória + D
; ------------------------------------------------------------

la 0x80
m 0x01
la 0x80
load_d
add_d
store_d
load_d
sub_d
store_d
load_d
and_d
store_d
load_d
or_d


; ------------------------------------------------------------
; 0x90 - 0x9F
; XOR + memória
; ------------------------------------------------------------

la 0x90
m 0xAA
la 0x90
load_d
xor_m
store_d
load_d
xor_m
store_d
load_d
not_d
xor_m
store_d
load_d


; ------------------------------------------------------------
; 0xA0 - 0xAF
; SWAP
; ------------------------------------------------------------

la 0xA0
m 0x7F
la 0xA0
load_d
add_sw
store_d
load_d
sub_sw
store_d
load_d
and_sw
or_sw
xor_sw
not_d


; ------------------------------------------------------------
; 0xB0 - 0xBF
; ZERO_LEFT + SWAP
; ------------------------------------------------------------

la 0xB0
m 0xAA
la 0xB0
load_d
add_zxsw_d
store_d
load_d
sub_zxsw_d
store_d
load_d
and_zxsw_d
or_zxsw_d
xor_zxsw_d
not_d


; ------------------------------------------------------------
; 0xC0 - 0xCF
; A como destino / modificadores
; ------------------------------------------------------------

la 0xC0
m 0x01
load_d
inc_a
dec_a
not_a
add_d
sub_d
and_d
or_d
xor_d
inc_d
dec_d
not_d
add_sw


; ------------------------------------------------------------
; 0xD0 - 0xDF
; Extremos 0xFE / operações repetidas
; ------------------------------------------------------------

la 0xD0
m 0xFE
load_d
inc_d
dec_d
inc_d
dec_d
not_d
add_d
sub_d
and_d
or_d
xor_d
add_sw
sub_sw


; ------------------------------------------------------------
; 0xE0 - 0xEF
; Mistura final
; ------------------------------------------------------------

la 0xE0
m 0x55
la 0xE0
load_d
add_d
store_d
load_d
xor_m
and_zx_d
or_d
sub_d
inc_d
dec_d
not_d


; ------------------------------------------------------------
; 0xF0 - 0xFF
; Último bloco
; HALT obrigatoriamente em 0xFF
; ------------------------------------------------------------

la 0xF0
m 0x00
load_d
inc_d
add_d
sub_d
and_d
or_d
xor_d
not_d
add_sw
sub_sw
and_sw
or_sw
not_a
halt