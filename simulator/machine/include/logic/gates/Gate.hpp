#pragma once

#include "logic/types/Bit.hpp"
#include <array>
#include <cstddef>

namespace logic {

class Gate {
public:
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
        return a and b;
    }

    [[nodiscard]]
    static constexpr Bit OR(Bit a, Bit b) noexcept {
        return a or b;
    }

    [[nodiscard]]
    static constexpr Bit XOR(Bit a, Bit b) noexcept {
        return a xor b;
    }

    [[nodiscard]]
    static constexpr Bit XNOR(Bit a, Bit b) noexcept {
        return not (a xor b);
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> NAND(
        const Bits<N>& a,
        const Bits<N>& b
    ) noexcept {
        Bits<N> result{};

        for (std::size_t i = 0; i < N; ++i)
            result[i] = NAND(a[i], b[i]);

        return result;
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> NOT(
        const Bits<N>& a
    ) noexcept {
        Bits<N> result{};

        for (std::size_t i = 0; i < N; ++i)
            result[i] = NOT(a[i]);

        return result;
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> AND(
        const Bits<N>& a,
        const Bits<N>& b
    ) noexcept {
        Bits<N> result{};

        for (std::size_t i = 0; i < N; ++i)
            result[i] = AND(a[i], b[i]);

        return result;
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> OR(
        const Bits<N>& a,
        const Bits<N>& b
    ) noexcept {
        Bits<N> result{};

        for (std::size_t i = 0; i < N; ++i)
            result[i] = OR(a[i], b[i]);

        return result;
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> XOR(
        const Bits<N>& a,
        const Bits<N>& b
    ) noexcept {
        Bits<N> result{};

        for (std::size_t i = 0; i < N; ++i)
            result[i] = XOR(a[i], b[i]);

        return result;
    }

    template <std::size_t N>
    [[nodiscard]]
    static constexpr Bits<N> XNOR(
        const Bits<N>& a,
        const Bits<N>& b
    ) noexcept {
        Bits<N> result{};

        for (std::size_t i = 0; i < N; ++i)
            result[i] = XNOR(a[i], b[i]);

        return result;
    }
};

} // namespace logic