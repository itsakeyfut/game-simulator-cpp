#pragma once

#include <string_view>

namespace gsim::core {

/// Semantic version of the simulation library.
struct Version {
    int major{};
    int minor{};
    int patch{};
};

/// Version this library was built with.
[[nodiscard]] Version version() noexcept;

/// Human readable form, e.g. "0.1.0"
[[nodiscard]] std::string_view version_string() noexcept;

}  // namespace gsim::core
