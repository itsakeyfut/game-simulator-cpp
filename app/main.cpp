#include <format>
#include <iostream>
#include <memory>

#include "gsim/core/Log.hpp"
#include "gsim/core/Version.hpp"

int main()
{
    gsim::core::Logger::instance().add_sink(std::make_shared<gsim::core::ConsoleSink>());

    const auto v = gsim::core::version();

    std::cout << std::format("gsim {} ({}.{}.{})\n", gsim::core::version_string(), v.major, v.minor,
                             v.patch);
    std::cout << "headless 3d game simulation\n";

    GSIM_LOG_INFO(gsim::core::LogCore, "logger ready with {} sink(s)",
                  gsim::core::Logger::instance().sink_count());
    GSIM_LOG_DEBUG(gsim::core::LogCore, "this line is dropped: LogCore starts at info");
    GSIM_LOG_WARNING(gsim::core::LogCore, "simulation loop not implemented yet");


    return 0;
}
