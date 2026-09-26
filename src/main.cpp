#include "compiler/Compiler.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    // ------------------------------------------------------------
    // ARGUMENTS
    // ------------------------------------------------------------
    //
    // Use:
    //
    //     asmc <input.asm> <output.bin> [architecture]
    //
    // If the architecture not informed, then the arch8 will be used
    // automatically.
    //

    if (argc != 3 && argc != 4) {
        std::cerr
            << "Use: asmc <input.asm> <output.bin> [architecture]\n";

        return 1;
    }

    // ------------------------------------------------------------
    // ARCHITECTURE
    // ------------------------------------------------------------

    compiler::Architecture architecture =
    compiler::Architecture::arch8;

    if (argc == 4) {

        const std::string arch = argv[3];

        if (arch == "arch8") {
            // Default architecture.
        }
        /*else if (arch == "arch16") {
            architecture = compiler::Architecture::arch16;
        }
        else if (arch == "arch32") {
            architecture = compiler::Architecture::arch32;
        }
        else if (arch == "arch64") {
            architecture = compiler::Architecture::arch64;
        }*/
        else {
            std::cerr
                << "Error: architecture not found: "
                << arch
                << "\n";

            return 1;
        }
    }

    // ------------------------------------------------------------
    // OPEN INPUT FILE
    // ------------------------------------------------------------

    std::ifstream in(argv[1]);

    if (!in) {
        std::cerr
            << "Error: It was not possible to open "
            << argv[1]
            << "\n";

        return 1;
    }

    // Loading the entire file allows the lexer to maintain
    // exact line and column positions in error messages, including
    // comments and empty lines.
    std::string source(
        std::istreambuf_iterator<char>(in),
        {}
    );

    try {

        // --------------------------------------------------------
        // COMPILE
        // --------------------------------------------------------

        compiler::Compiler c(architecture);

        auto code = c.compile(source);

        // --------------------------------------------------------
        // OPEN OUTPUT FILE
        // --------------------------------------------------------

        std::ofstream out(
            argv[2],
            std::ios::binary
        );

        if (!out) {
            std::cerr
                << "Error in creating file "
                << argv[2]
                << "\n";

            return 1;
        }
        // --------------------------------------------------------
        // WRITE BINARY
        // --------------------------------------------------------
        //
        // The external format is big-endian:
        // first the high byte, then the low byte.
        //
        // The instruction is 16 bits wide.
        // The architecture determines the remaining characteristics
        // of the CPU, including the maximum address width.
        //
        for (auto w : code) {

            out.put(
                static_cast<char>(
                    (w >> 8) & 0xFF
                )
            );

            out.put(
                static_cast<char>(
                    w & 0xFF
                )
            );
        }

        // --------------------------------------------------------
        // RESULT
        // --------------------------------------------------------

        std::cout
            << "Compilation complete.\n"
            << "Words generated: "
            << code.size()
            << "\n";
    }
    catch (const std::exception& e) {

        std::cerr
            << "Error: "
            << e.what()
            << "\n";

        return 1;
    }

    return 0;
}