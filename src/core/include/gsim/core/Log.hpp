#pragma once

#include <atomic>
#include <chrono>
#include <format>
#include <memory>
#include <mutex>
#include <source_location>
#include <string_view>
#include <utility>
#include <vector>

namespace gsim::core {

/// Ordered by severity, so `level >= LogLevel::Warning` is a valid filter.
enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    /// Only valid as a category threshold: silences everything.
    Off,
};

/// "info", "warning", ... Never empty.
[[nodiscard]] std::string_view to_string(LogLevel level) noexcept;

/// A named logging channel with its own runtime threshold.
///
/// The constructor is constexpr so that global categories are constant
/// initialized. They can therefore be used before main() without any static
/// initialization order problem.
class LogCategory
{
public:
    constexpr LogCategory(std::string_view name, LogLevel threshold) noexcept
        : name_{name}, threshold_{threshold}
    {
    }

    LogCategory(const LogCategory&) = delete;
    LogCategory& operator=(const LogCategory&) = delete;

    [[nodiscard]] std::string_view name() const noexcept
    {
        return name_;
    }

    [[nodiscard]] LogLevel threshold() const noexcept
    {
        return threshold_.load(std::memory_order_relaxed);
    }

    void set_threshold(LogLevel level) noexcept
    {
        threshold_.store(level, std::memory_order_relaxed);
    }

    [[nodiscard]] bool is_enabled(LogLevel level) const noexcept
    {
        return level != LogLevel::Off && level >= threshold();
    }

private:
    std::string_view name_;
    std::atomic<LogLevel> threshold_;
};

/// One formatted log line, handed to every registered sink.
struct LogRecord
{
    LogLevel level;
    std::string_view category;
    /// Already formatted. Valid only for the duration of the write() call.
    std::string_view message;
    std::source_location location;
    std::chrono::system_clock::time_point time;
};

/// Destination for log records. Implementations must be thread safe.
class LogSink
{
public:
    LogSink() = default;
    virtual ~LogSink() = default;

    LogSink(const LogSink&) = delete;
    LogSink& operator=(const LogSink&) = delete;

    virtual void write(const LogRecord& record) = 0;
};

/// Writes human readable lines to stderr.
class ConsoleSink final : public LogSink
{
public:
    void write(const LogRecord& record) override;
};

/// Registry of sinks. Categories live wherever they are defined; the logger
/// only knows where records go.
class Logger
{
public:
    [[nodiscard]] static Logger& instance() noexcept;

    void add_sink(std::shared_ptr<LogSink> sink);
    void clear_sinks() noexcept;
    [[nodiscard]] std::size_t sink_count() const noexcept;

    void dispatch(const LogRecord& record);

private:
    Logger() = default;

    mutable std::mutex mutex_;
    std::vector<std::shared_ptr<LogSink>> sinks_;
};

namespace detail {

void dispatch(LogLevel level, std::string_view category, std::string_view message,
              const std::source_location& location);

}  // namespace detail

/// Formats the message and hands it to every sink. Prefer the macros below,
/// which skip the formatting entirely when the category is not enabled.
template <typename... Args>
void log(const LogCategory& category, LogLevel level, const std::source_location& location,
         std::format_string<Args...> fmt, Args&&... args)
{
    detail::dispatch(level, category.name(), std::format(fmt, std::forward<Args>(args)...),
                     location);
}

}  // namespace gsim::core

// ---------------------------------------------------------------------------
// Category declaration / definition
//
//   Header:  GSIM_DECLARE_LOG_CATEGORY(LogPhysics);
//   Source:  GSIM_DEFINE_LOG_CATEGORY(LogPhysics, ::gsim::core::LogLevel::Info);
// ---------------------------------------------------------------------------

#define GSIM_DECLARE_LOG_CATEGORY(name) extern ::gsim::core::LogCategory name

#define GSIM_DEFINE_LOG_CATEGORY(name, threshold) \
    ::gsim::core::LogCategory name                \
    {                                             \
        #name, (threshold)                        \
    }

namespace gsim::core {

/// Catch-all category for the core module.
GSIM_DECLARE_LOG_CATEGORY(LogCore);

}  // namespace gsim::core

// ---------------------------------------------------------------------------
// Logging macros
//
// The threshold test happens before the arguments are evaluated, so a disabled
// GSIM_LOG_TRACE costs one relaxed atomic load and nothing else.
// ---------------------------------------------------------------------------

#define GSIM_LOG(category, level, ...)                                                        \
    do {                                                                                      \
        const ::gsim::core::LogLevel gsim_log_level_ = (level);                               \
        if ((category).is_enabled(gsim_log_level_)) {                                         \
            ::gsim::core::log((category), gsim_log_level_, ::std::source_location::current(), \
                              __VA_ARGS__);                                                   \
        }                                                                                     \
    } while (false)

#define GSIM_LOG_TRACE(category, ...) GSIM_LOG(category, ::gsim::core::LogLevel::Trace, __VA_ARGS__)
#define GSIM_LOG_DEBUG(category, ...) GSIM_LOG(category, ::gsim::core::LogLevel::Debug, __VA_ARGS__)
#define GSIM_LOG_INFO(category, ...) GSIM_LOG(category, ::gsim::core::LogLevel::Info, __VA_ARGS__)
#define GSIM_LOG_WARNING(category, ...) \
    GSIM_LOG(category, ::gsim::core::LogLevel::Warning, __VA_ARGS__)
#define GSIM_LOG_ERROR(category, ...) GSIM_LOG(category, ::gsim::core::LogLevel::Error, __VA_ARGS__)
#define GSIM_LOG_CRITICAL(category, ...) \
    GSIM_LOG(category, ::gsim::core::LogLevel::Critical, __VA_ARGS__)
