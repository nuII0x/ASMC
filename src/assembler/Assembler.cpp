#include "assembler/Assembler.hpp"

#include <stdexcept>
#include <string>

namespace compiler {

Assembler::Assembler(
    const cpu::InstructionSet& s,
    AddressWidth width
)
    : instructionSet(s),
      addressWidth(width)
{
}

std::uint64_t Assembler::maxAddress() const
{
    switch (addressWidth) {

    case AddressWidth::Bits8:
        return 0xFF;

    case AddressWidth::Bits16:
        return 0xFFFF;

    case AddressWidth::Bits32:
        return 0xFFFFFFFFULL;
    }

    // Should never happen, but avoids undefined behavior
    // if an invalid enum value is provided.
    throw std::runtime_error(
        "Invalid address width"
    );
}

std::size_t Assembler::instructionWordCount(
    const Instruction& i
) const
{
    // Jump pseudo-instructions expand to:
    //
    //     la <target>
    //     jmp_a / jeq_a / jlt_a / jgt_a / jc_a
    //
    // Therefore, they occupy two words.
    if (
        i.mnemonic == "jmp" ||
        i.mnemonic == "jeq" ||
        i.mnemonic == "jlt" ||
        i.mnemonic == "jgt" ||
        i.mnemonic == "jc"
    ) {
        return 2;
    }

    const auto* d = instructionSet.find(i.mnemonic);

    if (!d) {
        throw std::runtime_error(
            "Unknown instruction at line " +
            std::to_string(i.line) +
            ": " +
            i.mnemonic
        );
    }

    return 1;
}

void Assembler::collectLabels(const Program& p)
{
    labels.clear();

    std::vector<std::uint64_t> instructionAddresses;

    instructionAddresses.reserve(
        p.instructions.size() + 1
    );

    std::uint64_t address = 0;
    const std::uint64_t maximum = maxAddress();

    for (const auto& i : p.instructions) {

        // If the next instruction would start beyond the largest
        // representable address, the program has exceeded the
        // available address space.
        if (address > maximum) {
            throw std::runtime_error(
                "Program exceeds " +
                std::to_string(
                    static_cast<int>(
                        addressWidth == AddressWidth::Bits8
                            ? 8
                            : addressWidth == AddressWidth::Bits16
                                ? 16
                                : 32
                    )
                ) +
                "-bit address space"
            );
        }

        instructionAddresses.push_back(address);

        address += instructionWordCount(i);
    }

    // 'address' now represents the first address after the program.
    //
    // Example with 8-bit addresses:
    //
    // 256 words:
    // 0x00 ... 0xFF
    // final address = 0x100
    //
    // This is valid because 0x100 represents the end of the program,
    // not an address that will be executed.
    //
    // More than 256 words would cause the next address to exceed 0xFF.
    if (address > maximum + 1) {
        throw std::runtime_error(
            "Program exceeds " +
            std::to_string(
                static_cast<int>(
                    addressWidth == AddressWidth::Bits8
                        ? 8
                        : addressWidth == AddressWidth::Bits16
                            ? 16
                            : 32
                )
            ) +
            "-bit address space"
        );
    }

    // Address immediately after the last instruction.
    instructionAddresses.push_back(address);

    for (const auto& l : p.labels) {

        if (labels.contains(l.name)) {
            throw std::runtime_error(
                "Duplicate label: " +
                l.name
            );
        }

        labels[l.name] =
            instructionAddresses.at(l.instructionIndex);
    }
}

std::uint64_t Assembler::resolveAddress(
    const Operand& o
)
{
    std::uint64_t value = 0;

    if (o.type == Operand::Type::Number) {

        if (o.value < 0) {
            throw std::runtime_error(
                "Address cannot be negative: " +
                o.text
            );
        }

        value = static_cast<std::uint64_t>(o.value);

    }
    else if (o.type == Operand::Type::Identifier) {

        const auto it = labels.find(o.text);

        if (it == labels.end()) {
            throw std::runtime_error(
                "Unknown label: " +
                o.text
            );
        }

        value = it->second;
    }
    else {

        throw std::runtime_error(
            "Unsupported operand type"
        );
    }

    if (value > maxAddress()) {

        const int bits =
            addressWidth == AddressWidth::Bits8
                ? 8
                : addressWidth == AddressWidth::Bits16
                    ? 16
                    : 32;

        throw std::runtime_error(
            "Address " +
            o.text +
            " exceeds " +
            std::to_string(bits) +
            " bits"
        );
    }

    return value;
}

std::uint8_t Assembler::resolveLiteral8(
    const Operand& o
)
{
    std::uint64_t value = 0;

    if (o.type == Operand::Type::Number) {

        if (o.value < 0 || o.value > 0xFF) {
            throw std::runtime_error(
                "Literal does not fit in 8 bits: " +
                o.text
            );
        }

        value = static_cast<std::uint64_t>(o.value);

    }
    else if (o.type == Operand::Type::Identifier) {

        const auto it = labels.find(o.text);

        if (it == labels.end()) {
            throw std::runtime_error(
                "Unknown label: " +
                o.text
            );
        }

        value = it->second;
    }
    else {

        throw std::runtime_error(
            "Unsupported operand type"
        );
    }

    // Even if the assembler is configured for 16- or 32-bit addresses,
    // this function remains strictly 8-bit.
    //
    // This is necessary because the current ISA has instructions whose
    // operand occupies only the low byte of the word.
    if (value > 0xFF) {
        throw std::runtime_error(
            "Literal '" +
            o.text +
            "' does not fit in 8 bits"
        );
    }

    return static_cast<std::uint8_t>(value);
}

void Assembler::encodeInstruction(
    const Instruction& i,
    std::vector<cpu::Word>& output
)
{
    // ------------------------------------------------------------
    // JUMP PSEUDO-INSTRUCTIONS
    // ------------------------------------------------------------

    if (
        i.mnemonic == "jmp" ||
        i.mnemonic == "jeq" ||
        i.mnemonic == "jlt" ||
        i.mnemonic == "jgt" ||
        i.mnemonic == "jc"
    ) {

        if (i.operands.size() != 1) {
            throw std::runtime_error(
                "Instruction '" +
                i.mnemonic +
                "' requires exactly one target"
            );
        }

        // The current ISA loads the target through 'la',
        // whose literal is only 8 bits wide.
        //
        // Therefore, although the assembler can internally work
        // with 16- or 32-bit addresses, the current physical jump
        // still requires an address that fits in 8 bits.
        const auto target =
            resolveAddress(i.operands[0]);

        if (target > 0xFF) {
            throw std::runtime_error(
                "Jump target '" +
                i.operands[0].text +
                "' cannot be encoded by the current ISA: " +
                "the address loaded by 'la' is only 8 bits wide"
            );
        }

        // First word:
        //
        //     la <target>
        //
        // In the current ISA, DEST_A selects register A as the
        // destination and the low byte contains the literal.
        output.push_back(
            static_cast<cpu::Word>(
                cpu::DEST_A |
                static_cast<cpu::Word>(target)
            )
        );

        const char* physicalJump = nullptr;

        if (i.mnemonic == "jmp") {
            physicalJump = "jmp_a";
        }
        else if (i.mnemonic == "jeq") {
            physicalJump = "jeq_a";
        }
        else if (i.mnemonic == "jlt") {
            physicalJump = "jlt_a";
        }
        else if (i.mnemonic == "jgt") {
            physicalJump = "jgt_a";
        }
        else if (i.mnemonic == "jc") {
            physicalJump = "jc_a";
        }

        const auto* d =
            instructionSet.find(physicalJump);

        if (!d) {
            throw std::runtime_error(
                "Physical jump instruction not found: " +
                std::string(physicalJump)
            );
        }

        output.push_back(d->controlWord);

        return;
    }

    // ------------------------------------------------------------
    // NORMAL INSTRUCTION
    // ------------------------------------------------------------

    const auto* d =
        instructionSet.find(i.mnemonic);

    if (!d) {
        throw std::runtime_error(
            "Unknown instruction at line " +
            std::to_string(i.line) +
            ": " +
            i.mnemonic
        );
    }

    // ------------------------------------------------------------
    // INSTRUCTION WITH AN 8-BIT LITERAL
    // ------------------------------------------------------------

    if (
        d->operandEncoding == cpu::OperandEncoding::Literal8 &&
        i.operands.size() != 1
    ) {
        throw std::runtime_error(
            "Instruction '" +
            i.mnemonic +
            "' requires exactly one operand"
        );
    }

    // ------------------------------------------------------------
    // INSTRUCTION WITH NO OPERAND
    // ------------------------------------------------------------

    if (
        d->operandEncoding == cpu::OperandEncoding::None &&
        !i.operands.empty()
    ) {
        throw std::runtime_error(
            "Instruction '" +
            i.mnemonic +
            "' does not accept operands"
        );
    }

    cpu::Word w = d->controlWord;

    // If the instruction has an 8-bit literal,
    // place the literal in the low byte of the word.
    if (
        d->operandEncoding ==
        cpu::OperandEncoding::Literal8
    ) {
        w |= resolveLiteral8(i.operands[0]);
    }

    output.push_back(w);
}

std::vector<cpu::Word> Assembler::assemble(
    const Program& p
)
{
    // PASS 1
    //
    // Determine the address of each label, including pseudo-instructions
    // that expand to two words.
    collectLabels(p);

    std::vector<cpu::Word> r;

    r.reserve(p.instructions.size());

    // PASS 2
    //
    // Encode the instructions into machine words.
    for (const auto& i : p.instructions) {
        encodeInstruction(i, r);
    }

    return r;
}

}