#include "compiler/Compiler.hpp"

#include "assembler/Assembler.hpp"
#include "cpu/InstructionSet.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace compiler {

Compiler::Compiler(Architecture architecture)
    : architecture(architecture)
{
}

std::vector<cpu::Word> Compiler::compile(
    const std::string& s
)
{
    // Cada etapa tem uma responsabilidade pequena e testavel.
    // O texto vira tokens, os tokens viram AST e somente entao
    // a ISA gera as palavras binarias.

    Lexer l(s);
    auto t = l.tokenize();

    Parser p(t);
    auto program = p.parse();

    cpu::InstructionSet isa;

    AddressWidth addressWidth;

    switch (architecture) {
        case Architecture::arch8:
            addressWidth = AddressWidth::Bits8;
            break;
    }

    Assembler a(
        isa,
        addressWidth
    );

    return a.assemble(program);
}

}