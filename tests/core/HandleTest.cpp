#include <algorithm>
#include <format>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include "gsim/core/Handle.hpp"

namespace {

struct EntityTag;
struct MeshTag;

using EntityId = gsim::Handle<EntityTag>;
using MeshId = gsim::Handle<MeshTag>;

// Phantom tags make the two handle types unrelated, even though they have the
// same layout.
static_assert(!std::is_same_v<EntityId, MeshId>);
static_assert(!std::is_convertible_v<EntityId, MeshId>);
static_assert(sizeof(EntityId) == sizeof(gsim::u64));

// Everything is constexpr, so handles can be used in compile time tables.
static_assert(!EntityId{}.is_valid());
static_assert(EntityId{7, 2}.index() == 7);
static_assert(EntityId{7, 2}.generation() == 2);
static_assert(EntityId{7, 2} == EntityId{7, 2});
static_assert(EntityId{7, 2} != EntityId{7, 3});

TEST(Handle, DefaultConstructedIsInvalid)
{
    const EntityId id;

    EXPECT_FALSE(id.is_valid());
    EXPECT_FALSE(static_cast<bool>(id));
    EXPECT_EQ(id, EntityId::invalid());
}

TEST(Handle, KeepsIndexAndGenerationApart)
{
    const EntityId id{42, 7};

    EXPECT_TRUE(id.is_valid());
    EXPECT_EQ(id.index(), 42u);
    EXPECT_EQ(id.generation(), 7u);
}

TEST(Handle, IndexZeroGenerationZeroIsStillValid)
{
    const EntityId id{0, 0};

    EXPECT_TRUE(id.is_valid());
    EXPECT_EQ(id.index(), 0u);
}

TEST(Handle, SameSlotDifferentGenerationsAreNotEqual)
{
    const EntityId old_id{3, 1};
    const EntityId reused{3, 2};

    EXPECT_NE(old_id, reused);
    EXPECT_LT(old_id, reused);
}

TEST(Handle, SurvivesABitsRoundTrip)
{
    const EntityId id{123, 456};

    EXPECT_EQ(EntityId::from_bits(id.bits()), id);
    EXPECT_EQ(EntityId::from_bits(EntityId::invalid().bits()), EntityId::invalid());
}

TEST(Handle, HandlesTheHighestUsableIndex)
{
    const EntityId id{EntityId::max_index, 0};

    EXPECT_TRUE(id.is_valid());
    EXPECT_EQ(id.index(), EntityId::max_index);
}

TEST(Handle, WorksAsAnUnorderedMapKey)
{
    std::unordered_map<EntityId, int> health;
    health[EntityId{1, 0}] = 100;
    health[EntityId{1, 1}] = 50;

    EXPECT_EQ(health.size(), 2u);
    EXPECT_EQ(health.at(EntityId{1, 0}), 100);
    EXPECT_EQ(health.at(EntityId{1, 1}), 50);
}

TEST(Handle, WorksInAnUnorderedSet)
{
    const std::unordered_set<EntityId> ids{EntityId{1, 0}, EntityId{2, 0}, EntityId{1, 0}};

    EXPECT_EQ(ids.size(), 2u);
}

TEST(Handle, IsSortable)
{
    std::vector<EntityId> ids{EntityId{2, 0}, EntityId{0, 1}, EntityId{1, 0}};
    std::sort(ids.begin(), ids.end());

    EXPECT_EQ(ids[0], EntityId(1, 0));
    EXPECT_EQ(ids[1], EntityId(2, 0));
    EXPECT_EQ(ids[2], EntityId(0, 1));
}

TEST(Handle, FormatsAsIndexAndGeneration)
{
    EXPECT_EQ(std::format("{}", EntityId{42, 7}), "42:7");
    EXPECT_EQ(std::format("{}", EntityId::invalid()), "invalid");
    EXPECT_EQ(std::format("[{:>10}]", EntityId{1, 2}), "[       1:2]");
}

}  // namespace
