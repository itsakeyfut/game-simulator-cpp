#include "gsim/core/Log.hpp"

#include <cstdio>

namespace gsim::core {

GSIM_DEFINE_LOG_CATEGORY(LogCore, LogLevel::Info);

std::string_view to_string(LogLevel level) noexcept
{
    switch (level) {
        case LogLevel::Trace:
            return "trace";
        case LogLevel::Debug:
            return "debug";
        case LogLevel::Info:
            return "info";
        case LogLevel::Warning:
            return "warning";
        case LogLevel::Error:
            return "error";
        case LogLevel::Critical:
            return "critical";
        case LogLevel::Off:
            return "off";
    }
    return "unknown";
}

void ConsoleSink::write(const LogRecord& record)
{
    const auto stamp = std::chrono::floor<std::chrono::milliseconds>(record.time);

    // Errors also carry the call site; routine lines stay short and readable.
    std::string line;
    if (record.level >= LogLevel::Error) {
        line = std::format("[{:%H:%M:%S}] [{:<8}] [{}] {} ({}:{})\n", stamp,
                           to_string(record.level), record.category, record.message,
                           record.location.file_name(), record.location.line());
    } else {
        line = std::format("[{:%H:%M:%S}] [{:<8}] [{}] {}\n", stamp, to_string(record.level),
                           record.category, record.message);
    }

    // One fwrite per record: stderr is unbuffered, so partial writes from
    // several threads would interleave mid-line.
    std::fwrite(line.data(), 1, line.size(), stderr);
}

Logger& Logger::instance() noexcept
{
    static Logger logger;
    return logger;
}

void Logger::add_sink(std::shared_ptr<LogSink> sink)
{
    if (sink == nullptr) {
        return;
    }

    const std::lock_guard lock{mutex_};
    sinks_.push_back(std::move(sink));
}

void Logger::clear_sinks() noexcept
{
    const std::lock_guard lock{mutex_};
    sinks_.clear();
}

std::size_t Logger::sink_count() const noexcept
{
    const std::lock_guard lock{mutex_};
    return sinks_.size();
}

void Logger::dispatch(const LogRecord& record)
{
    // Copy the handles out so a sink cannot deadlock by logging from write().
    std::vector<std::shared_ptr<LogSink>> targets;
    {
        const std::lock_guard lock{mutex_};
        targets = sinks_;
    }

    for (const auto& sink : targets) {
        sink->write(record);
    }
}

namespace detail {

void dispatch(LogLevel level, std::string_view category, std::string_view message,
              const std::source_location& location)
{
    const LogRecord record{level, category, message, location, std::chrono::system_clock::now()};

    Logger::instance().dispatch(record);
}

}  // namespace detail

}  // namespace gsim::core
