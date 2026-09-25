#include "compiler/Compiler.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    // ------------------------------------------------------------
    // ARGUMENTOS
    // ------------------------------------------------------------
    //
    // Uso:
    //
    //     asmc <entrada.asm> <saida.bin> [arquitetura]
    //
    // Se a arquitetura nao for informada, arch8 sera utilizada
    // automaticamente.
    //

    if (argc != 3 && argc != 4) {
        std::cerr
            << "Uso: asmc <entrada.asm> <saida.bin> [arquitetura]\n";

        return 1;
    }

    // ------------------------------------------------------------
    // ARQUITETURA
    // ------------------------------------------------------------

    compiler::Architecture architecture =
        compiler::Architecture::arch8;

    if (argc == 4) {

        const std::string arch = argv[3];

        if (arch == "arch8") {
            architecture = compiler::Architecture::arch8;
        }
        else {
            std::cerr
                << "Erro: arquitetura não encontrada: "
                << arch
                << "\n";

            return 1;
        }
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

    // Carregar o arquivo inteiro permite que o lexer mantenha
    // linhas e colunas exatas nas mensagens de erro, inclusive
    // em comentarios e linhas vazias.
    std::string source(
        std::istreambuf_iterator<char>(in),
        {}
    );

    try {

        // --------------------------------------------------------
        // COMPILAR
        // --------------------------------------------------------

        compiler::Compiler c(architecture);

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
        //
        // O formato externo e big-endian:
        // primeiro o byte alto, depois o byte baixo.
        //
        // A instrucao possui 16 bits.
        // A arquitetura determina as demais caracteristicas
        // da CPU, incluindo a largura maxima do endereco.
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
        // RESULTADO
        // --------------------------------------------------------

        std::cout
            << "Compilacao concluida.\n"
            << "Palavras geradas: "
            << code.size()
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