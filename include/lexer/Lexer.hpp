#pragma once
#include "lexer/Token.hpp"
#include <string>
#include <vector>

namespace compiler {

// Converts assembly source text into tokens without interpreting the ISA.
// This separation allows new mnemonics to be added to InstructionSet
// without changing the basic source-reading logic.
class Lexer {
public:
    explicit Lexer(const std::string&);

    // Always terminates the token sequence with EndOfFile,
    // including for an empty source file.
    std::vector<Token> tokenize();

private:
    // peek inspects the current character; advance consumes it
    // and updates the current position.
    char peek() const;
    char advance();

    // Whitespace is ignored, but '\n' is preserved as an instruction
    // delimiter so the parser can maintain correct line information
    // and diagnostics.
    void skipWhitespace();
    void skipComment();

    // Numbers allow letters while being read to preserve prefixes
    // such as 0x; decimal or hexadecimal interpretation belongs
    // to the parser.
    Token readNumber();
    Token readIdentifier();
    Token makeToken(TokenType, const std::string&, int, int);

    std::string source;

    // position is a zero-based index into the source text;
    // line and column are one-based because they are displayed
    // directly in assembly source diagnostics.
    std::size_t position = 0;
    int line = 1;
    int column = 1;
};

}