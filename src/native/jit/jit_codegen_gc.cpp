// jit_codegen_gc.cpp — GC safepoint-poll / precise root-point / type-inference
// / inline dirty-card emission for NativeCodeGenerator (T2.4 module split of
// jit_engine.cpp).
//
// Pure move: EmitSafepointPoll, RecordGcPoint, PropagateTypes, EmitInlineDirtyCard
// were relocated verbatim from jit_engine.cpp; no logic changed.

#include "jit_engine.h"
#include "ArchTraits.h"
#include "jit_helpers.h"
#include "code_buffer.h"
#include "IEncoder.h"
#include "codegen_bridge.h"              // CHAOS_GC_SLOT_*
#include "../interpreter/ir_reg_alloc.h" // IROpCode, RegisterInstruction
#include <gc_root_scanner.h>
#include <gc/gc_card_table.h>      // g_card_l1, g_card_l1_size
#include <instantiation_engine.h>  // runtime_instantiation::CachedCallInfo
#if defined(__aarch64__)
#include "arm64_encoder.h"
#include "Arm64Encoder.h"
#else
#include "X64Encoder.h"
#endif

namespace chaos::il2cpp::jit {

void NativeCodeGenerator::EmitSafepointPoll() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitSafepointPoll");
    if (!config_.enable_safepoint_polls || config_.safepoint_fn == nullptr)
        return;
    // Spill only GC-ref cached regs BEFORE shadow space adjustment (offsets relative to frame RSP).
    // Non-GC ref values stay in registers across safepoints — GC only needs object refs on stack.
    if (config_.enable_register_caching && cached_slots_used_)
        SpillGcRefCachedRegs();
    // Spill colored GPRs holding ObjectRef values — StoreGpr with graph coloring
    // skips stack writes, so stack slots are stale for GC scanning.
    // Caller-colored vregs are excluded: StoreGpr already keeps their stack
    // slots up-to-date (write-through), and the colored register may have
    // been clobbered since the last StoreGpr.
    if (has_graph_coloring_) {
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            uint8_t colored_x64 = gcr_.gpr_color[vr];
            if (colored_x64 != 0xFF && vr < vreg_types_.size() && vreg_types_[vr] == kTypeObjectRef) {
                if (caller_colored_mask_ & (1ULL << vr))
                    continue;
                enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vr)), colored_x64);
            }
        }
    }
#if !defined(__aarch64__)
    enc_.EmitSubRI(AT::kStackReg, 32);
#endif
    uint32_t call_start = buf_.pos();
    enc_.EmitCallRipRel(0);
    slot_patches_.push_back(
        {call_start + kSlotPatchDispOff, UINT32_MAX, reinterpret_cast<void*>(config_.safepoint_fn)});
    slot_count_used_++;
    uint32_t call_pos = call_start;
    call_sites_.push_back({UINT32_MAX, call_pos});
    RecordGcPoint(call_pos);
#if !defined(__aarch64__)
    enc_.EmitAddRI(AT::kStackReg, 32);
#endif
}

void NativeCodeGenerator::RecordGcPoint(uint32_t native_offset) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::RecordGcPoint");
    GcPoint gp;
    gp.native_offset = native_offset;

    // Determine which vregs are live at this instruction point.
    // Default: report all ObjectRef vregs (conservative, backward compatible).
    // When liveness is active: only report vregs that are BOTH ObjectRef-typed
    // AND live at current_instr_index_.
    uint64_t live_mask = ~0ULL;
    if (use_liveness_ && liveness_computed_ && current_instr_index_ < live_in_.size()) {
        live_mask = live_in_[current_instr_index_];
    }

    // Count live ObjectRef vregs at this point
    uint32_t count = 0;
    for (uint32_t vr = 0; vr < kGprCount; ++vr) {
        if (vr < vreg_types_.size() && vreg_types_[vr] == kTypeObjectRef && (live_mask & (1ULL << vr)))
            ++count;
    }
    gp.slot_count = count;
    gp.slots = (count > 0) ? static_cast<GcSlot*>(CHAOS_IL2CPP_MALLOC(count * sizeof(GcSlot))) : nullptr;
    uint32_t idx = 0;
    for (uint32_t vr = 0; vr < kGprCount; ++vr) {
        if (vr < vreg_types_.size() && vreg_types_[vr] == kTypeObjectRef && (live_mask & (1ULL << vr))) {
            uint32_t off = GprOff(vr);
            // T2.2-C1: offset is 31-bit (top bit = interior kind) — a fixed
            // vreg GPR-file offset can never approach 2 GB, so no overflow
            // gate is needed here (the old 12-bit/4096 limit is lifted).
            gp.slots[idx].kind = GcSlotKind::Stack;
            gp.slots[idx].index = off / 8;
            // Also record in slot_map_entries_ for GcSlotMapV0
            slot_map_entries_.push_back(CHAOS_GC_SLOT_ENCODE(off, CHAOS_GC_SLOT_KIND_OBJECT));
            idx++;
        }
    }

    // T2.2-B: record live volatile (caller-saved) physical registers holding
    // GC refs at this point.  A vreg that is (a) ObjectRef-typed, (b) live, and
    // (c) colored to a VOLATILE physical register is a register root.  (As a
    // precision note: in the current write-through GC model every colored vreg
    // is also spilled to its stack slot at a call/safepoint, so these register
    // bits are forward-looking metadata for when call-crossing write-through is
    // eliminated — see T2.1/§2.3.)  Bits are indexed by physical x64 register
    // number; RDI(7)/R8-R09 are volatile roots, R12-R15 callee-saved stay in
    // the frame (scanned via the register window / saved slots).
    gp.live_reg_mask = 0;
    if (has_graph_coloring_) {
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            if (vr >= vreg_types_.size() || vreg_types_[vr] != kTypeObjectRef)
                continue;
            if (!(live_mask & (1ULL << vr)))
                continue;
            uint8_t phys = gcr_.gpr_color[vr];
            if (phys == 0xFF)
                continue;
            // Volatile/caller-saved physical regs on x64: RAX-R11 (0-11).
            // RDI(7) is callee-saved on Win64; keep it out of the volatile mask
            // (its value is in the frame's saved slot, scanned as a stack root).
            if (phys < 12)
                gp.live_reg_mask |= (1u << phys);
        }
    }
    gc_points_.push_back(gp);
}

void NativeCodeGenerator::PropagateTypes(const interpreter::RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::PropagateTypes");
    using interpreter::IROpCode;
    if (!instr.has_dst())
        return;
    uint8_t dst = instr.dst_reg();
    switch (instr.op_code()) {
        // Integer immediates
        case IROpCode::LdcI4:
            SetVregType(dst, kTypeInt32);
            break;
        case IROpCode::LdcI8:
        case IROpCode::ConvI:
        case IROpCode::ConvU:
        case IROpCode::Conv_I4:
        case IROpCode::Conv_I8:
        case IROpCode::ConvOvfI:
        case IROpCode::ConvOvfI4:
        case IROpCode::ConvOvfI8:
        case IROpCode::ConvOvfU:
        case IROpCode::ConvOvfU4:
        case IROpCode::ConvOvfU8:
        case IROpCode::LdVirtFtn:
        case IROpCode::Unbox:
        case IROpCode::LdLen:
        case IROpCode::LdArgA:
        case IROpCode::LdLocA:
        case IROpCode::SizeOf:
        case IROpCode::LdToken:
        case IROpCode::LocAlloc:
        case IROpCode::LdElem:
        case IROpCode::LdElemA:
        case IROpCode::LdElemNoChk:
        case IROpCode::LdElemANoChk:
        case IROpCode::Simd:
        case IROpCode::SimdFma:
            SetVregType(dst, kTypeInt64);
            break;

        // Float immediates and conversions
        case IROpCode::LdcR4:
        case IROpCode::Conv_R4:
            SetVregType(dst, kTypeFloat32);
            break;
        case IROpCode::LdcR8:
        case IROpCode::Conv_R8:
            SetVregType(dst, kTypeFloat64);
            break;
        case IROpCode::ConvRUn:
            SetVregType(dst, kTypeFloat64);
            break;

        // Object references
        case IROpCode::LdNull:
        case IROpCode::LdStr:
        case IROpCode::NewObj:
        case IROpCode::Box:
        case IROpCode::CastClass:
        case IROpCode::IsInst:
        case IROpCode::NewArr:
        case IROpCode::LdObj:
        case IROpCode::LdFtn:
            SetVregType(dst, kTypeObjectRef);
            break;

        // LdFld: use field type tags for precision when available
        // LdSFld: conservative ObjectRef (no field_index carried in RegisterInstruction)
        case IROpCode::LdFld: {
            if (config_.field_type_tags != nullptr && instr.imm.field_offset < config_.field_type_count) {
                uint8_t field_tag = config_.field_type_tags[instr.imm.field_offset];
                if (field_tag <= kTypeObjectRef) {
                    SetVregType(dst, field_tag);
                    break;
                }
            }
            // Conservative fallback
            SetVregType(dst, kTypeObjectRef);
            break;
        }
        case IROpCode::LdSFld:
            SetVregType(dst, kTypeObjectRef);
            break;

        // Call: use ret_tag from call_cache for precise return type
        case IROpCode::Call:
        case IROpCode::CallVirt:
        case IROpCode::CallBridge:
        case IROpCode::Calli: {
            uint8_t tag = kTypeObjectRef;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                auto& cached =
                    static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                if (cached.ret_tag != 0xFF && cached.ret_tag <= kTypeObjectRef)
                    tag = cached.ret_tag;
            } else if (config_.method_ret_tags != nullptr && current_instr_index_ < config_.method_ret_tag_count) {
                uint8_t rtag = config_.method_ret_tags[current_instr_index_];
                if (rtag <= kTypeObjectRef)
                    tag = rtag;
            }
            SetVregType(dst, tag);
            break;
        }

        // LdArg: use cached argument type tags for precision
        case IROpCode::LdArg: {
            uint8_t tag = kTypeObjectRef;
            if (config_.arg_type_tags != nullptr && instr.imm.operand_index < config_.arg_type_count) {
                uint8_t arg_tag = config_.arg_type_tags[instr.imm.operand_index];
                if (arg_tag <= kTypeObjectRef)
                    tag = arg_tag;
            }
            SetVregType(dst, tag);
            break;
        }

        // LdLoc, StLoc (src register holds stored type)
        case IROpCode::LdLoc:
            if (instr.has_src1() && instr.src1_reg() < vreg_types_.size())
                SetVregType(dst, vreg_types_[instr.src1_reg()]);
            else
                SetVregType(dst, kTypeVoid);
            break;

        // Dup: propagate source type
        case IROpCode::Dup:
            if (instr.has_src1() && instr.src1_reg() < vreg_types_.size())
                SetVregType(dst, vreg_types_[instr.src1_reg()]);
            else
                SetVregType(dst, kTypeVoid);
            break;

        // Arithmetic, bitwise, shift, comparison: propagate from src1
        case IROpCode::Add:
        case IROpCode::Sub:
        case IROpCode::Mul:
        case IROpCode::Div:
        case IROpCode::Rem:
        case IROpCode::Neg:
        case IROpCode::DivUn:
        case IROpCode::RemUn:
        case IROpCode::And:
        case IROpCode::Or:
        case IROpCode::Xor:
        case IROpCode::Not:
        case IROpCode::Shl:
        case IROpCode::Shr:
        case IROpCode::ShrUn:
        case IROpCode::Ceq:
        case IROpCode::Clt:
        case IROpCode::Cgt:
        case IROpCode::AddOvf:
        case IROpCode::SubOvf:
        case IROpCode::MulOvf:
        case IROpCode::Abs:
        case IROpCode::Min:
        case IROpCode::Max:
        case IROpCode::Popcnt:
        case IROpCode::Lzcnt:
            if (instr.has_src1() && instr.src1_reg() < vreg_types_.size())
                SetVregType(dst, vreg_types_[instr.src1_reg()]);
            else
                SetVregType(dst, kTypeInt64);
            break;

        // Default: conservative non-ref (Int64)
        default:
            SetVregType(dst, kTypeInt64);
            break;
    }
}


void NativeCodeGenerator::EmitInlineDirtyCard(uint8_t obj_reg) noexcept {
    // Inline the DirtyCard logic from gc_card_table.h, writing directly to
    // the 2-level card table.  Preserves obj_reg, RDX, R8.  Clobbers RAX, R11.
    //
    // Registers at entry: RAX,R11 scratch; RCX=obj (or whichever obj_reg).
    // The sequence follows DirtyCard() step by step:
    //   1. addr < g_heap_base  → skip (below heap)
    //   2. addr in nursery     → skip (young GC scans nursery precisely)
    //   3. idx = (addr - g_heap_base) >> kCardShift   (9)
    //   4. seg_idx = idx / 128,  card_idx = idx % 128
    //   5. seg_idx >= g_card_l1_size → skip
    //   6. seg = g_card_l1[seg_idx]; if null → skip
    //   7. if seg->cards[card_idx] != 0xFF → seg->cards[card_idx] = 0xFF

    using namespace chaos::il2cpp::runtime_core;

    // ── Step 1: below heap base? ────────────────────────────────────
    enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(&g_heap_base));
    enc_.EmitMovRM(AT::kExtraScratch2, AT::kScratchA, 0); // R11 = g_heap_base
    enc_.EmitCmpRR(obj_reg, AT::kExtraScratch2);
    uint32_t done_1 = buf_.pos();
    enc_.EmitJccRel32(kCC_B, 0); // JB .done (obj < heap_base)

    // ── Step 2: nursery fast skip ───────────────────────────────────
    enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(&g_nursery_range_begin));
    enc_.EmitMovRM(AT::kExtraScratch2, AT::kScratchA, 0); // R11 = nursery_begin
    enc_.EmitCmpRR(obj_reg, AT::kExtraScratch2);
    uint32_t compute_card = buf_.pos();
    enc_.EmitJccRel32(kCC_B, 0); // JB .compute_card

    enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(&g_nursery_range_end));
    enc_.EmitMovRM(AT::kExtraScratch2, AT::kScratchA, 0); // R11 = nursery_end
    enc_.EmitCmpRR(obj_reg, AT::kExtraScratch2);
    uint32_t done_2 = buf_.pos();
    enc_.EmitJccRel32(kCC_B, 0); // JB .done (in nursery)

    // ── Step 3: idx = (obj - g_heap_base) >> kCardShift ────────────
    uint32_t compute_card_pos = buf_.pos();
    buf_.Patch32(compute_card + 2, compute_card_pos - (compute_card + 6));

    enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(&g_heap_base));
    enc_.EmitMovRM(AT::kExtraScratch2, AT::kScratchA, 0); // R11 = g_heap_base
    enc_.EmitMovRR(AT::kScratchA, obj_reg);               // RAX = obj
    enc_.EmitSubRR(AT::kScratchA, AT::kExtraScratch2);    // RAX = obj - g_heap_base
    enc_.EmitShrRI(AT::kScratchA, 9);                     // RAX = idx (offset >> kCardShift)

    // Save idx for later card_idx computation.  Compute seg_idx.
    buf_.EmitByte(0x50);              // PUSH RAX (idx)
    enc_.EmitShrRI(AT::kScratchA, 7); // RAX = seg_idx (= idx / 128)
    buf_.EmitByte(0x50);              // PUSH RAX (seg_idx)

    // ── Step 5: seg_idx >= g_card_l1_size? ─────────────────────────
    enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(&g_card_l1_size));
    enc_.EmitMovRM(AT::kScratchA, AT::kScratchA, 0); // RAX = g_card_l1_size
    buf_.EmitByte(0x41);                             // REX.B for R11
    buf_.EmitByte(0x59);                             // POP R11 (seg_idx)
    enc_.EmitCmpRR(AT::kExtraScratch2, AT::kScratchA);
    uint32_t seg_oob = buf_.pos();
    enc_.EmitJccRel32(kCC_AE, 0); // JAE .pop_done (seg >= size → pop+done)

    // Step 6: load seg = g_card_l1[seg_idx]
    // RAX = g_card_l1_size (dead).  R11 = seg_idx.  Stack: [RSP] = idx.
    enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(&g_card_l1));
    enc_.EmitMovRM(AT::kScratchA, AT::kScratchA, 0); // RAX = g_card_l1 raw ptr
    // Manually encode MOV R11, [RAX + R11*8] - SIB addressing needed
    // since EmitMovRM only supports [base+disp].  REX.W REX.R = 0x4C.
    // Opcode 0x8B = MOV r64, r/m64.  ModRM: mod=00,reg=011(R11),rm=100(SIB).
    // SIB: scale=11(8), index=011(R11), base=000(RAX).
    buf_.EmitByte(0x4C);
    buf_.EmitByte(0x8B);
    buf_.EmitByte(0x1C);
    buf_.EmitByte(0xDB);

    // Step 7: null-check segment pointer
    // TEST R11,R11 -> ZF if null.  REX.W REX.B = 0x4D.
    buf_.EmitByte(0x4D);
    buf_.EmitByte(0x85);
    buf_.EmitByte(0xDB);
    uint32_t seg_null = buf_.pos();
    enc_.EmitJccRel32(kCC_E, 0); // JE .pop_done (seg is null)

    // Step 8: seg->cards[card_idx] = 0xFF (unconditional)
    buf_.EmitByte(0x58); // POP RAX (idx) -> stack clean
    buf_.EmitByte(0x83); // AND r/m32, imm8 (32-bit ops zero-extend)
    buf_.EmitByte(0xE0); // ModRM: mod=11, reg=4(/4 AND), rm=0(RAX)
    buf_.EmitByte(0x7F); // imm8 = 127 -> RAX = card_idx
    // MOV byte ptr [R11 + RAX], 0xFF via SIB
    buf_.EmitByte(0x41); // REX.B (base=R11 in SIB)
    buf_.EmitByte(0xC6); // MOV r/m8, imm8
    buf_.EmitByte(0x04); // ModRM: mod=00, reg=0(/0), rm=SIB
    buf_.EmitByte(0x03); // SIB: scale=1, index=RAX, base=R11
    buf_.EmitByte(0xFF); // imm8 = 0xFF
    uint32_t after_write = buf_.pos();
    buf_.EmitByte(0xEB); // JMP rel8 (skip over .pop_done)
    buf_.EmitByte(0x00); // placeholder, patched below

    // .pop_done: pop idx, fall through to .done
    uint32_t pop_done = buf_.pos();
    buf_.Patch32(seg_oob + 2, pop_done - (seg_oob + 6));
    buf_.Patch32(seg_null + 2, pop_done - (seg_null + 6));
    buf_.EmitByte(0x58); // POP RAX (idx)

    // .done: all paths converge
    uint32_t done_pos = buf_.pos();
    buf_.Patch32(done_1 + 2, done_pos - (done_1 + 6));
    buf_.Patch32(done_2 + 2, done_pos - (done_2 + 6));
    int8_t jmp_offset = static_cast<int8_t>(done_pos - (after_write + 2));
    buf_.Patch8(after_write + 1, static_cast<uint8_t>(jmp_offset));
}

} // namespace chaos::il2cpp::jit
