#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace compiler {

// Representacao intermediaria entre o parser e o assembler. Ela descreve o
// programa como foi escrito, sem assumir como cada instrucao sera codificada.
struct Operand {
    enum class Type {
        None,
        Number,
        Identifier,
        // Tipos reservados para expansoes futuras da linguagem assembly.
        Register,
        Memory,
    };

    Type type = Type::None;
    // Texto original, preservado para mensagens de erro claras e labels.
    std::string text;
    // Valor numerico ja convertido pelo parser quando type e Number.
    std::int64_t value = 0;
};

struct Instruction {
    // O assembler consulta este texto na InstructionSet para obter a codificacao.
    std::string mnemonic;
    std::vector<Operand> operands;
    // Linha de inicio da instrucao, usada para diagnosticos posteriores.
    int line = 0;
};

struct Label {
    std::string name;
    // Indice da proxima instrucao fonte. O assembler converte isso para endereco
    // real em palavras depois de considerar pseudo-instrucoes expandidas.
    // Uma label no fim do arquivo pode, portanto, apontar para instructions.size().
    std::size_t instructionIndex = 0;
};

struct Program {
    // Labels ficam separados das instrucoes para que o assembler consiga fazer
    // uma primeira passagem de resolucao antes de codificar qualquer palavra.
    std::vector<Instruction> instructions;
    std::vector<Label> labels;
};

}
