#ifndef CHAOS_IL2CPP_COMMON_ARITHMETIC_H_
#define CHAOS_IL2CPP_COMMON_ARITHMETIC_H_

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>

namespace chaos::il2cpp::common {

// Wrapping arithmetic (unsigned-cast to avoid UB).

inline CHAOS_IL2CPP_INT32 wrap_add(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        static_cast<CHAOS_IL2CPP_UINT32>(left) + static_cast<CHAOS_IL2CPP_UINT32>(right));
}

inline CHAOS_IL2CPP_INT32 wrap_sub(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        static_cast<CHAOS_IL2CPP_UINT32>(left) - static_cast<CHAOS_IL2CPP_UINT32>(right));
}

inline CHAOS_IL2CPP_INT32 wrap_mul(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        static_cast<CHAOS_IL2CPP_UINT32>(left) * static_cast<CHAOS_IL2CPP_UINT32>(right));
}

// Checked division and remainder.

inline CHAOS_IL2CPP_INT32 div(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    if (right == 0)
    {
        CHAOS_IL2CPP_FAIL();
    }
    if (left == CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32) && right == -1)
    {
        CHAOS_IL2CPP_FAIL();
    }
    return static_cast<CHAOS_IL2CPP_INT32>(left / right);
}

inline CHAOS_IL2CPP_INT32 rem(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    if (right == 0)
    {
        CHAOS_IL2CPP_FAIL();
    }
    if (left == CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32) && right == -1)
    {
        return 0;
    }
    return static_cast<CHAOS_IL2CPP_INT32>(left % right);
}

// Shift operations with masked shift amount.

inline CHAOS_IL2CPP_INT32 shift_left(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 amount) noexcept
{
    const auto shift = static_cast<CHAOS_IL2CPP_UINT32>(amount) & 31U;
    return static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>(value) << shift);
}

inline CHAOS_IL2CPP_INT32 shift_right(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 amount) noexcept
{
    const auto shift = static_cast<CHAOS_IL2CPP_UINT32>(amount) & 31U;
    if (shift == 0U)
    {
        return value;
    }
    const auto bits = static_cast<CHAOS_IL2CPP_UINT32>(value);
    if (value >= 0)
    {
        return static_cast<CHAOS_IL2CPP_INT32>(bits >> shift);
    }
    const auto fill = CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT32) << (32U - shift);
    return static_cast<CHAOS_IL2CPP_INT32>((bits >> shift) | fill);
}

inline CHAOS_IL2CPP_INT32 shift_right_unsigned(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 amount) noexcept
{
    const auto shift = static_cast<CHAOS_IL2CPP_UINT32>(amount) & 31U;
    return static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>(value) >> shift);
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_ARITHMETIC_H_
