```md
# ASMC — Assembly Compiler

C++20 assembler/compiler for an 8-bit CPU with 16-bit instructions.

Pipeline: source.asm -> Lexer -> Parser/AST -> Assembler -> machine.bin

The opcodes follow the 16-bit control word described below. The ALU operations use the `U/OP1/OP0` table and can be modified by `SW` and `ZX`.

The command list below is provided for convenience; `xxd` is used to copy the grouped bits in 2-byte chunks (1 word) per line to make human reading easier.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Usage

```bash
./build/asmc examples/wc.asm examples/wc.bin arch8
```

## Command to copy binary values to the clipboard:

```bash
xxd -b -c 2 examples/complex_super.bin | awk '{print $2 $3}' | xclip -selection clipboard
```

The `.bin` is written in big-endian: the high byte first, followed by the low byte of each 16-bit word.

## Description of the OPCodes

A 16-bit instruction, from the most significant bit to the least significant bit, looks like this:

CPU Instruction Table:

> CTRLSEL JUMP CARRY-IN L-OPSEL HALT A D A* U OP1 OP0 SW ZX LT EQ GT

### Instruction explanation

This section describes the function of each bit in the control field of the 16-bit instruction.

| Field | Description |
|---|---|
| **CTRL-SEL** | Selects the source of the output: `1` selects the result from the **ALU**; `0` selects the low byte of the 16-bit instruction. |
| **JUMP** | When active, it jumps to the address in the **ROM** stored in register **A**. |
| **CARRY-IN** | Receives the carry from the previous operation so it can be used by the next operation, under software control, and passed to the arithmetic unit (**AU**). |
| **L-OP-SEL** | *Left Operand Select*. Selects the source of the left operand used by the logic/arithmetic unit. `0` selects the value from **RAM**; `1` selects the value from the **A register**. |
| **0** | Reserved bit, fixed at `0`. |
| **A** | Sends the result to register **A** in the combined memory. |
| **D** | Sends the result to register **D** in the combined memory. |
| **A\*** | Sends the result to **RAM**, using the address stored in register **A**. |
| **U** | Selects the unit to be used: **LU** (*Logical Unit*) or **AU** (*Arithmetic Unit*). |
| **OP1** | Selection bit for the chosen unit’s operation. |
| **OP0** | Selection bit for the chosen unit’s operation. |
| **SW** | *Swap*. Swaps the **X** and **Y** operands before the operation. |
| **ZX** | Zeroes the left operand before the operation. For example, in an operation `A + B`, operand **B** is zeroed before the operation. |
| **LT** | Compares the result from the **LU** and indicates the *less than* condition. |
| **EQ** | Uses the `allzero` signal from the **LU** to indicate equality. When true in a jump instruction, the counter loads the address stored in register **A**. |
| **GT** | Compares the result from the **LU** and indicates the *greater than* condition. |

### Output latch and data transfer

The send opcodes use a **data latch** to temporarily hold the result of the operation. This way, the value remains stable until the clock goes low, and only then is the signal transferred to the data bus.

This behavior is especially important in transfers between **RAM** and the general-purpose registers. For example, when a value from RAM needs to be sent to a register without altering the value of the data, the **arithmetic unit (AU)** can be used to perform an addition with `0`.

```text
value + 0 = value
```

## Program counter and jumps

The program counter receives the new address from register `A`. When the conditional signal from the LU becomes active, the counter loads `A` and the next word comes from that address in the ROM.

The behavior described for `allzero` was mapped as follows:

| Mnemonic | Expansion | Use |
| --- | --- | --- |
| `jmp <label>` | `la <label>` + `jmp_a` | Unconditional jump. Forces `allzero` using `ZX + and`, so the counter loads `A`. |
| `jmp_a` | one word | Jumps to the address already in `A`. |
| `jz <label>` or `jzero <label>` | `la <label>` + `jz_a` | Jumps if the `allzero` signal from the LU is active. |
| `jz_a` or `jzero_a` | one word | Jumps to the address already in `A` if `allzero` is active. |

Important: `jz <label>` must load `A` with the destination before requesting the jump. Therefore, it behaves like a conditional break/loop when the circuit keeps the `allzero` signal from the LU stable/latched.

Infinite loop example:

```asm
loop:
    ; loop work
    jmp loop
```

Conditional break example:

```asm
loop:
    ; some LU operation uses allzero and emits true for the condition used by the EQ instruction,
    ; then jz loads the PC with the value in register A indicated by the A instruction.
    AND
    jz done

    ; continues the loop if allzero is not satisfied
    jmp loop

done:
    ; it was initially nop, until halt was implemented to freeze the system by stopping the clock
    halt
```

## ALU Table

| U | OP1 | OP0 | Mnemonic | Result |
| - | - | - | --------- | --------- |
| 0 | 0 | 0 | `and` | `X and Y` |
| 0 | 0 | 1 | `or` | `X or Y` |
| 0 | 1 | 0 | `xor` | `X xor Y` |
| 0 | 1 | 1 | `not` | `invert X` |
| 1 | 0 | 0 | `add` | `X + Y` |
| 1 | 0 | 1 | `inc` | `X + 1` |
| 1 | 1 | 0 | `sub` | `X - Y` |
| 1 | 1 | 1 | `dec` | `X - 1` |

Modifiers accepted by the assembler:
They can be combined to modify the operands.

For example:

| Suffix | Bits enabled | Effect |
| --- | --- | --- |
| *(none)* | — | Uses the normal operation. Example: `sub = X - Y` |
| `_sw` | SW | Swaps X and Y. Example: `sub_sw = Y - X` |
| `_zx` | ZX | Zeroes the left operand. Example: `sub_zx = 0 - Y` |
| `_zxsw` | ZX + SW | Swaps and zeroes the effective left operand. Example: `sub_zxsw = 0 - X` |

Destination suffixes accepted for writing the ALU result:

| Suffix | Destination |
| --- | --- |
| `_a` | register A |
| `_d` | register D |
| `_m` | RAM at the address stored in A |

Examples: `add_d` adds `X + Y` and writes to D; `xor_a` writes `X xor Y` to A; `inc_m` writes `X + 1` to RAM. The destination forms still use the operands provided by the circuit; they do not receive literal immediate values.

## Instructions currently accepted by the assembler

| Mnemonic | Generated word | Description |
| --- | --- | --- |
| `nop` | `0000 0000 0000 0000` | No active signals. |
| `a <lit8>` or `la <lit8>` | `0000 0100 xxxx xxxx` | Emits the 8-bit literal and stores it in A. |
| `d <lit8>` or `ld <lit8>` | `0000 0010 xxxx xxxx` | Emits the 8-bit literal and stores it in D. |
| `m <lit8>` or `lm <lit8>` | `0000 0001 xxxx xxxx` | Emits the 8-bit literal and stores it in RAM pointed to by A. |
| `lt` | `1000 0000 0000 0100` | Activates the less-than comparison. |
| `eq` | `1000 0000 0000 0010` | Activates the equal comparison. |
| `gt` | `1000 0000 0000 0001` | Activates the greater-than comparison. |
| `jmp <label>` or `jmp_a` | jump via register A | Loads the counter with A unconditionally. |
| `jz <label>` or `jz_a` | jump via register A | Loads the counter with A when `allzero` from the LU is active. |
| `and`, `or`, `xor`, `not` | depends on `U/OP1/OP0` | Selects a logical ALU operation. |
| `add`, `sub`, `inc`, `dec` | depends on `U/OP1/OP0` | Selects an arithmetic ALU operation. |
| `<alu>_sw`, `<alu>_zx`, `<alu>_zxsw` | operation + modifier | Selects the same operation with operand swap/zeroing. |
| `<alu>_a`, `<alu>_d`, `<alu>_m` | operation + destination | Writes the ALU result to A, D, or RAM. |

`jnz` uses the `allzero` condition, while `halt` triggers the hardware-defined clock stop mechanism.

## Included examples

The examples available in `examples/` are:

- `examples/wc.asm` — basic example of the current ISA.
- `examples/complex_super.asm` — stress program with ALU operations,
  modifiers, destinations, labels, and jump pseudo-instructions.
- `examples/stress_test.asm` — stress test for the ISA.
- `examples/test_carry-out.asm` — test for the `carry-out` mechanism.
- `examples/test_jump.asm` — test for jump instructions.

The corresponding `.bin` files are the binaries generated from the examples that were already compiled:

- `examples/wc.bin`
- `examples/complex_super.bin`
- `examples/stress_test.bin`
- `examples/test_jump.bin`

To generate the binaries:

```bash
./build/asmc examples/wc.asm examples/wc.bin
./build/asmc examples/complex_super.asm examples/complex_super.bin
./build/asmc examples/stress_test.asm examples/stress_test.bin
./build/asmc examples/test_carry-out.asm examples/test_carry-out.bin
./build/asmc examples/test_jump.asm examples/test_jump.bin
