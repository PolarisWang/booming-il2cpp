// Stub definitions for symbols normally provided by generated AOT code
// These are needed because chaos_runtime_core.lib (instantiation_engine.obj)
// references them, but GC unit tests don't have generated AOT modules.

#include <cstdint>
#include <cstddef>

extern "C" {

// chaos_il2cpp_aot_hotpatch_module — normally provided by codegen output
// GC tests don't use hotpatch, so null is safe.
const void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// External runtime dispatch table — normally provided by codegen output
const char* kChaosExternalRuntimeSubjects[1] = { nullptr };
void*       kChaosExternalRuntimeFnTable[1]  = { nullptr };
int32_t     kChaosExternalRuntimeCount = 0;

// Note: ChaosResolveExternalRuntimeFnTable is provided by
// chaos_interpreter (hotpatch_resolve.cpp), NOT defined here to avoid
// duplicate symbol conflicts when chaos_interpreter is linked.

}  // extern "C"

// ── DeepInlineCallees stub ────────────────────────────────────────────────
// Required by entry_direct.cpp (included in interpreter_entry.cpp unity build).
// The real implementation (in ir_optimizer.cpp) is not compiled in this tree.
namespace chaos::il2cpp::interpreter {
    struct IRMethod;
    struct RegisterMethod;
}
namespace chaos::il2cpp::runtime_core { struct PatchMethod; }
namespace chaos::il2cpp::codegen { struct NativeMethod; struct CodeGenConfig; }

namespace chaos::il2cpp::runtime_core {
bool DeepInlineCallees(
    interpreter::IRMethod& /*ir*/,
    PatchMethod& /*patch_method*/,
    uint32_t /*max_levels*/,
    uint32_t /*max_instructions*/) noexcept
{
    return false;
}
}

// ── Native codegen stubs ──────────────────────────────────────────────────
// Required by interpreter_entry.cpp (tiering: InterpreterEntryDirect calls
// CanGenerateNativeCode/GenerateNativeCode).  These stubs are only needed
// when chaos_codegen.lib is NOT linked.  When chaos_codegen.lib IS linked
// (e.g. in chaos_codegen_native_test), the real implementations take
// precedence through normal library resolution.
//
// The stubs are gated by a weak-linking trick: the duplicate-definition
// guard below prevents linker errors when chaos_codegen.lib provides the
// real symbols.  In MSVC, object-file symbols always win over .lib symbols,
// so these stubs are only active when chaos_codegen.lib is NOT linked.
//#ifndef CHAOS_IL2CPP_CODEGEN_NATIVE_METHOD_H_
// (auto-guard not reliable across translation units)
//
// Instead, we conditionalize on whether the test needs real codegen:
#ifndef CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE
namespace chaos::il2cpp::codegen {
bool CanGenerateNativeCode(
    const interpreter::RegisterMethod& /*rm*/) noexcept
{
    return false;
}

NativeMethod* GenerateNativeCode(
    const interpreter::RegisterMethod& /*rm*/,
    const CodeGenConfig& /*config*/) noexcept
{
    return nullptr;
}
}
#endif

// ── gc_card_table globals ──────────────────────────────────────────────────
// These are now defined in gc_card_table.cpp inside chaos_runtime_core.lib.
// The stubs below are no longer needed — uncomment if card-table globals are
// excluded from the lib in a future build configuration.
// namespace chaos { namespace il2cpp { namespace runtime_core {
// uintptr_t g_nursery_range_begin = 0;
// uintptr_t g_nursery_range_end = 0;
// }}}
// Note: GcYoungCollection is NOT stubbed here — the real implementation from
// chaos_runtime_core.lib (gc_young_collector.cpp) must be resolved for the
// young_collector_test and finalizer_test to work correctly.

// RegisterT4SehHandler is provided by chaos_codegen.lib (t4_seh_handler.cpp)
// which is linked by all test targets via CHAOS_GC_COMMON_LIBS.  No stub needed.

// ── Thread-local variable stubs ──────────────────────────────────────────
// These thread_local variables are referenced by inline functions in
// gc_region.h (NurseryAllocateFast, PohAllocateFast) which are called from
// gc_test_base.cpp and directly-compiled production source files.
// The real definitions live in gc_region.cpp and gc_api.cpp inside
// chaos_runtime_core.lib, but MSVC link.exe cannot search GNU ar format
// archives to resolve TLS relocations.  These stubs provide the definitions
// so that test targets that use NurseryAllocate / gc_test_base.cpp can link.
//
// NOTE: Tests that directly compile gc_region.cpp or gc_api.cpp do NOT
// need these stubs — they get the real definitions from those .cpp files.
#include <chaos/native_types.h>
#include "gc_region.h"
namespace chaos { namespace il2cpp { namespace runtime_core {
thread_local TLAB tls_tlab;
thread_local CHAOS_IL2CPP_SIZE tls_tlab_size = 0;
thread_local CHAOS_IL2CPP_SIZE tls_alloc_since_last_gc = 0;
thread_local CHAOS_IL2CPP_INT64 tls_total_allocated_bytes = 0;
}}}
