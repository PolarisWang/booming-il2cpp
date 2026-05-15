// Stub definitions for symbols normally provided by generated AOT code
// These are needed because chaos_runtime_core.lib (instantiation_engine.obj)
// references them, but GC unit tests don't have generated AOT modules.

#include <cstdint>

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