// JIT stubs for entry-jit build
// Provides minimal implementations of JIT functions needed by runtime_core and jit_precode.
// Real JIT compilation happens through chaos_jit.lib in full builds.

#include <chaos/log.h>
#include <codegen_bridge.h>
#include <jit_registration.h>
#include <jit_method.h>
#include <jit_engine.h>
#include <cstdint>

namespace chaos::il2cpp::jit {

// RegisterJitSehHandler — called by RuntimeInit.
// In this build, the T4 SEH handler is already registered by codegen/t4_seh_handler.cpp.
void RegisterJitSehHandler() noexcept {
    // No-op: VEH handler is registered by t4_seh_handler from chaos_codegen.lib
}

// FindT4CodeByAddress — called by GC thread state scanning.
// No T4 code is active in this build, so always return nullptr.
const JitMethod* FindT4CodeByAddress(const void*) noexcept {
    return nullptr;
}

// Compile — called by JitStubDispatchImpl when a JIT trampoline is hit.
// This build uses the interpreter/AOT path, so Compile should never be called.
// If it is called, log and fail rather than crash.
JitMethod* Compile(
    const interpreter::RegisterMethod&,
    const CompileConfig&) noexcept
{
    CHAOS_IL2CPP_LOG_ERROR_M("jit", "Compile() called but JIT compilation is not available in entry-jit build");
    return nullptr;
}

// JitMethod destructor — needed by JitRecompileToTier1 path (which shouldn't be reached)
JitMethod::~JitMethod() noexcept {
    // No-op dtor for stub
}

}  // namespace chaos::il2cpp::jit
