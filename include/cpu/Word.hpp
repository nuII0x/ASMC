#pragma once
#include <cstddef>
#include <cstdint>

namespace cpu {

// Tipos que representam diretamente os barramentos da CPU. Manter estes aliases
// centralizados evita que o tamanho de dados e enderecos fique espalhado pelo
// compilador quando a arquitetura evoluir.
using Byte = std::uint8_t;
using Address = std::uint8_t;
using Word = std::uint16_t;

// Usados tanto para documentar a arquitetura quanto para validar futuras
// extensoes que precisem conhecer o tamanho fisico dos barramentos.
constexpr std::size_t BYTE_BITS = 8;
constexpr std::size_t WORD_BITS = 16;

}
