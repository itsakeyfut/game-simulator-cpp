#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "gsim/core/Log.hpp"

namespace {

using gsim::core::LogLevel;

struct Captured
{
    LogLevel level{};
    std::string category;
    std::string message;
    std::uint_least32_t line{};
};

/// Sink that keeps records in memory so tests can inspect them.
class MemorySink final : public gsim::core::LogSink
{
public:
    void write(const gsim::core::LogRecord& record) override
    {
        records.push_back(Captured{record.level, std::string{record.category},
                                   std::string{record.message}, record.location.line()});
    }

    std::vector<Captured> records;
};

GSIM_DEFINE_LOG_CATEGORY(LogTestCategory, LogLevel::Trace);

class LogTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        auto& logger = gsim::core::Logger::instance();
        logger.clear_sinks();

        sink_ = std::make_shared<MemorySink>();
        logger.add_sink(sink_);

        LogTestCategory.set_threshold(LogLevel::Trace);
    }

    void TearDown() override
    {
        gsim::core::Logger::instance().clear_sinks();
    }

    [[nodiscard]] const std::vector<Captured>& records() const
    {
        return sink_->records;
    }

private:
    std::shared_ptr<MemorySink> sink_;
};

TEST_F(LogTest, FormatsArgumentsIntoTheMessage)
{
    GSIM_LOG_INFO(LogTestCategory, "spawned {} entities in {:.2f} ms", 42, 1.5);

    ASSERT_EQ(records().size(), 1u);
    EXPECT_EQ(records()[0].message, "spawned 42 entities in 1.50 ms");
    EXPECT_EQ(records()[0].level, LogLevel::Info);
    EXPECT_EQ(records()[0].category, "LogTestCategory");
}

TEST_F(LogTest, RecordsTheCallSite)
{
    const auto expected = static_cast<std::uint_least32_t>(__LINE__ + 1);
    GSIM_LOG_WARNING(LogTestCategory, "careful");

    ASSERT_EQ(records().size(), 1u);
    EXPECT_EQ(records()[0].line, expected);
}

TEST_F(LogTest, ThresholdDropsLowerLevels)
{
    LogTestCategory.set_threshold(LogLevel::Warning);

    GSIM_LOG_TRACE(LogTestCategory, "trace");
    GSIM_LOG_INFO(LogTestCategory, "info");
    GSIM_LOG_WARNING(LogTestCategory, "warning");
    GSIM_LOG_ERROR(LogTestCategory, "error");

    ASSERT_EQ(records().size(), 2u);
    EXPECT_EQ(records()[0].level, LogLevel::Warning);
    EXPECT_EQ(records()[1].level, LogLevel::Error);
}

TEST_F(LogTest, OffSilencesEverything)
{
    LogTestCategory.set_threshold(LogLevel::Off);

    GSIM_LOG_CRITICAL(LogTestCategory, "still here?");

    EXPECT_TRUE(records().empty());
}

TEST_F(LogTest, DisabledLevelDoesNotEvaluateArguments)
{
    LogTestCategory.set_threshold(LogLevel::Error);

    int calls = 0;
    const auto expensive = [&calls] {
        ++calls;
        return 1;
    };

    GSIM_LOG_DEBUG(LogTestCategory, "value {}", expensive());

    EXPECT_EQ(calls, 0);
    EXPECT_TRUE(records().empty());
}

TEST_F(LogTest, CategoriesAreIndependent)
{
    LogTestCategory.set_threshold(LogLevel::Error);

    GSIM_LOG_INFO(LogTestCategory, "dropped");
    GSIM_LOG_INFO(gsim::core::LogCore, "kept");

    ASSERT_EQ(records().size(), 1u);
    EXPECT_EQ(records()[0].category, "LogCore");
}

TEST_F(LogTest, EveryRegisteredSinkReceivesTheRecord)
{
    auto second = std::make_shared<MemorySink>();
    gsim::core::Logger::instance().add_sink(second);

    GSIM_LOG_INFO(LogTestCategory, "broadcast");

    EXPECT_EQ(records().size(), 1u);
    EXPECT_EQ(second->records.size(), 1u);
}

TEST(LogLevelName, EveryLevelHasAName)
{
    EXPECT_EQ(gsim::core::to_string(LogLevel::Trace), "trace");
    EXPECT_EQ(gsim::core::to_string(LogLevel::Info), "info");
    EXPECT_EQ(gsim::core::to_string(LogLevel::Critical), "critical");
    EXPECT_EQ(gsim::core::to_string(LogLevel::Off), "off");
}

}  // namespace
