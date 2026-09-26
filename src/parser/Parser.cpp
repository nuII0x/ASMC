#include "parser/Parser.hpp"
#include <stdexcept>

namespace compiler {

Parser::Parser(const std::vector<Token>& t) : tokens(t) {}

const Token& Parser::peek() const
{
    return tokens.at(position);
}

const Token& Parser::advance()
{
    return tokens.at(position++);
}

bool Parser::match(TokenType t)
{
    if (peek().type != t) {
        return false;
    }

    advance();
    return true;
}

void Parser::error(const std::string& m)
{
    throw std::runtime_error("Line " + std::to_string(peek().line) + ": " + m);
}

Program Parser::parse()
{
    Program p;

    // Empty lines are semantically neutral. Everything else is delegated to
    // parseLine, which consumes one label or instruction at a time.
    while (peek().type != TokenType::EndOfFile) {
        if (match(TokenType::NewLine)) {
            continue;
        }

        parseLine(p);
    }

    return p;
}

void Parser::parseLine(Program& p)
{
    if (peek().type != TokenType::Identifier) {
        error("expected identifier or instruction");
    }

    Token first = advance();

    if (match(TokenType::Colon)) {
        // The label points to the next source instruction. The assembler converts
        // this index into the actual ROM address because pseudo-instructions such
        // as jmp may occupy more than one 16-bit word.
        p.labels.push_back({first.text, p.instructions.size()});

        // If another token appears on the same line, such as "start: nop", we
        // do not consume it here. The next parse cycle will handle it as an instruction.
        if (peek().type == TokenType::NewLine) {
            advance();
        }

        return;
    }

    Instruction i{first.text, {}, first.line};

    // The grammar accepts operands separated by commas. The number of operands
    // accepted by each mnemonic is validated later by the assembler.
    if (peek().type != TokenType::NewLine && peek().type != TokenType::EndOfFile) {
        i.operands.push_back(parseOperand());

        while (match(TokenType::Comma)) {
            i.operands.push_back(parseOperand());
        }
    }

    // Characters that the lexer does not recognize must fail in the parser,
    // before any attempt is made to generate a partial binary.
    if (peek().type == TokenType::Unknown) {
        error("unexpected token: " + peek().text);
    }

    if (peek().type == TokenType::NewLine) {
        advance();
    }

    p.instructions.push_back(std::move(i));
}

Operand Parser::parseOperand()
{
    Token t = advance();

    if (t.type == TokenType::Number) {
        int base = 10;

        // The lexer preserves the 0x/0X prefix in the text; here we select the
        // base before calling stoll so that 42 and 0x2A represent the same value in the AST.
        if (t.text.size() > 2 && t.text[0] == '0' && (t.text[1] == 'x' || t.text[1] == 'X')) {
            base = 16;
        }

        return {Operand::Type::Number, t.text, std::stoll(t.text, nullptr, base)};
    }

    if (t.type == TokenType::Identifier) {
        // An unresolved reference may be a label. The assembler replaces it
        // with the final address after the entire program is known.
        return {Operand::Type::Identifier, t.text, 0};
    }

    error("invalid operand: " + t.text);
    return {};
}

}