; Programa de estresse da ISA atual.
; Testa literais, labels, saltos, comparacoes, ALU, modificadores,
; destinos A/D/RAM e varios padroes de controle.

start:
la 0x10
m 0x03
la 0x11
m 0x05
la 0x12
m 0x08
la 0x13
m 0x0D
la 0x14
m 0x15
la 0x15
m 0x22

la 0x03
load_d
add_d
add_sw
sub_d
sub_sw
inc_d
dec_d
and_d
or_d
xor_d
not_d

la 0x20
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
not_zx_d

la 0x30
m 0xAA
la 0x31
m 0x55
la 0x30
load_d
xor_m
la 0x32
store_d
la 0x31
load_d
or_m
la 0x33
store_d

la 0x40
load_d
add_d
la 0x41
store_d
la 0x42
load_d
sub_d
la 0x43
store_d

la 0x50
load_d
xor
jeq equal_case
jgt greater_case
jlt lower_case
jmp after_compare

equal_case:
la 0x60
m 0x01
jmp after_compare

greater_case:
la 0x60
m 0x02
jmp after_compare

lower_case:
la 0x60
m 0x03

after_compare:
la 0x70
m 0x10
la 0x71
m 0x20
la 0x72
m 0x30
la 0x73
m 0x40
la 0x74
m 0x50

la 0x70
load_d
add_d
la 0x75
store_d
la 0x71
load_d
sub_d
la 0x76
store_d
la 0x72
load_d
and_d
la 0x77
store_d
la 0x73
load_d
or_d
la 0x78
store_d
la 0x74
load_d
xor_d
la 0x79
store_d

la 0x80
load_d
add_sw
la 0x81
store_d
la 0x82
load_d
sub_sw
la 0x83
store_d
la 0x84
load_d
and_sw
la 0x85
store_d
la 0x86
load_d
or_sw
la 0x87
store_d
la 0x88
load_d
xor_sw
la 0x89
store_d

la 0x90
load_d
inc_a
la 0x91
load_d
dec_a
la 0x92
load_d
not_a

la 0xA0
load_d
jeq_a
la 0xA1
load_d
jgt_a
la 0xA2
load_d
jlt_a
la 0xA3
load_d
jmp_a

end:
halt
