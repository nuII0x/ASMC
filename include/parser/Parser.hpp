#pragma once
#include "lexer/Token.hpp"
#include "parser/AST.hpp"
#include <string>
#include <vector>

namespace compiler {

// Transforma a sequencia de tokens em Program. O parser valida a estrutura do
// assembly, enquanto a validade de cada mnemomico e de seus sinais pertence ao
// assembler e a InstructionSet.
class Parser {
public:
    explicit Parser(const std::vector<Token>&);

    // Consome todos os tokens ate EndOfFile e preserva labels como enderecos de
    // instrucoes, nao como offsets de bytes.
    Program parse();

private:
    // Helpers de navegacao: peek nao consome; advance consome; match consome
    // somente quando o tipo esperado esta presente.
    const Token& peek() const;
    const Token& advance();
    bool match(TokenType);

    // Uma linha pode conter uma label, uma instrucao ou ambos (label: nop).
    void parseLine(Program&);
    Operand parseOperand();

    // Todos os erros sintaticos passam por aqui para incluir a linha atual.
    void error(const std::string&);

    const std::vector<Token>& tokens;
    std::size_t position = 0;
};

}
