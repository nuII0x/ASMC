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
// Architectural numeric types and the fundamental logical bit.
//
//     Bit   -> single logical signal
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
// Architectural value concept
// ============================================================
//
// Types that can be represented as a physical collection of
// bits.
//
// Bit is included so that a single wire can also be represented
// by bits<Bit> and Bus<Bit>.
//

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
// Defines how many physical bits belong to each logical type.
//
// BitCount is the single source of truth for the physical width
// of the types used by the logic system.
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
// Represents the individual physical signals that form a
// logical value.
//
// Unlike Byte, Word, DWord and QWord, bits<T> does not represent
// a numeric value. Each element is an independent Bit signal.
//
// Examples:
//
//     bits<Bit>   -> one physical signal
//     bits<Byte>  -> eight physical signals
//     bits<Word>  -> sixteen physical signals
//     bits<DWord> -> thirty-two physical signals
//     bits<QWord> -> sixty-four physical signals
//

template <LogicValue T>
using bits = std::array<Bit, BitCount<T>>;

} // namespace logic