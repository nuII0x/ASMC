# mycpu-compiler

Esqueleto de compilador/assembler em C++20 para uma CPU de 8 bits com instruções de 16 bits.

Pipeline: source.asm -> Lexer -> Parser/AST -> Assembler -> machine.bin

Os opcodes seguem a palavra de controle de 16 bits descrita abaixo. As operações da ALU usam a tabela `U/OP1/OP0` e podem ser alteradas por `SW` e `ZX`.

## Build
cmake -S . -B build
cmake --build build

## Uso
./build/mycpu examples/test.asm examples/test.bin

O .bin é escrito em big-endian: byte alto seguido do byte baixo de cada palavra de 16 bits.
## Descrição dos OPCodes:
Instrução de 16 bits, começando do bit mais alto pro mais baixo fica assim:
Tabela de Instruções da CPU: 
> CTRLSEL JUMP CARRY-IN L-OPSEL HALT A D A* U OP1 OP0 SW ZX LT EQ GT
Explicação das instruções:
CTRL-SEL: Seleciona se a saída será a resposta da ALU (1) ou o byte baixo da instrução de 16bits. 
JUMP: Pula para o endereço da ROM que está no registrador A.
CARRY-IN: Recebe a sobra (carry) da soma anterior para ser usada via software e encaminhada para a unidade aritmética. 
L-OP-SEL: Left Operand Select: seleciona o operando da esquerda utilizado pela unidade lógica/aritmética. se for 0 usa o valor emitido pela RAM, se for 1 usa o que vem do registrador de endereço A. 
0 (coluna 5): Bit reservado/fixo em 0. 
A: Envia o resultado para o registrador A da memória combinada. 
D: Envia o resultado para o registrador D da memória combinada. 
A*: Envia o resultado para a memória RAM no endereço armazenado em A.
Uma ressalva nos opcodes de envio é que o resultado é emitido e segurado por um data latch, deixando o valor temporariamente fixo até o próximo pulso do clock, isso é importante para o resultado que vem da RAM e vai para um registrador de valor (D) seja salvo antes que o endereço seja mudado pela próxima instrução dos opcodes, para enviar da memória pro registrador ou do registrador para a memória é aproveitado a unidade de aritmética pra adicionar 0 e com isso o valor do resultado seja o mesmo que o operando da esquerda e chegue no destino com o mesmo valor.  
U: Seleciona qual unidade será utilizada: unidade lógica (LU) ou unidade aritmética (AU). 
OP1: Bit de seleção da operação da unidade escolhida. 
OP0: Bit de seleção da operação da unidade escolhida.
SW: Troca (inverte) os valores dos operandos X e Y antes da operação. 
ZX: Zera o operando da esquerda antes da operação. Especificamente o A + B por exemplo, vai zerar o B.
LT: Compara o resultado da LU e indica a condição menor que (less than). 
EQ: Usa o sinal allzero/igual da LU. Se verdadeiro em uma instrução de salto, o contador carrega o endereço que está no registrador A.
GT: Compara o resultado da LU e indica a condição maior que (greater than).

## Contador de programa e saltos

O contador de programa recebe o novo endereco pelo registrador `A`. Quando o sinal condicional vindo da LU fica ativo, o contador carrega `A` e a proxima palavra vem desse endereco da ROM.

O comportamento descrito para `allzero` foi mapeado assim:

| Mnemônico | Expansão | Uso |
| --- | --- | --- |
| `jmp <label>` | `la <label>` + `jmp_a` | Salto incondicional. Forca `allzero` usando `ZX + and`, entao o contador carrega `A`. |
| `jmp_a` | uma palavra | Salta para o endereco que ja esta em `A`. |
| `jz <label>` ou `jzero <label>` | `la <label>` + `jz_a` | Salta se o `allzero` da LU estiver ativo. |
| `jz_a` ou `jzero_a` | uma palavra | Salta para o endereco que ja esta em `A` se `allzero` estiver ativo. |

Importante: `jz <label>` precisa carregar `A` com o destino antes de pedir o salto. Portanto ele funciona como break/loop condicional quando o circuito mantem o `allzero` da LU estabilizado/latcheado ate a palavra de salto. Se o seu circuito recalcular `allzero` imediatamente usando o novo `A`, prefira carregar `A` antes da comparacao ou usar `jmp` incondicional.

Exemplo de loop infinito:

```asm
loop:
    ; trabalho do loop
    jmp loop
```

Exemplo de break condicional:

```asm
loop:
    ; alguma operacao da LU deixa allzero ativo quando deve sair
    xor
    jz done

    ; continua o loop se allzero nao estava ativo
    jmp loop

done:
    nop
```

## Tabela da ALU

| U | OP1 | OP0 | Mnemônico | Resultado |
| --- | --- | --- | --- | --- |
| 0 | 0 | 0 | `and` | `X and Y` |
| 0 | 0 | 1 | `or` | `X or Y` |
| 0 | 1 | 0 | `xor` | `X xor Y` |
| 0 | 1 | 1 | `not` | `invert X` |
| 1 | 0 | 0 | `add` | `X + Y` |
| 1 | 0 | 1 | `inc` | `X + 1` |
| 1 | 1 | 0 | `sub` | `X - Y` |
| 1 | 1 | 1 | `dec` | `X - 1` |

Modificadores aceitos pelo assembler:

| Sufixo | Bits ligados | Efeito |
| --- | --- | --- |
| sem sufixo | nenhum | Usa a operação normal. Exemplo: `sub` = `X - Y`. |
| `_sw` | `SW` | Troca X e Y. Exemplo: `sub_sw` = `Y - X`. |
| `_zx` | `ZX` | Zera o operando esquerdo. Exemplo: `sub_zx` = `0 - Y`. |
| `_zxsw` | `ZX` e `SW` | Troca e zera o operando esquerdo efetivo. Exemplo: `sub_zxsw` = `0 - X`. |

Sufixos de destino aceitos para gravar o resultado da ALU:

| Sufixo | Destino |
| --- | --- |
| `_a` | registrador A |
| `_d` | registrador D |
| `_m` | RAM no endereço armazenado em A |

Exemplos: `add_d` soma `X + Y` e grava em D; `xor_a` grava `X xor Y` em A; `inc_m` grava `X + 1` na RAM. As formas com destino ainda usam os operandos fornecidos pelo circuito; elas não recebem literal no assembly.

## Instruções atualmente aceitas pelo assembler

| Mnemônico | Palavra gerada | Descrição |
| --- | --- | --- |
| `nop` | `0000 0000 0000 0000` | Sem sinais ativos. |
| `a <lit8>` ou `la <lit8>` | `0000 0100 xxxx xxxx` | Emite o literal de 8 bits e grava em A. |
| `d <lit8>` ou `ld <lit8>` | `0000 0010 xxxx xxxx` | Emite o literal de 8 bits e grava em D. |
| `m <lit8>` ou `lm <lit8>` | `0000 0001 xxxx xxxx` | Emite o literal de 8 bits e grava na RAM apontada por A. |
| `lt` | `1000 0000 0000 0100` | Ativa comparação menor que. |
| `eq` | `1000 0000 0000 0010` | Ativa comparação igual. |
| `gt` | `1000 0000 0000 0001` | Ativa comparação maior que. |
| `jmp <label>` ou `jmp_a` | salto via registrador A | Carrega o contador com A de forma incondicional. |
| `jz <label>` ou `jz_a` | salto via registrador A | Carrega o contador com A quando `allzero` da LU esta ativo. |
| `and`, `or`, `xor`, `not` | depende de `U/OP1/OP0` | Seleciona uma operação lógica da ALU. |
| `add`, `sub`, `inc`, `dec` | depende de `U/OP1/OP0` | Seleciona uma operação aritmética da ALU. |
| `<alu>_sw`, `<alu>_zx`, `<alu>_zxsw` | operação + modificador | Seleciona a mesma operação com troca/zero de operandos. |
| `<alu>_a`, `<alu>_d`, `<alu>_m` | operação + destino | Grava o resultado da ALU em A, D ou RAM. |

Os mnemônicos `jnz` e `halt` continuam fora do assembler até que o comportamento inverso de `allzero` e parada do clock sejam definidos no hardware.
Comando para copiar os valores dos binários para o clipboard:
xxd -b -c 2 examples/complex_safe.bin | awk '{print $2 $3}' | xclip -selection clipboard

## Exemplos incluidos

- `examples/wc.asmx`: exemplo basico compativel com a ISA atual.
- `examples/complex_super.asm`: programa de estresse com operacoes da ALU,
  modificadores, destinos, labels e pseudo-instrucoes de salto.

Para gerar os binarios:

```bash
./build/mycpu examples/wc.asmx examples/wc.bin
./build/mycpu examples/complex_super.asm examples/complex_super.bin
```
