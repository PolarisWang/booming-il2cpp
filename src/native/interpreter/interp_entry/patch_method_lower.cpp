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
        // ── Exclusive: perform IR lowering ──
        // Deserialize AotCoreIr JSON → IRMethod.
    const char* json = patch_method->aot_core_ir_json;
    if (json == nullptr || json[0] == '\0') {
        // No JSON — create an empty IR with Ret.
        auto* ir = new interpreter::IRMethod();
        ir->instructions.push_back({});
        patch_method->cached_ir = ir;
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
                } else if (instr.direct_fn != nullptr && instr.direct_ret_tag != 0xFF) {
                    // direct_fn with pre-computed return tag — fill CachedCallInfo
                    // so Handle_Call can read the correct ValueTag without runtime
                    // reflection or string parsing.
                    call_cache[i].ret_tag = instr.direct_ret_tag;
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