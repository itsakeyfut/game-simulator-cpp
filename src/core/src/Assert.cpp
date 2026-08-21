#include "gsim/core/Assert.hpp"

#include <atomic>
#include <cstdio>
#include <cstdlib>

namespace gsim::core {

namespace {

void print(const AssertInfo& info, const char* kind)
{
    // string_view is not guaranteed to be null terminated, hence "%.*s".
    std::fprintf(stderr, "\n[%s] %s:%u (%s)\n", kind, info.location.file_name(),
                 static_cast<unsigned>(info.location.line()), info.location.function_name());
    std::fprintf(stderr, "  condition: %.*s\n", static_cast<int>(info.expression.size()),
                 info.expression.data());

    if (!info.message.empty()) {
        std::fprintf(stderr, "  message:   %.*s\n", static_cast<int>(info.message.size()),
                     info.message.data());
    }

    std::fflush(stderr);
}

void default_handler(const AssertInfo& info)
{
    print(info, "assert");
    std::abort();
}

std::atomic<AssertHandler> g_handler{&default_handler};

}  // namespace

AssertHandler set_assert_handler(AssertHandler handler) noexcept
{
    return g_handler.exchange(handler != nullptr ? handler : &default_handler);
}

AssertHandler assert_handler() noexcept
{
    return g_handler.load();
}

void report_assert_failure(std::string_view expression, std::string_view message,
                           std::source_location location)
{
    const AssertInfo info{expression, message, location};
    assert_handler()(info);
}

void report_unreachable(std::string_view message, std::source_location location)
{
    const AssertInfo info{"unreachable code reached", message, location};

    assert_handler()(info);

    // Reached only when a custom handler chose to return. This function is
    // [[noreturn]], so there is no way back.
    print(info, "unreachable");
    std::abort();
}

}  // namespace gsim::core
