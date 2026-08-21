#include <cstdint>
#include <string>

#include <gtest/gtest.h>

#include "gsim/core/Assert.hpp"

namespace {

struct Recorded
{
    int count{0};
    std::string expression;
    std::string message;
    std::uint_least32_t line{0};
};

Recorded g_recorded;

void recording_handler(const gsim::core::AssertInfo& info)
{
    ++g_recorded.count;
    g_recorded.expression = std::string{info.expression};
    g_recorded.message = std::string{info.message};
    g_recorded.line = info.location.line();
}

/// Swaps in a handler that records instead of aborting, so a failing check can
/// be observed from a test without killing the process.
class AssertTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        g_recorded = Recorded{};
        previous_ = gsim::core::set_assert_handler(&recording_handler);
    }

    void TearDown() override
    {
        gsim::core::set_assert_handler(previous_);
    }

private:
    gsim::core::AssertHandler previous_{nullptr};
};

TEST_F(AssertTest, PassingCheckDoesNothing)
{
    GSIM_CHECK(1 + 1 == 2);

    EXPECT_EQ(g_recorded.count, 0);
}

TEST_F(AssertTest, FailingCheckReportsTheExpressionText)
{
    const int index = 7;
    const int size = 4;

    GSIM_CHECK(index < size);

    EXPECT_EQ(g_recorded.count, 1);
    EXPECT_EQ(g_recorded.expression, "index < size");
    EXPECT_TRUE(g_recorded.message.empty());
}

TEST_F(AssertTest, FailingCheckCarriesTheMessage)
{
    GSIM_CHECK_MSG(false, "entity id must be alive");

    EXPECT_EQ(g_recorded.message, "entity id must be alive");
}

TEST_F(AssertTest, LocationPointsAtTheCallSite)
{
    const auto expected = static_cast<std::uint_least32_t>(__LINE__ + 1);
    GSIM_CHECK(false);

    EXPECT_EQ(g_recorded.line, expected);
}

TEST_F(AssertTest, ConditionIsEvaluatedExactlyOnce)
{
    int calls = 0;
    const auto probe = [&calls] {
        ++calls;
        return false;
    };

    GSIM_CHECK(probe());

    EXPECT_EQ(calls, 1);
    EXPECT_EQ(g_recorded.count, 1);
}

TEST_F(AssertTest, MacroIsASingleStatement)
{
    // Compiles only because the macro uses the do/while(false) idiom.
    if (true)
        GSIM_CHECK(false);
    else
        GSIM_CHECK(true);

    EXPECT_EQ(g_recorded.count, 1);
}

#if defined(GSIM_ENABLE_ASSERTS)

TEST_F(AssertTest, AssertIsActiveInThisBuild)
{
    GSIM_ASSERT(false);

    EXPECT_EQ(g_recorded.count, 1);
}

#else

TEST_F(AssertTest, AssertIsCompiledOutInThisBuild)
{
    int calls = 0;
    const auto probe = [&calls] {
        ++calls;
        return false;
    };

    GSIM_ASSERT(probe());

    EXPECT_EQ(calls, 0);
    EXPECT_EQ(g_recorded.count, 0);
}

#endif

}  // namespace
