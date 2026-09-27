#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace logic {

using Bit = bool;

template <std::size_t N>
using Bits = std::conditional_t<
    (N <= 8),
    std::uint8_t,
    std::conditional_t<
        (N <= 16),
        std::uint16_t,
        std::conditional_t<
            (N <= 32),
            std::uint32_t,
            std::uint64_t
        >
    >
>;

} // namespace logic