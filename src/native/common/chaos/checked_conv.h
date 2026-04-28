#ifndef CHAOS_IL2CPP_COMMON_CHECKED_CONV_H_
#define CHAOS_IL2CPP_COMMON_CHECKED_CONV_H_

#include <cstdint>
#include <limits>

namespace ChaosIl2cpp::Common {

inline std::intptr_t checked_conv_ovf_i1(std::intptr_t value) noexcept
{
    if (value < static_cast<std::intptr_t>(std::numeric_limits<std::int8_t>::min()) ||
        value > static_cast<std::intptr_t>(std::numeric_limits<std::int8_t>::max()))
    {
        std::abort();
    }
    return static_cast<std::intptr_t>(static_cast<std::int8_t>(value));
}

inline std::intptr_t checked_conv_ovf_u1(std::intptr_t value) noexcept
{
    if (value < static_cast<std::intptr_t>(0) ||
        value > static_cast<std::intptr_t>(std::numeric_limits<std::uint8_t>::max()))
    {
        std::abort();
    }
    return static_cast<std::intptr_t>(static_cast<std::uint8_t>(value));
}

inline std::intptr_t checked_conv_ovf_i2(std::intptr_t value) noexcept
{
    if (value < static_cast<std::intptr_t>(std::numeric_limits<std::int16_t>::min()) ||
        value > static_cast<std::intptr_t>(std::numeric_limits<std::int16_t>::max()))
    {
        std::abort();
    }
    return static_cast<std::intptr_t>(static_cast<std::int16_t>(value));
}

inline std::intptr_t checked_conv_ovf_u2(std::intptr_t value) noexcept
{
    if (value < static_cast<std::intptr_t>(0) ||
        value > static_cast<std::intptr_t>(std::numeric_limits<std::uint16_t>::max()))
    {
        std::abort();
    }
    return static_cast<std::intptr_t>(static_cast<std::uint16_t>(value));
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_CHECKED_CONV_H_
