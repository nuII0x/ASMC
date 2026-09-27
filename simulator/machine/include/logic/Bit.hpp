#pragma once

#include <array>
#include <cstddef>

namespace logic {
// ============================================================
// Basic types
// ============================================================

using Bit = bool;

template <std::size_t N>
using Bits = std::array<Bit, N>;

} // namespace logic