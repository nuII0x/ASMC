#pragma once

#include "logic/types/Bus.hpp"

#include <string_view>
#include <variant>
#include <vector>

namespace logic {

// ============================================================
// Chip
// ============================================================
//
// A Chip is a hardware component composed of gates,
// connections, and optionally internal state.
//
// A chip may expose any number of input and output buses.
//
// Examples:
//
//     NAND
//     XOR
//     MUX
//     Register
//     Counter
//     ALU
//     RAM
//     CPU
//
// The Chip does not define how many propagation steps exist
// inside a clock cycle.
//
// That is a global simulator concern.
//
// The simulator calls:
//
//     step()
//
// once for every propagation step.
//

class Chip {
public:

    // ========================================================
    // Bus interface
    // ========================================================
    //
    // A chip can expose buses based on the logical types
    // defined by Bit.hpp.
    //
    // No physical width is defined here.
    //

    using BusVariant = std::variant<
        Bus<Bit>,
        Bus<Byte>,
        Bus<Word>,
        Bus<DWord>,
        Bus<QWord>
    >;


    // ========================================================
    // Construction
    // ========================================================

    explicit Chip(std::string_view name) noexcept
        : name_(name)
    {
    }

    virtual ~Chip() = default;


    // ========================================================
    // Name
    // ========================================================

    [[nodiscard]]
    constexpr std::string_view name() const noexcept
    {
        return name_;
    }


    // ========================================================
    // Simulation step
    // ========================================================
    //
    // Executes one propagation step.
    //
    // The simulator is responsible for calling this method
    // repeatedly according to its global steps-per-clock
    // configuration.
    //
    // A chip must not assume that one call to step() represents
    // a complete clock cycle.
    //

    virtual void step() = 0;


    // ========================================================
    // Input buses
    // ========================================================

    [[nodiscard]]
    const std::vector<BusVariant>& inputs() const noexcept
    {
        return inputs_;
    }


    // ========================================================
    // Output buses
    // ========================================================

    [[nodiscard]]
    const std::vector<BusVariant>& outputs() const noexcept
    {
        return outputs_;
    }


protected:

    // ========================================================
    // Add input
    // ========================================================
    //
    // Registers an input bus as part of the chip interface.
    //

    template <LogicValue T>
    void addInput(Bus<T> bus)
    {
        inputs_.emplace_back(std::move(bus));
    }


    // ========================================================
    // Add output
    // ========================================================
    //
    // Registers an output bus as part of the chip interface.
    //

    template <LogicValue T>
    void addOutput(Bus<T> bus)
    {
        outputs_.emplace_back(std::move(bus));
    }


    // ========================================================
    // Mutable input collection
    // ========================================================

    [[nodiscard]]
    std::vector<BusVariant>& inputs() noexcept
    {
        return inputs_;
    }


    // ========================================================
    // Mutable output collection
    // ========================================================

    [[nodiscard]]
    std::vector<BusVariant>& outputs() noexcept
    {
        return outputs_;
    }


private:

    // ========================================================
    // Identity
    // ========================================================

    std::string_view name_;


    // ========================================================
    // Interface
    // ========================================================
    //
    // A chip can have any number of input and output buses.
    //

    std::vector<BusVariant> inputs_;
    std::vector<BusVariant> outputs_;
};

} // namespace logic