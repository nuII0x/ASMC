Machine

This directory contains the C++ simulator for the 8-bit CPU.

The simulator reproduces the behavior of the CPU architecture in software, providing an environment for executing and testing machine code.

Purpose

The simulator is used to:

- Simulate the CPU
- Execute machine code
- Test the instruction set
- Run Assembly programs
- Debug CPU behavior
- Test architectural changes
- Experiment with the CPU design

Architecture

The simulated computer uses an 8-bit CPU with 16-bit instructions/control words.

The simulator models the main components of the machine, including:

- Registers
- ALU
- Program Counter
- RAM
- ROM
- Control logic
- Clock
- Carry handling
- Conditional jumps
- HALT behavior

The implementation should remain close to the CPU architecture so that the simulator accurately represents the intended machine.

Logic

Basic digital operations may be implemented using simple logic primitives.

For example:

bool NAND(bool a, bool b) {
    return !(a & b);
}

bool INV(bool in) {
    return NAND(in, in);
}

More complex components can be constructed from these primitives when appropriate.

Machine Code

The simulator executes machine code produced by the assembler.

Assembly source
      │
      ▼
   Assembler
      │
      ▼
 Machine code
      │
      ▼
 Machine Simulator
      │
      ▼
      CPU

The simulator and assembler must follow the same ISA.

CPU Execution

The CPU is clock-driven.

The simulator should model CPU state changes according to clock cycles rather than treating an instruction as a single high-level operation.

This allows CPU behavior to be examined at the level of individual cycles and control signals.

Status

The simulator is under development.