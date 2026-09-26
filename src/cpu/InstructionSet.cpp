#include "cpu/InstructionSet.hpp"

namespace cpu {

InstructionSet::InstructionSet()
{
    const auto wordOr = [](Word a, Word b) {
        return static_cast<Word>(a | b);
    };

    // NOP: completely zeroed word.
    //
    // CTRL_SEL = 0, so this is a data-mode instruction.
    // The JUMP bit (bit 14) has no effect in data mode.
    instructions.emplace(
        "nop",
        InstructionDefinition{"nop", 0, OperandEncoding::None}
    );

    // ============================================================
    // DATA / LITERAL INSTRUCTIONS
    // ============================================================
    //
    // CTRL_SEL = 0.
    //
    // The low byte of the instruction contains the 8-bit literal.
    // The destination bits determine where the value is stored.
    //
    // Since CTRL_SEL = 0, the circuit ignores JUMP even if bit 14
    // happens to be set in the instruction word.

    instructions.emplace(
        "a",
        InstructionDefinition{"a", DEST_A, OperandEncoding::Literal8}
    );

    instructions.emplace(
        "la",
        InstructionDefinition{"la", DEST_A, OperandEncoding::Literal8}
    );

    instructions.emplace(
        "d",
        InstructionDefinition{"d", DEST_D, OperandEncoding::Literal8}
    );

    instructions.emplace(
        "ld",
        InstructionDefinition{"ld", DEST_D, OperandEncoding::Literal8}
    );

    instructions.emplace(
        "m",
        InstructionDefinition{"m", DEST_RAM, OperandEncoding::Literal8}
    );

    instructions.emplace(
        "lm",
        InstructionDefinition{"lm", DEST_RAM, OperandEncoding::Literal8}
    );

    // ============================================================
    // ALU INSTRUCTIONS
    // ============================================================
    //
    // CTRL_SEL = 1.
    //
    // The ALU operation is selected by UNIT_SELECT / OP1 / OP0.
    //
    // SWAP_OPERANDS and ZERO_LEFT are orthogonal modifiers:
    // the same operation can swap X/Y, zero the left operand,
    // or do both.
    //
    // None of these instructions enables JUMP.
    //
    // Therefore, even if the result is EQ, GT, or LT, the CPU
    // continues normally to the next instruction.

    const auto addAlu =
        [this, wordOr](const std::string& name, Word operation)
    {
        const Word base = wordOr(CTRL_SEL, operation);
        const Word sw = wordOr(base, SWAP_OPERANDS);
        const Word zx = wordOr(base, ZERO_LEFT);
        const Word zxsw = wordOr(zx, SWAP_OPERANDS);

        instructions.emplace(
            name,
            InstructionDefinition{name, base, OperandEncoding::None}
        );

        instructions.emplace(
            name + "_sw",
            InstructionDefinition{
                name + "_sw",
                sw,
                OperandEncoding::None
            }
        );

        instructions.emplace(
            name + "_zx",
            InstructionDefinition{
                name + "_zx",
                zx,
                OperandEncoding::None
            }
        );

        instructions.emplace(
            name + "_zxsw",
            InstructionDefinition{
                name + "_zxsw",
                zxsw,
                OperandEncoding::None
            }
        );

        const auto addDestinations =
            [this, wordOr](const std::string& mnemonic, Word control)
        {
            instructions.emplace(
                mnemonic + "_a",
                InstructionDefinition{
                    mnemonic + "_a",
                    wordOr(control, DEST_A),
                    OperandEncoding::None
                }
            );

            instructions.emplace(
                mnemonic + "_d",
                InstructionDefinition{
                    mnemonic + "_d",
                    wordOr(control, DEST_D),
                    OperandEncoding::None
                }
            );

            instructions.emplace(
                mnemonic + "_m",
                InstructionDefinition{
                    mnemonic + "_m",
                    wordOr(control, DEST_RAM),
                    OperandEncoding::None
                }
            );
        };

        addDestinations(name, base);
        addDestinations(name + "_sw", sw);
        addDestinations(name + "_zx", zx);
        addDestinations(name + "_zxsw", zxsw);
    };

    // Logical operations.
    addAlu("and", 0);
    addAlu("or", OP0);
    addAlu("xor", OP1);
    addAlu("not", OP1 | OP0);

    // Arithmetic operations.
    addAlu("add", UNIT_SELECT);
    addAlu("inc", UNIT_SELECT | OP0);
    addAlu("sub", UNIT_SELECT | OP1);
    addAlu("dec", UNIT_SELECT | OP1 | OP0);

    // ============================================================
    // SEMANTIC ALIASES
    // ============================================================
    //
    // These aliases are still regular ALU operations.
    // None of them enables JUMP.

    instructions.emplace(
        "load_d",
        InstructionDefinition{
            "load_d",
            CTRL_SEL | DEST_D | UNIT_SELECT | ZERO_LEFT,
            OperandEncoding::None
        }
    );

    instructions.emplace(
        "add_d",
        InstructionDefinition{
            "add_d",
            CTRL_SEL | DEST_D | UNIT_SELECT,
            OperandEncoding::None
        }
    );

    instructions.emplace(
        "store_d",
        InstructionDefinition{
            "store_d",
            CTRL_SEL |
            DEST_RAM |
            UNIT_SELECT |
            ZERO_LEFT |
            SWAP_OPERANDS,
            OperandEncoding::None
        }
    );

    // ============================================================
    // JUMPS
    // ============================================================
    //
    // IMPORTANT:
    //
    // JUMP = bit 14.
    //
    // Since all of these instructions have CTRL_SEL = 1, they
    // operate in ALU mode and the circuit may evaluate JUMP.
    //
    // The conceptual hardware logic is:
    //
    //     JumpEnable = CTRL_SEL AND JUMP
    //
    // and, for conditional jumps:
    //
    //     JumpTaken = CTRL_SEL AND JUMP AND ConditionSatisfied
    //
    // Therefore, a normal ALU instruction never jumps simply because
    // EQ, GT, or LT is true.

    // ------------------------------------------------------------
    // Unconditional jump
    // ------------------------------------------------------------
    //
    // jmp_a:
    //     loads the PC with the address stored in A.
    //
    // JUMP = 1.
    //
    // No condition is required.

    instructions.emplace(
        "jmp_a",
        InstructionDefinition{
            "jmp_a",
            CTRL_SEL | JUMP,
            OperandEncoding::None
        }
    );

    // ------------------------------------------------------------
    // Jump if equal to zero
    // ------------------------------------------------------------

    instructions.emplace(
        "jeq_a",
        InstructionDefinition{
            "jeq_a",
            CTRL_SEL | JUMP | COND_EQ,
            OperandEncoding::None
        }
    );

    instructions.emplace(
        "jz_a",
        InstructionDefinition{
            "jz_a",
            CTRL_SEL | JUMP | COND_EQ,
            OperandEncoding::None
        }
    );

    instructions.emplace(
        "jzero_a",
        InstructionDefinition{
            "jzero_a",
            CTRL_SEL | JUMP | COND_EQ,
            OperandEncoding::None
        }
    );

    // ------------------------------------------------------------
    // Jump if greater than zero
    // ------------------------------------------------------------

    instructions.emplace(
        "jgt_a",
        InstructionDefinition{
            "jgt_a",
            CTRL_SEL | JUMP | COND_GT,
            OperandEncoding::None
        }
    );

    // ------------------------------------------------------------
    // Jump if less than zero
    // ------------------------------------------------------------

    instructions.emplace(
        "jlt_a",
        InstructionDefinition{
            "jlt_a",
            CTRL_SEL | JUMP | COND_LT,
            OperandEncoding::None
        }
    );

    // ------------------------------------------------------------
    // Unconditional jump
    // ------------------------------------------------------------

    instructions.emplace(
        "jall_a",
        InstructionDefinition{
            "jall_a",
            CTRL_SEL | JUMP | COND_GT | COND_EQ | COND_LT,
            OperandEncoding::None
        }
    );

    // ------------------------------------------------------------
    // Jump if carry-out is 1
    // ------------------------------------------------------------

    instructions.emplace(
        "jc_a",
        InstructionDefinition{
            "jc_a",
            CTRL_SEL | CARRY_IN | JUMP | COND_GT | COND_EQ | COND_LT,
            OperandEncoding::None
        }
    );

    // ------------------------------------------------------------
    // Halt: freezes the entire system.
    // ------------------------------------------------------------

    instructions.emplace(
        "halt",
        InstructionDefinition{
            "halt",
            HALT,
            OperandEncoding::None
        }
    );
}

const InstructionDefinition* InstructionSet::find(const std::string& m) const
{
    // Centralizes instruction lookup.
    auto it = instructions.find(m);

    return it == instructions.end()
        ? nullptr
        : &it->second;
}

}