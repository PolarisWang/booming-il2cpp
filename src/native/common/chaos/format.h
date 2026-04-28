#ifndef CHAOS_IL2CPP_COMMON_FORMAT_H_
#define CHAOS_IL2CPP_COMMON_FORMAT_H_

#include <fmt/format.h>

#include <cstdint>
#include <string>

namespace ChaosIl2cpp::Common {

/// Efficient int32-to-string conversion (replaces snprintf + char[64] pattern).
inline CHAOS_IL2CPP_STRING format_int32(CHAOS_IL2CPP_INT32 value)
{
    return fmt::format("{}", value);
}

/// Float-to-string with "F1" format (one decimal place).
inline CHAOS_IL2CPP_STRING format_float(float value)
{
    return fmt::format("{:.1f}", static_cast<double>(value));
}

/// Double-to-string with "F1" format (one decimal place).
inline CHAOS_IL2CPP_STRING format_double(double value)
{
    return fmt::format("{:.1f}", value);
}

/// Generic format for any format-string + arguments.
template <typename... Args>
inline CHAOS_IL2CPP_STRING format_string(fmt::format_string<Args...> fmt_str, Args&&... args)
{
    return fmt::format(fmt_str, std::forward<Args>(args)...);
}

/// Append formatted output to an existing CHAOS_IL2CPP_STRING.
/// Useful in loop contexts (e.g. String.Join over enumerable).
template <typename... Args>
inline void format_append_to(CHAOS_IL2CPP_STRING& output, fmt::format_string<Args...> fmt_str, Args&&... args)
{
    fmt::format_to(std::back_inserter(output), fmt_str, std::forward<Args>(args)...);
}

}  // namespace ChaosIl2cpp::Common

#endif  // CHAOS_IL2CPP_COMMON_FORMAT_H_
