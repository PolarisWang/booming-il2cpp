// ── ReapplyInlining ──────────────────────────────────────────────────────────
// Called from ApplyPatchFromMemory after all methods are pre-lowered and the
// inlining map is fully populated.  Runs InlineLeafCallees on each method
// again to catch inlining opportunities that were missed during the initial
// pre-lowering pass (when callee IR was not yet available).
// After re-inlining, re-runs register allocation (AllocateRegisters) and
// caches the result in cached_reg_method.

namespace chaos::il2cpp::runtime_core {

// Forward declaration (defined in inlining.cpp)
static void InlineLeafCallees(interpreter::IRMethod& ir, PatchMethod& patch_method) noexcept;

void ReapplyInlining(PatchMethod* methods, uint32_t method_count) noexcept {
    if (methods == nullptr || method_count == 0)
        return;

    for (uint32_t i = 0; i < method_count; ++i) {
        auto& pm = methods[i];
        if (pm.cached_ir == nullptr)
            continue;

        auto* ir = static_cast<interpreter::IRMethod*>(pm.cached_ir);

        // Quick check: if no Call instructions exist, skip entirely.
        bool has_call = false;
        for (const auto& ci : ir->instructions) {
            if (ci.op_code == interpreter::IROpCode::Call || ci.op_code == interpreter::IROpCode::CallVirt ||
                ci.op_code == interpreter::IROpCode::CallBridge) {
                has_call = true;
                break;
            }
        }
        if (!has_call)
            continue;

        // Re-create call_cache for the (potentially already-inlined) instructions.
        uint32_t instr_count = static_cast<uint32_t>(ir->instructions.size());
        if (instr_count > 0) {
            auto* old_cc = static_cast<runtime_instantiation::CachedCallInfo*>(pm.call_cache);
            auto* new_cc = static_cast<runtime_instantiation::CachedCallInfo*>(
                CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(instr_count * sizeof(runtime_instantiation::CachedCallInfo)));
            if (new_cc == nullptr)
                continue;
            for (uint32_t j = 0; j < instr_count; ++j) {
                const auto& instr = ir->instructions[j];
                if (instr.op_code == interpreter::IROpCode::Call || instr.op_code == interpreter::IROpCode::CallVirt ||
                    instr.op_code == interpreter::IROpCode::CallBridge ||
                    instr.op_code == interpreter::IROpCode::CallVirtConstrained) {
                    if (instr.call_target != nullptr) {
                        new_cc[j] = runtime_instantiation::PrecacheCallTarget(instr.call_target);
                        // Phase 2.3 fallback: when PrecacheCallTarget misses but
                        // direct_fn is available from JSON deserialization, use it.
                        if (new_cc[j].ret_tag == 0xFF && instr.direct_fn != nullptr && instr.direct_ret_tag != 0xFF) {
                            new_cc[j].ret_tag = instr.direct_ret_tag;
                            new_cc[j].direct_ptr = instr.direct_fn;
                            new_cc[j].is_struct_ret = false;
                            new_cc[j].struct_size = 0;
                        }
                    } else if (instr.direct_fn != nullptr && instr.direct_ret_tag != 0xFF) {
                        new_cc[j].ret_tag = instr.direct_ret_tag;
                        new_cc[j].direct_ptr = instr.direct_fn;
                        new_cc[j].is_struct_ret = false;
                        new_cc[j].struct_size = 0;
                    } else {
                        new_cc[j].ret_tag = 0xFF;
                    }
                } else {
                    new_cc[j].ret_tag = 0xFF;
                }
            }
            // Free the old call_cache before replacing — both old and new
            // are domain-tagged allocations from the same domain scope.
            if (old_cc != nullptr) {
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(old_cc);
            }
            pm.call_cache = new_cc;
        }

        // Run inlining pass again (with new call_cache + full inlining map).
        InlineLeafCallees(*ir, pm);

        // Re-run register allocation after inlining changes IR.
        auto* old_reg = static_cast<interpreter::RegisterMethod*>(pm.cached_reg_method);
        delete old_reg; // RegisterMethod is heap-allocated via new
        auto* reg_method = new interpreter::RegisterMethod(interpreter::AllocateRegisters(*ir));
        pm.cached_reg_method = reg_method;
    }
}

} // namespace chaos::il2cpp::runtime_core
