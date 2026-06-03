// cmake/runtime-config/mobile-release.h — 移动端发布配置 (JIT=0, AOT=1)
//
// iOS/Android 发布版：AOT 预编译代码直接 dispatch，热更新 DLL 没有 AOT
// 入口所以自然走解释器。JIT 不可用（iOS W^X / Android 非 root 限制）。
// ============================================================================
#pragma once

#include <chaos/runtime/execution_config.h>

namespace chaos::il2cpp::runtime {

inline constexpr ExecutionConfig kRuntimeConfig{false, true, 0};

}
