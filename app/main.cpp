#include <format>
#include <iostream>

#include "gsim/core/Version.hpp"

int main()
{
    const auto v = gsim::core::version();

    std::cout << std::format("gsim {} ({}.{}.{})\n",
                             gsim::core::version_string(),
                             v.major, v.minor, v.patch);
    std::cout << "headless 3d game simulation\n";

    return 0;
}