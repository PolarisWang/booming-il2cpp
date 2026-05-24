#include "jit_engine.h"
#include "x64_encoder.h"
#include "code_buffer.h"
#include "jit_helpers.h"
#include "jit_seh.h"
#include "jit_unwind.h"
#include "slot_map.h"
#include "tree/jit_optimizer.h"
#include "jit_inline.h"        // g_inline_reverse_map
#include "IEncoder.h"
#include "ISehHandler.h"
#include "X64Encoder.h"

#include <gc_root_scanner.h>
#include <gc/gc_bgc.h>

#include "../interpreter/ir_reg_alloc.h"
#include "../interpreter/interpreter_vm.h"
#include "jit_reg_alloc.h"

#include <codegen_bridge.h>
#include <instantiation_engine.h>
#include <chaos/log.h>
#include <chaos/profile.h>

#include <cstdint>
#include <cstring>
#include <vector>

#if defined(_WIN64)
 #include <windows.h>
#endif

namespace chaos::il2cpp::jit {

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

// GcSlotMapV0 slot encoding uses 12 bits for RSP offsets (0-4095).
static_assert(kFrameSize <= 4096,
              "GcSlotMapV0 offset encoding limited to 12 bits");

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

// Global reverse slot map: callee token → (JitMethod*, slot_index).
// Used by the hotpatch callback to update RX slot tables when a method is patched.
ReverseSlotMap g_reverse_slot_map;

// Internal class that drives code generation.
class NativeCodeGenerator {
public:
    NativeCodeGenerator(const interpreter::RegisterMethod& rm,
                        const CompileConfig& config,
                        ISehHandler& seh)
        : rm_(rm), config_(config), encoder_(buf_), enc_(encoder_), seh_(seh) {
        is_tier0_ = (config_.compile_tier == CompileTier::kQuick);
    }

    JitMethod* Generate() noexcept;

private:
    const interpreter::RegisterMethod& rm_;
    CompileConfig config_;
    CodeBuffer buf_;
    X64Encoder encoder_;    // Concrete encoder writing to buf_
    IEncoder& enc_;          // Interface reference to encoder_
    ISehHandler& seh_;

    // When true, use quick JIT path: stack-only, no optimizer, no liveness, no deopt.
    bool is_tier0_ = false;

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

    // Cold-path jump patch records (for Throw/Rethrow).
    // A short JMP rel32 at patch_offset redirects to the cold section
    // appended after the epilogue so hot-path code stays contiguous.
    struct ColdPatch {
        uint32_t patch_offset;  // offset of JMP rel32 displacement field
    };
    std::vector<ColdPatch> cold_patches_;

    // Slot-based call tracking for call-site indirection.
    // Records which call instructions should use slot-based (call [rip+off])
    // emission instead of mov rax, imm64; call rax.
    struct SlotPatch {
        uint32_t patch_offset;       // buffer offset of the disp32 in call [rip+disp32]
        uint32_t call_site_index;    // index in call_sites_ for this call
        void*    target_fn;          // target function pointer to write into slot
    };
    std::vector<SlotPatch> slot_patches_;
    uint32_t slot_count_ = 0;       // total number of slots reserved
    uint32_t slot_count_used_ = 0;  // number of slots actually used (≤ slot_count_)

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

    // ── Precise GC: liveness analysis for slot map filtering ─────────────
    // Per-instruction live-in bitmask (1 << vreg).  Set by Generate() when
    // config_.enable_liveness is true.  Used by RecordGcPoint() to report
    // only ObjectRef vregs that are actually live at each GC point.
    std::vector<uint64_t> live_in_;

    // When true, RecordGcPoint() filters by live_in_ at current_instr_index_.
    // Default false so existing call sites (EmitSafepointPoll, EmitCallWithSpill)
    // continue to report all ObjectRef vregs conservatively.
    bool use_liveness_ = false;

    // Tracks whether liveness was computed in Generate().  Used by
    // RecordGcPoint() to decide whether live_in_ contains valid data.
    bool liveness_computed_ = false;

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

    // ── Prologue tracking (for .pdata/.xdata unwind info) ──────────────────
    // Byte offsets from function entry for each prologue instruction.
    // Set during prologue emission (lines ~2825-2833).
    uint32_t prologue_push_offsets_[8]{};  // Offsets of push rbp/rbx/rsi up to 5 cached regs
    uint32_t prologue_sub_rsp_offset_ = 0; // Offset of sub rsp, K
    uint32_t prologue_set_fpreg_offset_ = 0; // Offset of mov rbp, rsp
    uint32_t prologue_total_bytes_ = 0;    // Total prologue size in bytes
    uint8_t push_reg_nums_[8]{};           // x64 register numbers in push order
    uint32_t num_push_regs_ = 0;           // Number of push regs (3 + num_cache_regs_)
    uint32_t prologue_sub_rsp_size_ = 0;   // K value in sub rsp, K

    // .eh_frame DWARF CFI offset (Linux x64), 0 = not emitted.
    uint32_t eh_frame_offset_ = 0;

    // Error tracking: set by early-exit helpers; causes Generate() to return nullptr.
    bool failed_ = false;

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

    /// Returns true when an OOM or other unrecoverable error has occurred.
    /// Emit helpers check buf_.failed() internally; this is a combined check
    /// so Generate() can bail out early after any major emit section.
    bool CheckFailed() noexcept {
        if (buf_.failed()) failed_ = true;
        return failed_;
    }
};

void NativeCodeGenerator::LoadGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LoadGpr");
    if (vreg >= interpreter::kGPRegisters) { enc_.EmitMovRM(x64_reg, kRSP, static_cast<int32_t>(GprOff(vreg))); return; }

    // Graph coloring V2: colored vreg → direct reg-to-reg move
    if (has_graph_coloring_) {
        uint8_t colored_x64 = gcr_.gpr_color[vreg];
        if (colored_x64 != 0xFF) {
            if (x64_reg != colored_x64) enc_.EmitMovRR(x64_reg, colored_x64);
            return;
        }
    }
    // V1 cache hit
    if (config_.enable_register_caching) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != kNotCached) {
            if (x64_reg != cached) enc_.EmitMovRR(x64_reg, cached);
            return;
        }
    }
    // Load from stack
    enc_.EmitMovRM(x64_reg, kRSP, static_cast<int32_t>(GprOff(vreg)));
}

void NativeCodeGenerator::StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StoreGpr");
    if (vreg >= interpreter::kGPRegisters) { enc_.EmitMovMR(kRSP, static_cast<int32_t>(GprOff(vreg)), x64_reg); return; }

    // Graph coloring V2: colored vreg → direct reg-to-reg move (no stack write)
    if (has_graph_coloring_) {
        uint8_t colored_x64 = gcr_.gpr_color[vreg];
        if (colored_x64 != 0xFF) {
            if (x64_reg != colored_x64) enc_.EmitMovRR(colored_x64, x64_reg);
            // Caller-colored vregs: write through to stack so the stack slot
            // holds the correct value even if argument setup clobbers the
            // colored register before EmitCallWithSpill's pre-call spill.
            if (caller_colored_mask_ & (1ULL << vreg))
                enc_.EmitMovMR(kRSP, static_cast<int32_t>(GprOff(vreg)), colored_x64);
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
            enc_.EmitMovRR(cached, x64_reg);
            uint32_t slot = 0;
            for (; slot < kMaxCacheRegs; ++slot) {
                if (kCacheableX64Regs[slot] == cached) break;
            }
            if (slot < kMaxCacheRegs) cached_dirty_mask_ |= (1u << slot);
            return;
        }
    }
    // Not cached/spilled: write through to stack
    enc_.EmitMovMR(kRSP, static_cast<int32_t>(GprOff(vreg)), x64_reg);
}

void NativeCodeGenerator::LoadFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    // Graph coloring V2: colored FPR → direct reg-to-reg move
    if (has_graph_coloring_ && vreg >= kGprCount) {
        uint32_t fi = vreg - kGprCount;
        if (fi < 32) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (xmm_reg != colored_xmm)
                    enc_.EmitMovSDRR(xmm_reg, colored_xmm);
                return;
            }
        }
    }
    // Fallback: load from stack
    enc_.EmitMovSDRM(xmm_reg, kRSP, static_cast<int32_t>(FprOff(vreg)));
}

void NativeCodeGenerator::StoreFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    // Graph coloring V2: colored FPR → direct reg-to-reg move (no stack write)
    if (has_graph_coloring_ && vreg >= kGprCount) {
        uint32_t fi = vreg - kGprCount;
        if (fi < 32) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (xmm_reg != colored_xmm)
                    enc_.EmitMovSDRR(colored_xmm, xmm_reg);
                return;
            }
        }
    }
    // Fallback: write through to stack
    enc_.EmitMovSDMR(kRSP, static_cast<int32_t>(FprOff(vreg)), xmm_reg);
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
                enc_.EmitMovMR(kRSP, static_cast<int32_t>(GprOff(vr)), colored_x64);
            }
        }
    }
    enc_.EmitSubRI(kRSP, 32);
    enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(config_.safepoint_fn));
    uint32_t call_pos = buf_.pos();
    enc_.EmitCallReg(kRAX);
    call_sites_.push_back({UINT32_MAX, call_pos});
    RecordGcPoint(call_pos);
    enc_.EmitAddRI(kRSP, 32);
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
    if (use_liveness_ && liveness_computed_ &&
        current_instr_index_ < live_in_.size()) {
        live_mask = live_in_[current_instr_index_];
    }

    // Count live ObjectRef vregs at this point
    uint32_t count = 0;
    for (uint32_t vr = 0; vr < kGprCount; ++vr) {
        if (vr < vreg_types_.size() &&
            vreg_types_[vr] == kTypeObjectRef &&
            (live_mask & (1ULL << vr)))
            ++count;
    }
    gp.slot_count = count;
    gp.slots = (count > 0) ? static_cast<GcSlot*>(CHAOS_IL2CPP_MALLOC(count * sizeof(GcSlot))) : nullptr;
    uint32_t idx = 0;
    for (uint32_t vr = 0; vr < kGprCount; ++vr) {
        if (vr < vreg_types_.size() &&
            vreg_types_[vr] == kTypeObjectRef &&
            (live_mask & (1ULL << vr))) {
            uint32_t off = GprOff(vr);
            // Slot offset must fit in 12 bits (GcSlotMapV0 encoding).
            // > 4 KB frames are possible with localloc, but those flush
            // the register file to stack before the safepoint so the
            // fixed vreg slots themselves are always within the first 4 KB.
            if (off > 4095) {
                CHAOS_IL2CPP_LOG_ERROR("CRAG", "gc_slot_offset_overflow");
                continue;
            }
            gp.slots[idx].kind = GcSlotKind::Stack;
            gp.slots[idx].index = off / 8;
            // Also record in slot_map_entries_ for GcSlotMapV0
            slot_map_entries_.push_back(
                CHAOS_GC_SLOT_ENCODE(off, CHAOS_GC_SLOT_KIND_OBJECT));
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

    // LdFld: use field type tags for precision when available
    // LdSFld: conservative ObjectRef (no field_index carried in RegisterInstruction)
    case IROpCode::LdFld:
    {
        if (config_.field_type_tags != nullptr &&
            instr.imm.field_offset < config_.field_type_count) {
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
        SetVregType(dst, kTypeObjectRef); break;

    // Call: use ret_tag from call_cache for precise return type
    case IROpCode::Call:  case IROpCode::CallVirt:
    case IROpCode::CallBridge: case IROpCode::Calli: {
        uint8_t tag = kTypeObjectRef;
        if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
            auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
            if (cached.ret_tag != 0xFF && cached.ret_tag <= kTypeObjectRef)
                tag = cached.ret_tag;
        } else if (config_.method_ret_tags != nullptr &&
                   current_instr_index_ < config_.method_ret_tag_count) {
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
    case IROpCode::Add: case IROpCode::Sub: case IROpCode::Mul:
    case IROpCode::Div: case IROpCode::Rem: case IROpCode::Neg:
    case IROpCode::DivUn: case IROpCode::RemUn:
    case IROpCode::And: case IROpCode::Or: case IROpCode::Xor:
    case IROpCode::Not:
    case IROpCode::Shl: case IROpCode::Shr: case IROpCode::ShrUn:
    case IROpCode::Ceq: case IROpCode::Clt: case IROpCode::Cgt:
    case IROpCode::AddOvf: case IROpCode::SubOvf: case IROpCode::MulOvf:
    case IROpCode::Abs: case IROpCode::Min: case IROpCode::Max:
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
    // When op_reg and src2_reg share the same colored register (dst and src2
    // assigned the same x64 register by graph coloring) and src1 != src2,
    // loading src1 into op_reg destroys src2's value.  Load src2 into a
    // scratch register FIRST, then load src1, so the computation reads the
    // correct src2 value even after op_reg overwrites the shared register.
    bool src2_loaded = false;
    if (has_graph_coloring_ && !has_implicit && op_reg == src2_reg && src1 != src2) {
        src2_reg = (op_reg == kRCX) ? kRAX : kRCX;
        LoadGpr(src2_reg, src2);
        src2_loaded = true;
    }
    LoadGpr(op_reg, src1);
    if (src2 != UINT32_MAX && !src2_loaded) LoadGpr(src2_reg, src2);
    if (opc == IROpCode::Add) {
        enc_.EmitAdd32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Sub) {
        enc_.EmitSub32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Mul) {
        enc_.EmitImul32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Neg) {
        enc_.EmitNeg32(op_reg);
    } else if (opc == IROpCode::Div || opc == IROpCode::Rem) {
        EmitREXB(buf_, false, 0); buf_.EmitByte(0x99);  // cdq: sign-extend eax→edx:eax
        EmitREX(buf_, false, 7, kRCX); buf_.EmitByte(0xF7); buf_.EmitByte(ModRM(3, 7, kRCX));  // idiv ecx
        if (opc == IROpCode::Rem) enc_.EmitMovRR(op_reg, kRDX);
    } else if (opc == IROpCode::DivUn || opc == IROpCode::RemUn) {
        enc_.EmitXor32ZR(kRDX);  // xor edx, edx
        EmitREX(buf_, false, 6, kRCX); buf_.EmitByte(0xF7); buf_.EmitByte(ModRM(3, 6, kRCX));  // div ecx
        if (opc == IROpCode::RemUn) enc_.EmitMovRR(op_reg, kRDX);
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
    if (opc == IROpCode::Add) enc_.EmitAddSDRR(op_xmm, src2_xmm);
    else if (opc == IROpCode::Sub) enc_.EmitSubSDRR(op_xmm, src2_xmm);
    else if (opc == IROpCode::Mul) enc_.EmitMulSDRR(op_xmm, src2_xmm);
    else if (opc == IROpCode::Div) enc_.EmitDivSDRR(op_xmm, src2_xmm);
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
    // Same collision guard as EmitGprArithmetic: when op_reg and src2_reg
    // share a color and src1 != src2, load src2 into a scratch register first.
    bool src2_loaded = false;
    if (has_graph_coloring_ && op_reg == src2_reg && src1 != src2) {
        src2_reg = (op_reg == kRCX) ? kRAX : kRCX;
        LoadGpr(src2_reg, src2);
        src2_loaded = true;
    }
    LoadGpr(op_reg, src1);
    if (opc == IROpCode::Not) enc_.EmitNot32(op_reg);
    else if (opc == IROpCode::And) { if (src2 != UINT32_MAX && !src2_loaded) { LoadGpr(src2_reg, src2); } enc_.EmitAnd32RR(op_reg, src2_reg); }
    else if (opc == IROpCode::Or)  { if (src2 != UINT32_MAX && !src2_loaded) { LoadGpr(src2_reg, src2); } enc_.EmitOr32RR(op_reg, src2_reg); }
    else if (opc == IROpCode::Xor) { if (src2 != UINT32_MAX && !src2_loaded) { LoadGpr(src2_reg, src2); } enc_.EmitXor32RR(op_reg, src2_reg); }
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
        enc_.EmitMovRR(kRCX, kRSP);           // RCX = codegen_rsp
        enc_.EmitSubRI(kRSP, 32);              // shadow space for Win64 callee
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::DeoptSaveFrameState));
        EmitCallWithSpill(kRAX);
        enc_.EmitAddRI(kRSP, 32);              // restore shadow space
    }
    enc_.EmitMovImm64(kRAX, kDeoptMagic);
    enc_.EmitMovMR(kRSI, 0, kRAX);
    uint32_t patch_off = buf_.pos() + 1;
    enc_.EmitJmpRel32(0);
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
            enc_.EmitMovMR(kRSP, static_cast<int32_t>(GprOff(vreg)), x64r);
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
            enc_.EmitMovMR(kRSP, static_cast<int32_t>(GprOff(vreg)), x64r);
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
                enc_.EmitMovMR(kRSP, static_cast<int32_t>(GprOff(vr)), colored_x64);
            }
        }
    }
    enc_.EmitCallReg(reg);
    // Post-call reload: restore caller-saved colored vregs (R8-R11, colors 1-4)
    // that were clobbered by the call.  Only reload vregs that are still alive
    // at this point — the conservative approach reloads all caller-colored
    // vregs from their stack slots (spilled before the call above).
    if (has_graph_coloring_ && caller_colored_mask_) {
        uint64_t mask = caller_colored_mask_;
        for (uint32_t vr = 0; mask; ++vr) {
            if (mask & 1) {
                uint8_t colored_x64 = gcr_.gpr_color[vr];
                enc_.EmitMovRM(colored_x64, kRSP, static_cast<int32_t>(GprOff(vr)));
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
        enc_.EmitMovRIImm32(kRAX, static_cast<uint32_t>(instr.imm.i4));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdcI8: {
        if (!instr.has_dst()) return false;
        int64_t val = instr.imm.i8;
        if (val >= INT32_MIN && val <= INT32_MAX) enc_.EmitMovRI32(kRAX, static_cast<int32_t>(val));
        else enc_.EmitMovImm64(kRAX, static_cast<uint64_t>(val));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdcR4: {
        if (!instr.has_dst()) return false;
        // Load float constant, promote to double (matching RegisterExecute's
        // WriteFloat64(static_cast<double>(v), dst) semantics).
        float v; std::memcpy(&v, &instr.imm.i4, sizeof(v));
        uint32_t bits; std::memcpy(&bits, &v, sizeof(bits));
        enc_.EmitMovRIImm32(kRAX, bits); enc_.EmitMovdXrm(0, kRAX);
        enc_.EmitCvtss2sd(0, 0);  // promote float→double
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::LdcR8: {
        if (!instr.has_dst()) return false;
        uint64_t bits; std::memcpy(&bits, &instr.imm.r8, sizeof(bits));
        enc_.EmitMovImm64(kRAX, bits); enc_.EmitMovqXrm(0, kRAX);
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::LdNull: {
        if (!instr.has_dst()) return false;
        enc_.EmitXorZR(kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdArg: {
        if (!instr.has_dst()) return false;
        uint32_t arg_idx = instr.imm.operand_index;
        enc_.EmitMovRM(kRAX, kRBX, static_cast<int32_t>(arg_idx * 8));
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
        enc_.EmitMovMR(kRBX, static_cast<int32_t>(instr.imm.operand_index * 8), kRAX);
        return true;
    }

    case IROpCode::Ret: {
        // Spill cached regs before reading return value
        if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();
        if (instr.has_src1()) { LoadGpr(kRAX, instr.src1_reg()); enc_.EmitMovMR(kRSI, 0, kRAX); }
        // Restore callee-saved XMMs
        for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
            int32_t off = static_cast<int32_t>(kFrameSize + frame_align_adj_ + si * 16);
            enc_.EmitMovUPRM(callee_xmm_regs_[si], kRSP, off);
        }
        enc_.EmitAddRI(kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_));
        for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
            enc_.EmitPop(callee_saved_regs_[slot - 1]);
        enc_.EmitPop(kRSI); enc_.EmitPop(kRBX); enc_.EmitPop(kRBP); enc_.EmitRet();
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

    // ── Intrinsic: Abs / Min / Max (int32) ─────────────────────────
    case IROpCode::Abs: {
        // int32 abs: cdq (sign-extend eax→edx:eax), xor eax, edx, sub eax, edx
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        buf_.EmitByte(0x99);                          // cdq
        enc_.EmitXor32RR(kRAX, kRDX);                 // eax ^= edx
        enc_.EmitSub32RR(kRAX, kRDX);                 // eax -= edx
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Min: {
        // int32 min: dst = (src1 > src2) ? src2 : src1 → cmovg
        if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        LoadGpr(kRCX, instr.src2_reg());
        enc_.EmitCmpRR(kRAX, kRCX);
        enc_.EmitCmovcc(kCC_G, kRAX, kRCX);          // if a > b, a = b
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Max: {
        // int32 max: dst = (src1 < src2) ? src2 : src1 → cmovl
        if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        LoadGpr(kRCX, instr.src2_reg());
        enc_.EmitCmpRR(kRAX, kRCX);
        enc_.EmitCmovcc(kCC_L, kRAX, kRCX);          // if a < b, a = b
        StoreGpr(kRAX, instr.dst_reg());
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
            enc_.EmitJccRel32(kCC_NO, 0);
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
        enc_.EmitJmpRel32(0);
        branch_patches_.push_back({patch_off, target});
        return true;
    }

    case IROpCode::Beq: case IROpCode::BneUn: {
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
        enc_.EmitCmpRR(cmp_a, cmp_b);
        uint8_t jcc = (instr.op_code() == IROpCode::Beq) ? kCC_E : kCC_NE;
        uint32_t patch_off = buf_.pos() + 2;
        enc_.EmitJccRel32(jcc, 0);
        branch_patches_.push_back({patch_off, instr.imm.branch_target});
        return true;
    }

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
        enc_.EmitCmp32RR(cmp_a, cmp_b);
        uint8_t jcc = CmpToJccSigned(instr.op_code());
        uint32_t patch_off = buf_.pos() + 2;
        enc_.EmitJccRel32(jcc, 0);
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
        enc_.EmitTestRR(test_reg, test_reg);
        uint8_t jcc = (instr.op_code() == IROpCode::BrTrue) ? kCC_NE : kCC_E;
        uint32_t patch_off = buf_.pos() + 2;
        enc_.EmitJccRel32(jcc, 0);
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
            enc_.EmitCmpRI(kRAX, static_cast<int32_t>(target_count));
            uint32_t default_patch_off = buf_.pos() + 2;
            enc_.EmitJccRel32(kCC_AE, 0);
            branch_patches_.push_back({default_patch_off, targets[target_count]});

            // LEA r10, [rip + 10] — points to table start (10 bytes from LEA end)
            // LEA is 7 bytes. movsxd(4) + add(3) + jmp(3) = 10 bytes after LEA.
            uint32_t lea_pos = buf_.pos();
            enc_.EmitLeaRipRel(kR10, 10);
            // MOVSXD rax, [r10 + rax*4] — load sign-extended table entry
            // REX: W=1, R=0(rax), X=0(rax), B=1(r10) → 0x49
            buf_.EmitByte(0x49);
            buf_.EmitByte(0x63);                     // MOVSXD opcode
            buf_.EmitByte(ModRM(0, kRAX, 4));         // rm=4 = SIB escape
            buf_.EmitByte(SIB(2, kRAX, kR10));        // scale=4, index=rax, base=r10
            // ADD r10, rax
            enc_.EmitAddRR(kR10, kRAX);
            // JMP r10
            enc_.EmitJmpReg(kR10);

            // Emit jump table entries (placeholder values, patched by ResolveBranches)
            uint32_t table_pos = buf_.pos();
            for (uint32_t i = 0; i < target_count; ++i) {
                buf_.Emit32(0);  // placeholder
                jump_table_patches_.push_back({buf_.pos() - 4, table_pos, targets[i]});
            }
        } else {
            // ── Linear chain for small switches (< 4 cases) ──────────
            for (uint32_t i = 0; i < target_count; ++i) {
                enc_.EmitCmpRI(kRAX, static_cast<int32_t>(i));
                uint32_t patch_off = buf_.pos() + 2;
                enc_.EmitJccRel32(kCC_E, 0);
                branch_patches_.push_back({patch_off, targets[i]});
            }
            // No match: jump to default target at targets[target_count]
            uint32_t default_patch_off = buf_.pos() + 1;
            enc_.EmitJmpRel32(0);
            branch_patches_.push_back({default_patch_off, targets[target_count]});
        }
        return true;
    }

    // ── SEH opcodes ─────────────────────────────────────────────────
    case IROpCode::Leave: {
        uint32_t target = instr.imm.branch_target;

        // SEH V3: Check if any finally/fault clause covers the current
        // instruction.  If so, emit JitLeaveHelper call to set up pending_leave
        // and redirect to the innermost finally handler.
        bool has_finally_covering = false;
        for (const auto& clause : rm_.seh_clauses) {
            uint32_t flags = static_cast<uint32_t>(clause.flags);
            uint32_t try_start = static_cast<uint32_t>(clause.try_start_idx);
            uint32_t try_end = static_cast<uint32_t>(clause.try_end_idx);
            if ((flags == 2 || flags == 4) &&
                current_instr_index_ >= try_start &&
                current_instr_index_ < try_end) {
                has_finally_covering = true;
                break;
            }
        }

        if (has_finally_covering) {
            // Leave crossing a finally/fault boundary: call JitLeaveHelper
            // to resolve byte offsets at runtime and find the innermost handler.
            enc_.EmitMovRIImm32(kRCX, target);              // arg1: target_instr_idx
            enc_.EmitMovRIImm32(kRDX, current_instr_index_); // arg2: current_instr_idx
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(JitLeaveHelper));
            EmitCallWithSpill(kRAX);                          // RAX = handler addr or 0
            enc_.EmitTestRR(kRAX, kRAX);                     // test rax, rax
            uint32_t jz_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_E, 0);                     // jz .normal_jmp
            enc_.EmitJmpReg(kRAX);                            // jmp rax → handler
            // .normal_jmp: fall through to normal leave JMP
            uint32_t normal_pos = buf_.pos();
            int32_t jz_disp = static_cast<int32_t>(normal_pos - (jz_pos + 6));
            buf_.Patch32(jz_pos + 2, static_cast<uint32_t>(jz_disp));
            // Normal JMP to leave target (JitLeaveHelper returned 0 or
            // finally chain completed and returned to leave path).
            uint32_t patch_off = buf_.pos() + 1;
            enc_.EmitJmpRel32(0);
            branch_patches_.push_back({patch_off, target});
        } else {
            // No finally/fault covering — normal JMP (current behavior).
            if (target < current_instr_index_) EmitSafepointPoll();
            uint32_t patch_off = buf_.pos() + 1;
            enc_.EmitJmpRel32(0);
            branch_patches_.push_back({patch_off, target});
        }
        return true;
    }

    case IROpCode::EndFinally: {
        // SEH V3: call JitEndFinallyHelper to advance the finally/fault
        // unwind chain.  Returns next handler address (non-zero) or 0
        // (continue normally).
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(JitEndFinallyHelper));
        EmitCallWithSpill(kRAX);
        enc_.EmitTestRR(kRAX, kRAX);
        uint32_t jz_pos = buf_.pos();
        enc_.EmitJccRel32(kCC_E, 0);     // jz .continue
        enc_.EmitJmpReg(kRAX);            // jmp rax → next handler/leave target
        // .continue: normal fall-through
        uint32_t continue_pos = buf_.pos();
        int32_t jz_disp = static_cast<int32_t>(continue_pos - (jz_pos + 6));
        buf_.Patch32(jz_pos + 2, static_cast<uint32_t>(jz_disp));
        return true;
    }

    case IROpCode::EndFilter: {
        // EndFilter: pop the filter result from src1_reg and decide.
        //   non-zero (accept) → fall through to the handler (which is emitted
        //                       sequentially right after the filter)
        //   zero    (reject)  → call JitEndFinallyHelper to continue exception
        //                       search outward; jmp rax to the next handler
        //
        // Per the SEH layout convention used by code_generator.cpp:
        // for filter clauses, handler_start_offset stores the filter function
        // offset, and the actual handler follows the EndFilter sequentially.
        // The fall-through path therefore lands on the handler.
        if (instr.has_src1()) {
            LoadGpr(kRAX, instr.src1_reg());      // EAX = filter result
        } else {
            enc_.EmitMovImm64(kRAX, 0);           // defensive: treat as reject
        }
        enc_.EmitTestRR(kRAX, kRAX);
        // jne .accept  (filter result != 0 → fall through to handler)
        uint32_t jne_pos = buf_.pos();
        enc_.EmitJccRel32(kCC_NE, 0);
        // Reject path: dispatch via JitEndFinallyHelper.
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(JitEndFinallyHelper));
        EmitCallWithSpill(kRAX);
        enc_.EmitTestRR(kRAX, kRAX);
        uint32_t jz_pos = buf_.pos();
        enc_.EmitJccRel32(kCC_E, 0);              // jz .accept (no further handler)
        enc_.EmitJmpReg(kRAX);                    // jmp to next handler
        // .accept: fall through to the handler block (sequentially after EndFilter).
        uint32_t accept_pos = buf_.pos();
        int32_t jne_disp = static_cast<int32_t>(accept_pos - (jne_pos + 6));
        buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(jne_disp));
        int32_t jz_disp = static_cast<int32_t>(accept_pos - (jz_pos + 6));
        buf_.Patch32(jz_pos + 2, static_cast<uint32_t>(jz_disp));
        return true;
    }

    case IROpCode::Throw:
    case IROpCode::Rethrow: {
        // Deoptimize when register allocation is inconsistent
        // (should not happen — the interpreter verifier ensures correct
        //  src1 count for Throw/Rethrow; this is a safety check).
        if (!instr.has_src1()) return false;
        LoadGpr(kRCX, instr.src1_reg());  // RCX = exception object
        // JMP to cold section (patched after epilogue emission).
        // Cold section contains ChaosJitRaiseException call + INT3 safety net.
        // This keeps hot-path code contiguous for better icache behavior.
        uint32_t jmp_off = buf_.pos();
        enc_.EmitJmpRel32(0);
        cold_patches_.push_back({jmp_off + 1});
        return true;
    }

    case IROpCode::Ceq: case IROpCode::Clt: case IROpCode::Cgt: {
        if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst()) return false;
        // Use fixed scratch registers RAX/RCX, NOT graph-colored registers,
        // because the setcc sequence (MovRIImm32 + Setcc) destroys the
        // register used as cmp_a.  Using src1's colored register would
        // silently corrupt src1's value for any subsequent read of src1
        // through LoadGpr (which returns the colored register, not the stack).
        uint8_t cmp_a = kRAX, cmp_b = kRCX;
        LoadGpr(cmp_a, instr.src1_reg()); LoadGpr(cmp_b, instr.src2_reg());
        // Ceq: 64-bit compare (uint64_t equality, RegisterExecute uses == on uint64_t).
        // Clt/Cgt: 32-bit compare (RegisterExecute casts to int32_t first).
        if (opc == IROpCode::Ceq) {
            enc_.EmitCmpRR(cmp_a, cmp_b);
        } else {
            enc_.EmitCmp32RR(cmp_a, cmp_b);
        }
        uint8_t cc = (opc == IROpCode::Ceq) ? kCC_E : (opc == IROpCode::Clt) ? kCC_L : kCC_G;
        // IMPORTANT: use mov reg, 0 (NOT xor reg, reg) to preserve CMP flags.
        // xor reg,reg sets ZF=1 (result is zero), clobbering the flags sete reads.
        enc_.EmitMovRIImm32(cmp_a, 0);
        enc_.EmitSetcc(cc, cmp_a);
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
        enc_.EmitMovRR(kRCX, kRAX);           // rcx = rax (copy original)
        buf_.EmitByte(0x89); buf_.EmitByte(0xC1);  // mov ecx, eax (truncate to 32-bit)
        enc_.EmitMovsxd(kRCX, kRCX);           // movsxd rcx, ecx (sign-extend 32→64)
        enc_.EmitCmpRR(kRAX, kRCX);            // cmp rax, rcx
        {
            uint32_t jne_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_NE, 0);      // jne → deopt (overflow)
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
            enc_.EmitMovRR(kRCX, kRAX);
            buf_.EmitByte(0x89); buf_.EmitByte(0xC1);  // mov ecx, eax (32-bit truncate)
            enc_.EmitCmpRR(kRAX, kRCX);
            uint32_t jne_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_NE, 0);
            EmitDeoptSequence(current_instr_index_);
            uint32_t no_overflow = buf_.pos();
            int32_t disp = static_cast<int32_t>(no_overflow - (jne_pos + 6));
            buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(disp));
        } else {
            // ConvOvfU / ConvOvfU8: check sign bit
            enc_.EmitTestRR(kRAX, kRAX);
            uint32_t js_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_S, 0);
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
        enc_.EmitMovsxd(kRAX, kRAX);           // movsxd rax, eax (sign-extend 32→64)
        enc_.EmitCvtsi2ss(0, kRAX);            // cvtsi2ss xmm0, rax (int64→float)
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::Conv_R8: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        // int32→double: read GPR[src1], truncate to int32, convert to double.
        // Matches RegisterExecute: double v = static_cast<double>(static_cast<int32_t>(gpr[src1]));
        LoadGpr(kRAX, instr.src1_reg());
        enc_.EmitMovsxd(kRAX, kRAX);           // movsxd rax, eax (sign-extend 32→64)
        enc_.EmitCvtsi2sd(0, kRAX);            // cvtsi2sd xmm0, rax (int64→double)
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
        enc_.EmitCvtsi2sd(0, kRAX);               // cvtsi2sd xmm0, rax
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::Pop: return true;

    case IROpCode::Break: return true;

    case IROpCode::SizeOf: {
        if (!instr.has_dst()) return false;
        enc_.EmitMovRIImm32(kRAX, static_cast<uint32_t>(instr.imm.i4));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdToken: {
        if (!instr.has_dst()) return false;
        enc_.EmitMovRIImm32(kRAX, static_cast<uint32_t>(instr.imm.i4));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdFtn: {
        if (!instr.has_dst()) return false;
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(instr.imm.ptr));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdStr: {
        if (!instr.has_dst()) return false;
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(instr.imm.ptr));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdFld: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenLdFld));
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
        // Simple path -- always call CodegenStFld (full SATB barrier).
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
        LoadGpr(kR8, instr.src2_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStFld));
        EmitCallWithSpill(kRAX);
        return true;;
    }

    case IROpCode::StFldBarrier: {
        // Inline g_bgc_is_marking check + conditional barrier.
        // Fast path (not marking): call CodegenStFldNoBarrier (store + card mark).
        // Slow path (marking): call CodegenStFld (full SATB pre-write barrier).
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRCX, instr.src1_reg());           // obj
        enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);  // field_idx
        LoadGpr(kR8, instr.src2_reg());            // value
        // Inline g_bgc_is_marking check (preserves RCX/RDX/R8)
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(&chaos::il2cpp::runtime_core::g_bgc_is_marking));
        buf_.EmitByte(0x80);                          // cmp r/m8, imm8
        buf_.EmitByte(0x38);                          // ModRM: mod=00, reg=7, rm=0 → [rax]
        buf_.EmitByte(0x00);                          // imm8 = 0
        uint32_t marking_jmp_pos = buf_.pos();
        enc_.EmitJccRel32(kCC_NE, 0);              // jne .marking
        // Fast path: not marking → CodegenStFldNoBarrier
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStFldNoBarrier));
        EmitCallWithSpill(kRAX);
        uint32_t done_jmp_pos = buf_.pos();
        enc_.EmitJmpRel32(0);                       // jmp .done
        // Slow path: marking → CodegenStFld (full SATB barrier)
        uint32_t marking_pos = buf_.pos();
        buf_.Patch32(marking_jmp_pos + 2, marking_pos - (marking_jmp_pos + 6));
        // Reload args (spilled/colored regs may differ in slow path)
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
        LoadGpr(kR8, instr.src2_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStFld));
        EmitCallWithSpill(kRAX);
        uint32_t done_pos = buf_.pos();
        buf_.Patch32(done_jmp_pos + 1, done_pos - (done_jmp_pos + 5));
        return true;
    }

    case IROpCode::NewObj: {
        if (!instr.has_dst()) return false;
        uint32_t type_token = instr.imm.field_offset;
        uint32_t field_count = instr.imm.operand_index;
        if (field_count == 0) field_count = 1;
        // InterpreterObject size (64 bytes: SmallFieldArray 56 + type_token 4 + padding 4)
        static constexpr int32_t kObjSize = static_cast<int32_t>(sizeof(interpreter::InterpreterObject));

        if (false) {
            // ═══ TLAB inline allocation path ═══
            if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();
            EmitLoadTlsTlab(buf_);           // rax = &tls_tlab

            enc_.EmitMovRM(kRCX, kRAX, 8);   // rcx = tls_tlab.current
            enc_.EmitLeaRM(kRBX, kRCX, kObjSize); // rbx = new current
            enc_.EmitMovRM(kRDX, kRAX, 16);  // rdx = tls_tlab.end
            enc_.EmitCmpRR(kRBX, kRDX);
            uint32_t newobj_ja_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_A, 0);     // ja slow_path (patched later)

            // TLAB HIT: bump, zero-init, init struct
            enc_.EmitMovMR(kRAX, 8, kRBX);   // tls_tlab.current = new_ptr

            // Zero-init 64 bytes (4 × movups)
            enc_.EmitXorpsRR(0, 0);          // xorps xmm0, xmm0
            enc_.EmitMovUPSMR(kRCX, 0, 0);   // [rcx+0]
            enc_.EmitMovUPSMR(kRCX, 16, 0);  // [rcx+16]
            enc_.EmitMovUPSMR(kRCX, 32, 0);  // [rcx+32]
            enc_.EmitMovUPSMR(kRCX, 48, 0);  // [rcx+48]

            // Init SmallFieldArray: fields_ptr_ = &inline_[0] (at offset 24)
            enc_.EmitLeaRM(kRBX, kRCX, 24);
            enc_.EmitMovMR(kRCX, 0, kRBX);   // fields.fields_ptr_ = &inline_[0]
            enc_.EmitMovRI32(kRBX, 2);       // rbx = kInlineCapacity
            enc_.EmitMovMR(kRCX, 16, kRBX);  // fields.field_capacity_ = 2
            // field_count_ at offset 8 is already 0 from zero-init

            // Set type_token at offset 56 (4 bytes, upper 4 zero from zero-init)
            enc_.EmitMovRIImm32(kRBX, type_token);
            enc_.EmitMovMR(kRCX, 56, kRBX);  // obj->type_token = type_token

            StoreGpr(kRCX, instr.dst_reg());  // result = obj pointer

            uint32_t newobj_jmp_done_pos = buf_.pos();
            enc_.EmitJmpRel32(0);            // skip slow path

            // ═══ Slow path (TLAB miss) ═══
            uint32_t newobj_slow_pos = buf_.pos();
            buf_.Patch32(newobj_ja_pos + 2, newobj_slow_pos - (newobj_ja_pos + 6));

            {
                enc_.EmitMovRIImm32(kRCX, type_token);
                enc_.EmitMovRIImm32(kRDX, field_count);
                enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenNewObj));
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
            enc_.EmitMovRIImm32(kRCX, type_token);
            enc_.EmitMovRIImm32(kRDX, field_count);
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenNewObj));
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
        if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();
        EmitLoadTlsTlab(buf_);           // rax = &tls_tlab

        enc_.EmitMovRM(kRCX, kRAX, 8);   // rcx = tls_tlab.current
        enc_.EmitLeaRM(kRBX, kRCX, kBoxSize); // rbx = new current
        enc_.EmitMovRM(kRDX, kRAX, 16);  // rdx = tls_tlab.end
        enc_.EmitCmpRR(kRBX, kRDX);
        uint32_t box_ja_pos = buf_.pos();
        enc_.EmitJccRel32(kCC_A, 0);     // ja slow_path

        // TLAB HIT: bump, write value into BoxedValue
        enc_.EmitMovMR(kRAX, 8, kRBX);   // tls_tlab.current = new_ptr

        // Load the value to box from src1
        LoadGpr(kR8, instr.src1_reg());   // r8 = value

        // Zero-init 16 bytes then write tag + value
        enc_.EmitXorpsRR(0, 0);          // xorps xmm0, xmm0
        enc_.EmitMovUPSMR(kRCX, 0, 0);   // [rcx+0..15] = 0

        // Set tag = Int64 at offset 0 (4 bytes, struct_size at +4 = 0)
        enc_.EmitMovRIImm32(kRBX, static_cast<uint32_t>(interpreter::ValueTag::Int64));
        enc_.EmitMovMR(kRCX, 0, kRBX);   // BoxedValue::value.tag = Int64

        // Set value at offset 8 (InterpreterValue union slot)
        enc_.EmitMovMR(kRCX, 8, kR8);    // BoxedValue::value.i64 = value

        StoreGpr(kRCX, instr.dst_reg());  // result = boxed pointer

        uint32_t box_jmp_done_pos = buf_.pos();
        enc_.EmitJmpRel32(0);            // skip slow path

        // ═══ Slow path (TLAB miss) ═══
        uint32_t box_slow_pos = buf_.pos();
        buf_.Patch32(box_ja_pos + 2, box_slow_pos - (box_ja_pos + 6));

        {
            LoadGpr(kRCX, instr.src1_reg());
            enc_.EmitMovRIImm32(kRDX, static_cast<uint32_t>(interpreter::ValueTag::Int64));
            enc_.EmitMovRIImm32(kR8, instr.imm.field_offset);
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenBox));
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
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenUnbox));
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
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenLdLen));
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
        if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();
        EmitLoadTlsTlab(buf_);           // rax = &tls_tlab

        enc_.EmitMovRM(kRCX, kRAX, 8);   // rcx = tls_tlab.current
        enc_.EmitLeaRM(kRBX, kRCX, kArrSize); // rbx = new current
        enc_.EmitMovRM(kRDX, kRAX, 16);  // rdx = tls_tlab.end
        enc_.EmitCmpRR(kRBX, kRDX);
        uint32_t newarr_ja_pos = buf_.pos();
        enc_.EmitJccRel32(kCC_A, 0);     // ja slow_path

        // TLAB HIT: bump
        enc_.EmitMovMR(kRAX, 8, kRBX);   // tls_tlab.current = new_ptr

        // CodegenNewArrTlab(mem=rcx, length=rdx) — placement new + elements.resize
        LoadGpr(kRDX, instr.src1_reg());  // rdx = length
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenNewArrTlab));
        EmitCallWithSpill(kRAX);
        StoreGpr(kRAX, instr.dst_reg());

        uint32_t newarr_jmp_done_pos = buf_.pos();
        enc_.EmitJmpRel32(0);            // skip slow path

        // ═══ Slow path (TLAB miss) ═══
        uint32_t newarr_slow_pos = buf_.pos();
        buf_.Patch32(newarr_ja_pos + 2, newarr_slow_pos - (newarr_ja_pos + 6));

        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenNewArr));
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
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenLdElem));
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
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStElem));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::LdSFld: {
        if (!instr.has_dst()) return false;
        enc_.EmitMovRIImm32(kRCX, instr.imm.field_offset);
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenLdSFld));
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
        enc_.EmitMovRIImm32(kRCX, instr.imm.field_offset);
        LoadGpr(kRDX, instr.src1_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStSFld));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::CastClass: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenCastClass));
        EmitCallWithSpill(kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::IsInst: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenIsInst));
        EmitCallWithSpill(kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdVirtFtn: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenLdVirtFtn));
        EmitCallWithSpill(kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::InitObj: {
        if (!instr.has_src1()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenInitObj));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::StObj: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        LoadGpr(kRDX, instr.src2_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStObj));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::StInd: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        LoadGpr(kRCX, instr.src2_reg());
        enc_.EmitMovMR(kRAX, 0, kRCX);
        return true;
    }

    case IROpCode::LdInd: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovRM(kRAX, kRCX, 0);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdObj: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenLdObj));
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
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenCpblk));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::InitBlk: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        LoadGpr(kRDX, instr.src2_reg());
        if (instr.flags() & interpreter::kRegHasSrc3) LoadGpr(kR8, instr.src3_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenInitBlk));
        EmitCallWithSpill(kRAX);
        return true;
    }

    case IROpCode::LocAlloc: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRCX, instr.src1_reg());           // size
        if (localloc_extra_ > 0) {
            // Stack allocation from pre-allocated frame reserve.
            // RDX = base address of localloc reserve.
            enc_.EmitLeaRM(kRDX, kRSP,
                static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + 8));
            // R8 = bump counter pointer (8 bytes, at kFrameSize + align_adj + xmm_save).
            enc_.EmitLeaRM(kR8, kRSP,
                static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_));
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenLocAlloc));
        } else {
            // Fallback: heap allocation (no stack reserve — rare edge case).
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenLocAlloc));
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
        enc_.EmitLeaRM(kRAX, kRBX, static_cast<int32_t>(arg_idx * 8));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdLocA: {
        if (!instr.has_dst()) return false;
        uint32_t local_idx = instr.imm.operand_index;
        uint32_t vreg = kLocalRegBase + local_idx;
        enc_.EmitLeaRM(kRAX, kRSP, static_cast<int32_t>(GprOff(vreg)));
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
        for (uint32_t i = 4; i < arg_count; ++i) { LoadGpr(kRAX, first_arg_reg + i); enc_.EmitMovMR(kRSP, static_cast<int32_t>((i - 4) * 8), kRAX); }
        // Spill cached/colored registers before call (same as EmitCallWithSpill preamble)
        if (config_.enable_register_caching && cached_slots_used_) SpillCachedRegs();
        if (has_graph_coloring_) {
            for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                uint8_t colored_x64 = gcr_.gpr_color[vr];
                if (colored_x64 != 0xFF) {
                    if (caller_colored_mask_ & (1ULL << vr)) continue;
                    enc_.EmitMovMR(kRSP, static_cast<int32_t>(GprOff(vr)), colored_x64);
                }
            }
        }
        // Slot-based call: emit call [rip+0] placeholder, record SlotPatch.
        // The slot table is emitted after all instructions, before buf_.Seal().
        uint32_t call_start = buf_.pos();
        enc_.EmitCallRel32(0);  // 6 bytes: FF 15 00 00 00 00
        uint32_t call_pos = call_start;
        uint32_t slot_patch_offset = call_start + 2;  // disp32 starts at byte 2 of call [rip+disp32]
        slot_patches_.push_back({slot_patch_offset, static_cast<uint32_t>(call_sites_.size()), target_fn});
        slot_count_used_++;
        // Post-call reload (same as EmitCallWithSpill postamble)
        if (has_graph_coloring_ && caller_colored_mask_) {
            uint64_t mask = caller_colored_mask_;
            for (uint32_t vr = 0; mask; ++vr) {
                if (mask & 1) {
                    uint8_t colored_x64 = gcr_.gpr_color[vr];
                    enc_.EmitMovRM(colored_x64, kRSP, static_cast<int32_t>(GprOff(vr)));
                }
                mask >>= 1;
            }
        }
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
        enc_.EmitTestRR(kRAX, kRAX);
        uint32_t non_null_pos = buf_.pos();
        enc_.EmitJccRel32(kCC_NE, 0);
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
            enc_.EmitMovMR(kRSP, static_cast<int32_t>((i - 4) * 8), kRAX);
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

        // ── Inline PIC multi-slot fast path ─────────────────────────
        // Check for per-instruction PIC data from entry_direct.cpp.
        // When up to 3 PIC slots have resolved direct_fn entries and
        // arg_count <= 4, emit an inline type check chain + direct
        // calls instead of going through the full CodegenCallVirt
        // C helper.  RCX holds the object type_token (loaded once
        // from [obj+56]) and is reused across all slot checks.
        uint32_t pic_slot_count = 0;
        uint32_t inline_expected_types[3] = {};
        void* inline_direct_fns[3] = {};
        if (config_.per_instr_pic != nullptr && current_instr_index_ < config_.per_instr_pic_count) {
            const auto& pic = config_.per_instr_pic[current_instr_index_];
            if (pic.slot_count > 0 && arg_count <= 4) {
                pic_slot_count = pic.slot_count;
                for (uint32_t si = 0; si < pic_slot_count && si < 3; ++si) {
                    inline_expected_types[si] = pic.expected_type_tokens[si];
                    inline_direct_fns[si] = pic.direct_fns[si];
                }
            }
        }
        std::vector<uint32_t> inline_miss_jumps;   // jne positions (per slot)
        std::vector<uint32_t> inline_done_jumps;    // jmp .done positions (per slot)
        uint32_t inline_null_jmp_pos = 0;

        if (pic_slot_count > 0) {
            // Load receiver from register file (via LoadGpr for correct cache/coloring)
            LoadGpr(kRAX, first_arg_reg);
            enc_.EmitTestRR(kRAX, kRAX);
            inline_null_jmp_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_E, 0);  // je .use_c_helper (null receiver)

            // Load type_token from object at offset 56 (= sizeof(SmallFieldArray))
            // RCX holds type_token across ALL slot checks — loaded once here.
            enc_.EmitMovRM(kRCX, kRAX, 56);

            // Generate type check chain for up to pic_slot_count slots
            for (uint32_t si = 0; si < pic_slot_count; ++si) {
                // Compare RCX (type_token) with expected type for this slot
                enc_.EmitMovRIImm32(kRAX, inline_expected_types[si]);
                enc_.EmitCmpRR(kRAX, kRCX);
                // Miss jump — patched after all slot code is emitted
                inline_miss_jumps.push_back(buf_.pos());
                enc_.EmitJccRel32(kCC_NE, 0);  // jne .next (or .use_c_helper if last)

                // ── Slot hit: load args and call ──
                // Load Win64 calling convention registers from register file.
                // RCX is overwritten with first arg (type_token no longer needed).
                if (arg_count >= 1) LoadGpr(kRCX, first_arg_reg);
                if (arg_count >= 2) LoadGpr(kRDX, first_arg_reg + 1);
                if (arg_count >= 3) LoadGpr(kR8,  first_arg_reg + 2);
                if (arg_count >= 4) LoadGpr(kR9,  first_arg_reg + 3);

                // Direct call to pre-resolved AOT function pointer
                uint32_t call_pos = buf_.pos();
                enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(inline_direct_fns[si]));
                EmitCallWithSpill(kRAX);

                // Record call site for hotpatch tracking and GC point
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

                // Deoptimization metadata for inline path
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

                // Store return value before deopt check (deopt clobbers RAX)
                if (instr.has_dst()) StoreGpr(kRAX, instr.dst_reg());

                // Check ret_buf[0] for kDeoptMagic
                enc_.EmitMovRM(kRCX, kRSI, 0);
                enc_.EmitMovImm64(kRAX, kDeoptMagic);
                enc_.EmitCmpRR(kRAX, kRCX);
                uint32_t inline_jne_patch_off = buf_.pos() + 2;
                enc_.EmitJccRel32(kCC_NE, 0);   // jne .slot_done

                // Deopt path: jump to common deopt trampoline
                uint32_t inline_deopt_patch = buf_.pos() + 1;
                enc_.EmitJmpRel32(0);
                deopt_jump_patches_.push_back({inline_deopt_patch});

                // .slot_done (normal return — skip deopt path)
                uint32_t slot_done_off = buf_.pos();
                {
                    int32_t disp = static_cast<int32_t>(slot_done_off - (inline_jne_patch_off + 4));
                    buf_.Patch32(inline_jne_patch_off, static_cast<uint32_t>(disp));
                }

                // Jump past C helper path
                inline_done_jumps.push_back(buf_.pos());
                enc_.EmitJmpRel32(0);  // jmp .done
            }

            // ── Patch all slot miss jumps ──
            // For slot si, the miss jump target is right after this slot's
            // jmp .done instruction (= the next slot's check, or .use_c_helper
            // for the last slot).
            uint32_t c_helper_start = buf_.pos();
            for (size_t si = 0; si < inline_miss_jumps.size(); ++si) {
                uint32_t target;
                if (si + 1 < inline_done_jumps.size()) {
                    // Miss goes to next slot's check (right after this slot's jmp .done)
                    target = inline_done_jumps[si] + 5;  // jmp rel32 = 5 bytes
                } else {
                    // Last slot miss → fall through to C helper
                    target = c_helper_start;
                }
                int32_t disp = static_cast<int32_t>(target - (inline_miss_jumps[si] + 6));
                buf_.Patch32(inline_miss_jumps[si] + 2, static_cast<uint32_t>(disp));
            }
            // Patch null receiver jump to C helper as well
            {
                int32_t null_disp = static_cast<int32_t>(c_helper_start - (inline_null_jmp_pos + 6));
                buf_.Patch32(inline_null_jmp_pos + 2, static_cast<uint32_t>(null_disp));
            }
        }

        // ── Existing C helper path (CodegenCallVirt) ────────────────────
        // Build CodegenCallVirtArgs at [rsp + kCallVirtArgsOff]
        enc_.EmitLeaRM(kRCX, kRSP, static_cast<int32_t>(kCallVirtArgsOff));
        enc_.EmitLeaRM(kRAX, kRSP, static_cast<int32_t>(kGprFileOff));
        enc_.EmitMovMR(kRCX, 0, kRAX);    // gpr_base
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(config_.pic_dispatch_data));
        enc_.EmitMovMR(kRCX, 8, kRAX);    // pic_data
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(config_.dispatch_ctx));
        enc_.EmitMovMR(kRCX, 16, kRAX);   // dispatch_ctx
        enc_.EmitMovRIImm32(kRAX, current_instr_index_);
        enc_.EmitMovMR(kRCX, 24, kRAX);   // instruction_idx
        enc_.EmitMovRIImm32(kRAX, arg_count);
        enc_.EmitMovMR(kRCX, 28, kRAX);   // arg_count
        enc_.EmitMovRIImm32(kRAX, first_arg_reg);
        enc_.EmitMovMR(kRCX, 32, kRAX);   // first_arg_reg

        // method_token at offset 36 (from call_cache or 0)
        {
            uint32_t mt = 0;
            if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
                mt = cached.method_token;
            }
            enc_.EmitMovRIImm32(kRAX, mt);
            enc_.EmitMovMR(kRCX, 36, kRAX);   // method_token
        }

        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(instr.imm.ptr));
        enc_.EmitMovMR(kRCX, 40, kRAX);   // call_target
        enc_.EmitMovRIImm32(kRAX, instr.has_dst() ? 1 : 0);
        enc_.EmitMovMR(kRCX, 48, kRAX);   // has_dst
        enc_.EmitMovRIImm32(kRAX, static_cast<uint32_t>((instr.header >> 63) & 1));
        enc_.EmitMovMR(kRCX, 52, kRAX);   // is_instance_call
        enc_.EmitMovMR(kRCX, 56, kRSI);   // ret_buf

        // call CodegenCallVirt
        uint32_t call_pos = buf_.pos();
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenCallVirt));
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
        enc_.EmitMovRM(kRCX, kRSI, 0);
        enc_.EmitMovImm64(kRAX, kDeoptMagic);
        enc_.EmitCmpRR(kRAX, kRCX);
        uint32_t jne_patch_off = buf_.pos() + 2;
        enc_.EmitJccRel32(kCC_NE, 0);

        // Deopt path
        uint32_t deopt_patch_off = buf_.pos() + 1;
        enc_.EmitJmpRel32(0);
        deopt_jump_patches_.push_back({deopt_patch_off});

        // .normal:
        uint32_t normal_offset = buf_.pos();
        int32_t jne_disp = static_cast<int32_t>(normal_offset - (jne_patch_off + 4));
        buf_.Patch32(jne_patch_off, static_cast<uint32_t>(jne_disp));

        if (instr.has_dst()) StoreGpr(kRAX, instr.dst_reg());

        // .done (inline PIC fast path jumps here to skip C helper)
        {
            uint32_t done_pos = buf_.pos();
            for (size_t di = 0; di < inline_done_jumps.size(); ++di) {
                int32_t done_disp = static_cast<int32_t>(done_pos - (inline_done_jumps[di] + 5));
                buf_.Patch32(inline_done_jumps[di] + 1, static_cast<uint32_t>(done_disp));
            }
        }
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
    std::vector<uint8_t>& removed_mask,
    bool has_seh) noexcept
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

        // (a) Constant folding — binary pure arithmetic with both srcs constant.
        // Skip when SEH exists: handler code placed linearly between branches
        // and join points can define registers that don't reach the use through
        // normal control flow, causing incorrect constant propagation.
        if (!has_seh && IsFoldable(opc) && ri.has_dst() && ri.has_src1() && ri.has_src2()) {
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

JitMethod* NativeCodeGenerator::Generate() noexcept {
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

    // Pre-scan: count managed call instructions to reserve slot table entries.
    // slot_count_ = number of Call/CallBridge instructions (not Calli, not CallVirt).
    slot_count_ = 0;
    for (const auto& instr : rm_.instructions) {
        auto opc = instr.op_code();
        if (opc == interpreter::IROpCode::Call ||
            opc == interpreter::IROpCode::CallBridge) {
            slot_count_++;
        }
    }
    slot_patches_.reserve(slot_count_);
    slot_count_used_ = 0;

    // Initialize cached TLS info for inline TLAB access
    InitTlsTlabInfo();

    // ── Register allocation: Tier 0 skips entirely (stack-only) ───────────
    if (is_tier0_) {
        num_cache_regs_ = 0;
        has_graph_coloring_ = false;
        num_fpr_callee_ = 0;
        xmm_save_size_ = 0;
    } else if (config_.enable_register_caching) {
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
    prologue_push_offsets_[0] = buf_.pos();
    enc_.EmitPush(kRBP);
    prologue_set_fpreg_offset_ = buf_.pos();
    enc_.EmitMovRR(kRBP, kRSP);  // frame pointer chain for GC stack walking
    prologue_push_offsets_[1] = buf_.pos();
    enc_.EmitPush(kRBX);
    prologue_push_offsets_[2] = buf_.pos();
    enc_.EmitPush(kRSI);
    // Push additional callee-saved regs used for register caching
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        prologue_push_offsets_[3 + slot] = buf_.pos(),
        enc_.EmitPush(callee_saved_regs_[slot]);
    enc_.EmitMovRR(kRBX, kRCX); enc_.EmitMovRR(kRSI, kRDX);
    prologue_sub_rsp_offset_ = buf_.pos();
    prologue_sub_rsp_size_ = static_cast<uint32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_);
    enc_.EmitSubRI(kRSP, static_cast<int32_t>(prologue_sub_rsp_size_));
    prologue_total_bytes_ = buf_.pos() - prologue_push_offsets_[0];

    // Build push register list for unwind info: rbp, rbx, rsi, cached regs
    push_reg_nums_[0] = kRBP;
    push_reg_nums_[1] = kRBX;
    push_reg_nums_[2] = kRSI;
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        push_reg_nums_[3 + slot] = callee_saved_regs_[slot];
    num_push_regs_ = 3 + num_cache_regs_;

    // Save callee-saved XMM registers (used by graph coloring)
    // Stored in the area just below the regular frame (at RSP + kFrameSize + frame_align_adj_).
    for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
        int32_t off = static_cast<int32_t>(kFrameSize + frame_align_adj_ + si * 16);
        enc_.EmitMovUPSMR(kRSP, off, callee_xmm_regs_[si]);
    }

    // Zero-initialize all GPR stack slots — reading an uninitialized slot
    // produces garbage.  Done BEFORE colored-reg zeroing to use RDI as scratch.
    enc_.EmitXorZR(kRAX);
    enc_.EmitLeaRM(kRDI, kRSP, static_cast<int32_t>(kGprFileOff));
    enc_.EmitMovRIImm32(kRCX, kGprCount);
    buf_.EmitByte(0xF3);
    buf_.EmitByte(0x48);
    buf_.EmitByte(0xAB);

    // Zero the localloc bump pointer (RAX is still 0 from xor above)
    if (localloc_extra_ > 0) {
        enc_.EmitMovMR(kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_), kRAX);
    }

    // Initialize colored callee-saved regs to 0 (re-zeros RDI if colored,
    // since REP STOSQ above left RDI pointing past the GPR file).
    if (has_graph_coloring_) {
        for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
            enc_.EmitXorRR(callee_saved_regs_[slot], callee_saved_regs_[slot]);
    }

    // Zero caller-colored registers (R8-R11) — LoadGpr reads them directly
    // for vregs colored to caller-saved regs, bypassing the zeroed stack.
    // Without this, a read-before-write vreg in R8-R11 returns garbage from
    // the caller, while RegisterExecute and V2 (spill-to-stack) return 0.
    if (caller_colored_mask_) {
        for (uint32_t x64r = kR8; x64r <= kR11; ++x64r)
            enc_.EmitXorRR(x64r, x64r);
    }

    // ── GC mode switch: EnterCooperativeMode ──────────────────────────
    // Switch the thread to cooperative GC mode before any instruction
    // that allocates or accesses the managed heap.  This must happen
    // after the frame is set up (stack walking works) but before the
    // first managed object access.
    if (config_.cooperative_fn != nullptr) {
        enc_.EmitSubRI(kRSP, 32);  // shadow space for Win64 ABI
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(config_.cooperative_fn));
        uint32_t call_pos = buf_.pos();
        enc_.EmitCallReg(kRAX);
        enc_.EmitAddRI(kRSP, 32);
        // Record as GC point so stack is walkable during mode switch
        call_sites_.push_back({UINT32_MAX, call_pos});
        RecordGcPoint(call_pos);
    }

    // Bail out early if any emit above hit an OOM
    if (CheckFailed()) return nullptr;

    // ── Optimize instructions (tree IR pipeline + linear fallback) ──────
    // Creates a mutable copy of rm_.instructions for the optimizer.
    // Non-SEH methods: tree IR pipeline (Inliner → ConstFold → CSE → linearize).
    // SEH methods: existing linear OptimizeInstructions fallback.
    auto opt_instrs = rm_.instructions;
    std::vector<uint8_t> removed_mask;
    InlineResultBuffer inline_results;
    if (!is_tier0_ && config_.enable_optimizer) {
        if (!rm_.seh_clauses.empty()) {
            // SEH methods: use the existing linear optimizer
            OptimizeInstructions(opt_instrs, removed_mask, true);
        } else {
            // Non-SEH methods: tree IR pipeline with optional inlining.
            // Produces a clean instruction sequence (no removed_mask needed).
            std::vector<interpreter::RegisterInstruction> tree_opt;
            if (tree::OptimizeWithTreeIR(opt_instrs, tree_opt, false,
                                          rm_.max_regs, config_.enable_inlining,
                                          &inline_results)) {
                opt_instrs = std::move(tree_opt);
                n_instrs = static_cast<uint32_t>(opt_instrs.size());
                // Re-count call slots: inlining may have removed call instructions.
                slot_count_ = 0;
                for (const auto& instr : opt_instrs) {
                    auto opc = instr.op_code();
                    if (opc == interpreter::IROpCode::Call ||
                        opc == interpreter::IROpCode::CallBridge) {
                        slot_count_++;
                    }
                }
                slot_patches_.clear();
                slot_patches_.reserve(slot_count_);
                slot_count_used_ = 0;
            } else {
                // Fallback: linear optimizer (tree IR build failed or empty BBs)
                OptimizeInstructions(opt_instrs, removed_mask, false);
            }
        }
    }

    // ── Liveness analysis for precise GC slot maps ─────────────────────────
    // Computes per-instruction live-in bitmasks used by RecordGcPoint() to
    // report only ObjectRef vregs that are both typed-as-ref AND live at the
    // current instruction point.  Uses the optimized instruction stream so
    // liveness reflects the actual emitted code.
    // Skipped entirely in Tier 0 — GC uses conservative scanning.
    if (!is_tier0_ && config_.enable_liveness && n_instrs > 0)
    {
        live_in_.assign(n_instrs, 0);
        std::vector<uint64_t> live_out(n_instrs, 0);
        std::vector<uint64_t> def(n_instrs, 0);
        std::vector<uint64_t> use(n_instrs, 0);

        // Pass 1: Compute def and use for each instruction
        for (uint32_t i = 0; i < n_instrs; ++i) {
            const auto& inst = opt_instrs[i];
            if (inst.has_dst() && inst.dst_reg() < kGprCount)
                def[i] |= (1ULL << inst.dst_reg());
            if (inst.has_src1() && inst.src1_reg() < kGprCount)
                use[i] |= (1ULL << inst.src1_reg());
            if (inst.has_src2() && inst.src2_reg() < kGprCount)
                use[i] |= (1ULL << inst.src2_reg());
            if (inst.flags() & interpreter::kRegHasSrc3) {
                uint8_t src3 = inst.src3_reg();
                if (src3 < kGprCount)
                    use[i] |= (1ULL << src3);
            }
            // Calli: func_ptr vreg in imm.operand_index is an implicit source
            if (inst.op_code() == interpreter::IROpCode::Calli &&
                inst.imm.operand_index < kGprCount) {
                use[i] |= (1ULL << inst.imm.operand_index);
            }
        }

        // Pass 2: Iterative backward dataflow to fixed point
        bool changed = true;
        while (changed) {
            changed = false;
            for (int32_t i = static_cast<int32_t>(n_instrs) - 1; i >= 0; --i) {
                const auto& inst = opt_instrs[i];
                uint64_t new_live_out = 0;
                auto opc = inst.op_code();

                if (opc == interpreter::IROpCode::Switch) {
                    uint32_t n_targets = inst.imm.i4;
                    if (n_targets > 0 && inst.imm.ptr) {
                        const uint32_t* targets = static_cast<const uint32_t*>(inst.imm.ptr);
                        uint32_t limit = n_targets < 256 ? n_targets : 256;
                        for (uint32_t ti = 0; ti < limit; ++ti) {
                            if (targets[ti] < n_instrs)
                                new_live_out |= live_in_[targets[ti]];
                        }
                    }
                } else if (opc == interpreter::IROpCode::Br ||
                           opc == interpreter::IROpCode::Leave) {
                    uint32_t target = inst.imm.branch_target;
                    if (target < n_instrs)
                        new_live_out |= live_in_[target];
                } else if (opc == interpreter::IROpCode::BrTrue ||
                           opc == interpreter::IROpCode::BrFalse ||
                           opc == interpreter::IROpCode::Beq ||
                           opc == interpreter::IROpCode::BneUn ||
                           opc == interpreter::IROpCode::Blt ||
                           opc == interpreter::IROpCode::Bgt ||
                           opc == interpreter::IROpCode::Ble ||
                           opc == interpreter::IROpCode::Bge ||
                           opc == interpreter::IROpCode::BltUn ||
                           opc == interpreter::IROpCode::BgtUn ||
                           opc == interpreter::IROpCode::BleUn ||
                           opc == interpreter::IROpCode::BgeUn) {
                    uint32_t target = inst.imm.branch_target;
                    if (target < n_instrs)
                        new_live_out |= live_in_[target];
                    if (static_cast<uint32_t>(i) + 1 < n_instrs)
                        new_live_out |= live_in_[i + 1];
                } else if (opc == interpreter::IROpCode::Ret ||
                           opc == interpreter::IROpCode::Throw ||
                           opc == interpreter::IROpCode::Rethrow ||
                           opc == interpreter::IROpCode::EndFinally ||
                           opc == interpreter::IROpCode::EndFilter) {
                    // Terminator: no successors
                } else {
                    if (static_cast<uint32_t>(i) + 1 < n_instrs)
                        new_live_out |= live_in_[i + 1];
                }

                uint64_t new_live_in = use[i] | (new_live_out & ~def[i]);

                if (new_live_in != live_in_[i] || new_live_out != live_out[i]) {
                    live_in_[i] = new_live_in;
                    live_out[i] = new_live_out;
                    changed = true;
                }
            }
        }

        // Activate liveness filtering in RecordGcPoint
        use_liveness_ = true;
        liveness_computed_ = true;
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "Liveness computed for %u instructions, use_liveness=%d",
            n_instrs, (int)use_liveness_);
    }

    // Emit instructions
    for (uint32_t i = 0; i < n_instrs; ++i) {
        instr_offsets_[i] = buf_.pos();
        current_instr_index_ = i;
        const auto& instr = opt_instrs[i];
        if (!removed_mask.empty() && removed_mask[i]) continue;
        if (!EmitInstruction(instr)) {
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "Compile: unsupported opcode {} at pc={}, emitting deopt",
                static_cast<int>(instr.op_code()), i);
            if (!is_tier0_ && config_.enable_deopt) {
                EmitDeoptSequence(i);
                continue;
            }
            return nullptr;
        }
        PropagateTypes(instr);
    }

    // Bail out if any instruction emit failed (OOM).
    if (CheckFailed()) return nullptr;

    // deopt_return: shared deoptimization return point + stack frame epilogue.
    // All deopt paths (unsupported opcode, throw/rethrow, CallVirt PIC miss)
    // jump here.  The epilogue restores the stack frame and returns so that
    // InterpreterEntryDirect can check t_deopt_state.deopt_happened.
    deopt_return_pos_ = buf_.pos();

    // ── GC mode switch: EnterPreemptiveMode ───────────────────────────
    // Switch to preemptive mode before returning to native code.
    // In preemptive mode, safepoint requests don't spin — the thread
    // acknowledges and returns immediately, so the GC won't wait for it.
    if (config_.preemptive_fn != nullptr) {
        enc_.EmitSubRI(kRSP, 32);  // shadow space for Win64 ABI
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(config_.preemptive_fn));
        uint32_t call_pos = buf_.pos();
        enc_.EmitCallReg(kRAX);
        enc_.EmitAddRI(kRSP, 32);
        call_sites_.push_back({UINT32_MAX, call_pos});
        RecordGcPoint(call_pos);
    }

    // Restore callee-saved XMMs before deallocating frame
    for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
        int32_t off = static_cast<int32_t>(kFrameSize + frame_align_adj_ + si * 16);
        enc_.EmitMovUPRM(callee_xmm_regs_[si], kRSP, off);
    }
    enc_.EmitAddRI(kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_));
    for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
        enc_.EmitPop(callee_saved_regs_[slot - 1]);
    enc_.EmitPop(kRSI); enc_.EmitPop(kRBX); enc_.EmitPop(kRBP);
    enc_.EmitRet();

    // Sentinel entry for instr_offsets_ — SEH clause end indices may point
    // one past the last instruction (exclusive-end convention).
    instr_offsets_.push_back(buf_.pos());

    // ── Cold section (Throw/Rethrow handlers) ────────────────────────────
    // Emitted after the epilogue so hot-path instructions stay contiguous
    // (better icache utilization).  Each cold patch emits a short call to
    // ChaosJitRaiseException + INT3.  The VEH handler redirects RIP on
    // success, so we don't need register spill/reload (EmitCallReg is safe).
    //
    // NOTE: Do NOT use EmitCallWithSpill here — the cold path never returns
    // normally (VEH redirects RIP), so register state preservation is not
    // needed and would introduce unnecessary code in the hot section.
    for (auto& cp : cold_patches_) {
        uint32_t cold_target = buf_.pos();
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(ChaosJitRaiseException));
        enc_.EmitCallReg(kRAX);
        buf_.EmitByte(0xCC);  // INT3 safety net
        int32_t disp = static_cast<int32_t>(cold_target - (cp.patch_offset + 4));
        buf_.Patch32(cp.patch_offset, static_cast<uint32_t>(disp));
    }

    if (CheckFailed()) return nullptr;
    if (buf_.pos() == 0 || instr_offsets_.empty()) return nullptr;

    // Resolve branches (including deopt jump patches)
    ResolveBranches();
    if (CheckFailed()) return nullptr;

    // ── Emit SEH clause table ──────────────────────────────────────────
    // Appended after code body; VEH handler reads from nm->code + seh_table_offset.
    //
    // Filter clauses (flags == 0x1) follow a special convention to keep the
    // table fixed-size: the handler_start_offset slot stores the FILTER
    // FUNCTION offset, not the handler offset.  The actual handler is
    // emitted immediately after the filter (sequential layout).  The T4 VEH
    // dispatch path runs the filter, and if it returns non-zero, control
    // falls through into the handler; if zero, the search continues outward.
    uint32_t seh_offset = 0;
    if (!is_tier0_ && !rm_.seh_clauses.empty()) {
        seh_offset = buf_.pos();
        uint32_t count = static_cast<uint32_t>(rm_.seh_clauses.size());
        uint32_t max_idx = n_instrs;  // instr_offsets_ has n_instrs + 1 entries (sentinel at end)
        uint32_t emitted_count = 0;
        buf_.Emit32(count);
        for (const auto& clause : rm_.seh_clauses) {
            uint32_t try_start = static_cast<uint32_t>(clause.try_start_idx);
            uint32_t try_end = static_cast<uint32_t>(clause.try_end_idx);
            uint32_t handler_start = static_cast<uint32_t>(clause.handler_start_idx);
            const auto cflags_u32 = static_cast<uint32_t>(clause.flags);
            const bool is_filter = (cflags_u32 ==
                static_cast<uint32_t>(interpreter::SEHFlags::Filter));
            // For filter clauses, the dispatched offset is the filter function;
            // the actual handler follows the filter sequentially.
            uint32_t dispatch_idx = handler_start;
            // Validate indices: skip malformed clauses (defensive, not a crash).
            if (try_start >= max_idx || try_end > max_idx || dispatch_idx >= max_idx ||
                handler_start >= max_idx) {
                CHAOS_IL2CPP_LOG_WARN_M("codegen",
                    "Compile: SEH clause has out-of-range indices "
                    "(try_start={} try_end={} handler_start={} dispatch_idx={} max_idx={}), skipping",
                    try_start, try_end, handler_start, dispatch_idx, max_idx);
                continue;
            }
            buf_.Emit32(cflags_u32);
            buf_.Emit32(instr_offsets_[try_start]);
            buf_.Emit32(instr_offsets_[try_end]);
            buf_.Emit32(instr_offsets_[dispatch_idx]);
            buf_.Emit32(clause.class_token);
            ++emitted_count;
        }
        // Overwrite count with actual emitted count (skipped malformed clauses)
        buf_.Patch32(seh_offset, emitted_count);
    }

    // ── Emit .pdata/.xdata unwind info ──────────────────────────────────
    // Win64 RUNTIME_FUNCTION for OS stack unwinding (debugger, backtrace).
    // V1: no UNW_FLAG_EHANDLER (exception dispatch still via VEH handler).
    // OSR entry stub gets a separate RUNTIME_FUNCTION (V2).
    uint32_t unwind_data_offset = 0;
    uint32_t code_body_size = buf_.pos();  // Function body ends before metadata
    bool has_seh = !rm_.seh_clauses.empty();
#if defined(_WIN64)
    if (!is_tier0_ && prologue_total_bytes_ > 0 && num_push_regs_ > 0) {
        unwind_data_offset = EmitUnwindInfo(
            buf_, prologue_total_bytes_, prologue_sub_rsp_size_,
            num_push_regs_, push_reg_nums_, prologue_push_offsets_,
            prologue_sub_rsp_offset_, prologue_set_fpreg_offset_,
            has_seh);
    }

    // ── Emit .eh_frame DWARF CFI (Linux x64) ───────────────────────────────
#if defined(__linux__)
    if (!is_tier0_ && prologue_total_bytes_ > 0 && num_push_regs_ > 0) {
        uint32_t cie_off = EmitDwarfCie(buf_);
        EmitDwarfFde(buf_, cie_off, code_body_size,
                     num_push_regs_, push_reg_nums_);
        eh_frame_offset_ = cie_off;
    }
#endif

#endif

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
            enc_.EmitPush(kRBP);
            enc_.EmitMovRR(kRBP, kRSP);  // frame pointer chain
            enc_.EmitPush(kRBX);
            enc_.EmitPush(kRSI);
            // Push cached/colored regs (matches main prologue layout for correct epilogue)
            for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
                enc_.EmitPush(callee_saved_regs_[slot]);
            enc_.EmitSubRI(kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_));
            // Zero the localloc bump pointer for OSR entry
            if (localloc_extra_ > 0) {
                enc_.EmitXorRR(kRAX, kRAX);
                enc_.EmitMovMR(kRSP, static_cast<int32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_), kRAX);
            }
            // using rep movsq (RSI=source, RDI=dest, RCX=count)
            enc_.EmitLeaRM(kRDI, kRSP, static_cast<int32_t>(kGprFileOff));
            enc_.EmitMovRR(kRSI, kRCX);
            enc_.EmitMovRIImm32(kRCX, kGprCount + kFprCount);  // 96
            buf_.EmitByte(0xF3);  // REP prefix
            buf_.EmitByte(0x48);  // REX.W
            buf_.EmitByte(0xA5);  // MOVSQ

            // Set up register convention: RBX=args_buf, RSI=ret_buf
            enc_.EmitLeaRM(kRBX, kRSP, static_cast<int32_t>(kGprFileOff));
            enc_.EmitMovRR(kRSI, kRDX);

            // Initialize cached/colored registers from the stack frame
            if (config_.enable_register_caching) {
                for (uint32_t slot = 0; slot < num_cache_regs_; ++slot) {
                    uint8_t x64_reg = callee_saved_regs_[slot];
                    if (has_graph_coloring_) {
                        uint32_t vreg = x64_to_colored_vreg_[x64_reg];
                        if (vreg != 0xFF) {
                            enc_.EmitMovRM(x64_reg, kRBX, static_cast<int32_t>(vreg * 8));
                        }
                    } else {
                        uint32_t vreg = x64_to_cached_vreg_[x64_reg];
                        if (vreg != kNotCached) {
                            enc_.EmitMovRM(x64_reg, kRBX, static_cast<int32_t>(vreg * 8));
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
                    enc_.EmitXorRR(x64r, x64r);
            }

            // Initialize callee-saved XMM registers from RegisterFile FPR copy.
            // Must also save to the XMM save area so the epilogue (Ret/deopt_return)
            // can restore them correctly.
            for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
                uint8_t xmm_reg = callee_xmm_regs_[si];
                uint32_t fi = callee_xmm_fi_[si];
                int32_t fpr_off = static_cast<int32_t>(kFprFileOff + fi * 8);
                enc_.EmitMovSDRM(xmm_reg, kRSP, fpr_off);
                // Duplicate to XMM save area so Ret/deopt_return epilogue can restore
                int32_t save_off = static_cast<int32_t>(kFrameSize + frame_align_adj_ + si * 16);
                enc_.EmitMovUPSMR(kRSP, save_off, xmm_reg);
            }

            // ── GC mode switch: EnterCooperativeMode (OSR entry) ───────
            // OSR resumes mid-execution in a hot loop.  The thread must be
            // in cooperative mode before the first managed object access.
            if (config_.cooperative_fn != nullptr) {
                enc_.EmitSubRI(kRSP, 32);
                enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(config_.cooperative_fn));
                uint32_t call_pos = buf_.pos();
                enc_.EmitCallReg(kRAX);
                enc_.EmitAddRI(kRSP, 32);
                call_sites_.push_back({UINT32_MAX, call_pos});
                RecordGcPoint(call_pos);
            }

            // Resolve loop header target and jump there.
            // Since OSR always restarts at the backward branch target
            // (loop header), we call OsrResolveLoopHeader() which reads
            // t_deopt_state.osr_resume_pc and resolves it through the
            // persisted instr_offsets table to an absolute native address.
            enc_.EmitSubRI(kRSP, 32);                // shadow space for Win64
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::OsrResolveLoopHeader));
            EmitCallWithSpill(kRAX);
            enc_.EmitAddRI(kRSP, 32);                // restore shadow space

            // RAX now holds the target native address (or nullptr if resolution failed).
            // Null-check RAX before jumping: if null, return to caller (fall back to
            // interpreter).  The caller initialized ret_buf[0]=0, so a bare ret here
            // signals "OSR failed — continue interpreting".
            EmitTestRR(buf_, kRAX, kRAX);
            buf_.EmitByte(0x75);  // JNE rel8
            buf_.EmitByte(0x01);  // skip 1 byte (the RET)
            buf_.EmitByte(0xC3);  // RET
            // RAX non-null: jump to resolved loop header address.
            // Re-zero caller-colored regs (R8-R11) clobbered by the call.
            if (caller_colored_mask_) {
                for (uint32_t x64r = kR8; x64r <= kR11; ++x64r)
                    enc_.EmitXorRR(x64r, x64r);
            }
            enc_.EmitJmpReg(kRAX);
        }
    }

    if (CheckFailed()) return nullptr;

    // ── Emit call-site slot table ─────────────────────────────────────────
    // Slot table embedded in the RX code buffer for call [rip+off] indirection.
    // Each entry is a void* pointer to the target function.  Patch all call
    // instruction RIP-relative displacements now that we know the slot table
    // position.  The table is in RX memory; ReverseSlotMap::UpdateAll uses
    // VirtualProtect to write to it during hotpatch.
    uint32_t slot_table_offset = 0;
    if (slot_count_used_ > 0) {
        slot_table_offset = buf_.pos();
        for (uint32_t si = 0; si < slot_patches_.size(); ++si) {
            auto& sp = slot_patches_[si];
            buf_.Emit64(reinterpret_cast<uint64_t>(sp.target_fn));
            // Patch the RIP-relative displacement: slot_entry - (call_next_addr)
            uint32_t call_next = sp.patch_offset + 4;  // FF 15 <disp32> = 6 bytes
            int32_t disp = static_cast<int32_t>(
                (slot_table_offset + si * 8) - call_next);
            buf_.Patch32(sp.patch_offset, static_cast<uint32_t>(disp));
        }
    }

    // Seal code buffer — returns nullptr on OOM or failure
    if (CheckFailed()) return nullptr;
    uint32_t code_bytes = buf_.pos();
    void* code = buf_.Seal();
    if (code == nullptr) return nullptr;

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "Compile: {} instrs, {} bytes code, {} call sites",
        n_instrs, code_bytes, call_sites_.size());

    // Build JitMethod
    auto* nm = static_cast<JitMethod*>(CHAOS_IL2CPP_MALLOC(sizeof(JitMethod)));
    if (nm == nullptr) return nullptr;
    std::memset(nm, 0, sizeof(*nm));
    ::new (nm) JitMethod();
    nm->code = code;
    nm->code_size = code_bytes;
    nm->instr_count = n_instrs;
    nm->seh_table_offset = seh_offset;
    nm->osr_entry_offset = osr_entry;

    // Wire up call-site slot table (embedded in the RX code buffer).
    // call_site_slots points into the sealed buffer after buf_.Seal().
    // Slot entries are written with VirtualProtect during hotpatch.
    if (slot_count_used_ > 0) {
        nm->call_site_slots = reinterpret_cast<void**>(
            static_cast<uint8_t*>(code) + slot_table_offset);
        nm->call_site_slot_count = slot_count_used_;
        nm->call_site_capacity = slot_count_;

        // Register each slot in the reverse map for hotpatch updates.
        for (uint32_t si = 0; si < slot_patches_.size(); ++si) {
            auto& sp = slot_patches_[si];
            uint32_t callee_token = 0;
            if (sp.call_site_index < call_sites_.size()) {
                callee_token = call_sites_[sp.call_site_index].method_token;
            }
            if (callee_token != 0) {
                g_reverse_slot_map.Add(callee_token, nm, si);
            }
        }
    }

    // ── Populate inlined callee info ──────────────────────────────────
    // Records callee tokens and version snapshots for hot-update staleness
    // detection.  If a callee is later hotpatched, InlineReverseMap sets
    // nm->stale = true, triggering recompilation on next dispatch.
    if (inline_results.count > 0) {
        nm->inlined_callees = static_cast<JitMethod::InlinedCallee*>(
            CHAOS_IL2CPP_MALLOC(inline_results.count * sizeof(JitMethod::InlinedCallee)));
        if (nm->inlined_callees) {
            for (uint32_t ri = 0; ri < inline_results.count; ++ri) {
                nm->inlined_callees[ri].callee_token     = inline_results.callee_tokens[ri];
                nm->inlined_callees[ri].snapshot_version = inline_results.snapshot_versions[ri];
                g_inline_reverse_map.Add(inline_results.callee_tokens[ri], nm);
            }
            nm->inlined_callee_count = inline_results.count;
        }
    }

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
            nm->gc_slot_map = reinterpret_cast<GcSlotMapV0*>(map_data);
        }
    }

    nm->rbp_to_rsp_offset = 16 + num_cache_regs_ * 8 +
                            static_cast<uint32_t>(kFrameSize + frame_align_adj_ + xmm_save_size_ + localloc_extra_);

    // ── Allocate Win64 RUNTIME_FUNCTION for .pdata registration ─────────
#if defined(_WIN64)
    if (unwind_data_offset > 0) {
        nm->runtime_function = AllocRuntimeFunction(
            unwind_data_offset, code_body_size);
    }
#elif defined(__linux__)
    // Store .eh_frame DWARF CFI offset for __register_frame in RegisterNativeCodeSection.
    nm->eh_frame_offset = eh_frame_offset_;
#endif

    CHAOS_IL2CPP_LOG_INFO_M("codegen",
        "Generate: method compiled, code_size=%u, code=%p, slots=%u",
        nm ? nm->code_size : 0, nm ? nm->code : nullptr,
        slot_count_used_);
    return nm;
}

JitMethod* Compile(
    const interpreter::RegisterMethod& rm,
    const CompileConfig& config) noexcept {
    if (rm.instructions.empty()) return nullptr;
    ISehHandler& seh = GetSehHandler();
    NativeCodeGenerator gen(rm, config, seh);
    return gen.Generate();
}

bool CanCompile(const interpreter::RegisterMethod& rm) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (rm.instructions.empty()) return false;
    // Validate SEH clause indices — any out-of-range clause means the
    // IR is malformed.  Generate() skips them, but early rejection here
    // prevents partial codegen.
    uint32_t n_instrs = static_cast<uint32_t>(rm.instructions.size());
    for (const auto& clause : rm.seh_clauses) {
        if (static_cast<uint32_t>(clause.try_start_idx) >= n_instrs ||
            static_cast<uint32_t>(clause.try_end_idx) > n_instrs ||
            static_cast<uint32_t>(clause.handler_start_idx) >= n_instrs) {
            return false;
        }
    }
    return true;  // All opcodes accepted — unsupported ones deopt at runtime.
}

JitMethod::~JitMethod() noexcept {
    CHAOS_IL2CPP_FREE(call_sites);

    // Remove this method's slots from the reverse map (so hotpatch updates
    // don't try to write to freed JitMethod pointers).
    g_reverse_slot_map.RemoveAll(this);
    CHAOS_IL2CPP_FREE(deopt_entries);
    CHAOS_IL2CPP_FREE(deopt_values);
    CHAOS_IL2CPP_FREE(gc_points);
    // Unregister GC slot map before freeing the backing data.
    if (code != nullptr && gc_slot_map != nullptr) {
        chaos::il2cpp::runtime_core::GcUnregisterSlotMap(code);
    }
    CHAOS_IL2CPP_FREE(slot_map_data);
    CHAOS_IL2CPP_FREE(instr_offsets);
    // Free GcPoint.slots arrays (each allocated independently by RecordGcPoint)
    for (uint32_t i = 0; i < gc_point_count; ++i) {
        CHAOS_IL2CPP_FREE(gc_points[i].slots);
    }
#if defined(_WIN64)
    if (runtime_function != nullptr) {
        RtlDeleteFunctionTable(static_cast<PRUNTIME_FUNCTION>(runtime_function));
        CHAOS_IL2CPP_FREE(runtime_function);
        runtime_function = nullptr;
    }
#elif defined(__linux__)
    if (eh_frame_offset > 0 && code != nullptr) {
        const void* eh_frame = static_cast<const uint8_t*>(code) + eh_frame_offset;
        __deregister_frame(eh_frame);
    }
#endif
    code = nullptr;
}

JitMethod::JitMethod(JitMethod&& other) noexcept {
    std::memcpy(this, &other, sizeof(*this));
    std::memset(&other, 0, sizeof(other));
}

JitMethod& JitMethod::operator=(JitMethod&& other) noexcept {
    if (this != &other) {
        this->~JitMethod();
        std::memcpy(this, &other, sizeof(*this));
        std::memset(&other, 0, sizeof(other));
    }
    return *this;
}

}  // namespace chaos::il2cpp::jit