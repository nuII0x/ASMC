#pragma once

#include "logic/types/Bit.hpp"

#include <cstddef>

namespace logic {

// ============================================================
// Bus
// ============================================================
//
// A Bus is an ideal zero-resistance connection between a source
// and a destination.
//
// It performs no logical operation and stores no signal state.
//
// Every signal entering the bus is directly available at its
// output:
//
//     input[i] == output[i]
//
// The physical representation comes entirely from Bit.hpp:
//
//     Bit
//     bits<T>
//     LogicValue
//     BitCount
//
// The Bus does not redefine any of these concepts.
//
//
// Timing
// ------
//
// A Bus has no internal propagation delay.
//
// The simulator may still process the bus during a simulation
// step. This allows the global simulator to model propagation
// through a network of chips:
//
//     Gate -> Bus -> Gate
//
// The Bus itself simply forwards the current signals.
//

template <LogicValue T>
class Bus {
public:

    // ========================================================
    // Types
    // ========================================================

    using value_type   = T;
    using storage_type = bits<T>;


    // ========================================================
    // Construction
    // ========================================================

    constexpr Bus() noexcept = default;


    // ========================================================
    // Connection
    // ========================================================
    //
    // Connects the bus to an existing collection of physical
    // signals.
    //
    // The Bus does not copy or own the signals.
    //

    constexpr void connect(const storage_type& source) noexcept
    {
        source_ = &source;
    }


    // ========================================================
    // Propagation
    // ========================================================
    //
    // A Bus has zero propagation delay.
    //
    // The method exists so the Bus participates explicitly in
    // the simulator's step-based propagation model.
    //
    // No state needs to be updated here because the Bus is an
    // ideal connection.
    //

    constexpr void propagate() const noexcept
    {
        // Intentionally empty.
        //
        // The output is always the current source.
    }


    // ========================================================
    // Output
    // ========================================================
    //
    // Returns the signals currently present on the bus.
    //

    [[nodiscard]]
    constexpr const storage_type& output() const noexcept
    {
        return *source_;
    }


    // ========================================================
    // Bit access
    // ========================================================
    //
    // Provides direct access to an individual physical signal.
    //

    [[nodiscard]]
    constexpr const Bit& operator[](std::size_t index) const noexcept
    {
        return (*source_)[index];
    }


    // ========================================================
    // Connection state
    // ========================================================

    [[nodiscard]]
    constexpr bool connected() const noexcept
    {
        return source_ != nullptr;
    }


private:

    // ========================================================
    // Source
    // ========================================================
    //
    // The Bus does not own the signal storage.
    //
    // It represents the connection to an existing collection
    // of physical signals.
    //

    const storage_type* source_ = nullptr;
};

} // namespace logic