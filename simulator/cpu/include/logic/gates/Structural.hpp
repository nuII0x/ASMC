#pragma once
#include "logic/types/Bit.hpp"
#include <array>
#include <cstddef>
namespace structural::logic {
class Gate {
private:

    // ========================================================
    // Physical bit operation
    // ========================================================
    //
    // These are implementation primitives used internally by
    // the multibit gates.
    //
    // No public single-bit gate API is exposed.
    //

    [[nodiscard]]
    static constexpr Bit BITNAND(
        Bit a,
        Bit b
    ) noexcept
    {
        return not (a bitand b);
    }

public:

    // ========================================================
    // NAND
    // Fundamental logical primitive.
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> NAND(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = BITNAND(a[i], b[i]);

        return result;
    }


    // ========================================================
    // NOT
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> NOT(
        const bits<T>& a
    ) noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = BITNAND(a[i], a[i]);

        return result;
    }


    // ========================================================
    // AND
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> AND(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        return NOT(NAND(a, b));
    }


    // ========================================================
    // OR
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> OR(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        return NAND(
            NOT(a),
            NOT(b)
        );
    }


    // ========================================================
    // XOR
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> XOR(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        const auto n = NAND(a, b);

        return NAND(
            NAND(a, n),
            NAND(b, n)
        );
    }


    // ========================================================
    // XNOR
    // ========================================================

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> XNOR(
        const bits<T>& a,
        const bits<T>& b
    ) noexcept
    {
        return NOT(XOR(a, b));
    }


    // ========================================================
    // MUX
    // 2:1 Multiplexer
    // ========================================================
    //
    // s = 0 -> a
    // s = 1 -> b
    //
    // The data paths are multibit.
    // The select signal is a single physical bit represented
    // through bits<Bit>.
    //

    template <LogicValue T>
    [[nodiscard]]
    static constexpr bits<T> MUX(
        const bits<T>& a,
        const bits<T>& b,
        const bits<Bit>& s
    ) noexcept
    {
        const auto not_s = NOT(s);

        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i) {
            const Bit a_path = BITNAND(
                a[i],
                not_s[0]
            );

            const Bit b_path = BITNAND(
                b[i],
                s[0]
            );

            result[i] = BITNAND(
                a_path,
                b_path
            );
        }

        return result;
    }


    // ========================================================
    // DEMUX
    // 1:2 Demultiplexer
    // ========================================================
    //
    // s = 0 -> c0 = d, c1 = 0
    // s = 1 -> c0 = 0, c1 = d
    //
    // Both outputs contain the complete multibit signal.
    //

    template <LogicValue T>
    [[nodiscard]]
    static constexpr std::array<bits<T>, 2> DEMUX(
        const bits<T>& d,
        const bits<Bit>& s
    ) noexcept
    {
        const auto not_s = NOT(s);

        return {
            AND(d, not_s),
            AND(d, s)
        };
    }
};
class SRLatch {
public:

    constexpr SRLatch() noexcept = default;


    // ========================================================
    // Update
    // ========================================================

    [[nodiscard]]
    constexpr bits<Bit> update(
        const bits<Bit>& s,
        const bits<Bit>& r
    ) noexcept
    {
        const auto not_r = Gate::NOT(r);
        const auto feedback = Gate::OR(s, q_);

        q_ = Gate::AND(
            feedback,
            not_r
        );

        return q_;
    }


    // ========================================================
    // Q
    // ========================================================

    [[nodiscard]]
    constexpr bits<Bit> Q() const noexcept
    {
        return q_;
    }


    // ========================================================
    // Q'
    // ========================================================

    [[nodiscard]]
    constexpr bits<Bit> Qn() const noexcept
    {
        return Gate::NOT(q_);
    }


    // ========================================================
    // Reset
    // ========================================================

    constexpr void reset(
        const bits<Bit>& value = {}
    ) noexcept
    {
        q_ = value;
    }

private:

    bits<Bit> q_{};
};
class DLatch {
public:

    constexpr DLatch() noexcept = default;


    // ========================================================
    // Update
    // ========================================================

    [[nodiscard]]
    constexpr bits<Bit> update(
        const bits<Bit>& d,
        const bits<Bit>& en
    ) noexcept
    {
        const auto not_d = Gate::NOT(d);

        const auto s = Gate::AND(
            d,
            en
        );

        const auto r = Gate::AND(
            not_d,
            en
        );

        return latch_.update(s, r);
    }


    // ========================================================
    // Q
    // ========================================================

    [[nodiscard]]
    constexpr bits<Bit> Q() const noexcept
    {
        return latch_.Q();
    }


    // ========================================================
    // Q'
    // ========================================================

    [[nodiscard]]
    constexpr bits<Bit> Qn() const noexcept
    {
        return latch_.Qn();
    }


    // ========================================================
    // Reset
    // ========================================================

    constexpr void reset(
        const bits<Bit>& value = {}
    ) noexcept
    {
        latch_.reset(value);
    }

private:

    SRLatch latch_;
};
template <LogicValue T>
class DLatchN {
public:

    constexpr DLatchN() noexcept = default;


    // ========================================================
    // Update
    // ========================================================

    [[nodiscard]]
    constexpr bits<T> update(
        const bits<T>& in,
        const bits<Bit>& st
    ) noexcept
    {
        bits<T> out{};

        for (std::size_t i = 0; i < BitCount<T>; ++i) {
            const bits<Bit> d{in[i]};

            const auto q = latches_[i].update(
                d,
                st
            );

            out[i] = q[0];
        }

        return out;
    }


    // ========================================================
    // Reset
    // ========================================================

    constexpr void reset(
        const bits<T>& value = {}
    ) noexcept
    {
        for (std::size_t i = 0; i < BitCount<T>; ++i) {
            const bits<Bit> bit{value[i]};

            latches_[i].reset(bit);
        }
    }


    // ========================================================
    // Output
    // ========================================================

    [[nodiscard]]
    constexpr bits<T> Q() const noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = latches_[i].Q()[0];

        return result;
    }


    // ========================================================
    // Inverted output
    // ========================================================

    [[nodiscard]]
    constexpr bits<T> Qn() const noexcept
    {
        bits<T> result{};

        for (std::size_t i = 0; i < BitCount<T>; ++i)
            result[i] = latches_[i].Qn()[0];

        return result;
    }

private:

    std::array<DLatch, BitCount<T>> latches_{};
};
class DFF {
public:
    constexpr DFF() noexcept = default;

    [[nodiscard]]
    constexpr bits<Bit> update(
        const bits<Bit>& d,
        const bits<Bit>& cl,
        const bits<Bit>& st
    ) noexcept
    {
        // Master latch is enabled while clock and store are high.
        const auto master_enable = Gate::AND(cl, st);

        // Master captures the input.
        const auto master_q = master_.update(
            d,
            master_enable
        );

        // Slave is enabled while clock is low.
        const auto slave_enable = Gate::NOT(cl);

        // Slave captures the master's stored value.
        return slave_.update(
            master_q,
            slave_enable
        );
    }

    [[nodiscard]]
    constexpr bits<Bit> Q() const noexcept
    {
        return slave_.Q();
    }

    [[nodiscard]]
    constexpr bits<Bit> Qn() const noexcept
    {
        return slave_.Qn();
    }

    constexpr void reset(
        const bits<Bit>& value = {}
    ) noexcept
    {
        master_.reset(value);
        slave_.reset(value);
    }

private:
    DLatch master_;
    DLatch slave_;
};
class Register8 {
public:
    constexpr Register8() noexcept = default;

    constexpr void update(
        const bits<Byte>& d,
        const bits<Bit>& cl,
        const bits<Bit>& enable
    ) noexcept
    {
        for (std::size_t i = 0; i < BYTE_BITS; ++i) {
            dff_[i].update(
                bits<Bit>{d[i]},
                cl,
                enable
            );
        }
    }

    [[nodiscard]]
    constexpr bits<Byte> Q() const noexcept
    {
        bits<Byte> result{};

        for (std::size_t i = 0; i < BYTE_BITS; ++i)
            result[i] = dff_[i].Q()[0];

        return result;
    }

    [[nodiscard]]
    constexpr TriByte OUT(
        const bits<Bit>& enable
    ) const noexcept
    {
        return TRI8::drive(
            Q(),
            enable
        );
    }

    constexpr void reset(
        const bits<Byte>& value = {}
    ) noexcept
    {
        for (std::size_t i = 0; i < BYTE_BITS; ++i)
            dff_[i].reset(bits<Bit>{value[i]});
    }

private:
    std::array<DFF, BYTE_BITS> dff_{};
};
struct Register4x4_8 {

    using Bit  = logic::Bit;
    using Byte = logic::Byte;

    // ========================================================
    // Storage
    // ========================================================
    //
    // Physical organization:
    //
    // registers[row][column]
    //
    // 16 independent REGISTER8 components.
    //

    std::array<
        std::array<Register8, 4>,
        4
    > registers{};


    // ========================================================
    // Bus
    // ========================================================
    //
    // The buses do not perform logic.
    //
    // They represent the physical connections between the
    // registers and the rest of the circuit.
    //

    logic::Bus<Byte> data_bus{};


    // ========================================================
    // Decoder 2 -> 4
    // ========================================================
    //
    // The DLS circuit uses two independent 2-to-4 decoders:
    //
    //     row    -> R0 R1 R2 R3
    //     column -> C0 C1 C2 C3
    //
    // The decoder outputs are one-hot:
    //
    //     00 -> 1000
    //     01 -> 0100
    //     10 -> 0010
    //     11 -> 0001
    //
    // The two input bits are represented by bits<Bit>.
    //

    [[nodiscard]]
    static constexpr std::array<Bit, 4> decode2(
        const bits<Bit>& input
    ) noexcept
    {
        const Bit b0 = input[0];
        const Bit b1 = input[1];

        const Bit n0 = not b0;
        const Bit n1 = not b1;

        return {
            n1 bitand n0,
            n1 bitand b0,
            b1 bitand n0,
            b1 bitand b0
        };
    }


    // ========================================================
    // Row decoder
    // ========================================================

    [[nodiscard]]
    static constexpr std::array<Bit, 4> decode_row(
        const bits<Byte>& row
    ) noexcept
    {
        return decode2(
            bits<Bit>{
                row[0],
                row[1]
            }
        );
    }


    // ========================================================
    // Column decoder
    // ========================================================

    [[nodiscard]]
    static constexpr std::array<Bit, 4> decode_column(
        const bits<Byte>& column
    ) noexcept
    {
        return decode2(
            bits<Bit>{
                column[0],
                column[1]
            }
        );
    }


    // ========================================================
    // Write
    // ========================================================
    //
    // row × column selects exactly one REGISTER8.
    //
    // enable =
    //
    //     row_select
    //       AND
    //     column_select
    //       AND
    //     st
    //
    // All registers receive the same data and clock.
    // Only the selected register receives store=1.
    //

    constexpr void write(
        const bits<Byte>& row,
        const bits<Byte>& column,
        const bits<Bit>& st,
        const bits<Bit>& cl,
        const bits<Byte>& data
    ) noexcept
    {
        const auto rows =
            decode_row(row);

        const auto columns =
            decode_column(column);

        for (std::size_t r = 0; r < 4; ++r) {

            for (std::size_t c = 0; c < 4; ++c) {

                const Bit selected =
                    rows[r]
                    bitand
                    columns[c];

                const Bit enable =
                    selected
                    bitand
                    st[0];

                registers[r][c].update(
                    data,
                    cl,
                    bits<Bit>{enable}
                );
            }
        }
    }


    // ========================================================
    // Register selection
    // ========================================================
    //
    // addr is a 2-bit address.
    //
    //     00 -> 0
    //     01 -> 1
    //     10 -> 2
    //     11 -> 3
    //
    // It selects one of the four registers in the selected
    // row.
    //
    // This is kept separate from the write decoder because
    // addr is not part of the row × column write selection.
    //

    [[nodiscard]]
    constexpr const Register8& select(
        const bits<Byte>& row,
        const bits<Byte>& addr
    ) const noexcept
    {
        const std::size_t r =
            static_cast<std::size_t>(
                row[0]
                | static_cast<Bit>(row[1] << 1)
            );

        const std::size_t c =
            static_cast<std::size_t>(
                addr[0]
                | static_cast<Bit>(addr[1] << 1)
            );

        return registers[r][c];
    }


    // ========================================================
    // Output
    // ========================================================

    [[nodiscard]]
    constexpr bits<Byte> output(
        const bits<Byte>& row,
        const bits<Byte>& addr
    ) const noexcept
    {
        return select(row, addr).Q();
    }


    // ========================================================
    // Reset
    // ========================================================

    constexpr void reset() noexcept
    {
        for (auto& row : registers)
            for (auto& reg : row)
                reg.reset();
    }
};
struct SRAM256B
{
    std::array<Register4x4_8, 16> chips{};

    // 8-4BIT
    bits<Byte> chip_address{};
    bits<Byte> register_address{};

    // 4-1BIT
    bits<Bit> row{};
    bits<Bit> column{};

    constexpr void update(
        const bits<Byte>& addr,
        const bits<Bit>& e,
        const bits<Bit>& cl,
        const bits<Bit>& rw,
        const bits<Byte>& data
    ) noexcept
    {
        // 8-4BIT
        register_address = bits<Byte>{
            addr[0],
            addr[1],
            addr[2],
            addr[3]
        };

        chip_address = bits<Byte>{
            addr[4],
            addr[5],
            addr[6],
            addr[7]
        };

        // 4-1BIT
        row = bits<Bit>{
            chip_address[0],
            chip_address[1]
        };

        column = bits<Bit>{
            chip_address[2],
            chip_address[3]
        };

        // The two 2-to-4 decoders select exactly one
        // 4x4REG8.
        //
        // Every chip receives the same:
        //     register_address
        //     data
        //     rw
        //     cl
        //
        // Only the selected chip receives the enable.

        for (std::size_t i = 0; i < 16; ++i) {
            const bits<Bit> chip_enable{
                /* row_decoder[i >> 2] AND
                   column_decoder[i & 3] */
            };

            chips[i].update(
                register_address,
                row,
                column,
                rw,
                cl,
                data
            );
        }
    }
};
class TFF {
public:
    constexpr TFF() noexcept = default;

    constexpr void update(
        const bits<Bit>& t,
        const bits<Bit>& cl
    ) noexcept
    {
        const bits<Bit> d{
            Gate::XOR(t[0], dff_.Q()[0])
        };

        dff_.update(
            d,
            cl,
            bits<Bit>{true}
        );
    }

    [[nodiscard]]
    constexpr bits<Bit> Q() const noexcept
    {
        return dff_.Q();
    }

    [[nodiscard]]
    constexpr bits<Bit> QN() const noexcept
    {
        return dff_.QN();
    }

    constexpr void reset(
        const bits<Bit>& value = {}
    ) noexcept
    {
        dff_.reset(value);
    }

private:
    DFF dff_{};
};
struct CMEM
{
    // ========================================================
    // Registers
    // ========================================================

    Register8 A{};
    Register8 D{};

    // ========================================================
    // Memory
    // ========================================================

    SRAM256B ram{};

    // ========================================================
    // Internal buses
    // ========================================================

    bits<Byte> data_bus{};

    // ========================================================
    // Update
    // ========================================================

    constexpr void update(
        const bits<Bit>& r,
        const bits<Bit>& e,
        const bits<Byte>& a,
        const bits<Byte>& d,
        const bits<Bit>& a_write,
        const bits<Bit>& cl,
        const bits<Bit>& X
    ) noexcept
    {
        // ----------------------------------------------------
        // A register
        // ----------------------------------------------------

        A.update(
            a,
            cl,
            e
        );

        // ----------------------------------------------------
        // RAM address
        //
        // The A register drives the SRAM address bus.
        // ----------------------------------------------------

        const bits<Byte> address = A.Q();

        // ----------------------------------------------------
        // Data bus
        //
        // External d and RAM output share the 8-bit bus.
        //
        // During a RAM read, the RAM drives the bus.
        // During a RAM write, d drives the bus.
        // ----------------------------------------------------

        data_bus = d;

        // ----------------------------------------------------
        // RAM
        // ----------------------------------------------------

        ram.update(
            address,
            r,
            cl,
            a_write,
            data_bus
        );

        // ----------------------------------------------------
        // D register
        //
        // D receives the value present on the data bus.
        // ----------------------------------------------------

        D.update(
            data_bus,
            cl,
            e
        );
    }

    // ========================================================
    // A output
    // ========================================================

    [[nodiscard]]
    constexpr bits<Byte> output_A() const noexcept
    {
        return A.Q();
    }

    // ========================================================
    // D output
    // ========================================================

    [[nodiscard]]
    constexpr bits<Byte> output_D() const noexcept
    {
        return D.Q();
    }

    // ========================================================
    // *A output
    //
    // Data read from SRAM.
    // ========================================================

    [[nodiscard]]
    constexpr bits<Byte> output_a() const noexcept
    {
        return ram.OUT();
    }

    // ========================================================
    // Reset
    // ========================================================

    constexpr void reset() noexcept
    {
        A.reset();
        D.reset();
        ram.reset();

        data_bus = {};
    }
};
class STP
{
public:
    constexpr STP() noexcept = default;

    constexpr void update(
        const bits<Bit>& s,
        const bits<Byte>& serial,
        const bits<Bit>& cl
    ) noexcept
    {
        // Store the current serial byte into the active stage.
        if (s[0]) {
            if (stage_ == 0) {
                dest_.update(
                    serial,
                    cl,
                    bits<Bit>{true}
                );

                stage_ = 1;
            }
            else {
                instr_.update(
                    serial,
                    cl,
                    bits<Bit>{true}
                );

                stage_ = 0;
            }
        }

        // Keep both registers updated with the clock.
        else {
            dest_.update(
                dest_.Q(),
                cl,
                bits<Bit>{false}
            );

            instr_.update(
                instr_.Q(),
                cl,
                bits<Bit>{false}
            );
        }
    }

    [[nodiscard]]
    constexpr bits<Byte> DEST() const noexcept
    {
        return dest_.Q();
    }

    [[nodiscard]]
    constexpr bits<Byte> INSTR() const noexcept
    {
        return instr_.Q();
    }

    constexpr void reset() noexcept
    {
        dest_.reset();
        instr_.reset();
        stage_ = 0;
    }

private:
    Register8 dest_{};
    Register8 instr_{};

    // 0 = next store goes to DEST
    // 1 = next store goes to INSTR
    Bit stage_ = false;
};
class Adder
{
public:

    struct Result
    {
        Bit sum{};
        Bit carry{};
    };

    // ========================================================
    // Half Adder
    //
    // sum   = A XOR B
    // carry = A AND B
    // ========================================================

    [[nodiscard]]
    static constexpr Result half(
        Bit a,
        Bit b
    ) noexcept
    {
        return {
            .sum   = Gate::XOR(a, b),
            .carry = Gate::AND(a, b)
        };
    }

    // ========================================================
    // Full Adder
    //
    // Implemented as:
    //
    //        HA
    // A ─────┤
    // B ─────┤── sum1
    //          │
    //        HA
    // Cin ────┤
    //
    // Cout = C1 OR C2
    // ========================================================

    [[nodiscard]]
    static constexpr Result full(
        Bit a,
        Bit b,
        Bit carry_in
    ) noexcept
    {
        const Result first = half(a, b);
        const Result second = half(first.sum, carry_in);

        return {
            .sum = second.sum,
            .carry = Gate::OR(
                first.carry,
                second.carry
            )
        };
    }

    // ========================================================
    // N-bit addition
    // ========================================================

    [[nodiscard]]
    static constexpr bits<Byte> add(
        const bits<Byte>& a,
        const bits<Byte>& b,
        Bit carry_in,
        Bit& carry_out
    ) noexcept
    {
        bits<Byte> result{};

        Bit carry = carry_in;

        for (std::size_t i = 0; i < BYTE_BITS; ++i) {
            const Result bit = full(
                a[i],
                b[i],
                carry
            );

            result[i] = bit.sum;
            carry = bit.carry;
        }

        carry_out = carry;

        return result;
    }
};

class AddSub8 : public Adder
{
public:

    struct Result
    {
        bits<Byte> value{};
        Bit carry{};
        Bit overflow{};
    };

    [[nodiscard]]
    static constexpr Result compute(
        const bits<Byte>& a,
        const bits<Byte>& b,
        Bit subtract
    ) noexcept
    {
        /*
         * subtract = 0:
         *
         *     A + B
         *
         * subtract = 1:
         *
         *     A + ~B + 1
         *
         * Therefore:
         *
         *     A - B = A + ~B + 1
         */

        const bits<Byte> inverted_b = Gate::NOT(b);

        const bits<Byte> operand_b =
            Gate::MUX(
                b,
                inverted_b,
                bits<Bit>{subtract}
            );

        Bit carry{};

        const bits<Byte> result = Adder::add(
            a,
            operand_b,
            subtract,
            carry
        );

        const Bit overflow = overflow8(
            a,
            b,
            result,
            subtract
        );

        return {
            .value = result,
            .carry = carry,
            .overflow = overflow
        };
    }

private:

    // ========================================================
    // Signed overflow detection
    // ========================================================

    [[nodiscard]]
    static constexpr Bit overflow8(
        const bits<Byte>& a,
        const bits<Byte>& b,
        const bits<Byte>& result,
        Bit subtract
    ) noexcept
    {
        const Bit a_sign = a[BYTE_BITS - 1];
        const Bit b_sign = b[BYTE_BITS - 1];
        const Bit r_sign = result[BYTE_BITS - 1];

        const Bit sign_difference =
            Gate::XOR(a_sign, b_sign);

        const Bit result_difference =
            Gate::XOR(a_sign, r_sign);

        /*
         * Addition:
         *
         *   same signs + different result sign
         *
         * Subtraction:
         *
         *   different signs + different result sign
         */

        const Bit sign_condition =
            Gate::MUX(
                Gate::NOT(sign_difference),
                sign_difference,
                bits<Bit>{subtract}
            );

        return Gate::AND(
            sign_condition,
            result_difference
        );
    }
};

} // namespace structural::logic