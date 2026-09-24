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

// Faz as duas passagens finais do compilador:
// 1. resolve labels;
// 2. transforma cada Instruction do AST em palavras de controle da CPU.
class Assembler {
public:
    explicit Assembler(
        const cpu::InstructionSet&,
        AddressWidth addressWidth = AddressWidth::Bits8
    );

    // Monta o programa e retorna as palavras de controle.
    //
    // Pode lançar runtime_error para:
    // - labels duplicadas;
    // - programa maior que o espaço de endereçamento escolhido;
    // - literais fora de 8 bits;
    // - operandos inválidos;
    // - mnemônicos inexistentes na ISA.
    std::vector<cpu::Word> assemble(const Program&);

private:
    // Primeira passagem:
    // associa cada label ao endereço da instrução correspondente.
    void collectLabels(const Program&);

    // Quantas palavras uma instrução ocupará no binário final.
    std::size_t instructionWordCount(const Instruction&) const;

    // Segunda passagem:
    // valida a instrução e gera as palavras de máquina.
    void encodeInstruction(
        const Instruction&,
        std::vector<cpu::Word>&
    );

    // Resolve número ou label como literal de 8 bits.
    //
    // Usado pelas instruções que realmente possuem operandos de 8 bits
    // na ISA atual.
    std::uint8_t resolveLiteral8(const Operand&);

    // Resolve número ou label como endereço.
    //
    // O endereço pode ter 8, 16 ou 32 bits internamente, dependendo
    // da configuração do assembler.
    std::uint64_t resolveAddress(const Operand&);

    // Retorna o maior endereço representável pela configuração atual.
    std::uint64_t maxAddress() const;

    const cpu::InstructionSet& instructionSet;

    AddressWidth addressWidth;

    // O endereço é armazenado internamente em 64 bits para permitir
    // endereçamento configurável de até 32 bits.
    std::unordered_map<std::string, std::uint64_t> labels;
};

}