#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace compiler {

// Intermediate representation between the parser and assembler. It describes
// the program as written, without assuming how each instruction will be encoded.
struct Operand {
    enum class Type {
        None,
        Number,
        Identifier,
        // Reserved types for future assembly language extensions.
        Register,
        Memory,
    };

    Type type = Type::None;
    // Original text, preserved for clear error messages and labels.
    std::string text;
    // Numeric value already converted by the parser when type is Number.
    std::int64_t value = 0;
};

struct Instruction {
    // The assembler looks up this text in InstructionSet to obtain the encoding.
    std::string mnemonic;
    std::vector<Operand> operands;
    // Instruction's starting line, used for subsequent diagnostics.
    int line = 0;
};

struct Label {
    std::string name;
    // Index of the next source instruction. The assembler converts this into
    // an actual word address after accounting for expanded pseudo-instructions.
    // A label at the end of the file may therefore point to instructions.size().
    std::size_t instructionIndex = 0;
};

struct Program {
    // Labels are kept separate from instructions so the assembler can perform
    // a first resolution pass before encoding any words.
    std::vector<Instruction> instructions;
    std::vector<Label> labels;
};

}