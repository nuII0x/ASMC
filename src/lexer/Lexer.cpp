#include "lexer/Lexer.hpp"
#include <cctype>

namespace compiler {

Lexer::Lexer(const std::string& s) : source(s) {}

char Lexer::peek() const
{
    // The sentinel prevents out-of-bounds access and simplifies all
    // reading loops: '\0' always represents end of input.
    return position >= source.size() ? '\0' : source[position];
}

char Lexer::advance()
{
    char c = peek();

    if (c == '\0') {
        return c;
    }

    ++position;

    if (c == '\r') {
        // CRLF is treated as a single line break.
        if (peek() == '\n') {
            ++position;
        }

        ++line;
        column = 1;

        return '\n';
    }

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
    // Newline is not skipped here: it delimits instructions
    // and must be emitted as a token.
    while (peek() == ' ' || peek() == '\t' || peek() == '\r') {
        advance();
    }
}

void Lexer::skipComment()
{
    // ';' has already been recognized by tokenize. The line break
    // remains untouched so it can be emitted as NewLine on the next cycle.
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

    // Consume the entire sequence so that "0xFF" remains a single token.
    // The parser determines the base and converts the text to a numeric value.
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

    // Dots and underscores allow more descriptive labels, such as
    // loop.main or output_buffer, without requiring special parser rules.
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

        // Comments never reach the parser; only the line break that
        // terminates them remains relevant to the program structure.
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

        // The first character determines whether the text is a number
        // or an identifier.
        if (std::isdigit((unsigned char)c)) {
            t.push_back(readNumber());
            continue;
        }

        if (std::isalpha((unsigned char)c) || c == '_') {
            t.push_back(readIdentifier());
            continue;
        }

        // Single-character tokens are handled here so the reading functions
        // can remain focused on consuming text sequences.
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
            t.push_back(
                makeToken(
                    TokenType::Unknown,
                    std::string(1, c),
                    l,
                    co
                )
            );
            break;
        }
    }

    return t;
}

}