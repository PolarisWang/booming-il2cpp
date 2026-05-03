// Benchmark support header — provides stub definitions for symbols that the
// CodeGen pipeline emits but the hotupdate post-processing normally resolves.
//
// The genuine/generated/native-aot.generated.cpp references several symbols
// that are NOT defined in the raw generated output:
//   - chaos_managed_pointer_local_slot_tag (stripped by hotupdate pipeline)
//   - chaos_reflection_set_exception_metadata (runtime helper)
//   - chaos_reflection_get_exception_message (runtime helper)
//   - chaos_is_string_id / CHAOS_STRING_ID_TAG (in codegen_bridge.h)
//   - string_table::StringEntry, string_table::StringId (in string_table.h)
//
// In the hotupdate pipeline, generate_hotupdate_test.py _strip_extern_wrappers
// REMOVES the extern "C" chaos_external_runtime_* function bodies entirely,
// and _rename_and_fix_patch_file uncomments the managed_pointer constexpr.
//
// For benchmarks we need the file to compile as-is, so this header provides
// the missing definitions as minimal stubs. The stubs do NOT execute real
// runtime logic — they exist only to satisfy the linker so the benchmark
// executable can measure the IL-to-C++ translation path performance.
//
// This header is force-included via CMAKE_CXX_FLAGS " -include benchmark_support.h"
// and MUST NOT be #include-d from any normal (non-benchmark) translation unit.

#ifndef CHAOS_BENCHMARK_SUPPORT_H_
#define CHAOS_BENCHMARK_SUPPORT_H_

#include <chaos/common.h>
#include <cstdint>
#include <memory>

// Fix: CHAOS_IL2CPP_MAKE_UNIQUE(T) is defined without () in native_types.h,
// but monitor.h uses it as `entry = CHAOS_IL2CPP_MAKE_UNIQUE(MonitorRuntimeEntry);`
// which expands to the function pointer `std::make_unique<MonitorRuntimeEntry>`.
// Override to a proper call expression.
#pragma push_macro("CHAOS_IL2CPP_MAKE_UNIQUE")
#undef CHAOS_IL2CPP_MAKE_UNIQUE
#define CHAOS_IL2CPP_MAKE_UNIQUE(T) std::make_unique<T>()

// ── chaos_managed_pointer_local_slot_tag ──────────────────────────────────
// The CodeGen pipeline originally emits a constexpr alias in each TU, but
// the hotupdate post-processing comments it out (expecting native_hotupdate_config.h
// or the uncomment step to restore it). For benchmarks we define it here.
constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag =
    ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;

// ── chaos_string_id_tag ────────────────────────────────────────────────────
constexpr CHAOS_IL2CPP_INTPTR chaos_string_id_tag =
    static_cast<CHAOS_IL2CPP_INTPTR>(1)
    << (sizeof(CHAOS_IL2CPP_INTPTR) * 8 - 1);

// ── chaos_is_string_id (normally in codegen_bridge.h inside extern "C") ────
inline bool chaos_is_string_id(CHAOS_IL2CPP_INTPTR v) noexcept
{
    return (v & chaos_string_id_tag) != 0;
}

inline uint64_t chaos_extract_string_id(CHAOS_IL2CPP_INTPTR v) noexcept
{
    return static_cast<uint64_t>(v & ~chaos_string_id_tag);
}

inline CHAOS_IL2CPP_INTPTR chaos_make_string_id_value(uint64_t id) noexcept
{
    return chaos_string_id_tag | static_cast<CHAOS_IL2CPP_INTPTR>(id);
}

// ── Runtime helper stubs (for benchmark linkage only) ──────────────────────
// These are called by extern "C" chaos_external_runtime_* functions within
// the generated code. Real implementations would interact with the managed
// runtime; benchmark stubs are no-ops that return safe default values.

inline void chaos_reflection_set_exception_metadata(
    CHAOS_IL2CPP_INTPTR /*exception_obj*/,
    CHAOS_IL2CPP_INTPTR /*message*/)
{
    // Benchmark stub: no-op
}

inline CHAOS_IL2CPP_INTPTR chaos_reflection_get_exception_message(
    CHAOS_IL2CPP_INTPTR /*exception_obj*/)
{
    // Benchmark stub: return null
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

inline void chaos_gc_keepalive(CHAOS_IL2CPP_INTPTR /*obj*/)
{
    // Benchmark stub: no-op
}

inline void chaos_object_ctor(CHAOS_IL2CPP_INTPTR /*obj*/)
{
    // Benchmark stub: no-op
}

inline void chaos_monitor_enter(CHAOS_IL2CPP_INTPTR /*obj*/)
{
    // Benchmark stub: no-op
}

inline void chaos_monitor_exit(CHAOS_IL2CPP_INTPTR /*obj*/)
{
    // Benchmark stub: no-op
}

inline void chaos_thread_ctor(CHAOS_IL2CPP_INTPTR /*obj*/)
{
    // Benchmark stub: no-op
}

inline void chaos_thread_start(CHAOS_IL2CPP_INTPTR /*obj*/)
{
    // Benchmark stub: no-op
}

inline void chaos_thread_join()
{
    // Benchmark stub: no-op
}

inline void chaos_async_yield_get_result()
{
    // Benchmark stub: no-op
}

inline void chaos_default_interpolated_string_handler_append_string(
    CHAOS_IL2CPP_INTPTR /*handler*/,
    CHAOS_IL2CPP_INTPTR /*value*/)
{
    // Benchmark stub: no-op
}

inline void chaos_default_interpolated_string_handler_reset(
    CHAOS_IL2CPP_INTPTR /*handler*/)
{
    // Benchmark stub: no-op
}

inline void chaos_initialize_array_from_field_data_int32(
    CHAOS_IL2CPP_INTPTR /*array*/,
    CHAOS_IL2CPP_INTPTR /*field_data*/)
{
    // Benchmark stub: no-op
}

#endif // CHAOS_BENCHMARK_SUPPORT_H_
