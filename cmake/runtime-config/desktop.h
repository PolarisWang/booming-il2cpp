// cmake/runtime-config/desktop.h — desktop 平台执行配置 (JIT=1, AOT=1)
// ============================================================================
#pragma once

#include <chaos/runtime/execution_config.h>

namespace chaos::il2cpp::runtime {

inline constexpr ExecutionConfig kRuntimeConfig{true, true, 0};

}
