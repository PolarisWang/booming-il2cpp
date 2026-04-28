#ifndef CHAOS_IL2CPP_COMMON_STATUS_H_
#define CHAOS_IL2CPP_COMMON_STATUS_H_

#include <cstdint>

namespace ChaosIl2cpp::Common {

// Unified status codes covering both RuntimeAbi and CodegenBridge ranges.

enum class Status : std::int32_t {
    kOk = 0,
    kInvalidArgument = 1,
    kInvalidState = 2,
    kNotFound = 3,
    kNotSupported = 4,
    kManagedException = 5,
    kInternalError = 6,
    kMetadataResolutionFailed = 7,
    kClassInitFailed = 8,
    kRuntimeCallFailed = 9,
};

inline constexpr bool is_ok(Status s) noexcept { return s == Status::kOk; }
inline constexpr bool is_failure(Status s) noexcept { return s != Status::kOk; }

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_STATUS_H_
