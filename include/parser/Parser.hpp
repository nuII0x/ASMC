#pragma once
#include "lexer/Token.hpp"
#include "parser/AST.hpp"
#include <string>
#include <vector>

namespace compiler {

// Converts the token sequence into a Program. The parser validates the assembly
// structure, while mnemonic validity and operand constraints belong to the
// assembler and InstructionSet.
class Parser {
public:
    explicit Parser(const std::vector<Token>&);

    // Consumes all tokens up to EndOfFile and preserves labels as instruction
    // addresses, not as byte offsets.
    Program parse();

private:
    // Navigation helpers: peek does not consume; advance consumes; match consumes
    // only when the expected token type is present.
    const Token& peek() const;
    const Token& advance();
    bool match(TokenType);

    // A line may contain a label, an instruction, or both (label: nop).
    void parseLine(Program&);
    Operand parseOperand();

    // All syntax errors go through this function so the current line is included.
    void error(const std::string&);

    const std::vector<Token>& tokens;
    std::size_t position = 0;
};

}