#include "code_generator.h"
#include "x64_encoder.h"
#include "code_buffer.h"
#include "codegen_helpers.h"

#include "../interpreter/ir_reg_alloc.h"
#include "../interpreter/interpreter_vm.h"

#include <codegen_bridge.h>
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
// Total frame: 864 bytes

static constexpr uint32_t kShadowSize = 32;
static constexpr uint32_t kGprCount   = interpreter::kGPRegisters;  // 64
static constexpr uint32_t kFprCount   = interpreter::kFPRegisters;  // 32
static constexpr uint32_t kGprFileSize  = kGprCount * 8;  // 512 bytes
static constexpr uint32_t kFprFileSize  = kFprCount * 8;  // 256 bytes
static constexpr uint32_t kGprFileOff   = kShadowSize;     // 32
static constexpr uint32_t kFprFileOff   = kGprFileOff + kGprFileSize;  // 544
static constexpr uint32_t kCallVirtArgsOff = kFprFileOff + kFprFileSize; // 800
static constexpr uint32_t kFrameSize    = kCallVirtArgsOff + sizeof(CodegenCallVirtArgs);  // 864 bytes

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

    void SelectCacheableRegs() noexcept;
    void SpillCachedRegs() noexcept;
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
    void EmitDeoptSequence(uint32_t instr_pc) noexcept;
    void DumpCode() noexcept;
    void RecordGcPoint(uint32_t native_offset) noexcept;
};

void NativeCodeGenerator::LoadGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LoadGpr");
    if (config_.enable_register_caching && vreg < interpreter::kGPRegisters) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != kNotCached) {
            // Cache hit: reg-to-reg move (or no-op if same reg)
            if (x64_reg != cached) EmitMovRR(buf_, x64_reg, cached);
            return;
        }
    }
    // Cache miss: load from stack
    EmitMovRM(buf_, x64_reg, kRSP, static_cast<int32_t>(GprOff(vreg)));
}

void NativeCodeGenerator::StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StoreGpr");
    if (config_.enable_register_caching && vreg < interpreter::kGPRegisters) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != kNotCached && num_cache_regs_ > 0) {
            if (cached == x64_reg) {
                // Same reg — no instruction needed, just track dirty
                uint32_t slot = 0;
                for (; slot < kMaxCacheRegs; ++slot) {
                    if (kCacheableX64Regs[slot] == cached) break;
                }
                if (slot < kMaxCacheRegs) cached_dirty_mask_ |= (1u << slot);
                return;
            }
            // Different reg — update cached reg value
            EmitMovRR(buf_, cached, x64_reg);
            uint32_t slot = 0;
            for (; slot < kMaxCacheRegs; ++slot) {
                if (kCacheableX64Regs[slot] == cached) break;
            }
            if (slot < kMaxCacheRegs) cached_dirty_mask_ |= (1u << slot);
            return;
        }
    }
    // Not cached or caching disabled: write through to stack
    EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vreg)), x64_reg);
}

void NativeCodeGenerator::LoadFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    EmitMovSDRM(buf_, xmm_reg, kRSP, static_cast<int32_t>(FprOff(vreg)));
}

void NativeCodeGenerator::StoreFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    EmitMovSDMR(buf_, kRSP, static_cast<int32_t>(FprOff(vreg)), xmm_reg);
}

void NativeCodeGenerator::EmitSafepointPoll() noexcept {
    if (!config_.enable_safepoint_polls || config_.safepoint_fn == nullptr) return;
    // Spill cached regs BEFORE shadow space adjustment (offsets are relative to frame RSP)
    if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();
    EmitSubRI(buf_, kRSP, 32);
    EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.safepoint_fn));
    uint32_t call_pos = buf_.pos();
    EmitCallReg(buf_, kRAX);
    call_sites_.push_back({UINT32_MAX, call_pos});
    RecordGcPoint(call_pos);
    EmitAddRI(buf_, kRSP, 32);
}

void NativeCodeGenerator::RecordGcPoint(uint32_t native_offset) noexcept {
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
    case IROpCode::LdcR4: case IROpCode::Conv_R4: case IROpCode::ConvRUn:
        SetVregType(dst, kTypeFloat32); break;
    case IROpCode::LdcR8: case IROpCode::Conv_R8:
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
    case IROpCode::LdFld:  case IROpCode::LdSFld:
        SetVregType(dst, kTypeObjectRef); break;

    // Call: V1 conservatively marks return as ObjectRef
    case IROpCode::Call:  case IROpCode::CallVirt:
    case IROpCode::CallBridge: case IROpCode::Calli:
        SetVregType(dst, kTypeObjectRef); break;

    // LdArg: conservative — arguments may be objects
    case IROpCode::LdArg:
        SetVregType(dst, kTypeObjectRef); break;

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
    LoadGpr(kRAX, src1);
    if (src2 != UINT32_MAX) LoadGpr(kRCX, src2);
    if (opc == IROpCode::Add) {
        EmitAddRR(buf_, kRAX, kRCX);
    } else if (opc == IROpCode::Sub) {
        EmitSubRR(buf_, kRAX, kRCX);
    } else if (opc == IROpCode::Mul) {
        EmitImulRR(buf_, kRAX, kRCX);
    } else if (opc == IROpCode::Neg) {
        EmitNeg(buf_, kRAX);
    } else if (opc == IROpCode::Div || opc == IROpCode::Rem) {
        EmitREXB(buf_, true, 0); buf_.EmitByte(0x99);
        EmitREX(buf_, true, 7, kRCX); buf_.EmitByte(0xF7); buf_.EmitByte(ModRM(3, 7, kRCX));
        if (opc == IROpCode::Rem) EmitMovRR(buf_, kRAX, kRDX);
    } else if (opc == IROpCode::DivUn || opc == IROpCode::RemUn) {
        EmitXorZR(buf_, kRDX);
        EmitREX(buf_, true, 6, kRCX); buf_.EmitByte(0xF7); buf_.EmitByte(ModRM(3, 6, kRCX));
        if (opc == IROpCode::RemUn) EmitMovRR(buf_, kRAX, kRDX);
    }
    StoreGpr(kRAX, dst);
}

void NativeCodeGenerator::EmitFprArithmetic(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    LoadFpr(0, src1);
    if (src2 != UINT32_MAX && src2 >= kGprCount) LoadFpr(1, src2);
    if (opc == IROpCode::Add) EmitAddSDRR(buf_, 0, 1);
    else if (opc == IROpCode::Sub) EmitSubSDRR(buf_, 0, 1);
    else if (opc == IROpCode::Mul) EmitMulSDRR(buf_, 0, 1);
    else if (opc == IROpCode::Div) EmitDivSDRR(buf_, 0, 1);
    StoreFpr(0, dst);
}

void NativeCodeGenerator::EmitBitwise(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    LoadGpr(kRAX, src1);
    if (opc == IROpCode::Not) EmitNot(buf_, kRAX);
    else if (opc == IROpCode::And) { if (src2 != UINT32_MAX) { LoadGpr(kRCX, src2); EmitAndRR(buf_, kRAX, kRCX); } }
    else if (opc == IROpCode::Or)  { if (src2 != UINT32_MAX) { LoadGpr(kRCX, src2); EmitOrRR(buf_, kRAX, kRCX); } }
    else if (opc == IROpCode::Xor) { if (src2 != UINT32_MAX) { LoadGpr(kRCX, src2); EmitXorRR(buf_, kRAX, kRCX); } }
    StoreGpr(kRAX, dst);
}

void NativeCodeGenerator::EmitShift(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2, int32_t imm) noexcept {
    LoadGpr(kRAX, src1);
    if (src2 != UINT32_MAX) {
        LoadGpr(kRCX, src2);
        if (opc == IROpCode::Shl) EmitShlRCL(buf_, kRAX);
        else if (opc == IROpCode::Shr) EmitShrRCL(buf_, kRAX);
        else if (opc == IROpCode::ShrUn) EmitSarRCL(buf_, kRAX);
    } else {
        uint8_t shift = static_cast<uint8_t>(imm & 0x3F);
        if (opc == IROpCode::Shl) EmitShlRI(buf_, kRAX, shift);
        else if (opc == IROpCode::Shr) EmitShrRI(buf_, kRAX, shift);
        else if (opc == IROpCode::ShrUn) EmitSarRI(buf_, kRAX, shift);
    }
    StoreGpr(kRAX, dst);
}

void NativeCodeGenerator::ResolveBranches() noexcept {
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
}

void NativeCodeGenerator::EmitDeoptSequence(uint32_t instr_pc) noexcept {
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

void NativeCodeGenerator::EmitCallWithSpill(uint8_t reg) noexcept {
    if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();
    EmitCallReg(buf_, reg);
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
        float v; std::memcpy(&v, &instr.imm.i4, sizeof(v));
        uint32_t bits; std::memcpy(&bits, &v, sizeof(bits));
        EmitMovRIImm32(buf_, kRAX, bits); EmitMovdXrm(buf_, 0, kRAX);
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
        EmitAddRI(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_));
        for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
            EmitPop(buf_, kCacheableX64Regs[slot - 1]);
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
        LoadGpr(kRAX, instr.src1_reg());
        if (instr.has_src2()) LoadGpr(kRCX, instr.src2_reg());
        // Use 32-bit operations (eax/ecx) so x64 OF flag reflects 32-bit
        // signed overflow, matching IL add.ovf/sub.ovf/mul.ovf semantics.
        if (opc == IROpCode::AddOvf) {
            buf_.EmitByte(0x03); buf_.EmitByte(0xC1);  // add eax, ecx
        } else if (opc == IROpCode::SubOvf) {
            buf_.EmitByte(0x2B); buf_.EmitByte(0xC1);  // sub eax, ecx
        } else {
            buf_.EmitByte(0x0F); buf_.EmitByte(0xAF); buf_.EmitByte(0xC1);  // imul eax, ecx
        }
        {
            uint32_t jno_pos = buf_.pos();
            EmitJccRel32(buf_, kCC_NO, 0);
            EmitDeoptSequence(current_instr_index_);
            uint32_t no_overflow = buf_.pos();
            // JccRel32 is 6 bytes: 0F 8x + 4-byte offset at jno_pos+2.
            // Displacement is from end of instruction (jno_pos + 6).
            int32_t disp = static_cast<int32_t>(no_overflow - (jno_pos + 6));
            buf_.Patch32(jno_pos + 2, static_cast<uint32_t>(disp));
        }
        StoreGpr(kRAX, instr.dst_reg());
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
        LoadGpr(kRAX, instr.src1_reg()); LoadGpr(kRCX, instr.src2_reg());
        EmitCmpRR(buf_, kRAX, kRCX);
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
        LoadGpr(kRAX, instr.src1_reg());
        EmitTestRR(buf_, kRAX, kRAX);
        uint8_t jcc = (instr.op_code() == IROpCode::BrTrue) ? kCC_NE : kCC_E;
        uint32_t patch_off = buf_.pos() + 2;
        EmitJccRel32(buf_, jcc, 0);
        branch_patches_.push_back({patch_off, target});
        return true;
    }

    // ── Switch: chain-of-branches (V1) ──────────────────────────────
    case IROpCode::Switch: {
        if (!instr.has_src1()) return false;
        uint32_t target_count = static_cast<uint32_t>((instr.header >> 48) & 0x7FFF);
        auto* targets = static_cast<const uint32_t*>(instr.imm.ptr);
        if (targets == nullptr || target_count == 0) return false;
        LoadGpr(kRAX, instr.src1_reg());
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
        // Deoptimize to interpreter for actual exception dispatch.
        // The interpreter's exception handling walks SEH clauses and
        // dispatches to managed catch/finally handlers.
        if (config_.enable_deopt) {
            EmitDeoptSequence(current_instr_index_);
            return true;
        }
        return false;
    }

    case IROpCode::Ceq: case IROpCode::Clt: case IROpCode::Cgt: {
        if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg()); LoadGpr(kRCX, instr.src2_reg());
        EmitCmpRR(buf_, kRAX, kRCX);
        uint8_t cc = (opc == IROpCode::Ceq) ? kCC_E : (opc == IROpCode::Clt) ? kCC_L : kCC_G;
        EmitXorZR(buf_, kRAX); EmitSetcc(buf_, cc, kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Conv_I4: case IROpCode::Conv_I8:
    case IROpCode::ConvI: case IROpCode::ConvU:
    case IROpCode::ConvOvfI: case IROpCode::ConvOvfI4: case IROpCode::ConvOvfI8: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        if (instr.op_code() == IROpCode::Conv_I4 || instr.op_code() == IROpCode::ConvOvfI4) { buf_.EmitByte(0x48); buf_.EmitByte(0x63); buf_.EmitByte(0xC0); }  // movsxd rax, eax (sign-extend int32→int64)
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

    case IROpCode::Conv_R4: case IROpCode::Conv_R8: case IROpCode::ConvRUn: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadFpr(0, instr.src1_reg()); StoreFpr(0, instr.dst_reg());
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
        call_sites_.push_back({current_instr_index_, call_pos});
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
        uint32_t type_token  = instr.imm.field_offset;
        uint32_t field_count = 8;
        EmitMovRIImm32(buf_, kRCX, type_token);
        EmitMovRIImm32(buf_, kRDX, field_count);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenNewObj));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        call_sites_.push_back({current_instr_index_, call_pos});
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Box: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovRIImm32(buf_, kRDX, static_cast<uint32_t>(interpreter::ValueTag::Int64));
        EmitMovRIImm32(buf_, kR8, instr.imm.field_offset);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenBox));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        call_sites_.push_back({current_instr_index_, call_pos});
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Unbox: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenUnbox));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        call_sites_.push_back({current_instr_index_, call_pos});
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
        call_sites_.push_back({current_instr_index_, call_pos});
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::NewArr: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenNewArr));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        call_sites_.push_back({current_instr_index_, call_pos});
        RecordGcPoint(call_pos);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdElem: case IROpCode::LdElemA: {
        if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        LoadGpr(kRDX, instr.src2_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLdElem));
        uint32_t call_pos = buf_.pos();
        EmitCallWithSpill(kRAX);
        call_sites_.push_back({current_instr_index_, call_pos});
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
        call_sites_.push_back({current_instr_index_, call_pos});
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
        call_sites_.push_back({current_instr_index_, call_pos});
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
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(::CodegenLocAlloc));
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
        call_sites_.push_back({current_instr_index_, call_pos});
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
        call_sites_.push_back({current_instr_index_, call_pos});
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
        call_sites_.push_back({current_instr_index_, call_pos});
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

NativeMethod* NativeCodeGenerator::Generate() noexcept {
    uint32_t n_instrs = static_cast<uint32_t>(rm_.instructions.size());
    if (n_instrs == 0) return nullptr;
    instr_offsets_.resize(n_instrs, 0);

    // Initialize type inference state for all GPR vregs
    vreg_types_.assign(kGprCount, kTypeVoid);

    // Select hot vregs for register caching (V1)
    SelectCacheableRegs();
    frame_align_adj_ = (num_cache_regs_ % 2) * 8;  // 16-byte alignment for Win64 ABI

    // Prologue — push callee-saved regs, establish frame pointer
    EmitPush(buf_, kRBP);
    EmitMovRR(buf_, kRBP, kRSP);  // frame pointer chain for GC stack walking
    EmitPush(buf_, kRBX); EmitPush(buf_, kRSI);
    // Push additional callee-saved regs used for register caching
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        EmitPush(buf_, kCacheableX64Regs[slot]);
    EmitMovRR(buf_, kRBX, kRCX); EmitMovRR(buf_, kRSI, kRDX);
    EmitSubRI(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_));

    // Emit instructions
    for (uint32_t i = 0; i < n_instrs; ++i) {
        instr_offsets_[i] = buf_.pos();
        current_instr_index_ = i;
        const auto& instr = rm_.instructions[i];
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
    EmitAddRI(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_));
    for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
        EmitPop(buf_, kCacheableX64Regs[slot - 1]);
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
            // Push cached regs (matches main prologue layout for correct epilogue)
            for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
                EmitPush(buf_, kCacheableX64Regs[slot]);
            EmitSubRI(buf_, kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_));
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

            // Initialize cached registers from the stack frame so LoadGpr hits
            // the correct RegisterFile values instead of stale register data.
            if (config_.enable_register_caching) {
                for (uint32_t slot = 0; slot < num_cache_regs_; ++slot) {
                    uint8_t x64_reg = kCacheableX64Regs[slot];
                    uint32_t vreg = x64_to_cached_vreg_[x64_reg];
                    if (vreg < interpreter::kGPRegisters) {
                        // RBX already points to the GPR file on stack
                        EmitMovRM(buf_, x64_reg, kRBX, static_cast<int32_t>(vreg * 8));
                    }
                }
                cached_dirty_mask_ = 0;  // clean after initialization
            }

            // Jump to instruction 0
            uint32_t jmp_pos = buf_.pos();
            EmitJmpRel32(buf_, 0);
            uint32_t patch_off = jmp_pos + 1;
            uint32_t target_off = instr_offsets_[0];
            int32_t disp = static_cast<int32_t>(target_off - (patch_off + 4));
            buf_.Patch32(patch_off, static_cast<uint32_t>(disp));
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
                            static_cast<uint32_t>(kFrameSize + frame_align_adj_);

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