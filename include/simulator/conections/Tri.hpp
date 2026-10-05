enum class TriBit : std::uint8_t {
    Low,
    High,
    HighImpedance
};

class TriStateBuffer {
public:
    [[nodiscard]]
    static constexpr TriBit drive(
        Bit in,
        Bit enable
    ) noexcept
    {
        if (not enable)
            return TriBit::HighImpedance;

        return in
            ? TriBit::High
            : TriBit::Low;
    }
};
class TRI8 {
public:
    [[nodiscard]]
    static constexpr std::array<TriBit, BYTE_BITS> drive(
        const bits<Byte>& in,
        const bits<Bit>& enable
    ) noexcept
    {
        std::array<TriBit, BYTE_BITS> out{};

        for (std::size_t i = 0; i < BYTE_BITS; ++i) {
            out[i] = TriStateBuffer::drive(
                in[i],
                enable[0]
            );
        }

        return out;
    }
};