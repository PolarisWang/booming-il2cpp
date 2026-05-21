#include "code_generator.h"
#include "x64_encoder.h"
#include "code_buffer.h"
#include "codegen_helpers.h"
#include "t4_seh_handler.h"

#include "../interpreter/ir_reg_alloc.h"
#include "../interpreter/interpreter_vm.h"
#include "reg_alloc_graph_coloring.h"

#include <codegen_bridge.h>
#include <instantiation_engine.h>
#include <chaos/log.h>
#include <chaos/profile.h>

#include <cstdint>
#include <cstring>
#include <vector>

namespace chaos::il2cpp::codegen {

// ── Frame layout constants ─────────────────────────────────────────────────
// Stack frame (relative to RSP):
//   [rsp + 0 .. 32)       = Win64 shadow space (for callee calls)
//   [rsp + 32 .. 544)     = GPR file (virtual register 0..63, 512 bytes)
//   [rsp + 544 .. 800)    = FPR file (virtual register 64..95, 256 bytes)
//   [rsp + 800 .. 864)    = CallVirtArgs struct (only used for CallVirt)
// Total base frame: 864 bytes
//
// When LocAlloc is used, an additional reserve region is appended:
//   [rsp + base_frame_end .. +8)       = localloc_bump (uint32_t counter)
//   [rsp + base_frame_end+8 .. +4008)  = localloc_reserve (4KB scratch)
//   localloc_extra = 4008 bytes

static constexpr uint32_t kShadowSize = 32;
static constexpr uint32_t kGprCount   = interpreter::kGPRegisters;  // 64
static constexpr uint32_t kFprCount   = interpreter::kFPRegisters;  // 32
static constexpr uint32_t kGprFileSize  = kGprCount * 8;  // 512 bytes
static constexpr uint32_t kFprFileSize  = kFprCount * 8;  // 256 bytes
static constexpr uint32_t kGprFileOff   = kShadowSize;     // 32
static constexpr uint32_t kFprFileOff   = kGprFileOff + kGprFileSize;  // 544
static constexpr uint32_t kCallVirtArgsOff = kFprFileOff + kFprFileSize; // 800
static constexpr uint32_t kFrameSize    = kCallVirtArgsOff + sizeof(CodegenCallVirtArgs);  // 864 bytes

// LocAlloc reserve: bump counter + scratch region
static constexpr uint32_t kLocAllocReserveSize   = 4096;
static constexpr uint32_t kLocAllocBumpAndReserve = 8 + kLocAllocReserveSize;  // 4104
static constexpr uint32_t kMaxTlabInlineSize = 2048;  // max bytes per TLAB inline allocation

// Helper: RSP offset for a virtual GPR.
inline uint32_t GprOff(uint32_t vreg) noexcept {
    return kGprFileOff + vreg * 8;
}

// Helper: RSP offset for a virtual FPR (vreg 64+).
inline uint32_t FprOff(uint32_t vreg) noexcept {
    return kFprFileOff + (vreg - kGprCount) * 8;
}

// Register convention constants (mirrors ir_reg_alloc.h convention)
// r0-r7   = argument registers (mapped from LdArg operand_index)
// r8-r15  = local variable registers (mapped from LdLoc/StLoc operand_index)
// r16+    = evaluation stack virtual registers
static constexpr uint32_t kArgRegCount   = 8;
static constexpr uint32_t kLocalRegBase  = 8;

// Internal class that drives code generation.
class NativeCodeGenerator {
public:
    NativeCodeGenerator(const interpreter::RegisterMethod& rm,
                        const CodeGenConfig& config)
        : rm_(rm), config_(config) {}

    NativeMethod* Generate() noexcept;

private:
    const interpreter::RegisterMethod& rm_;
    CodeGenConfig config_;
    CodeBuffer buf_;

    // Per-instruction byte offset in the output buffer.
    std::vector<uint32_t> instr_offsets_;

    // Current instruction index in the Generate() loop.
    uint32_t current_instr_index_ = 0;

    // Branch patch records.
    struct BranchPatch {
        uint32_t patch_offset;
        uint32_t target_instr;
    };
    std::vector<BranchPatch> branch_patches_;

    // Deopt jump patch records.
    struct DeoptJumpPatch {
        uint32_t patch_offset;
    };
    std::vector<DeoptJumpPatch> deopt_jump_patches_;

    // Jump table patch records (for Switch with >=4 cases).
    struct JumpTablePatch {
        uint32_t table_entry_offset;  // buffer offset of this .int32 entry
        uint32_t table_base;          // buffer offset of the table start
        uint32_t target_instr;        // target instruction index
    };
    std::vector<JumpTablePatch> jump_table_patches_;

    // Call site metadata
    std::vector<CallSiteInfo> call_sites_;

    // Deoptimization metadata
    std::vector<DeoptEntry> deopt_entries_;
    std::vector<DeoptValue> deopt_values_;

    // GC points
    std::vector<GcPoint> gc_points_;

    // GC slot map entries (for GcSlotMapV0 serialization)
    std::vector<uint32_t> slot_map_entries_;

    // ── Conservative forward type inference ────────────────────────────
    // Per-vreg type state used to determine which vregs hold ObjectRefs
    // at GC safepoints.  Initialized to kTypeVoid in Generate().
    static constexpr uint8_t kTypeVoid     = 0;
    static constexpr uint8_t kTypeInt32    = 1;
    static constexpr uint8_t kTypeInt64    = 2;
    static constexpr uint8_t kTypeFloat32  = 3;
    static constexpr uint8_t kTypeFloat64  = 4;
    static constexpr uint8_t kTypeObjectRef = 5;

    std::vector<uint8_t> vreg_types_;
    inline void SetVregType(uint32_t vreg, uint8_t type) noexcept {
        if (vreg < vreg_types_.size()) vreg_types_[vreg] = type;
    }
    void PropagateTypes(const interpreter::RegisterInstruction& instr) noexcept;
    // ───────────────────────────────────────────────────────────────────

    // Position of the deopt_return shared epilogue label.
    uint32_t deopt_return_pos_ = 0;

    // ── Register caching V1 ──────────────────────────────────────────────
    // Maps hot virtual registers to callee-saved x64 registers (RDI, R12-R15).
    // Dirty-bit tracking avoids unnecessary stack writes.
    static constexpr uint8_t kCacheableX64Regs[5] = {7, 12, 13, 14, 15}; // RDI, R12-R15
    static constexpr uint32_t kMaxCacheRegs = 5;
    static constexpr uint8_t kNotCached = 0xFF;

    // vreg → x64 reg# (kNotCached if not cached)
    uint8_t cached_x64_for_vreg_[interpreter::kGPRegisters];
    // x64 reg → vreg (kNotCached if not used as cache)
    uint8_t x64_to_cached_vreg_[16];
    // Bit i set = cache slot i is in use (maps to kCacheableX64Regs[i])
    uint32_t cached_slots_used_ = 0;
    // Bit i set = cache slot i is dirty (needs spill)
    uint32_t cached_dirty_mask_ = 0;
    // Number of cacheable regs actually selected
    uint32_t num_cache_regs_ = 0;
    // Alignment adjustment (0 or 8 bytes) when num_cache_regs_ is odd
    // Keeps RSP 16-byte aligned per Win64 ABI.
    int32_t frame_align_adj_ = 0;

    // ── Graph-coloring register allocation (V2) ──────────────────────────
    // Replaces V1 frequency-based caching with Chaitin-Briggs coloring.
    // Results are mutually exclusive with V1: when active, cached_slots_used_=0.
    GraphColoringResult gcr_;
    bool has_graph_coloring_ = false;
    // Callee-saved x64 GPRs selected by graph coloring (subset of {7,12,13,14,15})
    uint8_t callee_x64_regs_[5];
    // x64 reg → vreg (0xFF = not colored); indexed by x64 register number
    uint8_t x64_to_colored_vreg_[16];
    // Pointer to current callee-saved register list (callee_x64_regs_ or kCacheableX64Regs)
    const uint8_t* callee_saved_regs_ = kCacheableX64Regs;

    // ── FPR (XMM) coloring ────────────────────────────────────────────────
    uint8_t callee_xmm_regs_[10];  // XMM6-XMM15 max
    uint8_t callee_xmm_fi_[10];    // FPR vreg index (fi) for each callee_xmm_regs_[slot]
    uint32_t num_fpr_callee_ = 0;
    int32_t xmm_save_size_ = 0;
    // Bitmask of vregs colored by allocator but filtered (caller-saved).
    // These vregs fall through to stack I/O, so the prologue zeros
    // their stack slots to prevent garbage reads (e.g. Calli func_ptr).
    uint64_t filtered_vreg_mask_ = 0;

    // Bitmask of vregs colored to caller-saved x64 registers (R8-R11).
    // These survive the callee-only filter — EmitCallWithSpill reloads
    // them after each runtime helper call.
    uint64_t caller_colored_mask_ = 0;

    // LocAlloc: extra frame bytes (bump counter + reserve) when method uses LocAlloc.
    uint32_t localloc_extra_ = 0;

    void SelectCacheableRegs() noexcept;
    void SpillCachedRegs() noexcept;
    void SpillGcRefCachedRegs() noexcept;
    void EmitCallWithSpill(uint8_t reg) noexcept;

    void LoadGpr(uint8_t x64_reg, uint32_t vreg) noexcept;
    void StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept;
    void LoadFpr(uint8_t xmm_reg, uint32_t vreg) noexcept;
    void StoreFpr(uint8_t xmm_reg, uint32_t vreg) noexcept;
    void EmitSafepointPoll() noexcept;
    void EmitGprArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;
    void EmitFprArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;
    void EmitBitwise(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;
    void EmitShift(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2, int32_t imm) noexcept;
    void ResolveBranches() noexcept;
    bool EmitInstruction(const interpreter::RegisterInstruction& instr) noexcept;
    void EmitDeoptSequence(uint32_t instr_pc, uint32_t osr_resume_pc = 0) noexcept;
    void DumpCode() noexcept;
    void RecordGcPoint(uint32_t native_offset) noexcept;
};

void NativeCodeGenerator::LoadGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LoadGpr");
    if (vreg >= interpreter::kGPRegisters) { EmitMovRM(buf_, x64_reg, kRSP, static_cast<int32_t>(GprOff(vreg))); return; }

    // Graph coloring V2: colored vreg → direct reg-to-reg move
    if (has_graph_coloring_) {
        uint8_t colored_x64 = gcr_.gpr_color[vreg];
        if (colored_x64 != 0xFF) {
            if (x64_reg != colored_x64) EmitMovRR(buf_, x64_reg, colored_x64);
            return;
        }
    }
    // V1 cache hit
    if (config_.enable_register_caching) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != kNotCached) {
            if (x64_reg != cached) EmitMovRR(buf_, x64_reg, cached);
            return;
        }
    }
    // Load from stack
    EmitMovRM(buf_, x64_reg, kRSP, static_cast<int32_t>(GprOff(vreg)));
}

void NativeCodeGenerator::StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StoreGpr");
    if (vreg >= interpreter::kGPRegisters) { EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vreg)), x64_reg); return; }

    // Graph coloring V2: colored vreg → direct reg-to-reg move (no stack write)
    if (has_graph_coloring_) {
        uint8_t colored_x64 = gcr_.gpr_color[vreg];
        if (colored_x64 != 0xFF) {
            if (x64_reg != colored_x64) EmitMovRR(buf_, colored_x64, x64_reg);
            // Caller-colored vregs: write through to stack so the stack slot
            // holds the correct value even if argument setup clobbers the
            // colored register before EmitCallWithSpill's pre-call spill.
            if (caller_colored_mask_ & (1ULL << vreg))
                EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vreg)), colored_x64);
            return;
        }
    }
    // V1 cache
    if (config_.enable_register_caching) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != kNotCached && num_cache_regs_ > 0) {
            if (cached == x64_reg) {
                uint32_t slot = 0;
                for (; slot < kMaxCacheRegs; ++slot) {
                    if (kCacheableX64Regs[slot] == cached) break;
                }
                if (slot < kMaxCacheRegs) cached_dirty_mask_ |= (1u << slot);
                return;
            }
            EmitMovRR(buf_, cached, x64_reg);
            uint32_t slot = 0;
            for (; slot < kMaxCacheRegs; ++slot) {
                if (kCacheableX64Regs[slot] == cached) break;
            }
            if (slot < kMaxCacheRegs) cached_dirty_mask_ |= (1u << slot);
            return;
        }
    }
    // Not cached/spilled: write through to stack
    EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vreg)), x64_reg);
}

void NativeCodeGenerator::LoadFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    // Graph coloring V2: colored FPR → direct reg-to-reg move
    if (has_graph_coloring_ && vreg >= kGprCount) {
        uint32_t fi = vreg - kGprCount;
        if (fi < 32) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (xmm_reg != colored_xmm)
                    EmitMovSDRR(buf_, xmm_reg, colored_xmm);
                return;
            }
        }
    }
    // Fallback: load from stack
    EmitMovSDRM(buf_, xmm_reg, kRSP, static_cast<int32_t>(FprOff(vreg)));
}

void NativeCodeGenerator::StoreFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    // Graph coloring V2: colored FPR → direct reg-to-reg move (no stack write)
    if (has_graph_coloring_ && vreg >= kGprCount) {
        uint32_t fi = vreg - kGprCount;
        if (fi < 32) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (xmm_reg != colored_xmm)
                    EmitMovSDRR(buf_, colored_xmm, xmm_reg);
                return;
            }
        }
    }
    // Fallback: write through to stack
    EmitMovSDMR(buf_, kRSP, static_cast<int32_t>(FprOff(vreg)), xmm_reg);
}

void NativeCodeGenerator::EmitSafepointPoll() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitSafepointPoll");
    if (!config_.enable_safepoint_polls || config_.safepoint_fn == nullptr) return;
    // Spill only GC-ref cached regs BEFORE shadow space adjustment (offsets relative to frame RSP).
    // Non-GC ref values stay in registers across safepoints — GC only needs object refs on stack.
    if (config_.enable_register_caching && cached_slots_used_) SpillGcRefCachedRegs();
    // Spill colored GPRs holding ObjectRef values — StoreGpr with graph coloring
    // skips stack writes, so stack slots are stale for GC scanning.
    // Caller-colored vregs are excluded: StoreGpr already keeps their stack
    // slots up-to-date (write-through), and the colored register may have
    // been clobbered since the last StoreGpr.
    if (has_graph_coloring_) {
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            uint8_t colored_x64 = gcr_.gpr_color[vr];
            if (colored_x64 != 0xFF && vr < vreg_types_.size() &&
                vreg_types_[vr] == kTypeObjectRef) {
                if (caller_colored_mask_ & (1ULL << vr)) continue;
                EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vr)), colored_x64);
            }
        }
    }
    EmitSubRI(buf_, kRSP, 32);
    EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.safepoint_fn));
    uint32_t call_pos = buf_.pos();
    EmitCallReg(buf_, kRAX);
    call_sites_.push_back({UINT32_MAX, call_pos});
    RecordGcPoint(call_pos);
    EmitAddRI(buf_, kRSP, 32);
}

void NativeCodeGenerator::RecordGcPoint(uint32_t native_offset) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::RecordGcPoint");
    GcPoint gp;
    gp.native_offset = native_offset;

    // Count live ObjectRef vregs at this point
    uint32_t count = 0;
    for (uint32_t vr = 0; vr < kGprCount; ++vr) {
        if (vr < vreg_types_.size() && vreg_types_[vr] == kTypeObjectRef) ++count;
    }
    gp.slot_count = count;
    gp.slots = (count > 0) ? static_cast<GcSlot*>(CHAOS_IL2CPP_MALLOC(count * sizeof(GcSlot))) : nullptr;
    uint32_t idx = 0;
    for (uint32_t vr = 0; vr < kGprCount; ++vr) {
        if (vr < vreg_types_.size() && vreg_types_[vr] == kTypeObjectRef) {
            gp.slots[idx].kind = GcSlotKind::Stack;
            gp.slots[idx].index = GprOff(vr) / 8;
            // Also record in slot_map_entries_ for GcSlotMapV0
            slot_map_entries_.push_back(
                CHAOS_GC_SLOT_ENCODE(GprOff(vr), CHAOS_GC_SLOT_KIND_OBJECT));
            idx++;
        }
    }
    gc_points_.push_back(gp);
}

void NativeCodeGenerator::PropagateTypes(
    const interpreter::RegisterInstruction& instr) noexcept
{
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::PropagateTypes");
    using interpreter::IROpCode;
    if (!instr.has_dst()) return;
    uint8_t dst = instr.dst_reg();
    switch (instr.op_code()) {
    // Integer immediates
    case IROpCode::LdcI4:           SetVregType(dst, kTypeInt32); break;
    case IROpCode::LdcI8:
    case IROpCode::ConvI:  case IROpCode::ConvU:
    case IROpCode::Conv_I4: case IROpCode::Conv_I8:
    case IROpCode::ConvOvfI: case IROpCode::ConvOvfI4: case IROpCode::ConvOvfI8:
    case IROpCode::ConvOvfU: case IROpCode::ConvOvfU4: case IROpCode::ConvOvfU8:
    case IROpCode::LdVirtFtn:
    case IROpCode::Unbox:  case IROpCode::LdLen:
    case IROpCode::LdArgA: case IROpCode::LdLocA:
    case IROpCode::SizeOf: case IROpCode::LdToken:
    case IROpCode::LocAlloc:
        SetVregType(dst, kTypeInt64); break;

    // Float immediates and conversions
    case IROpCode::LdcR4: case IROpCode::Conv_R4:
        SetVregType(dst, kTypeFloat32); break;
    case IROpCode::LdcR8: case IROpCode::Conv_R8:
        SetVregType(dst, kTypeFloat64); break;
    case IROpCode::ConvRUn:
        SetVregType(dst, kTypeFloat64); break;

    // Object references
    case IROpCode::LdNull: case IROpCode::LdStr:
    case IROpCode::NewObj: case IROpCode::Box:
    case IROpCode::CastClass: case IROpCode::IsInst:
    case IROpCode::NewArr:
    case IROpCode::LdObj:
    case IROpCode::LdFtn:
        SetVregType(dst, kTypeObjectRef); break;

    // LdFld/LdSFld: V1 conservatively marks as ObjectRef
    // TODO(B3): precise field type metadata (requires per-field type map from module)
    case IROpCode::LdFld:  case IROpCode::LdSFld:
        SetVregType(dst, kTypeObjectRef); break;

    // Call: use ret_tag from call_cache for precise return type
    case IROpCode::Call:  case IROpCode::CallVirt:
    case IROpCode::CallBridge: case IROpCode::Calli: {
        uint8_t tag = kTypeObjectRef;
        if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
            auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
            if (cached.ret_tag != 0xFF && cached.ret_tag <= kTypeObjectRef)
                tag = cached.ret_tag;
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
    case IROpCode::Add: case IROpCode::Sub: case IROpCode::Mul:
    case IROpCode::Div: case IROpCode::Rem: case IROpCode::Neg:
    case IROpCode::DivUn: case IROpCode::RemUn:
    case IROpCode::And: case IROpCode::Or: case IROpCode::Xor:
    case IROpCode::Not:
    case IROpCode::Shl: case IROpCode::Shr: case IROpCode::ShrUn:
    case IROpCode::Ceq: case IROpCode::Clt: case IROpCode::Cgt:
    case IROpCode::AddOvf: case IROpCode::SubOvf: case IROpCode::MulOvf:
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

void NativeCodeGenerator::EmitGprArithmetic(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitGprArithmetic");
    // Div/Rem have implicit eax/edx/ecx register requirements.
    bool has_implicit = (opc == IROpCode::Div || opc == IROpCode::Rem ||
                         opc == IROpCode::DivUn || opc == IROpCode::RemUn);
    uint8_t op_reg = kRAX;
    uint8_t src2_reg = kRCX;
    if (has_graph_coloring_ && !has_implicit) {
        if (dst < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[dst];
            if (c != 0xFF) op_reg = c;
        }
        if (src2 != UINT32_MAX && src2 < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[src2];
            if (c != 0xFF) src2_reg = c;
        }
    }
    LoadGpr(op_reg, src1);
    if (src2 != UINT32_MAX) LoadGpr(src2_reg, src2);
    if (opc == IROpCode::Add) {
        EmitAdd32RR(buf_, op_reg, src2_reg);
    } else if (opc == IROpCode::Sub) {
        EmitSub32RR(buf_, op_reg, src2_reg);
    } else if (opc == IROpCode::Mul) {
        EmitImul32RR(buf_, op_reg, src2_reg);
    } else if (opc == IROpCode::Neg) {
        EmitNeg32(buf_, op_reg);
    } else if (opc == IROpCode::Div || opc == IROpCode::Rem) {
        EmitREXB(buf_, false, 0); buf_.EmitByte(0x99);  // cdq: sign-extend eax→edx:eax
        EmitREX(buf_, false, 7, kRCX); buf_.EmitByte(0xF7); buf_.EmitByte(ModRM(3, 7, kRCX));  // idiv ecx
        if (opc == IROpCode::Rem) EmitMovRR(buf_, op_reg, kRDX);
    } else if (opc == IROpCode::DivUn || opc == IROpCode::RemUn) {
        EmitXor32ZR(buf_, kRDX);  // xor edx, edx
        EmitREX(buf_, false, 6, kRCX); buf_.EmitByte(0xF7); buf_.EmitByte(ModRM(3, 6, kRCX));  // div ecx
        if (opc == IROpCode::RemUn) EmitMovRR(buf_, op_reg, kRDX);
    }
    StoreGpr(op_reg, dst);
}

void NativeCodeGenerator::EmitFprArithmetic(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitFprArithmetic");
    // Pick the working XMM register: prefer src1's colored reg, else XMM0.
    uint8_t op_xmm = 0;
    if (has_graph_coloring_ && src1 >= kGprCount) {
        uint32_t fi = src1 - kGprCount;
        if (fi < 32 && gcr_.fpr_color[fi] != 0xFF)
            op_xmm = gcr_.fpr_color[fi];
    }
    LoadFpr(op_xmm, src1);
    uint8_t src2_xmm = 1;
    if (src2 != UINT32_MAX && src2 >= kGprCount) {
        if (has_graph_coloring_) {
            uint32_t fi = src2 - kGprCount;
            if (fi < 32 && gcr_.fpr_color[fi] != 0xFF)
                src2_xmm = gcr_.fpr_color[fi];
        }
        LoadFpr(src2_xmm, src2);
    }
    if (opc == IROpCode::Add) EmitAddSDRR(buf_, op_xmm, src2_xmm);
    else if (opc == IROpCode::Sub) EmitSubSDRR(buf_, op_xmm, src2_xmm);
    else if (opc == IROpCode::Mul) EmitMulSDRR(buf_, op_xmm, src2_xmm);
    else if (opc == IROpCode::Div) EmitDivSDRR(buf_, op_xmm, src2_xmm);
    StoreFpr(op_xmm, dst);
}

void NativeCodeGenerator::EmitBitwise(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitBitwise");
    uint8_t op_reg = kRAX;
    uint8_t src2_reg = kRCX;
    if (has_graph_coloring_) {
        if (dst < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[dst];
            if (c != 0xFF) op_reg = c;
        }
        if (src2 != UINT32_MAX && src2 < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[src2];
            if (c != 0xFF) src2_reg = c;
        }
    }
    LoadGpr(op_reg, src1);
    if (opc == IROpCode::Not) EmitNot32(buf_, op_reg);
    else if (opc == IROpCode::And) { if (src2 != UINT32_MAX) { LoadGpr(src2_reg, src2); EmitAnd32RR(buf_, op_reg, src2_reg); } }
    else if (opc == IROpCode::Or)  { if (src2 != UINT32_MAX) { LoadGpr(src2_reg, src2); EmitOr32RR(buf_, op_reg, src2_reg); } }
    else if (opc == IROpCode::Xor) { if (src2 != UINT32_MAX) { LoadGpr(src2_reg, src2); EmitXor32RR(buf_, op_reg, src2_reg); } }
    StoreGpr(op_reg, dst);
}

// ── Shift with proper 32-bit semantics ─────────────────────────────────
// RegisterExecute uses int32_t/uint32_t for shift operations, which means:
//   Shr (signed):   (int32_t)RAX >> CL  → sign-extend to 64-bit
//   ShrUn (unsigned): (uint32_t)RAX >> CL → zero-extend to 64-bit
// x64 32-bit ops automatically zero-extend to 64 bits.
void NativeCodeGenerator::EmitShift(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2, int32_t imm) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitShift");
    // For variable shifts (src2 present), shift count must be in CL (kRCX),
    // but the destination can be any register. For immediate shifts, any GPR works.
    uint8_t op_reg = kRAX;
    if (has_graph_coloring_ && dst < interpreter::kGPRegisters) {
        uint8_t c = gcr_.gpr_color[dst];
        if (c != 0xFF) op_reg = c;
    }
    LoadGpr(op_reg, src1);
    if (src2 != UINT32_MAX) {
        LoadGpr(kRCX, src2);
        EmitREXB(buf_, false, op_reg);  // REX.B for extended destination register
        if (opc == IROpCode::Shl) {
            buf_.EmitByte(0xD3); buf_.EmitByte(ModRM(3, 4, op_reg));
        } else if (opc == IROpCode::Shr) {
            buf_.EmitByte(0xD3); buf_.EmitByte(ModRM(3, 7, op_reg));
        } else if (opc == IROpCode::ShrUn) {
            buf_.EmitByte(0xD3); buf_.EmitByte(ModRM(3, 5, op_reg));
        }
    } else {
        uint8_t shift = static_cast<uint8_t>(imm & 0x1F);
        if (opc == IROpCode::Shl) {
            EmitREX(buf_, false, 4, op_reg); buf_.EmitByte(0xC1); buf_.EmitByte(ModRM(3, 4, op_reg)); buf_.EmitByte(shift);
        } else if (opc == IROpCode::Shr) {
            EmitREX(buf_, false, 7, op_reg); buf_.EmitByte(0xC1); buf_.EmitByte(ModRM(3, 7, op_reg)); buf_.EmitByte(shift);
        } else if (opc == IROpCode::ShrUn) {
            EmitREX(buf_, false, 5, op_reg); buf_.EmitByte(0xC1); buf_.EmitByte(ModRM(3, 5, op_reg)); buf_.EmitByte(shift);
        }
    }
    StoreGpr(op_reg, dst);
}

void NativeCodeGenerator::ResolveBranches() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::ResolveBranches");
    for (auto& bp : branch_patches_) {
        if (bp.target_instr >= instr_offsets_.size()) {
            buf_.Patch32(bp.patch_offset, 0);
            continue;
        }
        uint32_t target_off = instr_offsets_[bp.target_instr];
        uint32_t current_off = bp.patch_offset + 4;
        int32_t disp = static_cast<int32_t>(target_off - current_off);
        buf_.Patch32(bp.patch_offset, static_cast<uint32_t>(disp));
    }
    uint32_t deopt_ret_off = deopt_return_pos_;
    for (auto& djp : deopt_jump_patches_) {
        int32_t disp = static_cast<int32_t>(deopt_ret_off - (djp.patch_offset + 4));
        buf_.Patch32(djp.patch_offset, static_cast<uint32_t>(disp));
    }
    // Patch jump table entries: each stores target_off - table_base
    for (auto& jtp : jump_table_patches_) {
        uint32_t target_off = jtp.target_instr < instr_offsets_.size()
                              ? instr_offsets_[jtp.target_instr] : 0;
        int32_t disp = static_cast<int32_t>(target_off - jtp.table_base);
        buf_.Patch32(jtp.table_entry_offset, static_cast<uint32_t>(disp));
    }
}

void NativeCodeGenerator::EmitDeoptSequence(uint32_t instr_pc, uint32_t osr_resume_pc) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitDeoptSequence");
    uint32_t deopt_pos = buf_.pos();
    if (config_.enable_deopt) {
        // Spill any dirty cached regs so the stack frame is consistent
        if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();

        uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            DeoptValue dv;
            dv.reg_index    = vr;
            // Use inferred type from vreg_types_ (kType* constants match ValueTag exactly)
            dv.value_tag    = (vr < static_cast<uint32_t>(vreg_types_.size()))
                              ? vreg_types_[vr]
                              : static_cast<uint8_t>(interpreter::ValueTag::Int64);
            dv.is_spilled   = true;
            dv.spill_offset = static_cast<int16_t>(GprOff(vr));
            deopt_values_.push_back(dv);
        }
        for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
            DeoptValue dv;
            dv.reg_index    = vr;
            dv.value_tag    = static_cast<uint8_t>(interpreter::ValueTag::Float64);
            dv.is_spilled   = true;
            dv.spill_offset = static_cast<int16_t>(FprOff(vr));
            deopt_values_.push_back(dv);
        }
        uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
        DeoptEntry entry;
        entry.native_offset = deopt_pos;
        entry.instr_pc      = instr_pc;
        entry.osr_resume_pc = osr_resume_pc;
        entry.num_values    = n_vals;
        entry.values_offset = val_start;
        deopt_entries_.push_back(entry);

        // Emit CALL to DeoptSaveFrameState(RSP) — saves all register values
        // and type tags from the stack frame to t_deopt_state before we
        // write kDeoptMagic and return to InterpreterEntryDirect.
        EmitMovRR(buf_, kRCX, kRSP);           // RCX = codegen_rsp
        EmitSubRI(buf_, kRSP, 32);              // shadow space for Win64 callee
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::DeoptSaveFrameState));
        EmitCallWithSpill(kRAX);
        EmitAddRI(buf_, kRSP, 32);              // restore shadow space
    }
    EmitMovImm64(buf_, kRAX, kDeoptMagic);
    EmitMovMR(buf_, kRSI, 0, kRAX);
    uint32_t patch_off = buf_.pos() + 1;
    EmitJmpRel32(buf_, 0);
    deopt_jump_patches_.push_back({patch_off});
}

// ── Register caching V1 ──────────────────────────────────────────────────
void NativeCodeGenerator::SelectCacheableRegs() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::SelectCacheableRegs");
    if (!config_.enable_register_caching) return;

    // Count access frequency for each virtual register across all instructions.
    // Weight: dst counts 2, src counts 1 — dst access is more critical since it
    // implies a subsequent read by another instruction.
    uint32_t freq[interpreter::kGPRegisters] = {};
    for (const auto& ri : rm_.instructions) {
        if (ri.has_dst() && ri.dst_reg() < interpreter::kGPRegisters)
            freq[ri.dst_reg()] += 2;
        if (ri.has_src1() && ri.src1_reg() < interpreter::kGPRegisters)
            freq[ri.src1_reg()] += 1;
        if (ri.has_src2() && ri.src2_reg() < interpreter::kGPRegisters)
            freq[ri.src2_reg()] += 1;
    }

    // Initialize all mappings to "not cached"
    std::memset(cached_x64_for_vreg_, kNotCached, sizeof(cached_x64_for_vreg_));
    std::memset(x64_to_cached_vreg_, kNotCached, sizeof(x64_to_cached_vreg_));
    cached_slots_used_ = 0;
    cached_dirty_mask_ = 0;
    num_cache_regs_ = 0;

    // Select top-k vregs by frequency. Since kMaxCacheRegs is small (5), a
    // simple linear selection is sufficient — no need for partial sort.
    for (uint32_t slot = 0; slot < kMaxCacheRegs; ++slot) {
        uint32_t best_vreg = kNotCached;
        uint32_t best_freq = 0;
        for (uint32_t vr = 0; vr < interpreter::kGPRegisters; ++vr) {
            if (freq[vr] > best_freq && cached_x64_for_vreg_[vr] == kNotCached) {
                best_freq = freq[vr];
                best_vreg = vr;
            }
        }
        if (best_vreg == kNotCached || best_freq == 0) break;

        uint8_t x64r = kCacheableX64Regs[slot];
        cached_x64_for_vreg_[best_vreg] = x64r;
        x64_to_cached_vreg_[x64r] = static_cast<uint8_t>(best_vreg);
        cached_slots_used_ |= (1u << slot);
        ++num_cache_regs_;
    }
}

void NativeCodeGenerator::SpillCachedRegs() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::SpillCachedRegs");
    if (!config_.enable_register_caching || cached_slots_used_ == 0) return;
    if (cached_dirty_mask_ == 0) return;

    // For each dirty slot, write the cached x64 register to the vreg's stack slot
    uint32_t dirty = cached_dirty_mask_;
    for (uint32_t slot = 0; slot < kMaxCacheRegs && dirty; ++slot) {
        if (!(dirty & (1u << slot))) continue;
        dirty &= ~(1u << slot);
        uint8_t x64r = static_cast<uint8_t>(kCacheableX64Regs[slot]);
        uint32_t vreg = x64_to_cached_vreg_[x64r];
        if (vreg != kNotCached) {
            EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vreg)), x64r);
        }
    }
    cached_dirty_mask_ = 0;
}

void NativeCodeGenerator::SpillGcRefCachedRegs() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::SpillGcRefCachedRegs");
    if (!config_.enable_register_caching || cached_slots_used_ == 0) return;
    if (cached_dirty_mask_ == 0) return;

    // Only spill cache slots holding GC object references. Non-GC ref values
    // (int32, int64, float, etc.) stay in registers across safepoints since
    // the GC only needs to scan object references on the stack.
    uint32_t dirty = cached_dirty_mask_;
    for (uint32_t slot = 0; slot < kMaxCacheRegs && dirty; ++slot) {
        if (!(dirty & (1u << slot))) continue;
        dirty &= ~(1u << slot);
        uint8_t x64r = static_cast<uint8_t>(kCacheableX64Regs[slot]);
        uint32_t vreg = x64_to_cached_vreg_[x64r];
        if (vreg != kNotCached && vreg < vreg_types_.size() &&
            vreg_types_[vreg] == kTypeObjectRef) {
            EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vreg)), x64r);
            cached_dirty_mask_ &= ~(1u << slot);
        }
    }
}

void NativeCodeGenerator::EmitCallWithSpill(uint8_t reg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitCallWithSpill");
    if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();
    // Spill colored GPRs — StoreGpr with graph coloring skips the stack write,
    // so stack slots (read by deopt metadata, GC scanning, CodegenCallVirt)
    // hold stale values.  Write all colored vregs to their stack slots.
    // Caller-colored vregs are excluded: StoreGpr already keeps their stack
    // slots up-to-date (write-through), and argument setup may have clobbered
    // the colored register making a pre-call spill capture a stale value.
    if (has_graph_coloring_) {
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            uint8_t colored_x64 = gcr_.gpr_color[vr];
            if (colored_x64 != 0xFF) {
                if (caller_colored_mask_ & (1ULL << vr)) continue;
                EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vr)), colored_x64);
            }
        }
    }
    EmitCallReg(buf_, reg);
    // Post-call reload: restore caller-saved colored vregs (R8-R11, colors 1-4)
    // that were clobbered by the call.  Only reload vregs that are still alive
    // at this point — the conservative approach reloads all caller-colored
    // vregs from their stack slots (spilled before the call above).
    if (has_graph_coloring_ && caller_colored_mask_) {
        uint64_t mask = caller_colored_mask_;
        for (uint32_t vr = 0; mask; ++vr) {
            if (mask & 1) {
                uint8_t colored_x64 = gcr_.gpr_color[vr];
                EmitMovRM(buf_, colored_x64, kRSP, static_cast<int32_t>(GprOff(vr)));
            }
            mask >>= 1;
        }
    }
}

bool NativeCodeGenerator::EmitInstruction(const interpreter::RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitInstruction");
    using IROpCode = interpreter::IROpCode;
    auto opc = instr.op_code();
    auto& _buf = this->buf_;
    auto& _instr = instr;
    auto& _config = this->config_;
    auto& _rm = this->rm_;
    auto& _call_sites = this->call_sites_;
    auto& _branch_patches = this->branch_patches_;
    auto& _deopt_entries = this->deopt_entries_;
    auto& _deopt_values = this->deopt_values_;
    switch (opc) {

    case IROpCode::LdcI4: {
        if (!instr.has_dst()) return false;
        EmitMovRIImm32(buf_, kRAX, static_cast<uint32_t>(instr.imm.i4));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdcI8: {
        if (!instr.has_dst()) return false;
        int64_t val = instr.imm.i8;
        if (val >= INT32_MIN && val <= INT32_MAX) EmitMovRI32(buf_, kRAX, static_cast<int32_t>(val));
        else EmitMovImm64(buf_, kRAX, static_cast<uint64_t>(val));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdcR4: {
        if (!instr.has_dst()) return false;
        // Load float constant, promote to double (matching RegisterExecute's
        // WriteFloat64(static_cast<double>(v), dst) semantics).
        float v; std::memcpy(&v, &instr.imm.i4, sizeof(v));
        uint32_t bits; std::memcpy(&bits, &v, sizeof(bits));
        EmitMovRIImm32(buf_, kRAX, bits); EmitMovdXrm(buf_, 0, kRAX);
        EmitCvtss2sd(buf_, 0, 0);  // promote float→double
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::LdcR8: {
        if (!instr.has_dst()) return false;
        uint64_t bits; std::memcpy(&bits, &instr.imm.r8, sizeof(bits));
        EmitMovImm64(buf_, kRAX, bits); EmitMovqXrm(buf_, 0, kRAX);
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::LdNull: {
        if (!instr.has_dst()) return false;
        EmitXorZR(buf_, kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdArg: {
        if (!instr.has_dst()) return false;
        uint32_t arg_idx = instr.imm.operand_index;
        EmitMovRM(buf_, kRAX, kRBX, static_cast<int32_t>(arg_idx * 8));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdLoc: {
        if (!instr.has_dst()) return false;
        uint32_t local_idx = instr.imm.operand_index;
        uint32_t vreg = kLocalRegBase + local_idx;
        if (vreg < kGprCount) { LoadGpr(kRAX, vreg); StoreGpr(kRAX, instr.dst_reg()); }
        return true;
    }

    case IROpCode::StLoc: {
        if (!instr.has_src1()) return false;
        uint32_t local_idx = instr.imm.operand_index;
        uint32_t vreg = kLocalRegBase + local_idx;
        if (vreg < kGprCount) {
            LoadGpr(kRAX, instr.src1_reg()); StoreGpr(kRAX, vreg);
            if (instr.has_dst()) StoreGpr(kRAX, instr.dst_reg());
        }
        return true;
    }

    case IROpCode::StArg: {
        if (!instr.has_src1()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        EmitMovMR(buf_, kRBX, static_cast<int32_t>(instr.imm.operand_index * 8), kRAX);
        return true;
    }

    case IROpCode::Ret: {
        // Spill cached regs before reading return value
        if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();
        if (instr.has_src1()) { LoadGpr(kRAX, instr.src1_reg()); EmitMovMR(buf_, kRSI, 0, kRAX); }
        // Restore callee-saved XMMs
        for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
            int32_t off = static_cast<int32_t>(kFrameSize + frame_align_adj_ + si * 16);
            EmitMovUPRM(buf_, callee_xmm_regs_[si], kRSP, off);
        }
        EmitAddRI(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_));
        for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
            EmitPop(buf_, callee_saved_regs_[slot - 1]);
        EmitPop(buf_, kRSI); EmitPop(buf_, kRBX); EmitPop(buf_, kRBP); EmitRet(buf_);
        return true;
    }

    case IROpCode::Add: case IROpCode::Sub: case IROpCode::Mul:
    case IROpCode::Div: case IROpCode::Rem: case IROpCode::Neg:
    case IROpCode::DivUn: case IROpCode::RemUn: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        if (instr.src1_reg() >= kGprCount)
            EmitFprArithmetic(opc, instr.dst_reg(), instr.src1_reg(),
                              instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
        else
            EmitGprArithmetic(opc, instr.dst_reg(), instr.src1_reg(),
                              instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
        return true;
    }

    case IROpCode::AddOvf: case IROpCode::SubOvf: case IROpCode::MulOvf: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        uint8_t op_reg = kRAX;
        uint8_t src2_reg = kRCX;
        if (has_graph_coloring_) {
            if (instr.dst_reg() < interpreter::kGPRegisters) {
                uint8_t c = gcr_.gpr_color[instr.dst_reg()];
                if (c != 0xFF) op_reg = c;
            }
            if (instr.has_src2() && instr.src2_reg() < interpreter::kGPRegisters) {
                uint8_t c = gcr_.gpr_color[instr.src2_reg()];
                if (c != 0xFF) src2_reg = c;
            }
        }
        LoadGpr(op_reg, instr.src1_reg());
        if (instr.has_src2()) LoadGpr(src2_reg, instr.src2_reg());
        // Use 32-bit operations so x64 OF flag reflects 32-bit
        // signed overflow, matching IL add.ovf/sub.ovf/mul.ovf semantics.
        if (opc == IROpCode::AddOvf) {
            EmitREX(buf_, false, op_reg, src2_reg);
            buf_.EmitByte(0x03); buf_.EmitByte(ModRM(3, op_reg, src2_reg));
        } else if (opc == IROpCode::SubOvf) {
            EmitREX(buf_, false, op_reg, src2_reg);
            buf_.EmitByte(0x2B); buf_.EmitByte(ModRM(3, op_reg, src2_reg));
        } else {
            EmitREX(buf_, false, op_reg, src2_reg);
            buf_.EmitByte(0x0F); buf_.EmitByte(0xAF); buf_.EmitByte(ModRM(3, op_reg, src2_reg));
        }
        {
            uint32_t jno_pos = buf_.pos();
            EmitJccRel32(buf_, kCC_NO, 0);
            // Find the nearest backward branch target (loop header) for OSR
            // resume on overflow deoptimization.  Scanning backward from the
            // current instruction, the first backward branch found is the
            // nearest back-edge; its target is the loop header to resume at.
            uint32_t osr_pc = 0;
            for (uint32_t si = current_instr_index_; si > 0; --si) {
                uint32_t scan_idx = si - 1;
                const auto& scan_instr = rm_.instructions[scan_idx];
                if (scan_instr.is_branch() &&
                    scan_instr.imm.branch_target < scan_idx) {
                    osr_pc = scan_instr.imm.branch_target;
                    break;
                }
            }
            EmitDeoptSequence(current_instr_index_, osr_pc);
            uint32_t no_overflow = buf_.pos();
            // JccRel32 is 6 bytes: 0F 8x + 4-byte offset at jno_pos+2.
            // Displacement is from end of instruction (jno_pos + 6).
            int32_t disp = static_cast<int32_t>(no_overflow - (jno_pos + 6));
            buf_.Patch32(jno_pos + 2, static_cast<uint32_t>(disp));
        }
        StoreGpr(op_reg, instr.dst_reg());
        return true;
    }

    case IROpCode::And: case IROpCode::Or: case IROpCode::Xor: case IROpCode::Not: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        EmitBitwise(opc, instr.dst_reg(), instr.src1_reg(),
                    instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
        return true;
    }

    case IROpCode::Shl: case IROpCode::Shr: case IROpCode::ShrUn: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        EmitShift(opc, instr.dst_reg(), instr.src1_reg(),
                  instr.has_src2() ? instr.src2_reg() : UINT32_MAX,
                  instr.has_imm() ? instr.imm.i4 : 0);
        return true;
    }

    case IROpCode::Br: {
        uint32_t target = instr.imm.branch_target;
        if (target < current_instr_index_) EmitSafepointPoll();
        uint32_t patch_off = buf_.pos() + 1;
        EmitJmpRel32(buf_, 0);
        branch_patches_.push_back({patch_off, target});
        return true;
    }

    case IROpCode::Beq: case IROpCode::BneUn:
    case IROpCode::Blt: case IROpCode::Bgt:
    case IROpCode::Ble: case IROpCode::Bge:
    case IROpCode::BltUn: case IROpCode::BgtUn:
    case IROpCode::BleUn: case IROpCode::BgeUn: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        uint8_t cmp_a = kRAX, cmp_b = kRCX;
        if (has_graph_coloring_) {
            if (instr.src1_reg() < interpreter::kGPRegisters) {
                uint8_t c = gcr_.gpr_color[instr.src1_reg()];
                if (c != 0xFF) cmp_a = c;
            }
            if (instr.src2_reg() < interpreter::kGPRegisters) {
                uint8_t c = gcr_.gpr_color[instr.src2_reg()];
                if (c != 0xFF) cmp_b = c;
            }
            if (cmp_a == cmp_b) cmp_b = (cmp_a == kRAX) ? kRCX : kRAX;
        }
        LoadGpr(cmp_a, instr.src1_reg()); LoadGpr(cmp_b, instr.src2_reg());
        EmitCmpRR(buf_, cmp_a, cmp_b);
        uint8_t jcc = CmpToJccSigned(instr.op_code());
        uint32_t patch_off = buf_.pos() + 2;
        EmitJccRel32(buf_, jcc, 0);
        branch_patches_.push_back({patch_off, instr.imm.branch_target});
        return true;
    }

    case IROpCode::BrTrue: case IROpCode::BrFalse: {
        if (!instr.has_src1()) return false;
        uint32_t target = instr.imm.branch_target;
        if (target < current_instr_index_) EmitSafepointPoll();
        uint8_t test_reg = kRAX;
        if (has_graph_coloring_ && instr.src1_reg() < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[instr.src1_reg()];
            if (c != 0xFF) test_reg = c;
        }
        LoadGpr(test_reg, instr.src1_reg());
        EmitTestRR(buf_, test_reg, test_reg);
        uint8_t jcc = (instr.op_code() == IROpCode::BrTrue) ? kCC_NE : kCC_E;
        uint32_t patch_off = buf_.pos() + 2;
        EmitJccRel32(buf_, jcc, 0);
        branch_patches_.push_back({patch_off, target});
        return true;
    }

    // ── Switch: jump table (>=4 cases) or linear chain (<4 cases) ────
    case IROpCode::Switch: {
        if (!instr.has_src1()) return false;
        uint32_t target_count = static_cast<uint32_t>((instr.header >> 48) & 0x7FFF);
        auto* targets = static_cast<const uint32_t*>(instr.imm.ptr);
        if (targets == nullptr || target_count == 0) return false;
        LoadGpr(kRAX, instr.src1_reg());

        if (target_count >= 4) {
            // ── Jump table path ────────────────────────────────────────
            //   cmp   rax, target_count-1
            //   jae   default_branch
            //   lea   r10, [rip + table_off]   ; disp = 9 (fixed: past the 9 bytes after LEA)
            //   movsxd rax, [r10 + rax*4]       ; rax = table[value]
            //   add   r10, rax                  ; r10 = table_base + offset = target
            //   jmp   r10
            //   table: .int32 disp_to_target_i

            // Bounds check: if (value >= target_count) goto default
            EmitCmpRI(buf_, kRAX, static_cast<int32_t>(target_count));
            uint32_t default_patch_off = buf_.pos() + 2;
            EmitJccRel32(buf_, kCC_AE, 0);
            branch_patches_.push_back({default_patch_off, targets[target_count]});

            // LEA r10, [rip + 10] — points to table start (10 bytes from LEA end)
            // LEA is 7 bytes. movsxd(4) + add(3) + jmp(3) = 10 bytes after LEA.
            uint32_t lea_pos = buf_.pos();
            EmitLeaRipRel(buf_, kR10, 10);
            // MOVSXD rax, [r10 + rax*4] — load sign-extended table entry
            // REX: W=1, R=0(rax), X=0(rax), B=1(r10) → 0x49
            buf_.EmitByte(0x49);
            buf_.EmitByte(0x63);                     // MOVSXD opcode
            buf_.EmitByte(ModRM(0, kRAX, 4));         // rm=4 = SIB escape
            buf_.EmitByte(SIB(2, kRAX, kR10));        // scale=4, index=rax, base=r10
            // ADD r10, rax
            EmitAddRR(buf_, kR10, kRAX);
            // JMP r10
            EmitJmpReg(buf_, kR10);

            // Emit jump table entries (placeholder values, patched by ResolveBranches)
            uint32_t table_pos = buf_.pos();
            for (uint32_t i = 0; i < target_count; ++i) {
                buf_.Emit32(0);  // placeholder
                jump_table_patches_.push_back({buf_.pos() - 4, table_pos, targets[i]});
            }
        } else {
            // ── Linear chain for small switches (< 4 cases) ──────────
            for (uint32_t i = 0; i < target_count; ++i) {
                EmitCmpRI(buf_, kRAX, static_cast<int32_t>(i));
                uint32_t patch_off = buf_.pos() + 2;
                EmitJccRel32(buf_, kCC_E, 0);
                branch_patches_.push_back({patch_off, targets[i]});
            }
            // No match: jump to default target at targets[target_count]
            uint32_t default_patch_off = buf_.pos() + 1;
            EmitJmpRel32(buf_, 0);
            branch_patches_.push_back({default_patch_off, targets[target_count]});
        }
        return true;
    }

    // ── SEH opcodes ─────────────────────────────────────────────────
    case IROpCode::Leave: {
        // Leave is semantically equivalent to Br in native code:
        // the VEH handler manages exception unwinding if an exception
        // is in flight; the non-exceptional path just branches.
        uint32_t target = instr.imm.branch_target;
        if (target < current_instr_index_) EmitSafepointPoll();
        uint32_t patch_off = buf_.pos() + 1;
        EmitJmpRel32(buf_, 0);
        branch_patches_.push_back({patch_off, target});
        return true;
    }

    case IROpCode::EndFinally:
    case IROpCode::EndFilter: {
        // No-op in native code for V1.  The VEH handler redirects RIP
        // on the exceptional path; non-exceptional fall-through works.
        return true;
    }

    case IROpCode::Throw:
    case IROpCode::Rethrow: {
        // Native throw via VEH: load exception object into RCX and call
        // ChaosT4RaiseException.  The VEH handler walks the SEH clause table
        // embedded in this method's code, finds a matching catch/finally
        // handler, writes the exception object into all GPR register file
        // slots, and redirects RIP to the handler code.
        // If no handler is found, RaiseException returns and we hit INT3.
        if (!instr.has_src1()) return false;  // Should not happen
        LoadGpr(kRCX, instr.src1_reg());  // RCX = exception object
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(ChaosT4RaiseException));
        EmitCallWithSpill(kRAX);
        // VEH handler redirects RIP on success — execution never returns here.
        // INT3 safety net for the case where no handler is found.
        buf_.EmitByte(0xCC);  // INT3
        return true;
    }

    case IROpCode::Ceq: case IROpCode::Clt: case IROpCode::Cgt: {
        if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst()) return false;
        uint8_t cmp_a = kRAX, cmp_b = kRCX;
        if (has_graph_coloring_) {
            if (instr.src1_reg() < interpreter::kGPRegisters) {
                uint8_t c = gcr_.gpr_color[instr.src1_reg()];
                if (c != 0xFF) cmp_a = c;
            }
            if (instr.src2_reg() < interpreter::kGPRegisters) {
                uint8_t c = gcr_.gpr_color[instr.src2_reg()];
                if (c != 0xFF) cmp_b = c;
            }
            if (cmp_a == cmp_b) cmp_b = (cmp_a == kRAX) ? kRCX : kRAX;
        }
        LoadGpr(cmp_a, instr.src1_reg()); LoadGpr(cmp_b, instr.src2_reg());
        // Ceq: 64-bit compare (uint64_t equality, RegisterExecute uses == on uint64_t).
        // Clt/Cgt: 32-bit compare (RegisterExecute casts to int32_t first).
        if (opc == IROpCode::Ceq) {
            EmitCmpRR(buf_, cmp_a, cmp_b);
        } else {
            uint8_t rex_byte = REX(false, cmp_a, 0, cmp_b);
            if (rex_byte != 0x40) buf_.EmitByte(rex_byte);
            buf_.EmitByte(0x3B);
            buf_.EmitByte(ModRM(3, cmp_a, cmp_b));
        }
        uint8_t cc = (opc == IROpCode::Ceq) ? kCC_E : (opc == IROpCode::Clt) ? kCC_L : kCC_G;
        // IMPORTANT: use mov reg, 0 (NOT xor reg, reg) to preserve CMP flags.
        // xor reg,reg sets ZF=1 (result is zero), clobbering the flags sete reads.
        EmitMovRIImm32(buf_, cmp_a, 0);
        EmitSetcc(buf_, cc, cmp_a);
        StoreGpr(cmp_a, instr.dst_reg());
        return true;
    }

    case IROpCode::Conv_I4: case IROpCode::Conv_I8:
    case IROpCode::ConvI: case IROpCode::ConvU:
    case IROpCode::ConvOvfI: case IROpCode::ConvOvfI8: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        // Int32-converting opcodes: truncate to 32-bit via mov eax,eax (zero-extends).
        // Int64 opcodes (Conv_I8, ConvOvfI8): keep full 64-bit value.
        bool is_int32_op = (instr.op_code() != IROpCode::Conv_I8 && instr.op_code() != IROpCode::ConvOvfI8);
        if (is_int32_op) {
            buf_.EmitByte(0x89); buf_.EmitByte(0xC0);  // mov eax, eax (zero-extend 32→64)
        }
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::ConvOvfI4: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        // Check overflow: sign-extend 32-bit truncation back to 64-bit and compare.
        // If original ≠ sign-extend(truncate(original)), the value doesn't fit int32.
        EmitMovRR(buf_, kRCX, kRAX);           // rcx = rax (copy original)
        buf_.EmitByte(0x89); buf_.EmitByte(0xC1);  // mov ecx, eax (truncate to 32-bit)
        EmitMovsxd(buf_, kRCX, kRCX);           // movsxd rcx, ecx (sign-extend 32→64)
        EmitCmpRR(buf_, kRAX, kRCX);            // cmp rax, rcx
        {
            uint32_t jne_pos = buf_.pos();
            EmitJccRel32(buf_, kCC_NE, 0);      // jne → deopt (overflow)
            EmitDeoptSequence(current_instr_index_);
            uint32_t no_overflow = buf_.pos();
            int32_t disp = static_cast<int32_t>(no_overflow - (jne_pos + 6));
            buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(disp));
        }
        // Result: truncated 32-bit value in EAX (zero-extended to 64-bit in RAX)
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::ConvOvfU: case IROpCode::ConvOvfU4: case IROpCode::ConvOvfU8: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        if (instr.op_code() == IROpCode::ConvOvfU4) {
            // Check value fits in uint32: truncate + compare
            EmitMovRR(buf_, kRCX, kRAX);
            buf_.EmitByte(0x89); buf_.EmitByte(0xC1);  // mov ecx, eax (32-bit truncate)
            EmitCmpRR(buf_, kRAX, kRCX);
            uint32_t jne_pos = buf_.pos();
            EmitJccRel32(buf_, kCC_NE, 0);
            EmitDeoptSequence(current_instr_index_);
            uint32_t no_overflow = buf_.pos();
            int32_t disp = static_cast<int32_t>(no_overflow - (jne_pos + 6));
            buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(disp));
        } else {
            // ConvOvfU / ConvOvfU8: check sign bit
            EmitTestRR(buf_, kRAX, kRAX);
            uint32_t js_pos = buf_.pos();
            EmitJccRel32(buf_, kCC_S, 0);
            EmitDeoptSequence(current_instr_index_);
            uint32_t non_neg = buf_.pos();
            int32_t disp = static_cast<int32_t>(non_neg - (js_pos + 6));
            buf_.Patch32(js_pos + 2, static_cast<uint32_t>(disp));
        }
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Conv_R4: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        // int32→float: read GPR[src1], truncate to int32, convert to float.
        // Matches RegisterExecute: float v = static_cast<float>(static_cast<int32_t>(gpr[src1]));
        LoadGpr(kRAX, instr.src1_reg());
        EmitMovsxd(buf_, kRAX, kRAX);           // movsxd rax, eax (sign-extend 32→64)
        EmitCvtsi2ss(buf_, 0, kRAX);            // cvtsi2ss xmm0, rax (int64→float)
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::Conv_R8: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        // int32→double: read GPR[src1], truncate to int32, convert to double.
        // Matches RegisterExecute: double v = static_cast<double>(static_cast<int32_t>(gpr[src1]));
        LoadGpr(kRAX, instr.src1_reg());
        EmitMovsxd(buf_, kRAX, kRAX);           // movsxd rax, eax (sign-extend 32→64)
        EmitCvtsi2sd(buf_, 0, kRAX);            // cvtsi2sd xmm0, rax (int64→double)
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::ConvRUn: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        // uint32 → float64: load uint32 from GPR[src1], zero-extend to 64-bit,
        // convert to double, store to FPR[dst].  Matches interpreter VM:
        //   double result = static_cast<double>(static_cast<uint32_t>(gpr[src1]));
        LoadGpr(kRAX, instr.src1_reg());
        buf_.EmitByte(0x8B); buf_.EmitByte(0xC0);  // mov eax, eax (zero-extend 32→64)
        EmitCvtsi2sd(buf_, 0, kRAX);               // cvtsi2sd xmm0, rax
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::Pop: return true;

    case IROpCode::Break: return true;

    case IROpCode::SizeOf: {
        if (!instr.has_dst()) return false;
        EmitMovRIImm32(buf_, kRAX, static_cast<uint32_t>(instr.imm.i4));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdToken: {
        if (!instr.has_dst()) return false;
        EmitMovRIImm32(buf_, kRAX, static_cast<uint32_t>(instr.imm.i4));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdFtn: {
        if (!instr.has_dst()) return false;
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(instr.imm.ptr));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdStr: {
        if (!instr.has_dst()) return false;
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(instr.imm.ptr));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdFld: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovRIImm32(buf_, kRDX, instr.imm.field_offset);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLdFld));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        {
            uint32_t call_token = 0, call_module = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                call_token = cached.method_token;
                call_module = cached.module_id;
            }
            call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
        }
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::StFld: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovRIImm32(buf_, kRDX, instr.imm.field_offset);
        LoadGpr(kR8, instr.src2_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenStFld));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::NewObj: {
        if (!instr.has_dst()) return false;
        uint32_t type_token = instr.imm.field_offset;
        uint32_t field_count = instr.imm.operand_index;
        if (field_count == 0) field_count = 1;
        // InterpreterObject size (64 bytes: SmallFieldArray 56 + type_token 4 + padding 4)
        static constexpr int32_t kObjSize = static_cast<int32_t>(sizeof(interpreter::InterpreterObject));

        if (kObjSize <= static_cast<int32_t>(kMaxTlabInlineSize)) {
            // ═══ TLAB inline allocation path ═══
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenGetTlab));
            EmitCallWithSpill(kRAX);           // rax = &tls_tlab

            EmitMovRM(buf_, kRCX, kRAX, 8);   // rcx = tls_tlab.current
            EmitLeaRM(buf_, kRBX, kRCX, kObjSize); // rbx = new current
            EmitMovRM(buf_, kRDX, kRAX, 16);  // rdx = tls_tlab.end
            EmitCmpRR(buf_, kRBX, kRDX);
            uint32_t newobj_ja_pos = buf_.pos();
            EmitJccRel32(buf_, kCC_A, 0);     // ja slow_path (patched later)

            // TLAB HIT: bump, zero-init, init struct
            EmitMovMR(buf_, kRAX, 8, kRBX);   // tls_tlab.current = new_ptr

            // Zero-init 64 bytes (4 × movups)
            EmitXorpsRR(buf_, 0, 0);          // xorps xmm0, xmm0
            EmitMovUPSMR(buf_, kRCX, 0, 0);   // [rcx+0]
            EmitMovUPSMR(buf_, kRCX, 16, 0);  // [rcx+16]
            EmitMovUPSMR(buf_, kRCX, 32, 0);  // [rcx+32]
            EmitMovUPSMR(buf_, kRCX, 48, 0);  // [rcx+48]

            // Init SmallFieldArray: fields_ptr_ = &inline_[0] (at offset 24)
            EmitLeaRM(buf_, kRBX, kRCX, 24);
            EmitMovMR(buf_, kRCX, 0, kRBX);   // fields.fields_ptr_ = &inline_[0]
            EmitMovRI32(buf_, kRBX, 2);       // rbx = kInlineCapacity
            EmitMovMR(buf_, kRCX, 16, kRBX);  // fields.field_capacity_ = 2
            // field_count_ at offset 8 is already 0 from zero-init

            // Set type_token at offset 56 (4 bytes, upper 4 zero from zero-init)
            EmitMovRIImm32(buf_, kRBX, type_token);
            EmitMovMR(buf_, kRCX, 56, kRBX);  // obj->type_token = type_token

            StoreGpr(kRCX, instr.dst_reg());  // result = obj pointer

            uint32_t newobj_jmp_done_pos = buf_.pos();
            EmitJmpRel32(buf_, 0);            // skip slow path

            // ═══ Slow path (TLAB miss) ═══
            uint32_t newobj_slow_pos = buf_.pos();
            buf_.Patch32(newobj_ja_pos + 2, newobj_slow_pos - (newobj_ja_pos + 6));

            {
                EmitMovRIImm32(buf_, kRCX, type_token);
                EmitMovRIImm32(buf_, kRDX, field_count);
                EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenNewObj));
                uint32_t call_pos = buf_.pos();
                EmitCallWithSpill(kRAX);
                {
                    uint32_t call_token = 0, call_module = 0;
                    if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                        const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                        call_token = cached.method_token;
                        call_module = cached.module_id;
                    }
                    call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
                }
                RecordGcPoint(call_pos);
                StoreGpr(kRAX, instr.dst_reg());
            }

            uint32_t newobj_done_pos = buf_.pos();
            buf_.Patch32(newobj_jmp_done_pos + 1, newobj_done_pos - (newobj_jmp_done_pos + 5));
        } else {
            // Object too large for TLAB inline — direct GC allocation
            EmitMovRIImm32(buf_, kRCX, type_token);
            EmitMovRIImm32(buf_, kRDX, field_count);
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenNewObj));
            uint32_t call_pos = buf_.pos();
            EmitCallWithSpill(kRAX);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(kRAX, instr.dst_reg());
        }
        return true;
    }

    case IROpCode::Box: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        // BoxedValue = 16 bytes (single InterpreterValue, no heap-allocated fields)
        static constexpr int32_t kBoxSize = static_cast<int32_t>(sizeof(interpreter::BoxedValue));

        // ═══ TLAB inline allocation path ═══
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenGetTlab));
        EmitCallWithSpill(kRAX);           // rax = &tls_tlab

        EmitMovRM(buf_, kRCX, kRAX, 8);   // rcx = tls_tlab.current
        EmitLeaRM(buf_, kRBX, kRCX, kBoxSize); // rbx = new current
        EmitMovRM(buf_, kRDX, kRAX, 16);  // rdx = tls_tlab.end
        EmitCmpRR(buf_, kRBX, kRDX);
        uint32_t box_ja_pos = buf_.pos();
        EmitJccRel32(buf_, kCC_A, 0);     // ja slow_path

        // TLAB HIT: bump, write value into BoxedValue
        EmitMovMR(buf_, kRAX, 8, kRBX);   // tls_tlab.current = new_ptr

        // Load the value to box from src1
        LoadGpr(kR8, instr.src1_reg());   // r8 = value

        // Zero-init 16 bytes then write tag + value
        EmitXorpsRR(buf_, 0, 0);          // xorps xmm0, xmm0
        EmitMovUPSMR(buf_, kRCX, 0, 0);   // [rcx+0..15] = 0

        // Set tag = Int64 at offset 0 (4 bytes, struct_size at +4 = 0)
        EmitMovRIImm32(buf_, kRBX, static_cast<uint32_t>(interpreter::ValueTag::Int64));
        EmitMovMR(buf_, kRCX, 0, kRBX);   // BoxedValue::value.tag = Int64

        // Set value at offset 8 (InterpreterValue union slot)
        EmitMovMR(buf_, kRCX, 8, kR8);    // BoxedValue::value.i64 = value

        StoreGpr(kRCX, instr.dst_reg());  // result = boxed pointer

        uint32_t box_jmp_done_pos = buf_.pos();
        EmitJmpRel32(buf_, 0);            // skip slow path

        // ═══ Slow path (TLAB miss) ═══
        uint32_t box_slow_pos = buf_.pos();
        buf_.Patch32(box_ja_pos + 2, box_slow_pos - (box_ja_pos + 6));

        {
            LoadGpr(kRCX, instr.src1_reg());
            EmitMovRIImm32(buf_, kRDX, static_cast<uint32_t>(interpreter::ValueTag::Int64));
            EmitMovRIImm32(buf_, kR8, instr.imm.field_offset);
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenBox));
            uint32_t call_pos = buf_.pos();
            EmitCallWithSpill(kRAX);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(kRAX, instr.dst_reg());
        }

        uint32_t box_done_pos = buf_.pos();
        buf_.Patch32(box_jmp_done_pos + 1, box_done_pos - (box_jmp_done_pos + 5));
        return true;
    }

    case IROpCode::Unbox: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenUnbox));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        {
            uint32_t call_token = 0, call_module = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                call_token = cached.method_token;
                call_module = cached.module_id;
            }
            call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
        }
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdLen: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLdLen));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::NewArr: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        static constexpr int32_t kArrSize = static_cast<int32_t>(sizeof(interpreter::ArrayStorage));

        // ═══ TLAB inline allocation path ═══
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenGetTlab));
        EmitCallWithSpill(kRAX);           // rax = &tls_tlab

        EmitMovRM(buf_, kRCX, kRAX, 8);   // rcx = tls_tlab.current
        EmitLeaRM(buf_, kRBX, kRCX, kArrSize); // rbx = new current
        EmitMovRM(buf_, kRDX, kRAX, 16);  // rdx = tls_tlab.end
        EmitCmpRR(buf_, kRBX, kRDX);
        uint32_t newarr_ja_pos = buf_.pos();
        EmitJccRel32(buf_, kCC_A, 0);     // ja slow_path

        // TLAB HIT: bump
        EmitMovMR(buf_, kRAX, 8, kRBX);   // tls_tlab.current = new_ptr

        // CodegenNewArrTlab(mem=rcx, length=rdx) — placement new + elements.resize
        LoadGpr(kRDX, instr.src1_reg());  // rdx = length
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenNewArrTlab));
        EmitCallWithSpill(kRAX);
        StoreGpr(kRAX, instr.dst_reg());

        uint32_t newarr_jmp_done_pos = buf_.pos();
        EmitJmpRel32(buf_, 0);            // skip slow path

        // ═══ Slow path (TLAB miss) ═══
        uint32_t newarr_slow_pos = buf_.pos();
        buf_.Patch32(newarr_ja_pos + 2, newarr_slow_pos - (newarr_ja_pos + 6));

        LoadGpr(kRCX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenNewArr));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        {
            uint32_t call_token = 0, call_module = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                call_token = cached.method_token;
                call_module = cached.module_id;
            }
            call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
        }
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());

        uint32_t newarr_done_pos = buf_.pos();
        buf_.Patch32(newarr_jmp_done_pos + 1, newarr_done_pos - (newarr_jmp_done_pos + 5));
        return true;
    }

    case IROpCode::LdElem: case IROpCode::LdElemA: {
        if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        LoadGpr(kRDX, instr.src2_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLdElem));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        {
            uint32_t call_token = 0, call_module = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                call_token = cached.method_token;
                call_module = cached.module_id;
            }
            call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
        }
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::StElem: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        LoadGpr(kRDX, instr.src2_reg());
        if (instr.flags() & interpreter::kRegHasSrc3) LoadGpr(kR8, instr.src3_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenStElem));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::LdSFld: {
        if (!instr.has_dst()) return false;
        EmitMovRIImm32(buf_, kRCX, instr.imm.field_offset);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLdSFld));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        {
            uint32_t call_token = 0, call_module = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                call_token = cached.method_token;
                call_module = cached.module_id;
            }
            call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
        }
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::StSFld: {
        if (!instr.has_src1()) return false;
        EmitMovRIImm32(buf_, kRCX, instr.imm.field_offset);
        LoadGpr(kRDX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenStSFld));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::CastClass: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovRIImm32(buf_, kRDX, instr.imm.field_offset);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenCastClass));
        EmitCallWithSpill(kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::IsInst: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovRIImm32(buf_, kRDX, instr.imm.field_offset);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenIsInst));
        EmitCallWithSpill(kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdVirtFtn: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovRIImm32(buf_, kRDX, instr.imm.field_offset);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLdVirtFtn));
        EmitCallWithSpill(kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::InitObj: {
        if (!instr.has_src1()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenInitObj));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::StObj: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        LoadGpr(kRDX, instr.src2_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenStObj));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::StInd: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        LoadGpr(kRCX, instr.src2_reg());
        EmitMovMR(buf_, kRCX, 0, kRAX);
        return true;
    }

    case IROpCode::LdInd: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovRM(buf_, kRAX, kRCX, 0);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdObj: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLdObj));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        {
            uint32_t call_token = 0, call_module = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                call_token = cached.method_token;
                call_module = cached.module_id;
            }
            call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
        }
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Cpblk: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        LoadGpr(kRDX, instr.src2_reg());
        if (instr.flags() & interpreter::kRegHasSrc3) LoadGpr(kR8, instr.src3_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenCpblk));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::InitBlk: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        LoadGpr(kRDX, instr.src2_reg());
        if (instr.flags() & interpreter::kRegHasSrc3) LoadGpr(kR8, instr.src3_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenInitBlk));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::LocAlloc: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());           // size
        if (localloc_extra_ > 0) {
            // Stack allocation from pre-allocated frame reserve.
            // RDX = base address of localloc reserve.
            EmitLeaRM(buf_, kRDX, kRSP,
                static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + 8));
            // R8 = bump counter pointer (8 bytes, at kFrameSize + align_adj + xmm_save).
            EmitLeaRM(buf_, kR8, kRSP,
                static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_));
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLocAlloc));
        } else {
            // Fallback: heap allocation (no stack reserve — rare edge case).
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLocAlloc));
        }
        EmitCallWithSpill(kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Dup: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdArgA: {
        if (!instr.has_dst()) return false;
        uint32_t arg_idx = instr.imm.operand_index;
        EmitLeaRM(buf_, kRAX, kRBX, static_cast<int32_t>(arg_idx * 8));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdLocA: {
        if (!instr.has_dst()) return false;
        uint32_t local_idx = instr.imm.operand_index;
        uint32_t vreg = kLocalRegBase + local_idx;
        EmitLeaRM(buf_, kRAX, kRSP, static_cast<int32_t>(GprOff(vreg)));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Call: case IROpCode::CallBridge: {
        if (!instr.has_src1()) return false;
        uint32_t first_arg_reg = instr.src1_reg();
        uint32_t arg_count = instr.call_arg_count();
        void* target_fn = instr.imm.ptr;
        if (target_fn == nullptr) return false;
        static constexpr uint8_t kArgRegs[] = {kRCX, kRDX, kR8, kR9};
        uint32_t max_scratch = arg_count < 4 ? arg_count : 4;
        for (uint32_t i = 0; i < max_scratch; ++i) LoadGpr(kArgRegs[i], first_arg_reg + i);
        for (uint32_t i = 4; i < arg_count; ++i) { LoadGpr(kRAX, first_arg_reg + i); EmitMovMR(buf_, kRSP, static_cast<int32_t>((i - 4) * 8), kRAX); }
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(target_fn));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        {
            uint32_t call_token = 0, call_module = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                call_token = cached.method_token;
                call_module = cached.module_id;
            }
            call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
        }
        RecordGcPoint(call_pos);
        if (config_.enable_deopt) {
            uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
            for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                DeoptValue dv; dv.reg_index = vr;
                dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                               ? vreg_types_[vr]
                               : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                dv.is_spilled = true; dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                deopt_values_.push_back(dv);
            }
            for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                DeoptValue dv; dv.reg_index = vr; dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                dv.is_spilled = true; dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                deopt_values_.push_back(dv);
            }
            uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
            DeoptEntry entry; entry.native_offset = call_pos; entry.instr_pc = current_instr_index_; entry.num_values = n_vals; entry.values_offset = val_start;
            deopt_entries_.push_back(entry);
        }
        if (instr.has_dst()) StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Calli: {
        if (!instr.has_src1()) return false;
        uint32_t first_arg_reg = instr.src1_reg();
        uint32_t arg_count = instr.call_arg_count();
        uint32_t func_ptr_vreg = instr.imm.operand_index;

        LoadGpr(kRAX, func_ptr_vreg);
        EmitTestRR(buf_, kRAX, kRAX);
        uint32_t non_null_pos = buf_.pos();
        EmitJccRel32(buf_, kCC_NE, 0);
        // Null function pointer → deopt (fall-through)
        if (config_.enable_deopt) {
            EmitDeoptSequence(current_instr_index_);
        } else {
            return false;
        }
        // Patch jnz to land here (non-null case)
        uint32_t after_deopt = buf_.pos();
        int32_t disp = static_cast<int32_t>(after_deopt - (non_null_pos + 6));
        buf_.Patch32(non_null_pos + 2, static_cast<uint32_t>(disp));

        // Load args into calling convention registers
        static constexpr uint8_t kArgRegs[] = {kRCX, kRDX, kR8, kR9};
        uint32_t max_scratch = arg_count < 4 ? arg_count : 4;
        for (uint32_t i = 0; i < max_scratch; ++i) LoadGpr(kArgRegs[i], first_arg_reg + i);
        for (uint32_t i = 4; i < arg_count; ++i) {
            LoadGpr(kRAX, first_arg_reg + i);
            EmitMovMR(buf_, kRSP, static_cast<int32_t>((i - 4) * 8), kRAX);
        }

        LoadGpr(kRAX, func_ptr_vreg);
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        {
            uint32_t call_token = 0, call_module = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                call_token = cached.method_token;
                call_module = cached.module_id;
            }
            call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
        }
        RecordGcPoint(call_pos);

        if (config_.enable_deopt) {
            uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
            for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                DeoptValue dv; dv.reg_index = vr;
                dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                               ? vreg_types_[vr]
                               : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                dv.is_spilled = true; dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                deopt_values_.push_back(dv);
            }
            for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                DeoptValue dv; dv.reg_index = vr; dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                dv.is_spilled = true; dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                deopt_values_.push_back(dv);
            }
            uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
            DeoptEntry entry; entry.native_offset = call_pos; entry.instr_pc = current_instr_index_;
            entry.num_values = n_vals; entry.values_offset = val_start;
            deopt_entries_.push_back(entry);
        }
        if (instr.has_dst()) StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::CallVirt:
    case IROpCode::CallVirtConstrained: {
        if (!instr.has_src1()) return false;
        uint32_t first_arg_reg = instr.src1_reg();
        uint32_t arg_count = instr.call_arg_count();

        // Build CodegenCallVirtArgs at [rsp + kCallVirtArgsOff]
        EmitLeaRM(buf_, kRCX, kRSP, static_cast<int32_t>(kCallVirtArgsOff));
        EmitLeaRM(buf_, kRAX, kRSP, static_cast<int32_t>(kGprFileOff));
        EmitMovMR(buf_, kRCX, 0, kRAX);    // gpr_base
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.pic_dispatch_data));
        EmitMovMR(buf_, kRCX, 8, kRAX);    // pic_data
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.dispatch_ctx));
        EmitMovMR(buf_, kRCX, 16, kRAX);   // dispatch_ctx
        EmitMovRIImm32(buf_, kRAX, current_instr_index_);
        EmitMovMR(buf_, kRCX, 24, kRAX);   // instruction_idx
        EmitMovRIImm32(buf_, kRAX, arg_count);
        EmitMovMR(buf_, kRCX, 28, kRAX);   // arg_count
        EmitMovRIImm32(buf_, kRAX, first_arg_reg);
        EmitMovMR(buf_, kRCX, 32, kRAX);   // first_arg_reg

        // method_token at offset 36 (from call_cache or 0)
        {
            uint32_t mt = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                mt = cached.method_token;
            }
            EmitMovRIImm32(buf_, kRAX, mt);
            EmitMovMR(buf_, kRCX, 36, kRAX);   // method_token
        }

        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(instr.imm.ptr));
        EmitMovMR(buf_, kRCX, 40, kRAX);   // call_target
        EmitMovRIImm32(buf_, kRAX, instr.has_dst() ? 1 : 0);
        EmitMovMR(buf_, kRCX, 48, kRAX);   // has_dst
        EmitMovRIImm32(buf_, kRAX, static_cast<uint32_t>((instr.header >> 63) & 1));
        EmitMovMR(buf_, kRCX, 52, kRAX);   // is_instance_call
        EmitMovMR(buf_, kRCX, 56, kRSI);   // ret_buf

        // call CodegenCallVirt
        uint32_t call_pos = buf_.pos();
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenCallVirt));
        EmitCallWithSpill(kRAX);
        {
            uint32_t call_token = 0, call_module = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                call_token = cached.method_token;
                call_module = cached.module_id;
            }
            call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
        }
        RecordGcPoint(call_pos);

        // Deoptimization metadata for CallVirt: if CodegenCallVirt takes the
        // PIC miss path, it needs DeoptEntry/DeoptValue to extract type tags
        // for the register file at this call site.
        if (config_.enable_deopt) {
            uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
            for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                DeoptValue dv; dv.reg_index = vr;
                dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                               ? vreg_types_[vr]
                               : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                dv.is_spilled = true; dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                deopt_values_.push_back(dv);
            }
            for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                DeoptValue dv; dv.reg_index = vr; dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                dv.is_spilled = true; dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                deopt_values_.push_back(dv);
            }
            uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
            DeoptEntry entry; entry.native_offset = call_pos; entry.instr_pc = current_instr_index_; entry.num_values = n_vals; entry.values_offset = val_start;
            deopt_entries_.push_back(entry);
        }

        // Check ret_buf[0] for kDeoptMagic
        EmitMovRM(buf_, kRCX, kRSI, 0);
        EmitMovImm64(buf_, kRAX, kDeoptMagic);
        EmitCmpRR(buf_, kRAX, kRCX);
        uint32_t jne_patch_off = buf_.pos() + 2;
        EmitJccRel32(buf_, kCC_NE, 0);

        // Deopt path
        uint32_t deopt_patch_off = buf_.pos() + 1;
        EmitJmpRel32(buf_, 0);
        deopt_jump_patches_.push_back({deopt_patch_off});

        // .normal:
        uint32_t normal_offset = buf_.pos();
        int32_t jne_disp = static_cast<int32_t>(normal_offset - (jne_patch_off + 4));
        buf_.Patch32(jne_patch_off, static_cast<uint32_t>(jne_disp));

        if (instr.has_dst()) StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    default:
        if (config_.enable_deopt) {
            EmitDeoptSequence(current_instr_index_);
            return true;
        }
        return false;
    }
}

// ── OptimizeInstructions ──────────────────────────────────────────────────
// Performs constant folding + dead code elimination on register instructions.
// Two-pass: (1) use-count scan, (2) fold arithmetic + DCE + branch simplify.
// Does not change instruction count — marks removed entries to be skipped
// during emission via the returned removed_mask (empty = no optimizations).

static void OptimizeInstructions(
    CHAOS_IL2CPP_VECTOR(interpreter::RegisterInstruction)& instrs,
    std::vector<uint8_t>& removed_mask) noexcept
{
    uint32_t n = static_cast<uint32_t>(instrs.size());
    if (n == 0) return;

    // ── Pass 1: Use count ────────────────────────────────────────────
    uint32_t use_count[64] = {};
    for (uint32_t i = 0; i < n; ++i) {
        const auto& ri = instrs[i];
        if (ri.has_src1() && ri.src1_reg() < 64) ++use_count[ri.src1_reg()];
        if (ri.has_src2() && ri.src2_reg() < 64) ++use_count[ri.src2_reg()];
        if ((ri.flags() & interpreter::kRegHasSrc3) && ri.src3_reg() < 64) ++use_count[ri.src3_reg()];
    }

    // ── Header builder for folded instructions ───────────────────────
    auto MakeHdr = [](interpreter::IROpCode opc, uint8_t dst, uint8_t src1,
                       uint8_t src2, uint8_t flags) -> uint64_t {
        return static_cast<uint64_t>(opc) |
               (static_cast<uint64_t>(dst)   << 16) |
               (static_cast<uint64_t>(src1)  << 24) |
               (static_cast<uint64_t>(src2)  << 32) |
               (static_cast<uint64_t>(flags) << 40);
    };

    // Foldable pure-arithmetic opcodes (no side effects, no overflow).
    auto IsFoldable = [](interpreter::IROpCode opc) -> bool {
        return opc == interpreter::IROpCode::Add ||
               opc == interpreter::IROpCode::Sub ||
               opc == interpreter::IROpCode::Mul ||
               opc == interpreter::IROpCode::And ||
               opc == interpreter::IROpCode::Or  ||
               opc == interpreter::IROpCode::Xor ||
               opc == interpreter::IROpCode::Shl ||
               opc == interpreter::IROpCode::Shr;
    };

    auto FoldBinary = [](interpreter::IROpCode opc, int32_t a, int32_t b) -> int32_t {
        switch (opc) {
            case interpreter::IROpCode::Add: return a + b;
            case interpreter::IROpCode::Sub: return a - b;
            case interpreter::IROpCode::Mul: return a * b;
            case interpreter::IROpCode::And: return a & b;
            case interpreter::IROpCode::Or:  return a | b;
            case interpreter::IROpCode::Xor: return a ^ b;
            case interpreter::IROpCode::Shl: return static_cast<int32_t>(
                static_cast<uint32_t>(a) << (static_cast<uint32_t>(b) & 0x1F));
            case interpreter::IROpCode::Shr: return a >> (static_cast<uint32_t>(b) & 0x1F);
            default: return 0;
        }
    };

    // Find the most recent definition of `reg` as LdcI4 (scan backwards).
    // Returns nullptr if a backward branch targets an instruction between the def
    // and current_idx — loop back-edges can redefine the register on re-entry,
    // making the "constant" stale.
    auto FindDefLdcI4 = [&](uint32_t current_idx, uint32_t reg) -> const int32_t* {
        for (uint32_t j = current_idx; j > 0; --j) {
            const auto& prev = instrs[j - 1];
            if (prev.has_dst() && prev.dst_reg() == reg) {
                if (prev.op_code() == interpreter::IROpCode::LdcI4) {
                    // Check for backward branches from current_idx onward that
                    // target an instruction between the def (j-1) and current_idx.
                    // Such back-edges skip the def on loop re-entry, making the
                    // "constant" stale.
                    for (uint32_t k = current_idx; k < instrs.size(); ++k) {
                        const auto& bi = instrs[k];
                        auto bopc = bi.op_code();
                        if (bi.has_imm() &&
                            (bopc == interpreter::IROpCode::Br ||
                             bopc == interpreter::IROpCode::BrTrue ||
                             bopc == interpreter::IROpCode::BrFalse ||
                             bopc == interpreter::IROpCode::Beq ||
                             bopc == interpreter::IROpCode::BneUn ||
                             bopc == interpreter::IROpCode::Blt ||
                             bopc == interpreter::IROpCode::BltUn ||
                             bopc == interpreter::IROpCode::Bgt ||
                             bopc == interpreter::IROpCode::BgtUn ||
                             bopc == interpreter::IROpCode::Ble ||
                             bopc == interpreter::IROpCode::BleUn)) {
                            uint32_t target = bi.imm.branch_target;
                            // target < k = backward branch; target between def and
                            // current_idx means loop re-entry skips the LdcI4 def.
                            if (target < k &&
                                target >= j - 1 &&
                                target <= current_idx) {
                                return nullptr; // killed by loop back-edge
                            }
                        }
                    }
                    return &prev.imm.i4;
                }
                return nullptr; // non-LdcI4 def found
            }
        }
        return nullptr;
    };

    // ── Pass 2: Fold + DCE + branch simplify ─────────────────────────
    removed_mask.assign(n, 0);
    bool did_opt = false;

    for (uint32_t i = 0; i < n; ++i) {
        auto& ri = instrs[i];
        auto opc = ri.op_code();

        // (a) Constant folding — binary pure arithmetic with both srcs constant
        if (IsFoldable(opc) && ri.has_dst() && ri.has_src1() && ri.has_src2()) {
            const int32_t* v1 = FindDefLdcI4(i, ri.src1_reg());
            const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
            if (v1 && v2) {
                int32_t result = FoldBinary(opc, *v1, *v2);
                ri.header = MakeHdr(interpreter::IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                    interpreter::kRegHasDst | interpreter::kRegHasImm);
                ri.imm.i4 = result;
                did_opt = true;
                continue;
            }
        }

        // (a2) Constant folding — unary Neg/Not with constant src
        if (opc == interpreter::IROpCode::Neg && ri.has_dst() && ri.has_src1()) {
            const int32_t* v = FindDefLdcI4(i, ri.src1_reg());
            if (v) {
                ri.header = MakeHdr(interpreter::IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                    interpreter::kRegHasDst | interpreter::kRegHasImm);
                ri.imm.i4 = -*v;
                did_opt = true;
                continue;
            }
        }
        if (opc == interpreter::IROpCode::Not && ri.has_dst() && ri.has_src1()) {
            const int32_t* v = FindDefLdcI4(i, ri.src1_reg());
            if (v) {
                ri.header = MakeHdr(interpreter::IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                    interpreter::kRegHasDst | interpreter::kRegHasImm);
                ri.imm.i4 = ~(*v);
                did_opt = true;
                continue;
            }
        }

        // (b) DCE — unused LdcI4
        if (opc == interpreter::IROpCode::LdcI4 && ri.has_dst()) {
            uint8_t dst = ri.dst_reg();
            if (dst < 64 && use_count[dst] == 0) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }
        }

        // (c) BrFalse simplification
        if (opc == interpreter::IROpCode::BrFalse && ri.has_src1()) {
            const int32_t* v = FindDefLdcI4(i, ri.src1_reg());
            if (v) {
                uint32_t target = ri.imm.branch_target;
                if (*v == 0) {
                    // src == 0 → branch IS taken → unconditional Br
                    ri.header = MakeHdr(interpreter::IROpCode::Br, 0, 0, 0,
                                        interpreter::kRegIsBranch | interpreter::kRegHasImm);
                    ri.imm.branch_target = target;
                } else {
                    // src != 0 → branch NOT taken → remove (fall through)
                    removed_mask[i] = 1;
                }
                did_opt = true;
            }
        }

        // (d) Unbox elimination: Box(dst_b, src) + Unbox(dst_u, dst_b) → Dup(dst_u, src)
        if (opc == interpreter::IROpCode::Unbox && ri.has_src1()) {
            uint8_t unbox_src = ri.src1_reg();
            uint8_t unbox_dst = ri.has_dst() ? ri.dst_reg() : 0;
            for (uint32_t j = i; j > 0; --j) {
                auto& prev = instrs[j - 1];
                if (prev.has_dst() && prev.dst_reg() == unbox_src) {
                    if (prev.op_code() == interpreter::IROpCode::Box &&
                        prev.has_src1() && unbox_src < 64 &&
                        use_count[unbox_src] == 1)  // Box dst used only by this Unbox
                    {
                        uint8_t box_src1 = prev.src1_reg();
                        prev.header = MakeHdr(interpreter::IROpCode::Dup, unbox_dst, box_src1, 0,
                                              interpreter::kRegHasDst | interpreter::kRegHasSrc1);
                        prev.imm.i4 = 0;
                        removed_mask[i] = 1;
                        did_opt = true;
                    }
                    break;
                }
            }
        }

        // (e) Dead store elimination: consecutive StLoc to same local
        if (opc == interpreter::IROpCode::StLoc && ri.has_src1() && i > 0) {
            auto& prev = instrs[i - 1];
            if (prev.op_code() == interpreter::IROpCode::StLoc &&
                prev.imm.operand_index == ri.imm.operand_index &&
                !removed_mask[i - 1])
            {
                // Previous StLoc writes to the same local vreg and is immediately
                // overwritten — safe to remove regardless of use_count.
                removed_mask[i - 1] = 1;
                did_opt = true;
            }
        }

        // (e2) Extended dead store: non-adjacent StLoc to same local (EBB-safe scan)
        if (opc == interpreter::IROpCode::StLoc && ri.has_src1() && i > 0) {
            uint32_t local_idx = ri.imm.operand_index;
            const uint32_t kMaxScan = 20;
            for (uint32_t j = i; j > 0 && (i - j) < kMaxScan; --j) {
                uint32_t idx = j - 1;
                if (removed_mask[idx]) continue;
                auto& prev = instrs[idx];
                auto prev_opc = prev.op_code();
                // Stop at branches, calls, or terminators (different execution path)
                if (prev_opc == interpreter::IROpCode::Br ||
                    prev_opc == interpreter::IROpCode::BrTrue ||
                    prev_opc == interpreter::IROpCode::BrFalse ||
                    prev_opc == interpreter::IROpCode::Beq ||
                    prev_opc == interpreter::IROpCode::Blt ||
                    prev_opc == interpreter::IROpCode::Bgt ||
                    prev_opc == interpreter::IROpCode::Ble ||
                    prev_opc == interpreter::IROpCode::Bge ||
                    prev_opc == interpreter::IROpCode::BneUn ||
                    prev_opc == interpreter::IROpCode::BgeUn ||
                    prev_opc == interpreter::IROpCode::BgtUn ||
                    prev_opc == interpreter::IROpCode::BleUn ||
                    prev_opc == interpreter::IROpCode::BltUn ||
                    prev_opc == interpreter::IROpCode::Switch ||
                    prev_opc == interpreter::IROpCode::Call ||
                    prev_opc == interpreter::IROpCode::CallVirt)
                    break;
                if (prev_opc == interpreter::IROpCode::StLoc &&
                    prev.imm.operand_index == local_idx) {
                    removed_mask[idx] = 1;  // previous StLoc is dead
                    did_opt = true;
                    break;
                }
                if (prev_opc == interpreter::IROpCode::LdLoc &&
                    prev.src1_reg() == local_idx) {
                    break;  // LdLoc reads this local — not dead
                }
            }
        }

        // (f) Copy propagation: Dup elimination + forwarding
        if (opc == interpreter::IROpCode::Dup && ri.has_dst() && ri.has_src1()) {
            uint8_t dup_dst = ri.dst_reg();
            uint8_t dup_src = ri.src1_reg();
            // (f1) Dead Dup: dst never read
            if (dup_dst < 64 && use_count[dup_dst] == 0) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }
            // (f2) Single-use: propagate to immediately next instruction
            if (dup_dst < 64 && use_count[dup_dst] == 1 && i + 1 < n) {
                auto& next = instrs[i + 1];
                uint64_t nh = next.header;
                bool changed = false;
                if (next.has_src1() && next.src1_reg() == dup_dst) {
                    nh = (nh & ~(0xFFULL << 24)) | (static_cast<uint64_t>(dup_src) << 24);
                    changed = true;
                }
                if (next.has_src2() && next.src2_reg() == dup_dst) {
                    nh = (nh & ~(0xFFULL << 32)) | (static_cast<uint64_t>(dup_src) << 32);
                    changed = true;
                }
                if (changed) {
                    next.header = nh;
                    removed_mask[i] = 1;
                    did_opt = true;
                }
            }
            // (f3) Multi-hop Dup forwarding: find first non-removed user past i+1
            if (!removed_mask[i] && dup_dst < 64 && use_count[dup_dst] == 1) {
                for (uint32_t k = i + 1; k < n; ++k) {
                    if (removed_mask[k]) continue;
                    auto& later = instrs[k];
                    uint64_t nh = later.header;
                    bool changed = false;
                    if (later.has_src1() && later.src1_reg() == dup_dst) {
                        nh = (nh & ~(0xFFULL << 24)) | (static_cast<uint64_t>(dup_src) << 24);
                        changed = true;
                    }
                    if (later.has_src2() && later.src2_reg() == dup_dst) {
                        nh = (nh & ~(0xFFULL << 32)) | (static_cast<uint64_t>(dup_src) << 32);
                        changed = true;
                    }
                    if (changed) {
                        later.header = nh;
                        removed_mask[i] = 1;
                        did_opt = true;
                    }
                    break;  // only forward to the first non-removed user
                }
            }
        }

        // (g) Redundant LdLoc elimination
        if (opc == interpreter::IROpCode::LdLoc && ri.has_dst() && ri.has_src1()) {
            uint8_t ldst = ri.dst_reg();
            // (g1) Dead LdLoc: dst never read
            if (ldst < 64 && use_count[ldst] == 0) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }
            // (g2) Single-use StLoc→LdLoc forwarding (adjacent)
            if (ldst < 64 && use_count[ldst] == 1 && i > 0 && i + 1 < n) {
                auto& prev = instrs[i - 1];
                uint8_t local_vreg = ri.src1_reg();
                if (prev.op_code() == interpreter::IROpCode::StLoc &&
                    !removed_mask[i - 1] &&
                    prev.has_dst() && prev.dst_reg() == local_vreg &&
                    prev.has_src1())
                {
                    uint8_t store_src = prev.src1_reg();
                    auto& next = instrs[i + 1];
                    uint64_t nh = next.header;
                    bool changed = false;
                    if (next.has_src1() && next.src1_reg() == ldst) {
                        nh = (nh & ~(0xFFULL << 24)) | (static_cast<uint64_t>(store_src) << 24);
                        changed = true;
                    }
                    if (next.has_src2() && next.src2_reg() == ldst) {
                        nh = (nh & ~(0xFFULL << 32)) | (static_cast<uint64_t>(store_src) << 32);
                        changed = true;
                    }
                    if (changed) {
                        next.header = nh;
                        removed_mask[i] = 1;
                        did_opt = true;
                    }
                }
            }
            // (g3) Non-adjacent StLoc→LdLoc forwarding (single-use, EBB-safe)
            if (!removed_mask[i] && ldst < 64 && use_count[ldst] == 1 && i > 0) {
                uint8_t local_vreg = ri.src1_reg();
                const uint32_t kMaxScan = 20;
                // Scan backward for the defining StLoc
                for (uint32_t j = i; j > 0 && (i - j) < kMaxScan; --j) {
                    uint32_t idx = j - 1;
                    if (removed_mask[idx]) continue;
                    auto& prev = instrs[idx];
                    auto prev_opc = prev.op_code();
                    if (prev_opc == interpreter::IROpCode::StLoc &&
                        prev.imm.operand_index == local_vreg &&
                        prev.has_src1())
                    {
                        uint8_t store_src = prev.src1_reg();
                        // Scan forward from LdLoc for the single user
                        for (uint32_t k = i + 1; k < n; ++k) {
                            if (removed_mask[k]) continue;
                            auto& later = instrs[k];
                            uint64_t nh = later.header;
                            bool changed = false;
                            if (later.has_src1() && later.src1_reg() == ldst) {
                                nh = (nh & ~(0xFFULL << 24)) | (static_cast<uint64_t>(store_src) << 24);
                                changed = true;
                            }
                            if (later.has_src2() && later.src2_reg() == ldst) {
                                nh = (nh & ~(0xFFULL << 32)) | (static_cast<uint64_t>(store_src) << 32);
                                changed = true;
                            }
                            if (changed) {
                                later.header = nh;
                                removed_mask[i] = 1;
                                did_opt = true;
                            }
                            break;
                        }
                        break;
                    }
                    // Stop at branches/calls
                    if (prev_opc == interpreter::IROpCode::Br ||
                        prev_opc == interpreter::IROpCode::BrTrue ||
                        prev_opc == interpreter::IROpCode::BrFalse ||
                        prev_opc == interpreter::IROpCode::Beq ||
                        prev_opc == interpreter::IROpCode::Blt ||
                        prev_opc == interpreter::IROpCode::Bgt ||
                        prev_opc == interpreter::IROpCode::Ble ||
                        prev_opc == interpreter::IROpCode::Bge ||
                        prev_opc == interpreter::IROpCode::BneUn ||
                        prev_opc == interpreter::IROpCode::BgeUn ||
                        prev_opc == interpreter::IROpCode::BgtUn ||
                        prev_opc == interpreter::IROpCode::BleUn ||
                        prev_opc == interpreter::IROpCode::BltUn ||
                        prev_opc == interpreter::IROpCode::Switch ||
                        prev_opc == interpreter::IROpCode::Call ||
                        prev_opc == interpreter::IROpCode::CallVirt)
                        break;
                }
            }
        }

        // (h) Branch-to-branch forwarding + dead Br elimination
        if ((opc == interpreter::IROpCode::Br ||
             opc == interpreter::IROpCode::BrFalse ||
             opc == interpreter::IROpCode::BrTrue) && ri.has_imm())
        {
            uint32_t target = ri.imm.branch_target;

            // (h1) Dead unconditional Br: branch to next instruction → fall-through
            if (opc == interpreter::IROpCode::Br && target == i + 1) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }

            // (h2) Br/BrFalse/BrTrue forwarding: resolve Br → Br chains
            uint32_t resolved = target;
            uint32_t max_hop = 16;
            while (resolved < n && max_hop > 0) {
                auto& target_instr = instrs[resolved];
                if (target_instr.op_code() == interpreter::IROpCode::Br &&
                    target_instr.has_imm() && !removed_mask[resolved])
                {
                    uint32_t next_target = target_instr.imm.branch_target;
                    if (next_target == resolved) break;  // self-loop guard
                    resolved = next_target;
                    --max_hop;
                } else {
                    break;
                }
            }

            if (resolved != target) {
                ri.imm.branch_target = resolved;
                did_opt = true;
                // If unconditional Br now points to next instruction, remove it
                if (opc == interpreter::IROpCode::Br && resolved == i + 1) {
                    removed_mask[i] = 1;
                }
            }
        }

        // (i) Arithmetic identity intrinsics: eliminate redundant operations
        if (ri.has_dst() && ri.has_src1()) {
            bool identity_opt = false;

            if (opc == interpreter::IROpCode::Add || opc == interpreter::IROpCode::Sub ||
                opc == interpreter::IROpCode::Or  || opc == interpreter::IROpCode::Xor ||
                opc == interpreter::IROpCode::Shl || opc == interpreter::IROpCode::Shr ||
                opc == interpreter::IROpCode::ShrUn)
            {
                // x op 0 → x (src2 is constant 0)
                if (ri.has_src2()) {
                    const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
                    if (v2 && *v2 == 0) {
                        ri.header = MakeHdr(interpreter::IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                                            interpreter::kRegHasDst | interpreter::kRegHasSrc1);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    }
                }
            }

            if (!identity_opt && opc == interpreter::IROpCode::Mul && ri.has_src2()) {
                const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
                if (v2) {
                    if (*v2 == 1) {
                        // x * 1 → x
                        ri.header = MakeHdr(interpreter::IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                                            interpreter::kRegHasDst | interpreter::kRegHasSrc1);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    } else if (*v2 == 0) {
                        // x * 0 → 0
                        ri.header = MakeHdr(interpreter::IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                            interpreter::kRegHasDst | interpreter::kRegHasImm);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    }
                }
            }

            if (!identity_opt && opc == interpreter::IROpCode::And && ri.has_src2()) {
                if (ri.src1_reg() == ri.src2_reg()) {
                    // x & x → x
                    ri.header = MakeHdr(interpreter::IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                                        interpreter::kRegHasDst | interpreter::kRegHasSrc1);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                } else {
                    // x & 0 → 0 (src2 is constant 0)
                    const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
                    if (v2 && *v2 == 0) {
                        ri.header = MakeHdr(interpreter::IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                            interpreter::kRegHasDst | interpreter::kRegHasImm);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    }
                }
            }

            if (!identity_opt && opc == interpreter::IROpCode::Or && ri.has_src2()) {
                if (ri.src1_reg() == ri.src2_reg()) {
                    // x | x → x
                    ri.header = MakeHdr(interpreter::IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                                        interpreter::kRegHasDst | interpreter::kRegHasSrc1);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                }
            }

            if (!identity_opt && opc == interpreter::IROpCode::Xor && ri.has_src2()) {
                if (ri.src1_reg() == ri.src2_reg()) {
                    // x ^ x → 0
                    ri.header = MakeHdr(interpreter::IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                        interpreter::kRegHasDst | interpreter::kRegHasImm);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                }
            }

            if (!identity_opt && opc == interpreter::IROpCode::Sub) {
                if (ri.has_src2() && ri.src1_reg() == ri.src2_reg()) {
                    // x - x → 0
                    ri.header = MakeHdr(interpreter::IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                        interpreter::kRegHasDst | interpreter::kRegHasImm);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                }
            }

            if (identity_opt) {
                did_opt = true;
                continue;
            }
        }

        // (j) DivUn/RemUn by power of 2 → shift/and
        // NOTE: only apply when use_count[src2_reg] == 1 — the optimization
        // modifies the defining LdcI4's immediate value in-place.  If other
        // instructions also read that LdcI4, they'd see the wrong value.
        if (ri.has_dst() && ri.has_src1() && ri.has_src2()) {
            if (opc == interpreter::IROpCode::DivUn || opc == interpreter::IROpCode::RemUn) {
                uint8_t src2_reg = ri.src2_reg();
                if (src2_reg < 64 && use_count[src2_reg] == 1) {
                    const int32_t* v2 = FindDefLdcI4(i, src2_reg);
                    if (v2 && *v2 > 0) {
                        uint32_t shift = 0;
                        uint32_t uv = static_cast<uint32_t>(*v2);
                        if (uv && (uv & (uv - 1)) == 0) {
                            while ((uv >> shift) > 1) ++shift;
                            if (opc == interpreter::IROpCode::DivUn) {
                                // DivUn by 2^k → ShrUn by k (logical shift, unsigned)
                                ri.header = MakeHdr(interpreter::IROpCode::ShrUn, ri.dst_reg(), ri.src1_reg(), src2_reg,
                                                    interpreter::kRegHasDst | interpreter::kRegHasSrc1 | interpreter::kRegHasSrc2);
                                for (uint32_t j = i; j > 0; --j) {
                                    auto& prev = instrs[j - 1];
                                    if (prev.has_dst() && prev.dst_reg() == src2_reg &&
                                        prev.op_code() == interpreter::IROpCode::LdcI4) {
                                        prev.imm.i4 = static_cast<int32_t>(shift);
                                        break;
                                    }
                                }
                                did_opt = true;
                                continue;
                            } else {
                                // RemUn by 2^k → And with (2^k - 1)
                                uint32_t mask = (1u << shift) - 1u;
                                ri.header = MakeHdr(interpreter::IROpCode::And, ri.dst_reg(), ri.src1_reg(), src2_reg,
                                                    interpreter::kRegHasDst | interpreter::kRegHasSrc1 | interpreter::kRegHasSrc2);
                                for (uint32_t j = i; j > 0; --j) {
                                    auto& prev = instrs[j - 1];
                                    if (prev.has_dst() && prev.dst_reg() == src2_reg &&
                                        prev.op_code() == interpreter::IROpCode::LdcI4) {
                                        prev.imm.i4 = static_cast<int32_t>(mask);
                                        break;
                                    }
                                }
                                did_opt = true;
                                continue;
                            }
                        }
                    }
                }
            }
        }

        // (k) Ceq/Clt/Cgt self-comparison → constant
        if (ri.has_dst() && ri.has_src1() && ri.has_src2() && ri.src1_reg() == ri.src2_reg()) {
            if (opc == interpreter::IROpCode::Ceq) {
                // x == x → 1
                ri.header = MakeHdr(interpreter::IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                    interpreter::kRegHasDst | interpreter::kRegHasImm);
                ri.imm.i4 = 1;
                did_opt = true;
                continue;
            }
            if (opc == interpreter::IROpCode::Clt || opc == interpreter::IROpCode::Cgt) {
                // x < x → 0,  x > x → 0
                ri.header = MakeHdr(interpreter::IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                    interpreter::kRegHasDst | interpreter::kRegHasImm);
                ri.imm.i4 = 0;
                did_opt = true;
                continue;
            }
        }

        // (l) Branch-to-switch conversion: consecutive Beq (same src1 vs different LdcI4) → Switch
        if (opc == interpreter::IROpCode::Beq && ri.has_src1() && ri.has_src2() && !removed_mask[i]) {
            uint8_t switch_reg = ri.src1_reg();
            uint32_t max_cases = (n - i) / 2;  // each case = Beq (+ optional LdcI4)
            if (max_cases > 64) max_cases = 64;

            // Collect cases: (value_k, target_k) pairs
            struct BeqCase { int32_t value; uint32_t target; uint32_t beq_idx; };
            BeqCase cases[64];
            uint32_t case_count = 0;
            bool valid_chain = true;
            uint32_t default_target = n;  // fall-through

            for (uint32_t j = i; j < n && case_count < max_cases; ) {
                if (j > i) {
                    // Skip any LdcI4 that defines the Beq's src2
                    if (instrs[j].op_code() == interpreter::IROpCode::LdcI4 &&
                        instrs[j].has_dst() && !removed_mask[j]) {
                        ++j;
                        continue;
                    }
                }
                auto& cur = instrs[j];
                if (cur.op_code() != interpreter::IROpCode::Beq ||
                    !cur.has_src1() || !cur.has_src2() ||
                    cur.src1_reg() != switch_reg || removed_mask[j])
                {
                    // Check for trailing Br (default target)
                    if (cur.op_code() == interpreter::IROpCode::Br &&
                        cur.has_imm() && case_count >= 3) {
                        default_target = cur.imm.branch_target;
                        removed_mask[j] = 1;  // remove Br, Switch handles default
                        did_opt = true;
                    }
                    if (case_count < 3) valid_chain = false;
                    break;
                }
                // Find the LdcI4 defining this Beq's src2 (scan backward from j)
                uint8_t src2 = cur.src2_reg();
                const int32_t* val = FindDefLdcI4(j, src2);
                if (val == nullptr) { valid_chain = false; break; }
                cases[case_count].value = *val;
                cases[case_count].target = cur.imm.branch_target;
                cases[case_count].beq_idx = j;
                ++case_count;
                ++j;
            }

            if (valid_chain && case_count >= 3) {
                // Build targets array: [case_0, case_1, ..., case_n-1, default]
                static uint32_t s_switch_targets[256];
                uint32_t tc = case_count;
                for (uint32_t c = 0; c < tc; ++c) {
                    s_switch_targets[c] = cases[c].target;
                }
                s_switch_targets[tc] = default_target;

                // Replace first Beq with Switch
                uint64_t switch_header =
                    static_cast<uint64_t>(interpreter::IROpCode::Switch) |
                    (static_cast<uint64_t>(0) << 16) |      // no dst
                    (static_cast<uint64_t>(switch_reg) << 24) |
                    (static_cast<uint64_t>(0) << 32) |      // no src2
                    (static_cast<uint64_t>(interpreter::kRegHasSrc1 |
                                           interpreter::kRegHasImm |
                                           interpreter::kRegIsBranch) << 40) |
                    (static_cast<uint64_t>(tc & 0x7FFF) << 48);
                ri.header = switch_header;
                ri.imm.ptr = s_switch_targets;

                // Mark subsequent Beqs as removed
                for (uint32_t c = 1; c < tc; ++c) {
                    removed_mask[cases[c].beq_idx] = 1;
                }
                did_opt = true;
                continue;
            }
        }
    }

    if (!did_opt) removed_mask.clear();
}

NativeMethod* NativeCodeGenerator::Generate() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::Generate");
    uint32_t n_instrs = static_cast<uint32_t>(rm_.instructions.size());
    if (n_instrs == 0) return nullptr;
    instr_offsets_.resize(n_instrs, 0);

    // Initialize type inference state for all GPR vregs
    vreg_types_.assign(kGprCount, kTypeVoid);

    // Scan for LocAlloc: if used, enlarge frame to include stack reserve.
    // This avoids heap allocation (no leak, freed on method return).
    localloc_extra_ = 0;
    for (const auto& instr : rm_.instructions) {
        if (instr.op_code() == interpreter::IROpCode::LocAlloc) {
            localloc_extra_ = kLocAllocBumpAndReserve;
            break;
        }
    }

    // ── Register allocation: graph coloring (V2) or frequency cache (V1) ──
    if (config_.enable_register_caching) {
        gcr_ = AllocateRegistersGraphColoring(rm_);
        has_graph_coloring_ = false;
        std::memset(x64_to_colored_vreg_, 0xFF, sizeof(x64_to_colored_vreg_));
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            uint8_t x64r = gcr_.gpr_color[vr];
            if (x64r != 0xFF) {
                has_graph_coloring_ = true;
                x64_to_colored_vreg_[x64r] = static_cast<uint8_t>(vr);
            }
        }
        if (has_graph_coloring_) {
            // Print final coloring after filter for diagnostics
            std::printf("    [GC] %u GPR callee colors (%u instrs):", num_cache_regs_, n_instrs);
            for (uint32_t dv = 0; dv < kGprCount; ++dv) {
                if (gcr_.gpr_color[dv] != 0xFF)
                    std::printf(" r%u→x%u", dv, gcr_.gpr_color[dv]);
            }
            std::printf(" | caller_mask=0x%llx filtered_mask=0x%llx\n",
                        (unsigned long long)caller_colored_mask_,
                        (unsigned long long)filtered_vreg_mask_);

            // V3: keep all colored registers, build caller_colored_mask_ for
            // caller-saved regs (R8-R11, colors 1-4).  EmitCallWithSpill
            // reloads these after each runtime helper call.  Only callee-saved
            // regs (RDI=7, R12-R15=12-15) are PUSH/POP'd in prologue/epilogue.
            bool seen[16] = {};
            num_cache_regs_ = 0;
            filtered_vreg_mask_ = 0;
            caller_colored_mask_ = 0;
            for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                uint8_t x64r = gcr_.gpr_color[vr];
                if (x64r != 0xFF) {
                    bool is_callee = (x64r == 7) || (x64r >= 12 && x64r <= 15);
                    if (is_callee) {
                        if (!seen[x64r] && num_cache_regs_ < kMaxCacheRegs) {
                            seen[x64r] = true;
                            callee_x64_regs_[num_cache_regs_++] = x64r;
                        } else {
                            // Another vreg already claimed this callee-saved
                            // register.  The caller-filter already handles
                            // caller-saved duplicates, but graph coloring
                            // can still assign interfering vregs to the same
                            // callee-saved register (select-phase ordering:
                            // neighbor not yet colored when this vreg is
                            // selected).  Clear the color → stack spill.
                            gcr_.gpr_color[vr] = 0xFF;
                        }
                    } else {
                        // Caller-saved register (R8-R11, colors 1-4).
                        // Keep the color assignment.  EmitCallWithSpill
                        // will reload these from stack after each call.
                        caller_colored_mask_ |= (1ULL << vr);
                    }
                }
            }
            cached_slots_used_ = 0;
            cached_dirty_mask_ = 0;
            // Clear V1 cache mappings so spilled vregs fall through to stack
            std::memset(cached_x64_for_vreg_, 0xFF, sizeof(cached_x64_for_vreg_));
            callee_saved_regs_ = callee_x64_regs_;

            // ── FPR callee-saved filter ────────────────────────────────
            // Win64: XMM6-XMM15 (indices 6-15) are callee-saved.
            // XMM0-XMM5 are caller-saved — filter them out (spill to stack).
            num_fpr_callee_ = 0;
            bool xmm_seen[16] = {};
            for (uint32_t fi = 0; fi < 32; ++fi) {
                uint8_t xmm = gcr_.fpr_color[fi];
                if (xmm != 0xFF) {
                    if (xmm >= 6 && xmm <= 15) {  // callee-saved
                        if (!xmm_seen[xmm] && num_fpr_callee_ < 10) {
                            xmm_seen[xmm] = true;
                            callee_xmm_regs_[num_fpr_callee_] = xmm;
                            callee_xmm_fi_[num_fpr_callee_] = static_cast<uint8_t>(fi);
                            ++num_fpr_callee_;
                        } else {
                            gcr_.fpr_color[fi] = 0xFF;  // already claimed → spill
                        }
                    } else {
                        gcr_.fpr_color[fi] = 0xFF;  // caller-saved → spill
                    }
                }
            }
            xmm_save_size_ = static_cast<int32_t>(num_fpr_callee_) * 16;
        } else {
            SelectCacheableRegs();  // V1 fallback
        }
    } else {
        SelectCacheableRegs();
    }
    frame_align_adj_ = (num_cache_regs_ % 2) * 8;  // 16-byte alignment for Win64 ABI

    // Prologue — push callee-saved regs, establish frame pointer
    EmitPush(buf_, kRBP);
    EmitMovRR(buf_, kRBP, kRSP);  // frame pointer chain for GC stack walking
    EmitPush(buf_, kRBX); EmitPush(buf_, kRSI);
    // Push additional callee-saved regs used for register caching
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        EmitPush(buf_, callee_saved_regs_[slot]);
    EmitMovRR(buf_, kRBX, kRCX); EmitMovRR(buf_, kRSI, kRDX);
    EmitSubRI(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_));

    // Save callee-saved XMM registers (used by graph coloring)
    // Stored in the area just below the regular frame (at RSP + kFrameSize + frame_align_adj_).
    for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
        int32_t off = static_cast<int32_t>(kFrameSize + frame_align_adj_ + si * 16);
        EmitMovUPSMR(buf_, kRSP, off, callee_xmm_regs_[si]);
    }

    // Zero-initialize all GPR stack slots — reading an uninitialized slot
    // produces garbage.  Done BEFORE colored-reg zeroing to use RDI as scratch.
    EmitXorZR(buf_, kRAX);
    EmitLeaRM(buf_, kRDI, kRSP, static_cast<int32_t>(kGprFileOff));
    EmitMovRIImm32(buf_, kRCX, kGprCount);
    buf_.EmitByte(0xF3);
    buf_.EmitByte(0x48);
    buf_.EmitByte(0xAB);

    // Zero the localloc bump pointer (RAX is still 0 from xor above)
    if (localloc_extra_ > 0) {
        EmitMovMR(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_), kRAX);
    }

    // Initialize colored callee-saved regs to 0 (re-zeros RDI if colored,
    // since REP STOSQ above left RDI pointing past the GPR file).
    if (has_graph_coloring_) {
        for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
            EmitXorRR(buf_, callee_saved_regs_[slot], callee_saved_regs_[slot]);
    }

    // Zero caller-colored registers (R8-R11) — LoadGpr reads them directly
    // for vregs colored to caller-saved regs, bypassing the zeroed stack.
    // Without this, a read-before-write vreg in R8-R11 returns garbage from
    // the caller, while RegisterExecute and V2 (spill-to-stack) return 0.
    if (caller_colored_mask_) {
        for (uint32_t x64r = kR8; x64r <= kR11; ++x64r)
            EmitXorRR(buf_, x64r, x64r);
    }

    // ── Optimize instructions (constant folding + DCE) ───────────────────
    // Creates a mutable copy of rm_.instructions for the optimizer.
    auto opt_instrs = rm_.instructions;
    std::vector<uint8_t> removed_mask;
    if (config_.enable_optimizer)
        OptimizeInstructions(opt_instrs, removed_mask);

    // Emit instructions
    for (uint32_t i = 0; i < n_instrs; ++i) {
        instr_offsets_[i] = buf_.pos();
        current_instr_index_ = i;
        const auto& instr = opt_instrs[i];
        if (!removed_mask.empty() && removed_mask[i]) continue;
        if (!EmitInstruction(instr)) {
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "GenerateNativeCode: unsupported opcode {} at pc={}, emitting deopt",
                static_cast<int>(instr.op_code()), i);
            if (config_.enable_deopt) {
                EmitDeoptSequence(i);
                continue;
            }
            return nullptr;
        }
        PropagateTypes(instr);
    }

    // deopt_return: shared deoptimization return point + stack frame epilogue.
    // All deopt paths (unsupported opcode, throw/rethrow, CallVirt PIC miss)
    // jump here.  The epilogue restores the stack frame and returns so that
    // InterpreterEntryDirect can check t_deopt_state.deopt_happened.
    deopt_return_pos_ = buf_.pos();
    // Restore callee-saved XMMs before deallocating frame
    for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
        int32_t off = static_cast<int32_t>(kFrameSize + frame_align_adj_ + si * 16);
        EmitMovUPRM(buf_, callee_xmm_regs_[si], kRSP, off);
    }
    EmitAddRI(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_));
    for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
        EmitPop(buf_, callee_saved_regs_[slot - 1]);
    EmitPop(buf_, kRSI); EmitPop(buf_, kRBX); EmitPop(buf_, kRBP);
    EmitRet(buf_);

    // Sentinel entry for instr_offsets_ — SEH clause end indices may point
    // one past the last instruction (exclusive-end convention).
    instr_offsets_.push_back(buf_.pos());

    if (buf_.pos() == 0 || instr_offsets_.empty()) return nullptr;

    // Resolve branches (including deopt jump patches)
    ResolveBranches();

    // ── Emit SEH clause table ──────────────────────────────────────────
    // Appended after code body; VEH handler reads from nm->code + seh_table_offset.
    uint32_t seh_offset = 0;
    if (!rm_.seh_clauses.empty()) {
        seh_offset = buf_.pos();
        uint32_t count = static_cast<uint32_t>(rm_.seh_clauses.size());
        buf_.Emit32(count);
        for (const auto& clause : rm_.seh_clauses) {
            buf_.Emit32(static_cast<uint32_t>(clause.flags));
            buf_.Emit32(instr_offsets_[static_cast<uint32_t>(clause.try_start_idx)]);
            buf_.Emit32(instr_offsets_[static_cast<uint32_t>(clause.try_end_idx)]);
            buf_.Emit32(instr_offsets_[static_cast<uint32_t>(clause.handler_start_idx)]);
            buf_.Emit32(clause.class_token);
        }
    }

    // ── Emit OSR entry stub ─────────────────────────────────────────────
    // Enables true mid-execution OSR: copies the current RegisterFile to
    // the stack frame and jumps to instruction 0. The hot loop then
    // continues in native code from the already-populated register state.
    // Convention: RCX=RegisterFile*, RDX=ret_buf.
    uint32_t osr_entry = 0;
    {
        bool has_loop = false;
        for (uint32_t i = 0; i < n_instrs && !has_loop; ++i) {
            const auto& instr = rm_.instructions[i];
            if (instr.is_branch() && instr.imm.branch_target < i) {
                has_loop = true;
            }
        }
        if (has_loop) {
            osr_entry = buf_.pos();
            EmitPush(buf_, kRBP);
            EmitMovRR(buf_, kRBP, kRSP);  // frame pointer chain
            EmitPush(buf_, kRBX);
            EmitPush(buf_, kRSI);
            // Push cached/colored regs (matches main prologue layout for correct epilogue)
            for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
                EmitPush(buf_, callee_saved_regs_[slot]);
            EmitSubRI(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_));
            // Zero the localloc bump pointer for OSR entry
            if (localloc_extra_ > 0) {
                EmitXorRR(buf_, kRAX, kRAX);
                EmitMovMR(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_), kRAX);
            }
            // using rep movsq (RSI=source, RDI=dest, RCX=count)
            EmitLeaRM(buf_, kRDI, kRSP, static_cast<int32_t>(kGprFileOff));
            EmitMovRR(buf_, kRSI, kRCX);
            EmitMovRIImm32(buf_, kRCX, kGprCount + kFprCount);  // 96
            buf_.EmitByte(0xF3);  // REP prefix
            buf_.EmitByte(0x48);  // REX.W
            buf_.EmitByte(0xA5);  // MOVSQ

            // Set up register convention: RBX=args_buf, RSI=ret_buf
            EmitLeaRM(buf_, kRBX, kRSP, static_cast<int32_t>(kGprFileOff));
            EmitMovRR(buf_, kRSI, kRDX);

            // Initialize cached/colored registers from the stack frame
            if (config_.enable_register_caching) {
                for (uint32_t slot = 0; slot < num_cache_regs_; ++slot) {
                    uint8_t x64_reg = callee_saved_regs_[slot];
                    if (has_graph_coloring_) {
                        uint32_t vreg = x64_to_colored_vreg_[x64_reg];
                        if (vreg != 0xFF) {
                            EmitMovRM(buf_, x64_reg, kRBX, static_cast<int32_t>(vreg * 8));
                        }
                    } else {
                        uint32_t vreg = x64_to_cached_vreg_[x64_reg];
                        if (vreg != kNotCached) {
                            EmitMovRM(buf_, x64_reg, kRBX, static_cast<int32_t>(vreg * 8));
                        }
                    }
                }
                cached_dirty_mask_ = 0;  // clean after initialization
            }

            // Zero caller-colored registers (R8-R11) — matches regular prologue.
            // OSR entry only loads callee-saved regs from the stack (above);
            // caller-colored regs have no stack slot, so without this they
            // contain garbage from the caller, while RegisterExecute returns 0.
            if (caller_colored_mask_) {
                for (uint32_t x64r = kR8; x64r <= kR11; ++x64r)
                    EmitXorRR(buf_, x64r, x64r);
            }

            // Initialize callee-saved XMM registers from RegisterFile FPR copy.
            // Must also save to the XMM save area so the epilogue (Ret/deopt_return)
            // can restore them correctly.
            for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
                uint8_t xmm_reg = callee_xmm_regs_[si];
                uint32_t fi = callee_xmm_fi_[si];
                int32_t fpr_off = static_cast<int32_t>(kFprFileOff + fi * 8);
                EmitMovSDRM(buf_, xmm_reg, kRSP, fpr_off);
                // Duplicate to XMM save area so Ret/deopt_return epilogue can restore
                int32_t save_off = static_cast<int32_t>(kFrameSize + frame_align_adj_ + si * 16);
                EmitMovUPSMR(buf_, kRSP, save_off, xmm_reg);
            }

            // Resolve loop header target and jump there.
            // Since OSR always restarts at the backward branch target
            // (loop header), we call OsrResolveLoopHeader() which reads
            // t_deopt_state.osr_resume_pc and resolves it through the
            // persisted instr_offsets table to an absolute native address.
            EmitSubRI(buf_, kRSP, 32);                // shadow space for Win64
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::OsrResolveLoopHeader));
            EmitCallWithSpill(kRAX);
            EmitAddRI(buf_, kRSP, 32);                // restore shadow space

            // RAX now holds the target native address.
            // Re-zero caller-colored regs (R8-R11) clobbered by the call.
            if (caller_colored_mask_) {
                for (uint32_t x64r = kR8; x64r <= kR11; ++x64r)
                    EmitXorRR(buf_, x64r, x64r);
            }
            EmitJmpReg(buf_, kRAX);
        }
    }

    // Seal code buffer
    uint32_t code_bytes = buf_.pos();
    void* code = buf_.Seal();
    if (code == nullptr) return nullptr;

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "GenerateNativeCode: {} instrs, {} bytes code, {} call sites",
        n_instrs, code_bytes, call_sites_.size());

    // Build NativeMethod
    auto* nm = static_cast<NativeMethod*>(CHAOS_IL2CPP_MALLOC(sizeof(NativeMethod)));
    if (nm == nullptr) return nullptr;
    std::memset(nm, 0, sizeof(*nm));
    ::new (nm) NativeMethod();
    nm->code = code;
    nm->code_size = code_bytes;
    nm->instr_count = n_instrs;
    nm->seh_table_offset = seh_offset;
    nm->osr_entry_offset = osr_entry;

    // Persist instruction offset table for OSR loop header resolution.
    if (!instr_offsets_.empty()) {
        nm->instr_offset_count = static_cast<uint32_t>(instr_offsets_.size());
        nm->instr_offsets = static_cast<uint32_t*>(
            CHAOS_IL2CPP_MALLOC(nm->instr_offset_count * sizeof(uint32_t)));
        if (nm->instr_offsets) {
            std::memcpy(nm->instr_offsets, instr_offsets_.data(),
                        nm->instr_offset_count * sizeof(uint32_t));
        }
    }

    if (!call_sites_.empty()) {
        nm->call_site_count = static_cast<uint32_t>(call_sites_.size());
        nm->call_sites = static_cast<CallSiteInfo*>(CHAOS_IL2CPP_MALLOC(nm->call_site_count * sizeof(CallSiteInfo)));
        if (nm->call_sites) std::memcpy(nm->call_sites, call_sites_.data(), nm->call_site_count * sizeof(CallSiteInfo));
    }
    if (!deopt_entries_.empty()) {
        nm->deopt_entry_count = static_cast<uint32_t>(deopt_entries_.size());
        nm->deopt_entries = static_cast<DeoptEntry*>(CHAOS_IL2CPP_MALLOC(nm->deopt_entry_count * sizeof(DeoptEntry)));
        if (nm->deopt_entries) std::memcpy(nm->deopt_entries, deopt_entries_.data(), nm->deopt_entry_count * sizeof(DeoptEntry));
    }
    if (!deopt_values_.empty()) {
        uint32_t n = static_cast<uint32_t>(deopt_values_.size());
        nm->deopt_values = static_cast<DeoptValue*>(CHAOS_IL2CPP_MALLOC(n * sizeof(DeoptValue)));
        if (nm->deopt_values) std::memcpy(nm->deopt_values, deopt_values_.data(), n * sizeof(DeoptValue));
    }
    if (!gc_points_.empty()) {
        nm->gc_point_count = static_cast<uint32_t>(gc_points_.size());
        nm->gc_points = static_cast<GcPoint*>(CHAOS_IL2CPP_MALLOC(nm->gc_point_count * sizeof(GcPoint)));
        if (nm->gc_points) std::memcpy(nm->gc_points, gc_points_.data(), nm->gc_point_count * sizeof(GcPoint));
    }

    // Serialize GcSlotMapV0
    if (!slot_map_entries_.empty()) {
        uint32_t num_slots = static_cast<uint32_t>(slot_map_entries_.size());
        uint32_t map_size = sizeof(GcSlotMapV0) + num_slots * sizeof(uint32_t);
        auto* map_data = static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(map_size));
        if (map_data) {
            auto* sm = reinterpret_cast<GcSlotMapV0*>(map_data);
            sm->frame_size = static_cast<uint32_t>(kFrameSize + frame_align_adj_);
            sm->num_gc_slots = num_slots;
            std::memcpy(sm->slots, slot_map_entries_.data(), num_slots * sizeof(uint32_t));
            nm->slot_map_data = map_data;
            nm->slot_map_size = map_size;
        }
    }

    nm->rbp_to_rsp_offset = 16 + num_cache_regs_ * 8 +
                            static_cast<uint32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_);

    return nm;
}

NativeMethod* GenerateNativeCode(
    const interpreter::RegisterMethod& rm,
    const CodeGenConfig& config) noexcept {
    if (rm.instructions.empty()) return nullptr;
    NativeCodeGenerator gen(rm, config);
    return gen.Generate();
}

bool CanGenerateNativeCode(const interpreter::RegisterMethod& rm) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (rm.instructions.empty()) return false;
    return true;  // All opcodes accepted — unsupported ones deopt at runtime.
}

NativeMethod::~NativeMethod() noexcept {
    CHAOS_IL2CPP_FREE(call_sites);
    CHAOS_IL2CPP_FREE(deopt_entries);
    CHAOS_IL2CPP_FREE(deopt_values);
    CHAOS_IL2CPP_FREE(gc_points);
    CHAOS_IL2CPP_FREE(slot_map_data);
    CHAOS_IL2CPP_FREE(instr_offsets);
    // Free GcPoint.slots arrays (each allocated independently by RecordGcPoint)
    for (uint32_t i = 0; i < gc_point_count; ++i) {
        CHAOS_IL2CPP_FREE(gc_points[i].slots);
    }
    code = nullptr;
}

NativeMethod::NativeMethod(NativeMethod&& other) noexcept {
    std::memcpy(this, &other, sizeof(*this));
    std::memset(&other, 0, sizeof(other));
}

NativeMethod& NativeMethod::operator=(NativeMethod&& other) noexcept {
    if (this != &other) {
        this->~NativeMethod();
        std::memcpy(this, &other, sizeof(*this));
        std::memset(&other, 0, sizeof(other));
    }
    return *this;
}

}  // namespace chaos::il2cpp::codegen