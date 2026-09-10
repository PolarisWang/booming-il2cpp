// eager_compile.cpp — Eager JIT compilation of reg_ir methods at patch load time
//
// Called from ApplyPatchFromMemory (Phase 1.2) for each patched method that
// has pre-allocated register IR (v2+ .patchdata) but no AOT entry (i.e. the
// method's IL has changed — keep_native = false).
//
// By JIT-compiling eagerly at load time, the method runs at native speed on
// first call, skipping both the interpreter and the lazy tier-up path.
//
// Library layering: this file lives in chaos_interpreter, which links against
// chaos_jit.  The declaration in interpreter_entry.h is the thin bridge that
// lets chaos_runtime_core (patch_loader.cpp) call us.

#include <jit_engine.h>       // Compile, CompileConfig
#include <jit_method.h>       // JitMethod
#include <chaos/runtime/execution_config.h>
#include "interpreter_entry.h" // PatchMethod forward declaration

// Full type definitions needed when JIT is enabled (eager_compile body is active).
// PatchMethod (tier_state, dispatch_entry, cached_native_method) and
// HotpatchEntryV0 are defined in these runtime-core headers.
#include "patch_loader.h"
#include "codegen_bridge.h"

namespace chaos::il2cpp::runtime_core {

void EagerCompilePatchMethods(PatchMethod* methods, uint32_t method_count) noexcept {
#if CHAOS_IL2CPP_ENABLE_JIT
    if (methods == nullptr || method_count == 0) return;

    for (uint32_t i = 0; i < method_count; ++i) {
        auto& pm = methods[i];

        // Skip methods that already have a native code path.
        if (pm.cached_native_method != nullptr) continue;
        if (pm.tier_state.load(std::memory_order_acquire) >= PatchMethod::kJitted) continue;

        // Need pre-allocated register IR — the JSON path (v1) is too slow
        // to deserialize + register-allocate for eager compilation.
        // (The lazy JIT tier-up handles v1 methods on first call.)
        if (pm.reg_ir_data == nullptr) continue;
        if (pm.cached_reg_method == nullptr) continue;

        auto* rm = static_cast<interpreter::RegisterMethod*>(pm.cached_reg_method);
        if (rm == nullptr || rm->instructions.empty()) continue;

        // Check whether the JIT can compile this method.
        if (!chaos::il2cpp::jit::CanCompile(*rm)) continue;

        // Compile with deopt disabled — eagerly compiled code follows the
        // AOT dispatch path (Step A0) which does not support deoptimization.
        chaos::il2cpp::jit::CompileConfig cfg;
        cfg.enable_deopt = false;
        cfg.enable_liveness = true;      // precise GC slot maps
        cfg.enable_inlining = false;     // already inlined in Phase 1.2
        cfg.method_token = pm.token;
        cfg.method_module_id = pm.module_id;

        auto* nm = chaos::il2cpp::jit::Compile(*rm, cfg);
        if (nm == nullptr) continue;

        // Cache on PatchMethod and sync to dispatch entry so both
        // Step A0 and Step A paths can use the generated code.
        pm.cached_native_method = nm;
        if (auto* entry = static_cast<HotpatchEntryV0*>(pm.dispatch_entry); entry != nullptr) {
            entry->direct_ptr = nm->code;
        }
        pm.tier_state.store(PatchMethod::kJitted, std::memory_order_release);
    }
#endif
}

}  // namespace chaos::il2cpp::runtime_core
