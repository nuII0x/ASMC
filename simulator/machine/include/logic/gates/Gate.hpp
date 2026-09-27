#pragma once

#include "logic/types/Bit.hpp"
#include <cstddef>

namespace logic {

class Gate {
public:
    // Single-bit gates

    [[nodiscard]]
    static constexpr Bit NAND(Bit a, Bit b) noexcept {
        return not (a bitand b);
    }

    [[nodiscard]]
    static constexpr Bit NOT(Bit a) noexcept {
        return not a;
    }

    [[nodiscard]]
    static constexpr Bit AND(Bit a, Bit b) noexcept {
        return a bitand b;
    }

    [[nodiscard]]
    static constexpr Bit OR(Bit a, Bit b) noexcept {
        return a bitor b;
    }

    [[nodiscard]]
    static constexpr Bit XOR(Bit a, Bit b) noexcept {
        return a xor b;
    }

    [[nodiscard]]
    static constexpr Bit XNOR(Bit a, Bit b) noexcept {
        return not (a xor b);
    }


    // N-bit gates

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> NAND(
        Bits<N> a,
        Bits<N> b
    ) noexcept {
        return static_cast<Bits<N>>(compl(a bitand b));
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> NOT(
        Bits<N> a
    ) noexcept {
        return static_cast<Bits<N>>(compl a);
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> AND(
        Bits<N> a,
        Bits<N> b
    ) noexcept {
        return static_cast<Bits<N>>(a bitand b);
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> OR(
        Bits<N> a,
        Bits<N> b
    ) noexcept {
        return static_cast<Bits<N>>(a bitor b);
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> XOR(
        Bits<N> a,
        Bits<N> b
    ) noexcept {
        return static_cast<Bits<N>>(a xor b);
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> XNOR(
        Bits<N> a,
        Bits<N> b
    ) noexcept {
        return static_cast<Bits<N>>(compl(a xor b));
    }
};

} // namespace logic