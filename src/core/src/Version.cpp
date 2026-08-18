#include "gsim/core/Version.hpp"

namespace gsim::core {

Version version() noexcept
{
    return Version{GSIM_VERSION_MAJOR, GSIM_VERSION_MINOR, GSIM_VERSION_PATCH};
}

std::string_view version_string() noexcept
{
    return GSIM_VERSION_STRING;
}

}  // namespace gsim::core
