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
