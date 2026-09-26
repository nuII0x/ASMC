#pragma once

#include "cpu/InstructionSet.hpp"
#include "parser/AST.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace compiler {

enum class AddressWidth {
    Bits8,
    Bits16,
    Bits32
};

// Performs the compiler's two final passes:
// 1. resolves labels;
// 2. lowers each AST Instruction into CPU control words.
class Assembler {
public:
    explicit Assembler(
        const cpu::InstructionSet&,
        AddressWidth addressWidth = AddressWidth::Bits8
    );
    // Assembles the program and returns the resulting control words.
    //
    // May throw runtime_error for:
    // - duplicate labels;
    // - programs that exceed the selected address space;
    // - literals that do not fit in 8 bits;
    // - invalid operands;
    // - mnemonics not defined by the ISA.
    std::vector<cpu::Word> assemble(const Program&);

private:
    // First pass:
    // maps each label to the address of its corresponding instruction.
    void collectLabels(const Program&);

    // Number of words the instruction will occupy in the final binary.    
    std::size_t instructionWordCount(const Instruction&) const;

    // Second pass:
    // validates the instruction and emits the machine words.
    void encodeInstruction(
        const Instruction&,
        std::vector<cpu::Word>&
    );

    // Resolves a number or label as an 8-bit literal.
    //
    // Used by instructions that actually take 8-bit operands
    // in the current ISA.
    std::uint8_t resolveLiteral8(const Operand&);

    // Resolves a number or label as an address.
    //
    // The address may be 8, 16, or 32 bits internally, depending
    // on the assembler configuration.
    std::uint64_t resolveAddress(const Operand&);

    // Returns the largest address representable by the current configuration.
    std::uint64_t maxAddress() const;

    const cpu::InstructionSet& instructionSet;

    AddressWidth addressWidth;

    // Addresses are stored internally as 64-bit values to support
    // configurable address widths up to 32 bits.
    std::unordered_map<std::string, std::uint64_t> labels;
};

}