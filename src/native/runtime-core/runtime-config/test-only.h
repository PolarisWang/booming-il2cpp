// test-only.h — Runtime config: AOT only (JIT disabled, background thread crash)
#pragma once
#define CHAOS_IL2CPP_ENABLE_JIT 0
#define CHAOS_IL2CPP_ENABLE_AOT 1
#define CHAOS_IL2CPP_ENABLE_ASSERTIONS 1
#define CHAOS_IL2CPP_ENABLE_VERBOSE_LOGGING 1
#include <cstdint>
namespace chaos { namespace il2cpp { namespace runtime {
struct RuntimeConfig {
    bool jit = false; bool aot = true; bool eager_compile = false;
    bool verbose_logging = true; bool enable_assertions = true;
};
inline constexpr RuntimeConfig kRuntimeConfig{};
}}}
