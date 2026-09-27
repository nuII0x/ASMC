#pragma once

#include "logic/types/Bit.hpp"

namespace behavioral::logic {

// ============================================================
// Gates
// ============================================================

class Gate {
public:

    // --------------------------------------------------------
    // Single-bit gates
    // --------------------------------------------------------

    [[nodiscard]]
    static constexpr Bit NAND(Bit a, Bit b) noexcept
    {
        return not (a bitand b);
    }

    [[nodiscard]]
    static constexpr Bit NOT(Bit a) noexcept
    {
        return not a;
    }

    [[nodiscard]]
    static constexpr Bit AND(Bit a, Bit b) noexcept
    {
        return a bitand b;
    }

    [[nodiscard]]
    static constexpr Bit OR(Bit a, Bit b) noexcept
    {
        return a bitor b;
    }

    [[nodiscard]]
    static constexpr Bit XOR(Bit a, Bit b) noexcept
    {
        return a xor b;
    }

    [[nodiscard]]
    static constexpr Bit XNOR(Bit a, Bit b) noexcept
    {
        return not (a xor b);
    }


    // --------------------------------------------------------
    // N-bit gates
    // --------------------------------------------------------

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T NAND(T a, T b) noexcept
    {
        return static_cast<T>(compl(a bitand b));
    }

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T NOT(T a) noexcept
    {
        return static_cast<T>(compl a);
    }

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T AND(T a, T b) noexcept
    {
        return a bitand b;
    }

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T OR(T a, T b) noexcept
    {
        return a bitor b;
    }

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T XOR(T a, T b) noexcept
    {
        return a xor b;
    }

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T XNOR(T a, T b) noexcept
    {
        return static_cast<T>(compl(a xor b));
    }
};

} // namespace logic