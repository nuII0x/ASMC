#pragma once

#include "logic/types/Bit.hpp"

namespace behavioral::logic {

// ============================================================
// Gates
// ============================================================

class Gate {
public:

    // ========================================================
    // NAND
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T NAND(T a, T b) noexcept
    {
        return static_cast<T>(compl(a bitand b));
    }


    // ========================================================
    // NOT
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T NOT(T a) noexcept
    {
        return static_cast<T>(compl a);
    }


    // ========================================================
    // AND
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T AND(T a, T b) noexcept
    {
        return a bitand b;
    }


    // ========================================================
    // OR
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T OR(T a, T b) noexcept
    {
        return a bitor b;
    }


    // ========================================================
    // XOR
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T XOR(T a, T b) noexcept
    {
        return a xor b;
    }


    // ========================================================
    // XNOR
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T XNOR(T a, T b) noexcept
    {
        return static_cast<T>(compl(a xor b));
    }


    // ========================================================
    // MUX
    // 2:1 Multiplexer
    // ========================================================
    //
    // s = 0 -> a
    // s = 1 -> b
    //

    template <LogicValue T>
    [[nodiscard]]
    static constexpr T MUX(
        T a,
        T b,
        Bit s
    ) noexcept
    {
        return s ? b : a;
    }
	// ========================================================
    // DEMUX
    // 1:2 Demultiplexer
    // ========================================================
    //
    // s = 0 -> a
    // s = 1 -> b
	template <LogicValue T>
	[[nodiscard]]
	static constexpr std::array<bits<T>, 2> DEMUX(
    	const bits<T>& d,
    	Bit s
	) noexcept
	{
    	return {
       	 	d bitand not s,
       		d bitand s
    	};
	}

};

} // namespace behavioral::logic