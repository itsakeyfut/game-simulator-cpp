#pragma once

#include <source_location>
#include <string_view>

namespace gsim::core {

/// Everything the assert machinery knows about one failure.
struct AssertInfo
{
    /// Stringified condition, e.g. "index < size()".
    std::string_view expression;
    /// Optional human readable explanation. Empty when none was given.
    std::string_view message;
    /// File / line / function of the failling GSIM_ASSERT or GSIM_CHECK.
    std::source_location location;
};

/// A handler may return; execution then continues past the failed check.
/// The default handler prints to stderr and calls std::abort().
using AssertHandler = void (*)(const AssertInfo&);

/// Installs a new handler and returns the previous one. Thread safe.
AssertHandler set_assert_handler(AssertHandler handler) noexcept;

/// Currently installed handler. Never null.
[[nodiscard]] AssertHandler assert_handler() noexcept;

/// Called by the macros below. Not [[noreturn]]: a custom handler may return.
void report_assert_failure(std::string_view expression, std::string_view message,
                           std::source_location location = std::source_location::current());

/// For code paths that must never be taken. Always terminates.
[[noreturn]] void report_unreachable(
    std::string_view message, std::source_location location = std::source_location::current());

}  // namespace gsim::core


// ---------------------------------------------------------------------------
// Macros
//
//   GSIM_CHECK  - always active, in every build type.
//   GSIM_ASSERT - active only when GSIM_ENABLE_ASSERTS is defined.
//
// Use GSIM_CHECK for invariants whose violation corrupts the simulation, and
// GSIM_ASSERT for hot-path sanity checks you are willing to pay for only in
// development builds.
// ---------------------------------------------------------------------------

#define GSIM_CHECK_MSG(cond, msg)                              \
    do {                                                       \
        if (!static_cast<bool>(cond)) [[unlikely]] {           \
            ::gsim::core::report_assert_failure(#cond, (msg)); \
        }                                                      \
    } while (false)

#define GSIM_CHECK(cond) GSIM_CHECK_MSG(cond, "")

/// Compiles the expression away without evaluating it. sizeof is an
/// unevaluated context, so the condition is still type checked - a disabled
/// assert cannot silently rot when the code around it changes.
#define GSIM_DETAIL_DISCARD(cond) ((void)sizeof(static_cast<bool>(cond)))

#if defined(GSIM_ENABLE_ASSERTS)
#define GSIM_ASSERT_MSG(cond, msg) GSIM_CHECK_MSG(cond, msg)
#define GSIM_ASSERT(cond) GSIM_CHECK(cond)
#else
#define GSIM_ASSERT_MSG(cond, msg) GSIM_DETAIL_DISCARD(cond)
#define GSIM_ASSERT(cond) GSIM_DETAIL_DISCARD(cond)
#endif

#define GSIM_UNREACHABLE(msg) ::gsim::core::report_unreachable(msg)
