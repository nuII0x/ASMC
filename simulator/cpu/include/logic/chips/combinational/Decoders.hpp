#pragma once

#include "logic/Gates.hpp"
namespace combinational::logic
{
    class Decoder2to4
    {
    public:
        struct Output
        {
            Bit l0;
            Bit l1;
            Bit l2;
            Bit l3;
        };

        [[nodiscard]]
        static constexpr Output decode(Bit A, Bit B) noexcept
        {
            const Bit notA = NOT(A);
            const Bit notB = NOT(B);

            return {
                AND(notB, notA), // l0 = !B & !A
                AND(notB, A),    // l1 = !B &  A
                AND(B, notA),    // l2 =  B & !A
                AND(B, A)        // l3 =  B &  A
            };
        }
    };
    class Decoder4to16
    {
    public:
        struct Output
        {
            Byte low;
            Byte high;
        };

        [[nodiscard]]
        static constexpr Output decode(
            Bit A,
            Bit B,
            Bit C,
            Bit D
        ) noexcept
        {
            const auto lowDecoder = Decoder2to4::decode(A, B);
            const auto highDecoder = Decoder2to4::decode(C, D);

            return {
                static_cast<Byte>(
                    AND(highDecoder.l0, lowDecoder.l0) |
                    (AND(highDecoder.l0, lowDecoder.l1) << 1) |
                    (AND(highDecoder.l0, lowDecoder.l2) << 2) |
                    (AND(highDecoder.l0, lowDecoder.l3) << 3) |
                    (AND(highDecoder.l1, lowDecoder.l0) << 4) |
                    (AND(highDecoder.l1, lowDecoder.l1) << 5) |
                    (AND(highDecoder.l1, lowDecoder.l2) << 6) |
                    (AND(highDecoder.l1, lowDecoder.l3) << 7)
                ),

                static_cast<Byte>(
                    AND(highDecoder.l2, lowDecoder.l0) |
                    (AND(highDecoder.l2, lowDecoder.l1) << 1) |
                    (AND(highDecoder.l2, lowDecoder.l2) << 2) |
                    (AND(highDecoder.l2, lowDecoder.l3) << 3) |
                    (AND(highDecoder.l3, lowDecoder.l0) << 4) |
                    (AND(highDecoder.l3, lowDecoder.l1) << 5) |
                    (AND(highDecoder.l3, lowDecoder.l2) << 6) |
                    (AND(highDecoder.l3, lowDecoder.l3) << 7)
                )
            };
        }
    };
    
}
