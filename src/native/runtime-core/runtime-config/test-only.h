// test-only.h — Runtime configuration for verification/testing builds.
// JIT enabled for native execution; debugging JIT crash on tier background thread.
#pragma once

#define CHAOS_IL2CPP_ENABLE_JIT 1
#define CHAOS_IL2CPP_ENABLE_AOT 1
#define CHAOS_IL2CPP_ENABLE_ASSERTIONS 0
#define CHAOS_IL2CPP_ENABLE_VERBOSE_LOGGING 0

#include <cstdint>
namespace chaos { namespace il2cpp { namespace runtime {
struct RuntimeConfig {
    bool jit = false;             // JIT disabled (QuickJIT crashes on v1 JSON methods)
    bool aot = true;
    bool eager_compile = false;
    bool verbose_logging = true;
    bool enable_assertions = true;
};
inline constexpr RuntimeConfig kRuntimeConfig{};
}}}
