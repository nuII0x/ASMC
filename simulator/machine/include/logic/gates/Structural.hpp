#pragma once

#include "logic/types/Bit.hpp"

#include <cstddef>

namespace structural::logic {

// ============================================================
// NAND
// Fundamental logical primitive.
// Every other gate is built from NAND.
// ============================================================

class gate {
public:

    // --------------------------------------------------------
    // 1-bit gates
    // --------------------------------------------------------

    [[nodiscard]]
    static constexpr Bit NAND(Bit a, Bit b) noexcept
    {
        return not (a and b);
    }

    [[nodiscard]]
    static constexpr Bit NOT(Bit a) noexcept
    {
        return NAND(a, a);
    }

    [[nodiscard]]
    static constexpr Bit AND(Bit a, Bit b) noexcept
    {
        const Bit n = NAND(a, b);
        return NOT(n);
    }

    [[nodiscard]]
    static constexpr Bit OR(Bit a, Bit b) noexcept
    {
        return NAND(
            NOT(a),
            NOT(b)
        );
    }

    [[nodiscard]]
    static constexpr Bit XOR(Bit a, Bit b) noexcept
    {
        const Bit n = NAND(a, b);

        return NAND(
            NAND(a, n),
            NAND(b, n)
        );
    }

    [[nodiscard]]
    static constexpr Bit XNOR(Bit a, Bit b) noexcept
    {
        const Bit x = XOR(a, b);
        return NOT(x);
    }


    // --------------------------------------------------------
    // N-bit gates
    // --------------------------------------------------------

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> NAND(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>::value; ++i)
            result[i] = NAND(a[i], b[i]);

        return result;
    }


    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> NOT(
        const bits<T>& a
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>::value; ++i)
            result[i] = NOT(a[i]);

        return result;
    }


    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> AND(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>::value; ++i)
            result[i] = AND(a[i], b[i]);

        return result;
    }


    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> OR(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>::value; ++i)
            result[i] = OR(a[i], b[i]);

        return result;
    }


    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> XOR(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>::value; ++i)
            result[i] = XOR(a[i], b[i]);

        return result;
    }


    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> XNOR(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>::value; ++i)
            result[i] = XNOR(a[i], b[i]);

        return result;
    }
};

} // namespace logic
