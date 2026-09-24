#pragma once
#include <string>

namespace compiler {

// Categorias lexicas entendidas pela gramatica atual. Alguns tokens (Register,
// colchetes e operadores) ja existem para facilitar a evolucao da linguagem,
// embora o parser atual ainda aceite apenas numeros e identificadores como
// operandos.
enum class TokenType {
    Identifier,
    Number,
    Register,
    Comma,
    Colon,
    LBracket,
    RBracket,
    Plus,
    Minus,
    NewLine,
    EndOfFile,
    Unknown,
};

struct Token {
    TokenType type;
    // Trecho original do fonte; nao e normalizado pelo lexer.
    std::string text;
    // Posicoes humanas, ambas iniciando em 1, para mensagens de erro uteis.
    int line;
    int column;
};

}
