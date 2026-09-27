#pragma once

#include "logic/types/Bit.hpp"

#include <cstddef>

namespace structural::logic {

// ============================================================
// Gates
// ============================================================

class Gate {
public:

    // ========================================================
    // NAND
    // Fundamental logical primitive.
    // ========================================================

    // --------------------------------------------------------
    // Single-bit
    // --------------------------------------------------------

    [[nodiscard]]
    static constexpr Bit NAND(Bit a, Bit b) noexcept
    {
        return not (a bitand b);
    }

    // --------------------------------------------------------
    // Multi-bit
    // --------------------------------------------------------

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> NAND(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = NAND(a[i], b[i]);

        return result;
    }


    // ========================================================
    // NOT
    // ========================================================

    // --------------------------------------------------------
    // Single-bit
    // --------------------------------------------------------

    [[nodiscard]]
    static constexpr Bit NOT(Bit a) noexcept
    {
        return NAND(a, a);
    }

    // --------------------------------------------------------
    // Multi-bit
    // --------------------------------------------------------

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> NOT(
        const bits<T>& a
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = NOT(a[i]);

        return result;
    }


    // ========================================================
    // AND
    // ========================================================

    // --------------------------------------------------------
    // Single-bit
    // --------------------------------------------------------

    [[nodiscard]]
    static constexpr Bit AND(Bit a, Bit b) noexcept
    {
        const Bit n = NAND(a, b);
        return NAND(n, n);
    }

    // --------------------------------------------------------
    // Multi-bit
    // --------------------------------------------------------

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> AND(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = AND(a[i], b[i]);

        return result;
    }


    // ========================================================
    // OR
    // ========================================================

    // --------------------------------------------------------
    // Single-bit
    // --------------------------------------------------------

    [[nodiscard]]
    static constexpr Bit OR(Bit a, Bit b) noexcept
    {
        return NAND(
            NAND(a, a),
            NAND(b, b)
        );
    }

    // --------------------------------------------------------
    // Multi-bit
    // --------------------------------------------------------

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> OR(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = OR(a[i], b[i]);

        return result;
    }


    // ========================================================
    // XOR
    // ========================================================

    // --------------------------------------------------------
    // Single-bit
    // --------------------------------------------------------

    [[nodiscard]]
    static constexpr Bit XOR(Bit a, Bit b) noexcept
    {
        const Bit n = NAND(a, b);

        return NAND(
            NAND(a, n),
            NAND(b, n)
        );
    }

    // --------------------------------------------------------
    // Multi-bit
    // --------------------------------------------------------

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> XOR(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = XOR(a[i], b[i]);

        return result;
    }


    // ========================================================
    // XNOR
    // ========================================================

    // --------------------------------------------------------
    // Single-bit
    // --------------------------------------------------------

    [[nodiscard]]
    static constexpr Bit XNOR(Bit a, Bit b) noexcept
    {
        const Bit x = XOR(a, b);
        return NAND(x, x);
    }

    // --------------------------------------------------------
    // Multi-bit
    // --------------------------------------------------------

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> XNOR(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = XNOR(a[i], b[i]);

        return result;
    }


    // ========================================================
    // MUX
    // 2:1 Multiplexer
    // ========================================================
    //
    // s = 0 -> a
    // s = 1 -> b
    //
    // Structural implementation using NAND only.
    //

    // --------------------------------------------------------
    // Single-bit
    // --------------------------------------------------------

    [[nodiscard]]
    static constexpr Bit MUX(
        Bit a,
        Bit b,
        Bit s
    ) noexcept
    {
        const Bit not_s = NAND(s, s);

        const Bit a_path = NAND(a, not_s);
        const Bit b_path = NAND(b, s);

        return NAND(a_path, b_path);
    }

    // --------------------------------------------------------
    // Multi-bit
    // --------------------------------------------------------

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> MUX(
        const bits<T>& a,
        const bits<T>& b,
        Bit s
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = MUX(a[i], b[i], s);

        return result;
    }
};

} // namespace structural::logic
