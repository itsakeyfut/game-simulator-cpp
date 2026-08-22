#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

/// Vocabulary types shared by every module live directly in `gsim`.
/// Module specific services (Logger, Assert, ...) stay in `gsim::<module>`.
namespace gsim {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

/// Index / size type. Matches std::size_t so it can be used with containers
/// without a conversion warning.
using usize = std::size_t;

/// Signed counterpart of usize, for pointer difference and reverse loops.
using isize = std::ptrdiff_t;

// The 3D math in phase 2 assumes IEEE 754 single precision throughout: exact
// sizes, and well defined infinity / NaN behaviour.
static_assert(sizeof(f32) == 4, "f32 must be 32 bit");
static_assert(sizeof(f64) == 8, "f64 must be 64 bit");
static_assert(std::numeric_limits<f32>::is_iec559, "f32 must be IEEE 754");
static_assert(std::numeric_limits<f64>::is_iec559, "f64 must be IEEE 754");

}  // namespace gsim
