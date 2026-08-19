#include <format>
#include <string>

#include <gtest/gtest.h>

#include <gsim/core/Version.hpp>

namespace {

TEST(Version, ComponentsAreNonNegative)
{
    const auto v = gsim::core::version();

    EXPECT_GE(v.major, 0);
    EXPECT_GE(v.minor, 0);
    EXPECT_GE(v.patch, 0);
}

TEST(Version, StringIsNotEmpty)
{
    EXPECT_FALSE(gsim::core::version_string().empty());
}

TEST(Version, StringMatchesComponents)
{
    const auto v = gsim::core::version();
    const std::string expected = std::format("{}.{}.{}", v.major, v.minor, v.patch);

    EXPECT_EQ(gsim::core::version_string(), expected);
}

TEST(Version, DefaultConstructedIsZeroed)
{
    const gsim::core::Version v{};

    EXPECT_EQ(v.major, 0);
    EXPECT_EQ(v.minor, 0);
    EXPECT_EQ(v.patch, 0);
}

}  // namespace
