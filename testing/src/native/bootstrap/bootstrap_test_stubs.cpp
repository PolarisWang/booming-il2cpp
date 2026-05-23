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
