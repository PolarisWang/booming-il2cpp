// metadata_test_stubs.cpp — Stubs for symbols referenced by chaos_bootstrap/interpreter
// that are normally provided by generated AOT code.

#include <cstdint>
#include <cstddef>

extern "C" {

// chaos_il2cpp_aot_hotpatch_module — normally provided by codegen output.
// The metadata registry test doesn't use hotpatch registration, so null is safe.
const void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// External runtime dispatch table — normally provided by codegen output.
const char* kChaosExternalRuntimeSubjects[1] = { nullptr };
void*       kChaosExternalRuntimeFnTable[1]  = { nullptr };
int32_t     kChaosExternalRuntimeCount = 0;

}  // extern "C"
