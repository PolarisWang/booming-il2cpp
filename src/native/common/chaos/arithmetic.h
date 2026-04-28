#ifndef CHAOS_IL2CPP_COMMON_ARITHMETIC_H_
#define CHAOS_IL2CPP_COMMON_ARITHMETIC_H_

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>

namespace ChaosIl2cpp::Common {

// Wrapping arithmetic (unsigned-cast to avoid UB).

inline std::int32_t wrap_add(std::int32_t left, std::int32_t right) noexcept
{
    return static_cast<std::int32_t>(
        static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right));
}

inline std::int32_t wrap_sub(std::int32_t left, std::int32_t right) noexcept
{
    return static_cast<std::int32_t>(
        static_cast<std::uint32_t>(left) - static_cast<std::uint32_t>(right));
}

inline std::int32_t wrap_mul(std::int32_t left, std::int32_t right) noexcept
{
    return static_cast<std::int32_t>(
        static_cast<std::uint32_t>(left) * static_cast<std::uint32_t>(right));
}

// Checked division and remainder.

inline std::int32_t div(std::int32_t left, std::int32_t right)
{
    if (right == 0)
    {
        std::abort();
    }
    if (left == std::numeric_limits<std::int32_t>::min() && right == -1)
    {
        std::abort();
    }
    return static_cast<std::int32_t>(left / right);
}

inline std::int32_t rem(std::int32_t left, std::int32_t right)
{
    if (right == 0)
    {
        std::abort();
    }
    if (left == std::numeric_limits<std::int32_t>::min() && right == -1)
    {
        return 0;
    }
    return static_cast<std::int32_t>(left % right);
}

// Shift operations with masked shift amount.

inline std::int32_t shift_left(std::int32_t value, std::int32_t amount) noexcept
{
    const auto shift = static_cast<std::uint32_t>(amount) & 31U;
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(value) << shift);
}

inline std::int32_t shift_right(std::int32_t value, std::int32_t amount) noexcept
{
    const auto shift = static_cast<std::uint32_t>(amount) & 31U;
    if (shift == 0U)
    {
        return value;
    }
    const auto bits = static_cast<std::uint32_t>(value);
    if (value >= 0)
    {
        return static_cast<std::int32_t>(bits >> shift);
    }
    const auto fill = std::numeric_limits<std::uint32_t>::max() << (32U - shift);
    return static_cast<std::int32_t>((bits >> shift) | fill);
}

inline std::int32_t shift_right_unsigned(std::int32_t value, std::int32_t amount) noexcept
{
    const auto shift = static_cast<std::uint32_t>(amount) & 31U;
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(value) >> shift);
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_ARITHMETIC_H_
