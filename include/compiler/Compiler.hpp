#pragma once

#include "assembler/Assembler.hpp"
#include "cpu/InstructionSet.hpp"

#include <string>
#include <vector>

namespace compiler {

class Compiler {
public:
    explicit Compiler(
        AddressWidth addressWidth = AddressWidth::Bits8
    );

    std::vector<cpu::Word> compile(
        const std::string& source
    );

private:
    AddressWidth addressWidth;
};

}