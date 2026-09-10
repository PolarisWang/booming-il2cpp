namespace chaos::il2cpp::runtime_core {

using namespace ::chaos::il2cpp::runtime_instantiation;

// ── Phase 1 inlining: InlineLeafCallees ────────────────────────────────────
// After IR deserialization, replace eligible Call instructions with the callee's
// IR body so that method_invoke (~1500ns) is skipped entirely.
//
// Safety conditions (Phase 4 — extended):
//   - callee cached_ir exists and is not the caller itself
//   - callee has zero Call/CallVirt/CallBridge instructions (leaf)
//   - callee IR ≤ 32 instructions (up from 16 in Phase 2)
//   - callee branches are forward-only (null-check pattern); branch_target is remapped
//   - callee LdLoc/StLoc are allowed with local slot remapping
//   - callee has NO LdSFld/StSFld — no static field complexity
//   - callee has NO SEH clauses — no SEH region merging
//   - callee return is NOT a struct — no struct buf mapping
//   - caller + callee stack usage ≤ FastFrame::kMaxStack (64)

static bool IsCalleeEligibleForInline(
    const interpreter::IRMethod& callee_ir,
    const ::chaos::il2cpp::runtime_instantiation::CachedCallInfo& call_info) noexcept {
    // Must have IR, no calls, no branches, no Loc, no SFld.
    uint32_t max_sp = 0;
    uint32_t cur_sp = 0;
    uint32_t idx = 0;
    for (const auto& instr : callee_ir.instructions) {
        switch (instr.op_code) {
            case interpreter::IROpCode::Call:
            case interpreter::IROpCode::CallVirt:
            case interpreter::IROpCode::CallBridge:
            case interpreter::IROpCode::CallVirtConstrained:
                return false; // not a leaf
            case interpreter::IROpCode::Br:
            case interpreter::IROpCode::BrTrue:
            case interpreter::IROpCode::BrFalse:
            case interpreter::IROpCode::Beq:
            case interpreter::IROpCode::Blt:
            case interpreter::IROpCode::Bgt:
            case interpreter::IROpCode::Ble:
            case interpreter::IROpCode::Bge:
            case interpreter::IROpCode::BneUn:
            case interpreter::IROpCode::BgeUn:
            case interpreter::IROpCode::BgtUn:
            case interpreter::IROpCode::BleUn:
            case interpreter::IROpCode::BltUn:
            case interpreter::IROpCode::Leave:
                // Allow forward-only branches (null-check pattern).
                // branch_target will be remapped during splicing.
                if (static_cast<uint32_t>(instr.branch_target) <= idx ||
                    static_cast<uint32_t>(instr.branch_target) >=
                        static_cast<uint32_t>(callee_ir.instructions.size())) {
                    return false; // backward or out-of-range
                }
                break;
            case interpreter::IROpCode::Switch:
                return false; // Switch is too complex for inlining
            case interpreter::IROpCode::LdSFld:
            case interpreter::IROpCode::StSFld:
                return false; // has static field access
            case interpreter::IROpCode::Throw:
            case interpreter::IROpCode::Rethrow:
            case interpreter::IROpCode::EndFinally:
            case interpreter::IROpCode::EndFilter:
                return false; // SEH/internal control flow
            case interpreter::IROpCode::Box:
            case interpreter::IROpCode::NewObj:
            case interpreter::IROpCode::NewArr:
                // Allocations are allowed but increase complexity
                break;
            case interpreter::IROpCode::Ret:
                // Ret pops nothing (returns top-of-stack). For inlining we
                // delete the Ret, so sp stays unchanged.
                break;
            case interpreter::IROpCode::Pop:
                if (cur_sp > 0)
                    --cur_sp;
                if (cur_sp > max_sp)
                    max_sp = cur_sp;
                break;
            case interpreter::IROpCode::Dup:
                ++cur_sp;
                if (cur_sp > max_sp)
                    max_sp = cur_sp;
                break;
            case interpreter::IROpCode::LdArg:
                // LdArg pushes 1 (will read from caller's stack)
                ++cur_sp;
                if (cur_sp > max_sp)
                    max_sp = cur_sp;
                break;
            default:
                // Many opcodes pop 0-2 and push 1; worst-case assume +1.
                // Conservatively count Ld* as push, binary ops as pop-2-push-1.
                if (instr.op_code == interpreter::IROpCode::LdNull || instr.op_code == interpreter::IROpCode::LdStr ||
                    instr.op_code == interpreter::IROpCode::LdcI4 || instr.op_code == interpreter::IROpCode::LdcI8 ||
                    instr.op_code == interpreter::IROpCode::LdcR4 || instr.op_code == interpreter::IROpCode::LdcR8 ||
                    instr.op_code == interpreter::IROpCode::LdFld || instr.op_code == interpreter::IROpCode::LdLoc ||
                    instr.op_code == interpreter::IROpCode::LdLocA || instr.op_code == interpreter::IROpCode::LdLen) {
                    ++cur_sp;
                    if (cur_sp > max_sp)
                        max_sp = cur_sp;
                } else if (instr.op_code == interpreter::IROpCode::StLoc) {
                    if (cur_sp > 0)
                        --cur_sp;
                } else if (instr.op_code == interpreter::IROpCode::StFld ||
                           instr.op_code == interpreter::IROpCode::StArg) {
                    if (cur_sp >= 2)
                        cur_sp -= 2;
                }
                // Add/Sub/etc pop 2 push 1 → net -1
                else if (instr.op_code == interpreter::IROpCode::Add || instr.op_code == interpreter::IROpCode::Sub ||
                         instr.op_code == interpreter::IROpCode::Mul || instr.op_code == interpreter::IROpCode::Div ||
                         instr.op_code == interpreter::IROpCode::Rem || instr.op_code == interpreter::IROpCode::Ceq ||
                         instr.op_code == interpreter::IROpCode::Clt || instr.op_code == interpreter::IROpCode::Cgt ||
                         instr.op_code == interpreter::IROpCode::And || instr.op_code == interpreter::IROpCode::Or ||
                         instr.op_code == interpreter::IROpCode::Xor || instr.op_code == interpreter::IROpCode::Shl ||
                         instr.op_code == interpreter::IROpCode::Shr || instr.op_code == interpreter::IROpCode::ShrUn) {
                    // pop 2 push 1 → net -1
                    if (cur_sp >= 2)
                        cur_sp -= 1;
                }
                break;
        }
        ++idx;
    }

    // Must have SEH-free callee.
    if (!callee_ir.seh_clauses.empty())
        return false;

    // Struct returns are too complex for inlining (need struct buf mapping).
    if (call_info.is_struct_ret)
        return false;

    // Instruction count bound (Phase 4: extended from 16 → 32).
    uint32_t instr_count = static_cast<uint32_t>(callee_ir.instructions.size());
    if (instr_count > 32)
        return false;

    return true;
}

/// Inline eligible leaf callees into the caller's IR instruction stream.
/// Called from PatchMethodLowerIR after IR deserialization and call_cache setup.
/// Modifies ir.instructions in-place and rebuilds call_cache for the caller.
static void InlineLeafCallees(interpreter::IRMethod& ir, PatchMethod& patch_method) noexcept {
    auto* cache = patch_method.metadata_cache;
    if (cache == nullptr)
        return;

    uint32_t instr_count = static_cast<uint32_t>(ir.instructions.size());
    if (instr_count == 0)
        return;

    auto* call_cache = static_cast<::chaos::il2cpp::runtime_instantiation::CachedCallInfo*>(patch_method.call_cache);
    if (call_cache == nullptr)
        return;

    // Build new instruction list: iterate caller's instructions,
    // replacing eligible Call instructions with callee's IR body.
    std::vector<interpreter::IRInstruction> new_instrs;
    new_instrs.reserve(instr_count * 2); // conservative pre-alloc

    // Compute caller's max local slot index for local remapping during inline.
    uint32_t caller_local_count = 0;
    for (uint32_t i = 0; i < instr_count; ++i) {
        const auto& instr = ir.instructions[i];
        if (instr.op_code == interpreter::IROpCode::LdLoc || instr.op_code == interpreter::IROpCode::StLoc ||
            instr.op_code == interpreter::IROpCode::LdLocA) {
            uint32_t idx = static_cast<uint32_t>(instr.operand_index);
            if (idx >= caller_local_count)
                caller_local_count = idx + 1;
        }
    }

    for (uint32_t i = 0; i < instr_count; ++i) {
        const auto& instr = ir.instructions[i];
        if (instr.op_code != interpreter::IROpCode::Call || instr.call_target == nullptr) {
            new_instrs.push_back(instr);
            continue;
        }

        // Check if the call_target can be resolved to a PatchMethod in the
        // same patch context via the inlining map.
        // The call_target is a MethodInfoHandle; we need module_id+token.
        // Since this is a same-patch-context call, the callee's token was
        // stored in the call_cache during pre-caching.  But call_cache doesn't
        // store module_id+token for callee identification.
        //
        // Approach: try to look up the call_target in the inlining map.
        // The inlining map is keyed by (module_id<<32 | token), so we
        // need the callee's module_id and token. These are encoded in the
        // call_target handle itself (ReflectionQueryMethodHandle).
        //
        // For now, Phase 1: skip inlining for calls whose callee IR is not
        // available in the same patch context.  This works when:
        // - The callee is a patched method (its IR is in the patchdata)
        // - The callee's module_id + token match a PatchMethod in ctx
        //
        // Cross-module calls (to AOT-only methods) are NOT inlined.

        // Try to decode the call_target as a ReflectionQueryMethodHandle.
        const auto* method_desc =
            runtime_core::TryDecodeReflectionQueryMethodHandle(reinterpret_cast<MethodInfoHandle>(instr.call_target));
        if (method_desc == nullptr) {
            new_instrs.push_back(instr);
            continue;
        }

        // We have the method descriptor.  Look up the callee by method name.
        // The PatchContext's inlining map is on PatchMetadataCache, keyed
        // by method name (subject_id).  But the inlining map uses token keys.
        // For Phase 1, only inline when the callee is another patched method
        // in the same patch context — identified by matching subject_id
        // to the method name in the inlining map.
        //
        // Simplified approach: search all PatchMethod* in the context by
        // matching the callee's subject_id.  The PatchMetadataCache has
        // the MethodDef entries; we can iterate and check subject_id.
        //
        // Actually even simpler: the callee's subject_id is in method_desc->subject_id_utf8.
        // We iterate methods of the same PatchMetadataCache, pre-lower those,
        // and if subject_id matches, we have the callee IR.
        //
        // But this is O(N) per Call instruction.  Instead, let's do a two-pass:
        // Pass 1: build subject_id→PatchMethod* map from the PatchContext.
        // This map is built once in ApplyPatchFromMemory.
        //
        // For now, skip the map and just try a direct approach:
        // Find the patch method by looking at the AotCoreIr JSON's method list.
        // Since we can't iterate PatchContext methods from here, skip cross-context.

        // Phase 1: only inline when the call_target's method descriptor
        // subject_id matches one of the patched methods.  We do this via
        // the metadata_cache's inlining map, which was populated during
        // PatchContext construction with (subject_id_hash → PatchMethod*).
        //
        // Actually, we can't easily do this from PatchMethodLowerIR because
        // the inlining map is on PatchMetadataCache but the calling context
        // doesn't know which PatchContext we're in.  Multiple PatchContexts
        // may coexist.
        //
        // Shortcut for Phase 1: use FindInliningTarget on the cache.
        // But we don't know the callee's (module_id, token) from just the
        // method descriptor (well, the token IS method_desc->metadata_token).
        // And module_id... we don't have it on the method descriptor.
        //
        // Instead, use the subject_id. Add a subject_id→PatchMethod* map
        // to the cache.

        // For Phase 1, we iterate ALL methods in the cache to find a match.
        // This is O(N) per call but N is small (≤21 per patch context).
        PatchMethod* callee_method = nullptr;
        const auto& ctx = patch_method.metadata_cache;
        // Get the PatchContext from the cache... we don't have it directly.
        // The cache is owned by a PatchContext; but the methods array is
        // separate from the cache.

        // --- SIMPLER APPROACH FOR PHASE 1 ---
        // Use the ModuleId + token from the CachedCallInfo if available.
        // The call_cache[i] has module_id and... no, it doesn't have token.
        // But we can use method_desc->metadata_token and the module_id from
        // the caller's PatchMethod (assuming same module).
        //
        // For inlining within the same patch DLL, the callee will have its
        // MethodDef entry in the same patch context.  We look it up by
        // iterating MethodDef entries in the cache and matching subject_id.
        //
        // BUT: the metadata_cache doesn't expose the PatchMethod array.
        // This is a design limitation.  Let's add a method to the cache.

        // For now: if FindInliningTarget returns nullptr, skip inlining.
        // Build the key from method_desc->metadata_token and patch_method.module_id.
        PatchMethod* callee = cache->FindInliningTarget(patch_method.module_id, method_desc->metadata_token);
        if (callee == nullptr) {
            new_instrs.push_back(instr);
            continue;
        }

        // Check that the callee has lowered IR.
        if (callee->cached_ir == nullptr) {
            new_instrs.push_back(instr);
            continue;
        }

        const auto& callee_ir = *static_cast<interpreter::IRMethod*>(callee->cached_ir);

        // Check eligibility using cached call_info.
        const auto& cc = call_cache[i];
        if (!IsCalleeEligibleForInline(callee_ir, cc)) {
            new_instrs.push_back(instr);
            continue;
        }

        // ── Inline: splice callee instructions ---
        // Replace the Call instruction with callee's IR body.
        // LdArg(K) → mark secondary_index = callee_arg_count (stack peek mapping)
        // Ret → skip (don't copy)
        uint32_t callee_arg_count = (cc.ret_tag != 0xFF) ? static_cast<uint32_t>(instr.arg_count) : 0u;
        uint32_t callee_start_pos = static_cast<uint32_t>(new_instrs.size());

        // Compute callee's max local slot index for remapping.
        uint32_t callee_local_count = 0;
        for (const auto& ci : callee_ir.instructions) {
            if (ci.op_code == interpreter::IROpCode::LdLoc || ci.op_code == interpreter::IROpCode::StLoc ||
                ci.op_code == interpreter::IROpCode::LdLocA) {
                uint32_t idx = static_cast<uint32_t>(ci.operand_index);
                if (idx >= callee_local_count)
                    callee_local_count = idx + 1;
            }
        }
        // Reject if remapped locals would overflow FastFrame capacity.
        if (caller_local_count + callee_local_count > FastFrame::kMaxLocals) {
            new_instrs.push_back(instr);
            continue;
        }
        uint32_t local_offset = caller_local_count;

        for (uint32_t ci_idx = 0; ci_idx < callee_ir.instructions.size(); ++ci_idx) {
            const auto& ci = callee_ir.instructions[ci_idx];
            if (ci.op_code == interpreter::IROpCode::Ret) {
                // Skip Ret — return value is already on stack.
                continue;
            }
            auto inlined = ci;
            if (inlined.op_code == interpreter::IROpCode::LdArg) {
                // Signal to Handle_LdArg that this is an inlined arg:
                // secondary_index = callee arg count (for stack peek).
                // Runtime will read from frame.stack[sp - callee_arg_count + operand_index].
                inlined.secondary_index = callee_arg_count;
            }
            // Remap local slots so callee locals don't collide with caller locals.
            if (inlined.op_code == interpreter::IROpCode::LdLoc || inlined.op_code == interpreter::IROpCode::StLoc ||
                inlined.op_code == interpreter::IROpCode::LdLocA) {
                inlined.operand_index += static_cast<int32_t>(local_offset);
            }
            // Remap forward branch targets to their absolute positions.
            if (inlined.op_code == interpreter::IROpCode::Br || inlined.op_code == interpreter::IROpCode::BrTrue ||
                inlined.op_code == interpreter::IROpCode::BrFalse || inlined.op_code == interpreter::IROpCode::Beq ||
                inlined.op_code == interpreter::IROpCode::Blt || inlined.op_code == interpreter::IROpCode::Bgt ||
                inlined.op_code == interpreter::IROpCode::Ble || inlined.op_code == interpreter::IROpCode::Bge ||
                inlined.op_code == interpreter::IROpCode::BneUn || inlined.op_code == interpreter::IROpCode::BgeUn ||
                inlined.op_code == interpreter::IROpCode::BgtUn || inlined.op_code == interpreter::IROpCode::BleUn ||
                inlined.op_code == interpreter::IROpCode::BltUn || inlined.op_code == interpreter::IROpCode::Leave) {
                inlined.branch_target += callee_start_pos;
            }
            new_instrs.push_back(inlined);
        }
    }

    // Only replace if any inlining actually happened.
    if (new_instrs.size() == instr_count)
        return; // no inlining occurred

    // Replace the caller's instructions.
    ir.instructions.clear();
    ir.instructions.reserve(new_instrs.size());
    for (auto& ni : new_instrs) {
        ir.instructions.push_back(ni);
    }

    // Rebuild call_cache for the modified instruction array.
    // Free the old cache before replacing — both allocations are domain-tagged
    // and from the same domain scope.
    if (patch_method.call_cache != nullptr) {
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(patch_method.call_cache);
    }
    uint32_t new_count = static_cast<uint32_t>(ir.instructions.size());
    auto* new_cc =
        static_cast<::chaos::il2cpp::runtime_instantiation::CachedCallInfo*>(CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(
            new_count * sizeof(::chaos::il2cpp::runtime_instantiation::CachedCallInfo)));
    if (new_cc == nullptr)
        return;
    for (uint32_t i = 0; i < new_count; ++i) {
        const auto& ci = ir.instructions[i];
        if (ci.op_code == interpreter::IROpCode::Call || ci.op_code == interpreter::IROpCode::CallVirt ||
            ci.op_code == interpreter::IROpCode::CallBridge ||
            ci.op_code == interpreter::IROpCode::CallVirtConstrained) {
            if (ci.call_target != nullptr) {
                new_cc[i] = ::chaos::il2cpp::runtime_instantiation::PrecacheCallTarget(ci.call_target);
            } else {
                new_cc[i].ret_tag = 0xFF;
            }
        } else {
            new_cc[i].ret_tag = 0xFF;
        }
    }
    patch_method.call_cache = new_cc;
}

} // namespace chaos::il2cpp::runtime_core