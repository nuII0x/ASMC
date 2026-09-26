#pragma once
#include <string>

namespace compiler {

// Lexical categories recognized by the current grammar. Some tokens (registers,
// brackets, and operators) already exist to support future language extensions,
// even though the current parser still accepts only numbers and identifiers as
// operands.
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
    // Original source text; the lexer does not normalize it.
    std::string text;
    // Human-readable positions, both one-based, for useful error diagnostics.
    int line;
    int column;
};

}