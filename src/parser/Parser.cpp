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
    throw std::runtime_error("Linha " + std::to_string(peek().line) + ": " + m);
}

Program Parser::parse()
{
    Program p;

    // Linhas vazias sao semanticamente neutras. Todo o restante e delegado a
    // parseLine, que consome uma label ou instrucao por vez.
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
        error("esperado identificador ou instrucao");
    }

    Token first = advance();

    if (match(TokenType::Colon)) {
        // A label aponta para a proxima instrucao fonte. O assembler transforma
        // esse indice em endereco real de ROM, porque pseudo-instrucoes como
        // jmp podem ocupar mais de uma Word de 16 bits.
        p.labels.push_back({first.text, p.instructions.size()});

        // Se houver outra palavra na mesma linha, como "inicio: nop", nao a
        // consumimos aqui. O proximo ciclo de parse a tratara como instrucao.
        if (peek().type == TokenType::NewLine) {
            advance();
        }

        return;
    }

    Instruction i{first.text, {}, first.line};

    // A gramatica aceita operandos separados por virgula. A quantidade aceita
    // por cada mnemomico e validada mais tarde pelo assembler.
    if (peek().type != TokenType::NewLine && peek().type != TokenType::EndOfFile) {
        i.operands.push_back(parseOperand());

        while (match(TokenType::Comma)) {
            i.operands.push_back(parseOperand());
        }
    }

    // Caracteres que o lexer nao reconhece devem falhar logo no parser, antes
    // de qualquer tentativa de gerar um binario parcial.
    if (peek().type == TokenType::Unknown) {
        error("token inesperado: " + peek().text);
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

        // O lexer preserva 0x/0X no texto; aqui escolhemos a base antes de usar
        // stoll para que 42 e 0x2A representem o mesmo valor no AST.
        if (t.text.size() > 2 && t.text[0] == '0' && (t.text[1] == 'x' || t.text[1] == 'X')) {
            base = 16;
        }

        return {Operand::Type::Number, t.text, std::stoll(t.text, nullptr, base)};
    }

    if (t.type == TokenType::Identifier) {
        // Uma referencia ainda nao resolvida pode ser uma label. O assembler a
        // substitui pelo endereco final depois de conhecer todo o programa.
        return {Operand::Type::Identifier, t.text, 0};
    }

    error("operando invalido: " + t.text);
    return {};
}

}
