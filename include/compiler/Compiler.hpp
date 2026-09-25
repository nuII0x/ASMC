#pragma once

#include "assembler/Assembler.hpp"
#include "cpu/InstructionSet.hpp"

#include <string>
#include <vector>

namespace compiler {

enum class Architecture {
    arch8
};

class Compiler {
public:
    explicit Compiler(
        Architecture architecture = Architecture::arch8
    );

    std::vector<cpu::Word> compile(
        const std::string& source
    );

private:
    Architecture architecture;
};

}