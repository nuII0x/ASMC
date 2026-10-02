#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>

namespace logic {

// ============================================================
// Logical types
// ============================================================
//
// Fundamental types used by both the compiler and the CPU
// simulator.
//
//     Bit   -> 1-bit logical signal
//     Byte  -> 8-bit value
//     Word  -> 16-bit value
//     DWord -> 32-bit value
//     QWord -> 64-bit value
//

using Bit   = bool;
using Byte  = std::uint8_t;
using Word  = std::uint16_t;
using DWord = std::uint32_t;
using QWord = std::uint64_t;


// ============================================================
// Bit widths
// ============================================================
//
// Physical width of each architectural type.
//

inline constexpr std::size_t BYTE_BITS = 8;
inline constexpr std::size_t WORD_BITS = 16;


// ============================================================
// Architectural value concept
// ============================================================

template <typename T>
concept LogicValue =
    std::same_as<T, Bit>   ||
    std::same_as<T, Byte>  ||
    std::same_as<T, Word>  ||
    std::same_as<T, DWord> ||
    std::same_as<T, QWord>;


// ============================================================
// Bit count
// ============================================================
//
// Number of physical bits belonging to each logical type.
//

template <LogicValue T>
inline constexpr std::size_t BitCount =
    std::same_as<T, Bit>
        ? 1
        : sizeof(T) * 8;


// ============================================================
// Physical bit collection
// ============================================================
//
// Represents the individual physical signals that compose a
// logical value.
//
// This is different from Byte, Word, DWord and QWord:
// those represent numeric values, while bits<T> represents
// their individual physical signals.
//

template <LogicValue T>
using bits = std::array<Bit, BitCount<T>>;

} // namespace logic
