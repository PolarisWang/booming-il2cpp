/// bootstrap_test_stubs.cpp — Shared extern stubs for bootstrap test linkage.
///
/// The AOT codegen library (chaos_codegen) and interpreter (chaos_interpreter)
/// reference these symbols.  Tests that link against those libraries must
/// provide definitions to satisfy the linker.

#include <cstdint>

extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// ── Exception type table ──────────────────────────────────────────────────
//
// exception_helpers.cpp (ResolveTypeByName) references these UNCONDITIONALLY
// since cb129e362 added the compile-time exception-type lookup.  The table is
// only ever emitted into a generated module
// (NativeAotLoweringPlanner.ModuleRegistration.Dispatch.cs, BuildExceptionTypeTable),
// so any target linking chaos_runtime_core without generated code must supply it.
//
// The bootstrap tests are in exactly that position and cannot rely on the
// factory's default stub: their CMakeLists passes WITHOUT_CODEGEN_STUB, which
// suppresses tests/contracts/native/runtime-core/gc_test_stubs.cpp — the file
// that otherwise provides these two symbols.  Hence they must be defined here
// (or inline in the TU, as bootstrap_stress.cpp does).
//
// Count 0 is the safe value: ResolveTypeByName loops `for (i = 0; i <
// kChaosExceptionTypeCount; ++i)`, so it iterates zero times and falls through
// to the reflection-image path — the pre-cb129e362 behaviour.  The array is
// sized 1 and never dereferenced.
//
// The explicit `extern` keyword is LOAD-BEARING.  `extern "C"` sets language
// linkage (name mangling) but does NOT change storage-class linkage: a
// namespace-scope `const` still defaults to INTERNAL linkage.  Without `extern`
// these are compiled as internal symbols, never reach the object's symbol
// table, and the LNK2019 persists with NO compiler diagnostic to explain it.
// Same contract as the referencing TU (exception_helpers.cpp declares them
// `extern const ...`).
#include <chaos/type_info.h>

extern "C" {

extern const chaos::il2cpp::common::ChaosExceptionTypeEntryV0
    kChaosExceptionTypes[1] = { { nullptr, nullptr } };
extern const int32_t kChaosExceptionTypeCount = 0;

}  // extern "C"

