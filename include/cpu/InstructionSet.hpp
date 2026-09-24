#pragma once

#include "cpu/Word.hpp"

#include <string>
#include <unordered_map>

namespace cpu {

// Mascaras dos sinais da palavra de controle de 16 bits.
//
// Layout, do bit mais significativo para o menos significativo:
//
// CTRL_SEL | JUMP | CARRY_IN | LEFT_OPERAND_ADDRESS | HALT |
// A | D | A* | UNIT_SELECT | OP1 | OP0 |
// SWAP_OPERANDS | ZERO_LEFT | LT | EQ | GT
//
// Bit 11 liga o halt, envia um sinal 1 impedindo o recebimento do clock deixando o sistema congelado.
//
// O bit 14 (JUMP) somente tem efeito quando CTRL_SEL = 1,
// ou seja, no modo ALU.
//
// No modo data (CTRL_SEL = 0), o circuito ignora JUMP.
//
// Cada valor abaixo e uma mascara, portanto os sinais podem ser
// combinados com o operador | para formar uma unica cpu::Word.
enum ControlBit : Word {

    // ------------------------------------------------------------
    // Bits 15 e 14
    // ------------------------------------------------------------

    // Bit 15:
    // seleciona o modo ALU.
    //
    // 0 = modo data/literal
    // 1 = modo ALU
    CTRL_SEL = 1u << 15,

    // Bit 14:
    // autoriza o salto do contador de programa.
    //
    // IMPORTANTE:
    // JUMP somente funciona quando CTRL_SEL = 1.
    //
    // Portanto:
    //
    // CTRL_SEL = 0, JUMP = 0 -> sem salto
    // CTRL_SEL = 0, JUMP = 1 -> JUMP ignorado
    // CTRL_SEL = 1, JUMP = 0 -> operacao ALU normal
    // CTRL_SEL = 1, JUMP = 1 -> salto permitido
    JUMP = 1u << 14,

    // ------------------------------------------------------------
    // Bits 13 e 12
    // ------------------------------------------------------------

    // Bit 13:
    // fornece o carry produzido pela operacao anterior
    // para a unidade aritmetica.
    CARRY_IN = 1u << 13,

    // Bit 12:
    // escolhe A como operando esquerdo.
    //
    // Em zero, o circuito usa a RAM como operando esquerdo.
    LEFT_OPERAND_ADDRESS = 1u << 12,

    // ------------------------------------------------------------
    // Bit 11:
    // HALT: interrompe a execução da CPU.
    //
    // 0 = execução normal
    // 1 = interrompe o clock e desativa o fornecimento de sinal
    //     para os enables dos componentes.
    HALT = 1u << 11,

    // ------------------------------------------------------------
    // Bits 10, 9 e 8
    // ------------------------------------------------------------

    // Destinos que recebem o resultado estabilizado no data latch.
    DEST_A = 1u << 10,
    DEST_D = 1u << 9,
    DEST_RAM = 1u << 8,

    // ------------------------------------------------------------
    // Bits 7, 6 e 5
    // ------------------------------------------------------------

    // Selecao da operacao da ALU.
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
    // Bits 4 e 3
    // ------------------------------------------------------------

    // Ajustes feitos nos operandos antes da operacao selecionada.
    //SWAP_OPERANDS troca o X pelo Y, e vice-versa;
    //ZERO_LEFT se 0 mantém os valores como estão, se for 1 zera o operando da esquerda, exemplo: X + Y, zera x, se SWAP_OPERANDS = 1 e ZERO_LEFT = 1, então zera Y.
    SWAP_OPERANDS = 1u << 4,
    ZERO_LEFT = 1u << 3,

    // ------------------------------------------------------------
    // Bits 2, 1 e 0
    // ------------------------------------------------------------

    // Seletores de condicao utilizados pelo controle de salto.
    //
    // O bloco Condition recebe o resultado final de 8 bits da ALU
    // e produz as informacoes:
    //
    //     LT = resultado < 0
    //     EQ = resultado == 0
    //     GT = resultado > 0
    //
    // Estes bits da instrucao nao calculam a condicao.
    // Eles selecionam qual condicao deve ser usada pelo salto.
    COND_LT = 1u << 2,
    COND_EQ = 1u << 1,
    COND_GT = 1u << 0,
};

// Define como os operandos escritos no assembly ocupam a palavra final.
//
// Literal8:
//     usa exclusivamente os oito bits menos significativos.
//     A instrucao deve permanecer no modo data (CTRL_SEL = 0).
//
enum class OperandEncoding {
    None,
    Literal8,
};

// Receita imutavel de codificacao de um mnemomico.
//
// controlWord contem os sinais de controle da instrucao.
// operandEncoding informa ao assembler como interpretar o operando
// escrito no assembly.
struct InstructionDefinition {
    std::string mnemonic;
    Word controlWord;
    OperandEncoding operandEncoding;
};

// Registro das instrucoes que o hardware atual suporta de forma definida.
//
// A busca e sensivel a maiusculas/minusculas porque o lexer preserva
// o texto original do arquivo assembly.
class InstructionSet {
public:
    InstructionSet();

    // Retorna nullptr quando o mnemomico nao pertence a esta ISA.
    //
    // O ponteiro permanece valido enquanto este InstructionSet existir,
    // pois o registro e montado no construtor e nao e alterado depois disso.
    const InstructionDefinition* find(const std::string&) const;

private:
    std::unordered_map<std::string, InstructionDefinition> instructions;
};

}