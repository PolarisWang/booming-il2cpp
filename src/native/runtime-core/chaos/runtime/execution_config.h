// chaos/runtime/execution_config.h — 执行引擎配置（编译时常量宏）
//
// 定义 CHAOS_IL2CPP_ENABLE_JIT 和 CHAOS_IL2CPP_ENABLE_AOT 两个编译时宏。
// 这两个宏由 CMake 选择的 cmake/runtime-config/<profile>.h 文件提供。
//
// 解释器始终可用。这些宏控制哪些 ADDITIONAL native 执行引擎被激活。
//
// 使用方式：
//   #include <chaos/runtime/execution_config.h>
//   #if CHAOS_IL2CPP_ENABLE_JIT
//       jit::Compile(...);
//   #endif
//   #if CHAOS_IL2CPP_ENABLE_AOT
//       ExecuteAotCode(...);
//   #endif
//
// 平台配置对照表：
// ┌─────────────────┬─────┬─────┬──────────────────────────────────────────┐
// │ 配置             │ JIT │ AOT │ 目标平台                                  │
// ├─────────────────┼─────┼─────┼──────────────────────────────────────────┤
// │ desktop         │  1  │  1  │ Linux/macOS/Windows 桌面全模式              │
// │ mobile-release  │  0  │  1  │ iOS/Android 发布版（热更 DLL 走解释器）     │
// │ test-only       │  0  │  0  │ 纯解释器（单元测试/CI/模拟器）              │
// └─────────────────┴─────┴─────┴──────────────────────────────────────────┘
//
// 注意：CHAOS_IL2CPP_ENABLE_JIT/AOT 是编译时常量。运行时切换不在本方案
// 支持范围内。需要在运行时切换的场景应使用 tier_state per-method 机制。
// ============================================================================

#pragma once

// ── 加载平台特定的宏定义 ─────────────────────────────────────────────
// CMake 在 runtime-core CMakeLists.txt 中定义此宏，指向具体的
// cmake/runtime-config/<profile>.h。
#ifndef CHAOS_IL2CPP_RUNTIME_CONFIG_FILE
#error "CHAOS_IL2CPP_RUNTIME_CONFIG_FILE not defined — set via CMake (see runtime-core/CMakeLists.txt)"
#endif
#include CHAOS_IL2CPP_RUNTIME_CONFIG_FILE

// ── 编译时断言：确保宏已定义 ────────────────────────────────────────
#ifndef CHAOS_IL2CPP_ENABLE_JIT
#error "CHAOS_IL2CPP_ENABLE_JIT must be defined by the runtime config file"
#endif
#ifndef CHAOS_IL2CPP_ENABLE_AOT
#error "CHAOS_IL2CPP_ENABLE_AOT must be defined by the runtime config file"
#endif
