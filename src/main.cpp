#include "compiler/Compiler.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr
            << "Uso: mycpu <entrada.asm> <saida.bin> <-8|-16|-32>\n";
        return 1;
    }

    // ------------------------------------------------------------
    // LARGURA DO ENDERECO
    // ------------------------------------------------------------

    compiler::AddressWidth addressWidth;

    const std::string width = argv[3];

    if (width == "-8") {
        addressWidth = compiler::AddressWidth::Bits8;
    }
    else if (width == "-16") {
        addressWidth = compiler::AddressWidth::Bits16;
    }
    else if (width == "-32") {
        addressWidth = compiler::AddressWidth::Bits32;
    }
    else {
        std::cerr
            << "Erro: largura de endereco invalida: "
            << width
            << "\n"
            << "Use -8, -16 ou -32.\n";

        return 1;
    }

    // ------------------------------------------------------------
    // ABRIR ARQUIVO DE ENTRADA
    // ------------------------------------------------------------

    std::ifstream in(argv[1]);

    if (!in) {
        std::cerr
            << "Erro: nao foi possivel abrir "
            << argv[1]
            << "\n";

        return 1;
    }

    // Carregar o arquivo inteiro permite que o lexer mantenha linhas
    // e colunas exatas nas mensagens de erro, inclusive em comentarios
    // e linhas vazias.
    std::string source(
        std::istreambuf_iterator<char>(in),
        {}
    );

    try {

        // --------------------------------------------------------
        // COMPILAR
        // --------------------------------------------------------

        compiler::Compiler c(addressWidth);

        auto code = c.compile(source);

        // --------------------------------------------------------
        // ABRIR ARQUIVO DE SAIDA
        // --------------------------------------------------------

        std::ofstream out(
            argv[2],
            std::ios::binary
        );

        if (!out) {
            std::cerr
                << "Erro ao criar "
                << argv[2]
                << "\n";

            return 1;
        }

        // --------------------------------------------------------
        // ESCREVER BINARIO
        // --------------------------------------------------------

        // O formato externo e big-endian de forma explicita:
        // primeiro o byte alto, depois o baixo.
        //
        // Isso independe do endianness da maquina host.
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
        // RESULTADO
        // --------------------------------------------------------

        std::cout
            << "Compilacao concluida.\n"
            << "Palavras geradas: "
            << code.size()
            << "\n"
            << "Largura de endereco: "
            << width
            << "\n";

    }
    catch (const std::exception& e) {

        std::cerr
            << "Erro: "
            << e.what()
            << "\n";

        return 1;
    }

    return 0;
}