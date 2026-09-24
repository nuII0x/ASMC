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

    // Nunca deveria acontecer, mas evita comportamento indefinido
    // caso um valor inválido de enum seja recebido.
    throw std::runtime_error(
        "Largura de endereco invalida"
    );
}

std::size_t Assembler::instructionWordCount(
    const Instruction& i
) const
{
    // Pseudo-instrucoes de salto ocupam:
    //
    //     la <destino>
    //     jmp_a / jeq_a / jlt_a / jgt_a / jc_a
    //
    // Portanto, duas palavras.
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
            "Instrucao desconhecida na linha " +
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

        // Se a próxima instrução começaria depois do maior endereço
        // representável, o programa já ultrapassou o espaço disponível.
        if (address > maximum) {
            throw std::runtime_error(
                "Programa excede " +
                std::to_string(
                    static_cast<int>(
                        addressWidth == AddressWidth::Bits8
                            ? 8
                            : addressWidth == AddressWidth::Bits16
                                ? 16
                                : 32
                    )
                ) +
                " bits de endereco"
            );
        }

        instructionAddresses.push_back(address);

        address += instructionWordCount(i);
    }

    // 'address' aqui representa o primeiro endereço depois do programa.
    //
    // Exemplo com 8 bits:
    //
    // 256 palavras:
    // 0x00 ... 0xFF
    // address final = 0x100
    //
    // Isso é válido, porque 0x100 representa o fim do programa,
    // não um endereço que será executado.
    //
    // Mais de 256 palavras faria o próximo endereço ultrapassar 0xFF.
    if (address > maximum + 1) {
        throw std::runtime_error(
            "Programa excede " +
            std::to_string(
                static_cast<int>(
                    addressWidth == AddressWidth::Bits8
                        ? 8
                        : addressWidth == AddressWidth::Bits16
                            ? 16
                            : 32
                )
            ) +
            " bits de endereco"
        );
    }

    // Endereço imediatamente após a última instrução.
    instructionAddresses.push_back(address);

    for (const auto& l : p.labels) {

        if (labels.contains(l.name)) {
            throw std::runtime_error(
                "Label duplicado: " +
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
                "Endereco nao pode ser negativo: " +
                o.text
            );
        }

        value = static_cast<std::uint64_t>(o.value);

    }
    else if (o.type == Operand::Type::Identifier) {

        const auto it = labels.find(o.text);

        if (it == labels.end()) {
            throw std::runtime_error(
                "Label desconhecido: " +
                o.text
            );
        }

        value = it->second;
    }
    else {

        throw std::runtime_error(
            "Tipo de operando ainda nao suportado"
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
            "Endereco " +
            o.text +
            " excede " +
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
                "Literal excede 8 bits: " +
                o.text
            );
        }

        value = static_cast<std::uint64_t>(o.value);

    }
    else if (o.type == Operand::Type::Identifier) {

        const auto it = labels.find(o.text);

        if (it == labels.end()) {
            throw std::runtime_error(
                "Label desconhecido: " +
                o.text
            );
        }

        value = it->second;
    }
    else {

        throw std::runtime_error(
            "Tipo de operando ainda nao suportado"
        );
    }

    // Mesmo que o assembler esteja configurado para 16 ou 32 bits,
    // esta função continua sendo estritamente de 8 bits.
    //
    // Isso é necessário porque a ISA atual possui instruções cujo
    // operando ocupa somente o byte baixo da Word.
    if (value > 0xFF) {
        throw std::runtime_error(
            "Literal '" +
            o.text +
            "' nao cabe em 8 bits"
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
    // PSEUDO-INSTRUCOES DE SALTO
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
                "Instrucao '" +
                i.mnemonic +
                "' exige exatamente um destino"
            );
        }

        // A ISA atual carrega o destino através de 'la',
        // cujo literal possui apenas 8 bits.
        //
        // Portanto, embora o assembler possa trabalhar internamente
        // com enderecos de 16 ou 32 bits, o salto físico atual ainda
        // precisa de um endereço que caiba em 8 bits.
        const auto target =
            resolveAddress(i.operands[0]);

        if (target > 0xFF) {
            throw std::runtime_error(
                "Destino do salto '" +
                i.operands[0].text +
                "' nao pode ser codificado pela ISA atual: " +
                "o endereco carregado por 'la' possui apenas 8 bits"
            );
        }

        // Primeira palavra:
        //
        //     la <target>
        //
        // Na ISA atual, DEST_A seleciona o registrador A como destino
        // e o byte baixo contém o literal.
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
                "Instrucao fisica de salto inexistente: " +
                std::string(physicalJump)
            );
        }

        output.push_back(d->controlWord);

        return;
    }

    // ------------------------------------------------------------
    // INSTRUCAO NORMAL
    // ------------------------------------------------------------

    const auto* d =
        instructionSet.find(i.mnemonic);

    if (!d) {
        throw std::runtime_error(
            "Instrucao desconhecida na linha " +
            std::to_string(i.line) +
            ": " +
            i.mnemonic
        );
    }

    // ------------------------------------------------------------
    // INSTRUCAO COM LITERAL DE 8 BITS
    // ------------------------------------------------------------

    if (
        d->operandEncoding == cpu::OperandEncoding::Literal8 &&
        i.operands.size() != 1
    ) {
        throw std::runtime_error(
            "Instrucao '" +
            i.mnemonic +
            "' exige exatamente um operando"
        );
    }

    // ------------------------------------------------------------
    // INSTRUCAO SEM OPERANDO
    // ------------------------------------------------------------

    if (
        d->operandEncoding == cpu::OperandEncoding::None &&
        !i.operands.empty()
    ) {
        throw std::runtime_error(
            "Instrucao '" +
            i.mnemonic +
            "' nao aceita operandos"
        );
    }

    cpu::Word w = d->controlWord;

    // Se a instrução possuir literal de 8 bits,
    // coloca o literal no byte baixo da Word.
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
    // PASSAGEM 1
    //
    // Descobre o endereço de cada label considerando inclusive
    // pseudo-instruções que ocupam duas Words.
    collectLabels(p);

    std::vector<cpu::Word> r;

    r.reserve(p.instructions.size());

    // PASSAGEM 2
    //
    // Converte as instruções em Words.
    for (const auto& i : p.instructions) {
        encodeInstruction(i, r);
    }

    return r;
}

}