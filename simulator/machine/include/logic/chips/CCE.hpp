#pragma once

#include "logic/types/Bit.hpp"

#include "logic/chips/Clock.hpp"
#include "logic/chips/DFF.hpp"
#include "logic/Gates.hpp"
#include "logic/chips/ThreeStateBuffer.hpp"

namespace behavioral::logic {

class CCE {
public:

    struct Inputs {
        logic::Bit START{};
        logic::Bit HALT{};
    };

    struct Outputs {
        logic::Bit CLOCK{};
        logic::Bit RESET{};
        logic::Bit HALTED{};
        logic::Bit ENABLE{};
    };

public:

    constexpr CCE() noexcept = default;

    constexpr void reset() noexcept
    {
        bootFF_.reset();
        haltFF_.reset();
    }

    [[nodiscard]]
    constexpr Outputs update(
        Inputs inputs
    ) noexcept
    {
        // ====================================================
        // CLOCK
        // ====================================================

        const logic::Bit clock = clock_.output();


        // ====================================================
        // BOOT_FF
        // ====================================================
        //
        // D       <- START
        // RESET   <- START
        // CLOCK   <- CLOCK
        //
        // Q       -> RESET
        //

        const auto boot = bootFF_.update({
            .D     = inputs.START,
            .RESET = inputs.START,
            .CLOCK = clock
        });


        // ====================================================
        // HALT_FF
        // ====================================================
        //
        // D       <- HALT
        // RESET   <- HALT
        // CLOCK   <- CLOCK
        //
        // Q       -> HALTED
        // Q       -> NOT
        // !Q      -> ENABLE
        //

        const auto halt = haltFF_.update({
            .D     = inputs.HALT,
            .RESET = inputs.HALT,
            .CLOCK = clock
        });


        // ====================================================
        // NOT
        // ====================================================

        const logic::Bit notHalt = not_.update(halt.Q);


        // ====================================================
        // 3-STATE BUFFER #1
        // ====================================================
        //
        // DATA   <- CLOCK
        // ENABLE <- NOT(HALT_FF.Q)
        //

        const logic::Bit clockGate = clockBuffer_.update({
            .input  = clock,
            .enable = notHalt
        });


        // ====================================================
        // 3-STATE BUFFER #2
        // ====================================================
        //
        // DATA   <- BUFFER #1
        // ENABLE <- BOOT_FF.Q
        //
        // OUTPUT -> CLOCK
        //

        const logic::Bit outputClock = bootBuffer_.update({
            .input  = clockGate,
            .enable = boot.Q
        });


        return {
            .CLOCK  = outputClock,
            .RESET  = boot.Q,
            .HALTED = halt.Q,
            .ENABLE = halt.notQ
        };
    }

private:

    Clock clock_;

    DFF bootFF_;
    DFF haltFF_;

    Not not_;

    ThreeStateBuffer clockBuffer_;
    ThreeStateBuffer bootBuffer_;
};

} // namespace behavioral::logic