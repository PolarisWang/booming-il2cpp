#ifndef CHAOS_IL2CPP_COMMON_CHECKED_CONV_H_
#define CHAOS_IL2CPP_COMMON_CHECKED_CONV_H_

#include <cstdint>
#include <limits>

namespace chaos::il2cpp::common {

inline CHAOS_IL2CPP_INTPTR checked_conv_ovf_i1(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value < static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT8)) ||
        value > static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_INT8)))
    {
        CHAOS_IL2CPP_ABORT();
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(value));
}

inline CHAOS_IL2CPP_INTPTR checked_conv_ovf_u1(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value < static_cast<CHAOS_IL2CPP_INTPTR>(0) ||
        value > static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT8)))
    {
        CHAOS_IL2CPP_ABORT();
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(value));
}

inline CHAOS_IL2CPP_INTPTR checked_conv_ovf_i2(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value < static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT16)) ||
        value > static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_INT16)))
    {
        CHAOS_IL2CPP_ABORT();
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(value));
}

inline CHAOS_IL2CPP_INTPTR checked_conv_ovf_u2(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value < static_cast<CHAOS_IL2CPP_INTPTR>(0) ||
        value > static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT16)))
    {
        CHAOS_IL2CPP_ABORT();
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT16>(value));
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_CHECKED_CONV_H_
