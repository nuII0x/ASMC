#pragma once
#include <cstddef>
#include <cstdint>

namespace cpu {

// Types that directly represent the CPU's hardware buses. Keeping these aliases
// centralized prevents data and address widths from being scattered throughout
// the compiler as the architecture evolves.
using Byte = std::uint8_t;
using Address = std::uint8_t;
using Word = std::uint16_t;

// Used both to document the architecture and to validate future extensions
// that need to know the physical width of the buses.
constexpr std::size_t BYTE_BITS = 8;
constexpr std::size_t WORD_BITS = 16;

}