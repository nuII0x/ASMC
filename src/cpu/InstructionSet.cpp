#include "cpu/InstructionSet.hpp"

namespace cpu {

InstructionSet::InstructionSet()
{
    const auto wordOr = [](Word a, Word b) {
        return static_cast<Word>(a | b);
    };

    // NOP: palavra totalmente zerada.
    //
    // CTRL_SEL = 0, portanto esta e uma instrucao do modo data.
    // O bit JUMP (bit 14) nao tem efeito no modo data.
    instructions.emplace(
        "nop",
        InstructionDefinition{"nop", 0, OperandEncoding::None}
    );

    // ============================================================
    // INSTRUCOES DE DADOS / LITERAIS
    // ============================================================
    //
    // CTRL_SEL = 0.
    //
    // O byte baixo da instrucao contem o literal de 8 bits.
    // Os bits de destino determinam onde o valor sera armazenado.
    //
    // Como CTRL_SEL = 0, o circuito ignora JUMP mesmo que o bit 14
    // eventualmente apareca no campo da instrucao.

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
    // INSTRUCOES DA ALU
    // ============================================================
    //
    // CTRL_SEL = 1.
    //
    // A ALU e selecionada por UNIT_SELECT / OP1 / OP0.
    //
    // SWAP_OPERANDS e ZERO_LEFT sao modificadores ortogonais:
    // uma mesma operacao pode trocar X/Y, zerar o operando esquerdo,
    // ou fazer as duas coisas.
    //
    // Nenhuma destas instrucoes ativa JUMP.
    //
    // Portanto, mesmo que o resultado seja EQ, GT ou LT, a CPU
    // continua normalmente para a proxima instrucao.

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

    // Operacoes logicas.
    addAlu("and", 0);
    addAlu("or", OP0);
    addAlu("xor", OP1);
    addAlu("not", OP1 | OP0);

    // Operacoes aritmeticas.
    addAlu("add", UNIT_SELECT);
    addAlu("inc", UNIT_SELECT | OP0);
    addAlu("sub", UNIT_SELECT | OP1);
    addAlu("dec", UNIT_SELECT | OP1 | OP0);

    // ============================================================
    // ALIASES SEMANTICOS
    // ============================================================
    //
    // Estes aliases continuam sendo operacoes normais da ALU.
    // Nenhum deles ativa JUMP.

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
    // SALTOS
    // ============================================================
    //
    // IMPORTANTE:
    //
    // JUMP = bit 14.
    //
    // Como todas estas instrucoes possuem CTRL_SEL = 1, elas estao
    // no modo ALU e o circuito pode considerar o sinal JUMP.
    //
    // A logica conceitual do hardware sera:
    //
    //     JumpEnable = CTRL_SEL AND JUMP
    //
    // e, para saltos condicionais:
    //
    //     JumpTaken = CTRL_SEL AND JUMP AND ConditionSatisfied
    //
    // Portanto uma instrucao normal da ALU nunca salta apenas porque
    // EQ, GT ou LT ficou verdadeiro.

    // ------------------------------------------------------------
    // Salto incondicional
    // Para fazer um salto incondicional: JumpAllways = CTRL_SEL AND JUMP AND ConditionForced------------------------------------------------------------
    //
    // jmp_a:
    //     carrega o PC com o endereco armazenado em A.
    //
    // JUMP = 1.
    //
    // Nenhuma condicao e necessaria.

   instructions.emplace(
    "jmp_a",
    InstructionDefinition{
        "jmp_a",
        CTRL_SEL | JUMP,
        OperandEncoding::None
    }
);

// ------------------------------------------------------------
// Salto se igual a zero
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
// Salto se maior que zero
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
// Salto se menor que zero
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
// Salto incondicional
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
// Salto se carry-out for 1
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
// Halt, congela o sistema completamente.
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
    // Centraliza a busca das instrucoes.
    auto it = instructions.find(m);

    return it == instructions.end()
        ? nullptr
        : &it->second;
}

}