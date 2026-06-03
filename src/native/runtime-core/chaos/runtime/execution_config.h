// chaos/runtime/execution_config.h — 执行引擎配置（编译时常量）
//
// 定义 ExecutionConfig struct，平台特定的 kRuntimeConfig 值由 CMake 选择
// 的 runtime-config/*.h 文件提供。
//
// 解释器始终可用。这些 bit 控制哪些 ADDITIONAL native 执行引擎被激活。
// 热更新方法（运行时加载的 DLL）没有 AOT 或 JIT 入口，总走解释器。
//
// 使用方式：
//   #include <chaos/runtime/execution_config.h>
//   if constexpr (kRuntimeConfig.jit) { jit::Compile(...); }
//   if constexpr (kRuntimeConfig.aot) { ExecuteAotCode(...); }
// ============================================================================

#pragma once

#include <cstdint>

namespace chaos::il2cpp::runtime {

/// 执行引擎配置位域。
///
/// 平台配置对照表：
/// ┌─────────────────┬─────┬─────┬──────────────────────────────────────────┐
/// │ 配置             │ JIT │ AOT │ 目标平台                                  │
/// ├─────────────────┼─────┼─────┼──────────────────────────────────────────┤
/// │ desktop         │  1  │  1  │ Linux/macOS/Windows 桌面全模式              │
/// │ mobile-release  │  0  │  1  │ iOS/Android 发布版（热更 DLL 走解释器）     │
/// │ test-only       │  0  │  0  │ 纯解释器（单元测试/CI/模拟器）             │
/// └─────────────────┴─────┴─────┴──────────────────────────────────────────┘
struct alignas(2) ExecutionConfig {
    // ── Core execution engines ──────────────────────────────────────────
    bool jit  : 1;  ///< JIT runtime codegen (T3→T4 tier-up)
    bool aot  : 1;  ///< Precompiled AOT native code dispatch

    // ── Expansion budget: 14 bits for future capabilities ──────────────
    // 候选 bit（只示意不实现，防命名冲突）:
    //   bit 2: eventpipe       — EventPipe diagnostic tracing
    //   bit 3: pgo_collect     — PGO data collection
    //   bit 4: w_x_mode        — W^X enforcement mode
    //   bit 5: r2r             — Ready-to-run global offset
    uint16_t reserved : 14;

    // ── Predefined platform profiles ───────────────────────────────────
    static constexpr ExecutionConfig TestOnly() noexcept { return {}; }
    static constexpr ExecutionConfig DesktopDev() noexcept { return {true, true, 0}; }
    static constexpr ExecutionConfig MobileRelease() noexcept { return {false, true, 0}; }
};

}  // namespace chaos::il2cpp::runtime

// ── 加载平台特定的 kRuntimeConfig 定义 ─────────────────────────────
// CMake 在 runtime-core CMakeLists.txt 中定义此宏，指向具体的
// cmake/runtime-config/<profile>.h。
#ifndef CHAOS_IL2CPP_RUNTIME_CONFIG_FILE
#error "CHAOS_IL2CPP_RUNTIME_CONFIG_FILE not defined — set via CMake (see runtime-core/CMakeLists.txt)"
#endif
#include CHAOS_IL2CPP_RUNTIME_CONFIG_FILE
