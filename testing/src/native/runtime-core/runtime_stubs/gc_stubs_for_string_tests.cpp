// gc_stubs_for_string_tests.cpp — Minimal GC/threading stubs for string_stubs_test
//
// string_stubs.cpp (inside chaos_runtime_core.lib) references GcAllocate,
// GcAllocateAtomic, threading functions, and tls_tlab, but these symbols
// are not provided by the lib itself.  This file supplies minimal stub
// implementations so the linker can resolve them.
//
// IMPORTANT: This file must NOT include "runtime_stubs/stub_common.h" or
// "gc/gc_helpers.h" because those headers declare GcAllocate/GcAllocateAtomic
// (without noexcept), which would conflict with the definitions here.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>

// Include string_table.h for the StringView/Resolve stub below.
// This is safe — string_table.h has no conflicts with our stub declarations.
#include "string_table.h"

// Minimal TLAB definition — matches the layout expected by gc_region.h
// without pulling in the full GC header chain.
struct TLAB { void* start = nullptr; void* end = nullptr; };

namespace chaos { namespace il2cpp { namespace runtime_core {

// Thread-local TLAB — referenced by string_stubs.cpp through
// the GcAllocateAtomic → NurseryAllocateAtomic chain.
thread_local TLAB tls_tlab;

// GcAllocateAtomic: uses std::calloc (zeroed) so the GC contract is met.
// This matches the declaration in gc_helpers.h (non-noexcept).
void* GcAllocateAtomic(std::size_t size) {
    return std::calloc(1, size);
}

// GcAllocate: same approach — zeroed calloc.
void* GcAllocate(std::size_t size) {
    return std::calloc(1, size);
}

// InitYoungGeneration — normally initialises the nursery. No-op for tests.
void InitYoungGeneration() {}

namespace threading {

int32_t AllocateThreadId() { return 1; }
void RegisterThread(int32_t, void*) {}
void UnregisterThread() {}
void EnterCooperativeMode() {}

}  // namespace threading
}}}  // namespace chaos::il2cpp::runtime_core

// Stub for string_table::Resolve — our tests never pass string_id-tagged values,
// so this is never reached in practice. Required because string_stubs.cpp
// calls it from resolve_string_arg().
namespace chaos { namespace il2cpp { namespace string_table {
StringView Resolve(StringId) { return StringView{}; }
}}}  // namespace chaos::il2cpp::string_table

// Extern "C" AOT stubs — required by chaos_bootstrap / chaos_interpreter.
namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" {
void* chaos_il2cpp_aot_hotpatch_module = nullptr;
const void* kChaosExternalRuntimeSubjects = nullptr;
const void* kChaosExternalRuntimeFnTable = nullptr;
int32_t kChaosExternalRuntimeCount = 0;
}
}}}  // namespace chaos::il2cpp::runtime_core
