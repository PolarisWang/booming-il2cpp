namespace chaos::il2cpp::runtime_core {

// ── PatchMethod lazy IR deserialization ─────────────────────────────────
// Uses per-PatchMethod CAS state machine (ir_state: 0=uninit, 1=lowering, 2=done)
// to avoid global mutex contention across threads.

// Forward declarations (defined in other sub-files)
static void* ResolveSubjectId(const char* subject_id, void* user_data) noexcept;
static void* ResolveDirectFn(const char* subject_id, void* user_data) noexcept;
static void CacheSignature(PatchMethod* patch_method) noexcept;
static void InlineLeafCallees(interpreter::IRMethod& ir, PatchMethod& patch_method) noexcept;

void PatchMethodLowerIR(uintptr_t method_key) noexcept {
    if (method_key == 0) return;

    auto* patch_method = reinterpret_cast<PatchMethod*>(method_key);

    auto& state = patch_method->ir_state;

    // Fast path: already lowered.
    if (state.load(std::memory_order_acquire) == 2) return;

    // Try to claim the lowering slot (0 → 1 via CAS).
    uint32_t expected = 0;
    if (state.compare_exchange_strong(expected, 1, std::memory_order_acq_rel)) {
        // ── Phase 0: v2 pre-allocated register IR path ────────────────
        // When the .patchdata carries pre-serialized RegisterInstruction[] data,
        // skip JSON deserialization and register allocation entirely.
        if (patch_method->reg_ir_data != nullptr && patch_method->reg_ir_instr_count > 0) {
            auto* reg_method = new interpreter::RegisterMethod();
            auto* raw_instrs = static_cast<const interpreter::RegisterInstruction*>(
                patch_method->reg_ir_data);
            reg_method->instructions.assign(raw_instrs,
                raw_instrs + patch_method->reg_ir_instr_count);
            reg_method->max_regs = patch_method->reg_ir_max_regs;

            // Copy SEH clauses if present (data follows instructions).
            if (patch_method->reg_ir_seh_count > 0) {
                const auto* seh_data = reinterpret_cast<const interpreter::SEHClause*>(
                    raw_instrs + patch_method->reg_ir_instr_count);
                reg_method->seh_clauses.assign(seh_data,
                    seh_data + patch_method->reg_ir_seh_count);
            }

            patch_method->cached_reg_method = reg_method;

            // Pre-cache signature for fast path.
            if (!patch_method->cached_sig_valid) {
                CacheSignature(patch_method);
            }

            // No call_cache needed (call-site metadata not available in binary IR).
            // MIC will populate CachedCallInfo on first invocation.

            // Create a minimal IRMethod for compatibility (entry_direct.cpp expects it).
            auto* ir = new interpreter::IRMethod();
            ir->instructions.push_back({});  // placeholder Ret
            patch_method->cached_ir = ir;

            state.store(2, std::memory_order_release);
            return;
        }

        // ── Phase 1: Deserialize AotCoreIr JSON → IRMethod (v1 path) ──
        const char* json = patch_method->aot_core_ir_json;
    if (json == nullptr || json[0] == '\0') {
        // No JSON — create an empty IR with Ret.
        auto* ir = new interpreter::IRMethod();
        ir->instructions.push_back({});
        patch_method->cached_ir = ir;
        state.store(2, std::memory_order_release);
        return;
    }

    size_t json_length = std::strlen(json);
    auto* ir = new interpreter::IRMethod(
        DeserializeAotCoreIrMethod(
            json,
            json_length,
            ResolveSubjectId,
            patch_method->metadata_cache,
            ResolveDirectFn,
            patch_method->metadata_cache));

    patch_method->cached_ir = ir;

    // Pre-cache signature so the fast path can use it immediately.
    if (!patch_method->cached_sig_valid) {
        CacheSignature(patch_method);
    }

    // Pre-cache call-site metadata for every Call instruction.
    // This eliminates TryDecodeReflectionQueryMethodHandle + ResolveParameterType
    // + IsValueTypeByHandle + LayoutEngine at each call dispatch.
    uint32_t instr_count = static_cast<uint32_t>(ir->instructions.size());
    if (instr_count > 0) {
        auto* call_cache = static_cast<runtime_instantiation::CachedCallInfo*>(
            CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(instr_count * sizeof(runtime_instantiation::CachedCallInfo)));
        for (uint32_t i = 0; i < instr_count; ++i) {
            const auto& instr = ir->instructions[i];
            if (instr.op_code == interpreter::IROpCode::Call ||
                instr.op_code == interpreter::IROpCode::CallVirt ||
                instr.op_code == interpreter::IROpCode::CallBridge ||
                instr.op_code == interpreter::IROpCode::CallVirtConstrained) {
                if (instr.call_target != nullptr) {
                    call_cache[i] = runtime_instantiation::PrecacheCallTarget(
                        instr.call_target);
                    // Phase 2.3: Fallback to direct_fn when PrecacheCallTarget
                    // couldn't resolve direct_ptr but the JSON deserialization
                    // already set direct_fn via the three-tier ResolveDirectFn
                    // callback.  This catches cases where:
                    //   - call_target resolves to a valid MethodInfoHandle
                    //   - BUT PrecacheCallTarget's HotpatchNameRegistry lookup
                    //     misses (subject_id parsing format mismatch, etc.)
                    //   - AND ResolveDirectFn found the entry via AotDirectTable
                    //     or ExternalRuntimeFnTable
                    //
                    // Without this fallback, Handle_Call goes through
                    // method_invoke (~1500-2200ns) for every call instruction
                    // even though the AOT function pointer is available.
                    if (call_cache[i].ret_tag == 0xFF &&
                        instr.direct_fn != nullptr &&
                        instr.direct_ret_tag != 0xFF) {
                        call_cache[i].ret_tag = instr.direct_ret_tag;
                        call_cache[i].direct_ptr = instr.direct_fn;
                        call_cache[i].is_struct_ret = false;
                        call_cache[i].struct_size = 0;
                    }
                } else if (instr.direct_fn != nullptr && instr.direct_ret_tag != 0xFF) {
                    // direct_fn with pre-computed return tag — fill CachedCallInfo
                    // so Handle_Call/InterpreterDispatchRaw can call the AOT thunk
                    // directly (MIC path) with the correct calling convention.
                    // direct_ptr is set to direct_fn so the MIC path bypasses
                    // MethodInvoke for Tier 3 cross-assembly calls.
                    call_cache[i].ret_tag = instr.direct_ret_tag;
                    call_cache[i].direct_ptr = instr.direct_fn;
                    call_cache[i].is_struct_ret = false;
                    call_cache[i].struct_size = 0;
                } else {
                    call_cache[i].ret_tag = 0xFF; // not cached
                }
            } else {
                call_cache[i].ret_tag = 0xFF; // not a call
            }
        }
        patch_method->call_cache = call_cache;
    }

    // ── Phase 1 inlining: inline eligible leaf calls ───────────────────
    // After IR deserialization and call_cache setup, attempt to inline
    // callee IR for any Call instruction that meets safety conditions:
    //   - callee IR available, leaf (no Call instructions), ≤8 instr
    //   - no branches, no LdLoc/StLoc, no LdSFld/StSFld, no SEH
    //   - struct returns are too complex — skip
    //   - recursive inlining NOT attempted (single level only)
    InlineLeafCallees(*ir, *patch_method);

        // ── Register allocation pass ────────────────────────────────────
        // Convert stack-based IRMethod to register-based RegisterMethod.
        // The RegisterMethod is stored in cached_reg_method for use by
        // the RegisterExecute fast path (A1.3).
        auto* reg_method = new interpreter::RegisterMethod(
            interpreter::AllocateRegisters(*ir));
        patch_method->cached_reg_method = reg_method;

        // Mark as done (release so readers see complete state).
        state.store(2, std::memory_order_release);
    } else {
        // Another thread is lowering — spin-wait for completion.
        while (state.load(std::memory_order_acquire) != 2) {}
    }
}

}  // namespace chaos::il2cpp::runtime_core