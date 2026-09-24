#pragma once
#include "lexer/Token.hpp"
#include <string>
#include <vector>

namespace compiler {

// Converte o texto assembly em tokens sem interpretar a ISA. Essa separacao faz
// com que novos mnemomicos sejam adicionados na InstructionSet, sem alterar a
// leitura basica de arquivos fonte.
class Lexer {
public:
    explicit Lexer(const std::string&);

    // Sempre termina a sequencia com EndOfFile, inclusive para arquivo vazio.
    std::vector<Token> tokenize();

private:
    // peek observa o caractere atual; advance o consome e atualiza a posicao.
    char peek() const;
    char advance();

    // Espacos comuns sao ignorados, mas '\n' e preservado como delimitador de
    // instrucao para que o parser consiga manter linhas e diagnosticos corretos.
    void skipWhitespace();
    void skipComment();

    // Numeros aceitam letras durante a leitura para preservar prefixos como 0x;
    // a interpretacao decimal ou hexadecimal pertence ao parser.
    Token readNumber();
    Token readIdentifier();
    Token makeToken(TokenType, const std::string&, int, int);

    std::string source;
    // position e indice baseado em zero no texto; line e column sao baseados em
    // um porque aparecem diretamente para quem escreveu o arquivo assembly.
    std::size_t position = 0;
    int line = 1;
    int column = 1;
};

}
