// cmake/runtime-config/test-only.h — 测试/CI 配置 (JIT=0, AOT=0)
//
// 纯解释器模式，不依赖任何 native codegen。用于单元测试、CI、模拟器。
// ============================================================================
#pragma once

#define CHAOS_IL2CPP_ENABLE_JIT 0
#define CHAOS_IL2CPP_ENABLE_AOT 0
