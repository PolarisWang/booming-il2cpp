// Stub definitions for symbols normally provided by generated AOT code
// These are needed because chaos_runtime_core.lib (instantiation_engine.obj)
// references them, but GC unit tests don't have generated AOT modules.

#include <cstdint>
// For ChaosExceptionTypeEntryV0 — the element type of kChaosExceptionTypes.
// Must be the real definition, not a forward declaration: the stub defines an
// array of these, so the type has to be complete.
#include <chaos/type_info.h>

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

// Exception type table — normally provided by codegen output.
//
// exception_helpers.cpp (ResolveTypeByName) references these unconditionally
// since cb129e362 added the compile-time exception-type lookup, but the table is
// only ever emitted into the per-project generated ChaosGeneratedModule.cpp
// (NativeAotLoweringPlanner.ModuleRegistration.Dispatch.cs, BuildExceptionTypeTable).
// A standalone test target links chaos_runtime_core without any generated module,
// so it must supply them — same situation as kChaosExternalRuntime* above.
//
// The emitter's own comment states the intent: "Always emit both symbols so the
// runtime can reference them unconditionally without a link error."  That holds
// for a generated module; this file carries the same contract for test binaries.
//
// Count 0 is the safe value: the lookup loop in ResolveTypeByName is
// `for (i = 0; i < kChaosExceptionTypeCount; ++i)`, so it iterates zero times and
// falls through to the existing reflection-image path, which is exactly the
// pre-cb129e362 behaviour.
//
// Both symbols are const-qualified in the referencing TU
// (exception_helpers.cpp: `extern const ChaosExceptionTypeEntryV0 ...[]` /
// `extern const int32_t ...Count`).  Const-ness is part of the symbol's type:
// dropping it here would declare a DIFFERENT symbol, leave the original
// unresolved, and keep the LNK2001.  The array is sized 1 and never dereferenced
// because the count is 0.
//
// The explicit `extern` keyword is LOAD-BEARING, exactly as the comment in
// exception_helpers.cpp says of the referencing side.  `extern "C"` sets
// language linkage (i.e. name mangling) but does NOT change storage-class
// linkage: a namespace-scope `const` still defaults to INTERNAL linkage.  Without
// `extern` these two definitions are compiled as internal symbols, never reach
// the object's symbol table, and the LNK2001 persists with NO compiler
// diagnostic to explain it.  Verified with a 5-line repro: the same block with
// and without `extern` yields zero vs two External symbols.
extern const ChaosExceptionTypeEntryV0 kChaosExceptionTypes[1] = { { nullptr, nullptr } };
extern const int32_t kChaosExceptionTypeCount = 0;

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

// RegisterJitSehHandler is provided by chaos_jit.lib (WinSehHandler.cpp)
// which is linked by all test targets via CHAOS_GC_COMMON_LIBS.  No stub needed.
