#ifndef CHAOS_IL2CPP_COMMON_STATUS_H_
#define CHAOS_IL2CPP_COMMON_STATUS_H_

#include <cstdint>

namespace chaos::il2cpp::common {

// Unified status codes covering both RuntimeAbi and CodegenBridge ranges.

enum class Status : CHAOS_IL2CPP_INT32 {
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

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_STATUS_H_
