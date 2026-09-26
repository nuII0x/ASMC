#pragma once

#include "cpu/Word.hpp"

#include <string>
#include <unordered_map>

namespace cpu {

// Masks for the control signals in the 16-bit control word.
//
// Layout, from most significant bit to least significant bit:
//
// CTRL_SEL | JUMP | CARRY_IN | LEFT_OPERAND_ADDRESS | HALT |
// A | D | A* | UNIT_SELECT | OP1 | OP0 |
// SWAP_OPERANDS | ZERO_LEFT | LT | EQ | GT
//
// Bit 11 enables HALT. It sends a signal that prevents the clock
// from being received, leaving the system frozen.
//
// Bit 14 (JUMP) only has an effect when CTRL_SEL = 1,
// that is, in ALU mode.
//
// In data mode (CTRL_SEL = 0), the circuit ignores JUMP.
//
// Each value below is a mask, so the signals can be combined
// with the | operator to form a single cpu::Word.
enum ControlBit : Word {

    // ------------------------------------------------------------
    // Bits 15 and 14
    // ------------------------------------------------------------

    // Bit 15:
    // selects ALU mode.
    //
    // 0 = data/literal mode
    // 1 = ALU mode
    CTRL_SEL = 1u << 15,

    // Bit 14:
    // enables program-counter jumps.
    //
    // IMPORTANT:
    // JUMP only works when CTRL_SEL = 1.
    //
    // Therefore:
    //
    // CTRL_SEL = 0, JUMP = 0 -> no jump
    // CTRL_SEL = 0, JUMP = 1 -> JUMP ignored
    // CTRL_SEL = 1, JUMP = 0 -> normal ALU operation
    // CTRL_SEL = 1, JUMP = 1 -> jump enabled
    JUMP = 1u << 14,

    // ------------------------------------------------------------
    // Bits 13 and 12
    // ------------------------------------------------------------

    // Bit 13:
    // provides the carry produced by the previous operation
    // to the arithmetic unit.
    CARRY_IN = 1u << 13,

    // Bit 12:
    // selects A as the left operand.
    //
    // When cleared, the circuit uses RAM as the left operand.
    LEFT_OPERAND_ADDRESS = 1u << 12,

    // ------------------------------------------------------------
    // Bit 11:
    // HALT: stops CPU execution.
    //
    // 0 = normal execution
    // 1 = stops the clock and disables the enable signals
    //     supplied to the components.
    HALT = 1u << 11,

    // ------------------------------------------------------------
    // Bits 10, 9, and 8
    // ------------------------------------------------------------

    // Destinations that receive the stabilized result from the data latch.
    DEST_A = 1u << 10,
    DEST_D = 1u << 9,
    DEST_RAM = 1u << 8,

    // ------------------------------------------------------------
    // Bits 7, 6, and 5
    // ------------------------------------------------------------

    // ALU operation selection.
    //
    // U OP1 OP0:
    //
    // 000 = AND
    // 001 = OR
    // 010 = XOR
    // 011 = NOT
    // 100 = ADD
    // 101 = INC
    // 110 = SUB
    // 111 = DEC
    UNIT_SELECT = 1u << 7,
    OP1 = 1u << 6,
    OP0 = 1u << 5,

    // ------------------------------------------------------------
    // Bits 4 and 3
    // ------------------------------------------------------------

    // Operand transformations applied before the selected operation.
    //
    // SWAP_OPERANDS exchanges X and Y.
    //
    // ZERO_LEFT:
    // 0 = leave the operands unchanged
    // 1 = zero the left operand
    //
    // Example:
    //
    // X + Y
    // ZERO_LEFT = 1 -> 0 + Y
    //
    // If SWAP_OPERANDS = 1 and ZERO_LEFT = 1:
    //
    // X + Y
    //      ↓ swap
    // Y + X
    //      ↓ zero left operand
    // 0 + X
    SWAP_OPERANDS = 1u << 4,
    ZERO_LEFT = 1u << 3,

    // ------------------------------------------------------------
    // Bits 2, 1, and 0
    // ------------------------------------------------------------

    // Condition selectors used by jump control.
    //
    // The Condition block receives the ALU's final 8-bit result
    // and produces:
    //
    //     LT = result < 0
    //     EQ = result == 0
    //     GT = result > 0
    //
    // These instruction bits do not calculate the condition.
    // They select which condition is used by the jump.
    COND_LT = 1u << 2,
    COND_EQ = 1u << 1,
    COND_GT = 1u << 0
};

// Defines how operands written in assembly are encoded
// into the final machine word.
//
// Literal8:
//     uses only the eight least significant bits.
//     The instruction must remain in data mode (CTRL_SEL = 0).
//
enum class OperandEncoding {
    None,
    Literal8,
};

// Immutable encoding definition for a mnemonic.
//
// controlWord contains the instruction's control signals.
// operandEncoding tells the assembler how to encode the operand
// written in the assembly source.
struct InstructionDefinition {
    std::string mnemonic;
    Word controlWord;
    OperandEncoding operandEncoding;
};

// Registry of instructions currently supported by the hardware.
//
// Lookup is case-sensitive because the lexer preserves the original
// text from the assembly source file.
class InstructionSet {
public:
    InstructionSet();

    // Returns nullptr when the mnemonic is not part of this ISA.
    //
    // The pointer remains valid as long as this InstructionSet exists,
    // because the registry is built in the constructor and is not
    // modified afterward.
    const InstructionDefinition* find(const std::string&) const;

private:
    std::unordered_map<std::string, InstructionDefinition> instructions;
};

}