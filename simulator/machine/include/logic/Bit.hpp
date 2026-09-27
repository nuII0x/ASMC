#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>

namespace logic {

// ============================================================
// Logical types
// ============================================================

using Bit   = bool;
using Byte  = std::uint8_t;
using Word  = std::uint16_t;
using DWord = std::uint32_t;
using QWord = std::uint64_t;


// ============================================================
// Architectural value concept
// ============================================================

template <typename T>
concept LogicValue =
    std::same_as<T, Byte>  ||
    std::same_as<T, Word>  ||
    std::same_as<T, DWord> ||
    std::same_as<T, QWord>;


// ============================================================
// Bit count
// ============================================================

template <typename T>
struct BitCount;

template <>
struct BitCount<Bit> {
    static constexpr std::size_t value = 1;
};

template <>
struct BitCount<Byte> {
    static constexpr std::size_t value = 8;
};

template <>
struct BitCount<Word> {
    static constexpr std::size_t value = 16;
};

template <>
struct BitCount<DWord> {
    static constexpr std::size_t value = 32;
};

template <>
struct BitCount<QWord> {
    static constexpr std::size_t value = 64;
};


// ============================================================
// Physical bit collection
// ============================================================

template <typename T>
using bits = std::array<Bit, BitCount<T>::value>;

} // namespace logic