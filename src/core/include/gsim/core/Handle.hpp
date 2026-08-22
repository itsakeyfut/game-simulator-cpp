#pragma once

#include <compare>
#include <format>
#include <functional>
#include <string>

#include "gsim/core/Types.hpp"

namespace gsim {

/// A generation indexed handle: a slot index plus a counter that is bumped
/// every time the slot is reused.
///
/// Storing raw indices in game code is a classic source of use-after-free:
/// entity 7 dies, slot 7 is reused by a new entity, and stale code silently
/// operates on the wrong object. Comparing the generation catches that.
///
/// `Tag` is a phantom type: it is never defined and never stored. It only
/// exists so that Handle<EntityTag> and Handle<MeshTag> are distinct types and
/// cannot be passed to each other's functions.
///
///     struct EntityTag;                     // declared, never defined
///     using EntityId = Handle<EntityTag>;
template <typename Tag>
class Handle
{
public:
    using IndexType = u32;
    using GenerationType = u32;

    /// Reserved by the invalid handle, so slots stop one short of 2^32.
    static constexpr IndexType max_index = ~IndexType{0} - 1;

    /// Default constructed handles are invalid.
    constexpr Handle() noexcept = default;

    constexpr Handle(IndexType index, GenerationType generation) noexcept
        : bits_{(static_cast<u64>(generation) << 32) | static_cast<u64>(index)}
    {
    }

    [[nodiscard]] static constexpr Handle invalid() noexcept
    {
        return Handle{};
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept
    {
        return bits_ != invalid_bits;
    }

    [[nodiscard]] constexpr IndexType index() const noexcept
    {
        return static_cast<IndexType>(bits_ & 0xFFFF'FFFFu);
    }

    [[nodiscard]] constexpr GenerationType generation() const noexcept
    {
        return static_cast<GenerationType>(bits_ >> 32);
    }

    /// The packed representation. Useful as a hash key or for serialization.
    [[nodiscard]] constexpr u64 bits() const noexcept
    {
        return bits_;
    }

    [[nodiscard]] static constexpr Handle from_bits(u64 bits) noexcept
    {
        Handle handle;
        handle.bits_ = bits;
        return handle;
    }

    /// Explicit so that `if (id)` works but `int n = id;` does not.
    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
        return is_valid();
    }

    /// Gives ==, !=, <, <=, > and >= in one line. Ordering follows the packed
    /// bits, which is arbitrary but stable - enough for sorting and std::map.
    [[nodiscard]] friend constexpr auto operator<=>(const Handle&,
                                                    const Handle&) noexcept = default;

private:
    static constexpr u64 invalid_bits = ~u64{0};

    u64 bits_{invalid_bits};
};

}  // namespace gsim

/// Lets handles be keys in unordered_map / unordered_set.
template <typename Tag>
struct std::hash<gsim::Handle<Tag>>
{
    [[nodiscard]] std::size_t operator()(const gsim::Handle<Tag>& handle) const noexcept
    {
        return std::hash<gsim::u64>{}(handle.bits());
    }
};

/// Lets handles be logged directly: GSIM_LOG_INFO(LogCore, "spawned {}", id);
/// Prints "index:generation", or "invalid".
template <typename Tag>
struct std::formatter<gsim::Handle<Tag>> : std::formatter<std::string>
{
    template <typename Context>
    auto format(const gsim::Handle<Tag>& handle, Context& context) const
    {
        if (!handle.is_valid()) {
            return std::formatter<std::string>::format("invalid", context);
        }

        return std::formatter<std::string>::format(
            std::format("{}:{}", handle.index(), handle.generation()), context);
    }
};
