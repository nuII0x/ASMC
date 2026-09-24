#include "lexer/Lexer.hpp"
#include <cctype>

namespace compiler {

Lexer::Lexer(const std::string& s) : source(s) {}

char Lexer::peek() const
{
    // O sentinela evita acessar source fora dos limites e simplifica todos os
    // lacos de leitura: '\0' sempre significa fim do arquivo.
    return position >= source.size() ? '\0' : source[position];
}

char Lexer::advance()
{
    char c = peek();
    if (c == '\0') {
        return c;
    }

    ++position;

    // A coluna aponta para o proximo caractere a ser lido. Ao cruzar uma quebra
    // de linha, a proxima posicao visivel para o usuario passa a ser coluna 1.
    if (c == '\n') {
        ++line;
        column = 1;
    } else {
        ++column;
    }

    return c;
}

void Lexer::skipWhitespace()
{
    // Newline nao entra aqui: ele delimita instrucoes e precisa virar token.
    while (peek() == ' ' || peek() == '\t' || peek() == '\r') {
        advance();
    }
}

void Lexer::skipComment()
{
    // O ';' ja foi reconhecido por tokenize. A quebra de linha fica intacta para
    // ser emitida no proximo ciclo como NewLine.
    while (peek() != '\0' && peek() != '\n') {
        advance();
    }
}

Token Lexer::makeToken(TokenType t, const std::string& s, int l, int c)
{
    return {t, s, l, c};
}

Token Lexer::readNumber()
{
    int l = line;
    int c = column;
    std::string v;

    // Consumir a sequencia inteira preserva "0xFF" em um unico token. O parser
    // decide a base e converte o texto para um valor numerico.
    while (std::isalnum((unsigned char)peek())) {
        v += advance();
    }

    return makeToken(TokenType::Number, v, l, c);
}

Token Lexer::readIdentifier()
{
    int l = line;
    int c = column;
    std::string v;

    // Pontos e underscores permitem labels mais descritivas, como loop.main ou
    // buffer_saida, sem precisarem de regras especiais no parser.
    while (std::isalnum((unsigned char)peek()) || peek() == '_' || peek() == '.') {
        v += advance();
    }

    return makeToken(TokenType::Identifier, v, l, c);
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> t;

    for (;;) {
        skipWhitespace();

        char c = peek();
        if (c == '\0') {
            t.push_back(makeToken(TokenType::EndOfFile, "", line, column));
            break;
        }

        // Comentarios nao chegam ao parser; apenas a quebra de linha que os
        // encerra continua relevante para a estrutura do programa.
        if (c == ';') {
            skipComment();
            continue;
        }

        if (c == '\n') {
            int l = line;
            int co = column;
            advance();
            t.push_back(makeToken(TokenType::NewLine, "\\n", l, co));
            continue;
        }

        // A primeira letra define se o texto e um numero ou identificador.
        if (std::isdigit((unsigned char)c)) {
            t.push_back(readNumber());
            continue;
        }

        if (std::isalpha((unsigned char)c) || c == '_') {
            t.push_back(readIdentifier());
            continue;
        }

        // Tokens de um caractere sao tratados aqui para manter as funcoes de
        // leitura focadas somente em sequencias de texto.
        int l = line;
        int co = column;

        switch (c) {
        case ',':
            advance();
            t.push_back(makeToken(TokenType::Comma, ",", l, co));
            break;
        case ':':
            advance();
            t.push_back(makeToken(TokenType::Colon, ":", l, co));
            break;
        case '[':
            advance();
            t.push_back(makeToken(TokenType::LBracket, "[", l, co));
            break;
        case ']':
            advance();
            t.push_back(makeToken(TokenType::RBracket, "]", l, co));
            break;
        case '+':
            advance();
            t.push_back(makeToken(TokenType::Plus, "+", l, co));
            break;
        case '-':
            advance();
            t.push_back(makeToken(TokenType::Minus, "-", l, co));
            break;
        default:
            advance();
            t.push_back(makeToken(TokenType::Unknown, std::string(1, c), l, co));
            break;
        }
    }

    return t;
}

}
