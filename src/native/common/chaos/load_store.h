#ifndef CHAOS_IL2CPP_COMMON_LOAD_STORE_H_
#define CHAOS_IL2CPP_COMMON_LOAD_STORE_H_

#include <cstdint>
#include <cstring>

namespace ChaosIl2cpp::Common {

// Float32 store/load via bitcast through intptr_t.

inline std::intptr_t store_float32(float value) noexcept
{
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(value));
    return static_cast<std::intptr_t>(static_cast<std::uint64_t>(bits));
}

inline float load_float32(std::intptr_t value) noexcept
{
    const auto bits = static_cast<std::uint32_t>(static_cast<std::uint64_t>(value));
    float result = 0.0f;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

// Float64 store/load via bitcast through intptr_t.

inline std::intptr_t store_float64(double value) noexcept
{
    std::uint64_t bits = 0;
    std::memcpy(&bits, &value, sizeof(value));
    std::intptr_t result = 0;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

inline double load_float64(std::intptr_t value) noexcept
{
    std::uint64_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    double result = 0.0;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

// Int64 store/load via memcpy.

inline std::intptr_t store_int64(std::int64_t value) noexcept
{
    std::intptr_t result = 0;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

inline std::int64_t load_int64(std::intptr_t value) noexcept
{
    std::int64_t result = 0;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

// UInt64 store/load via memcpy.

inline std::intptr_t store_uint64(std::uint64_t value) noexcept
{
    std::intptr_t result = 0;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

inline std::uint64_t load_uint64(std::intptr_t value) noexcept
{
    std::uint64_t result = 0;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_LOAD_STORE_H_
