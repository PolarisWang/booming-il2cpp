// ABI exports: extern "C" linkage for managed/NativeAOT callability.
#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#include "jit_engine.h"
#include "x64_encoder.h"
#include "code_buffer.h"
#include "jit_helpers.h"
#include "jit_seh.h"
#include "jit_unwind.h"
#include "slot_map.h"
#include "tree/jit_optimizer.h"
#include "jit_inline.h" // g_inline_reverse_map
#include "IEncoder.h"
#include "ISehHandler.h"
#include "X64Encoder.h"
#include "arm64_encoder.h" // EmitFcmgt4S, EmitFcmeq4S, etc. — free-function API used on both x64 and ARM64
#if defined(__aarch64__)
#include "Arm64Encoder.h" // Arm64Encoder class wrapper (IEncoder impl, only needed on ARM64)
#endif

#include <gc_root_scanner.h>
#include <gc/gc_bgc.h>
#include <gc/gc_card_table.h>

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

// GCC unwinder .eh_frame registration — no header declares these on all GCC versions.
#if defined(__linux__)
extern "C" void __register_frame(const void*);
extern "C" void __deregister_frame(const void*);
#endif

namespace chaos::il2cpp::jit {

// ── Architecture selection ───────────────────────────────────────────────────
#if defined(__aarch64__)
using AT = ArchTraits<Arch::kARM64>;
#else
using AT = ArchTraits<Arch::kX64>;
#endif

// ── Frame layout constants ─────────────────────────────────────────────────
// Stack frame (relative to RSP/SP):
//
// x64 layout:
//   [rsp + 0 .. 32)       = Win64 shadow space (for callee calls)
//   [rsp + 32 .. 544)     = GPR file (virtual register 0..63, 512 bytes)
//   [rsp + 544 .. 1568)   = FPR file (virtual register 64..95, 1024 bytes, 32-byte YMM slots)
//   [rsp + 1568 .. 1632)  = CallVirtArgs struct
//   Total: 1632 bytes
//
// ARM64 layout:
//   [sp + 0 .. 512)       = GPR file (virtual register 0..63, 512 bytes)
//   [sp + 512 .. 1024)    = FPR file (virtual register 64..95, 512 bytes, 16-byte Q slots)
//   [sp + 1024 .. 1088)   = CallVirtArgs struct
//   Total: 1088 bytes
//
// When LocAlloc is used, an additional reserve region is appended:
//   [rsp + base_frame_end .. +8)       = localloc_bump (uint32_t counter)
//   [rsp + base_frame_end+8 .. +4008)  = localloc_reserve (4KB scratch)
//   localloc_extra = 4008 bytes

#if defined(__aarch64__)
static constexpr uint32_t kShadowSize = 0;   // No shadow space on ARM64
static constexpr uint32_t kFprSlotSize = 16; // 128-bit NEON Q registers
#else
static constexpr uint32_t kShadowSize = 32;  // Win64 shadow space
static constexpr uint32_t kFprSlotSize = 32; // YMM 256-bit slots (AVX)
#endif

static constexpr uint32_t kGprCount = interpreter::kGPRegisters; // 64
static constexpr uint32_t kFprCount = interpreter::kFPRegisters; // 32
static constexpr uint32_t kGprFileSize = kGprCount * 8;          // 512 bytes
static constexpr uint32_t kFprFileSize = kFprCount * kFprSlotSize;
static constexpr uint32_t kGprFileOff = kShadowSize;
static constexpr uint32_t kFprFileOff = kGprFileOff + kGprFileSize;
static constexpr uint32_t kCallVirtArgsOff = kFprFileOff + kFprFileSize;
static constexpr uint32_t kFrameSize = kCallVirtArgsOff + sizeof(CodegenCallVirtArgs);

// GcSlotMapV0 slot encoding reserves the top bit for the interior kind flag,
// leaving 31 bits for the RSP/frame offset (up to ~2 GB) — effectively
// unbounded for stack frames.
static_assert(kFrameSize < (1u << 31), "GcSlotMapV0 offset must fit in 31 bits");

// LocAlloc reserve: bump counter + scratch region
static constexpr uint32_t kLocAllocReserveSize = 4096;
static constexpr uint32_t kLocAllocBumpAndReserve = 8 + kLocAllocReserveSize; // 4104
static constexpr uint32_t kMaxTlabInlineSize = 2048;                          // max bytes per TLAB inline allocation

// VEX/AVX encoding toggle: true → use VEX 3-operand encoding for SIMD ops
// ARM64 NEON does not use VEX — all operations are true 3-operand via arm64_encoder.h.
#if defined(__aarch64__)
static constexpr bool kUseVexEncoding = false;
#else
static constexpr bool kUseVexEncoding = true;
#endif

// Helper: RSP offset for a virtual GPR.
inline uint32_t GprOff(uint32_t vreg) noexcept {
    return kGprFileOff + vreg * 8;
}

// Helper: RSP offset for a virtual FPR (vreg 64+).
inline uint32_t FprOff(uint32_t vreg) noexcept {
    return kFprFileOff + (vreg - kGprCount) * kFprSlotSize;
}

// Register convention constants (mirrors ir_reg_alloc.h convention)
// r0-r7   = argument registers (mapped from LdArg operand_index)
// r8-r15  = local variable registers (mapped from LdLoc/StLoc operand_index)
// r16+    = evaluation stack virtual registers
static constexpr uint32_t kArgRegCount = 8;
static constexpr uint32_t kLocalRegBase = 8;

// Global reverse slot map: callee token → (JitMethod*, slot_index).
// Used by the hotpatch callback to update RX slot tables when a method is patched.
ReverseSlotMap g_reverse_slot_map;

// ARM64: patch a forward B.cond instruction (local forward jump).
// On ARM64, B.cond has a 19-bit imm19 field (±1MB).  For forward jumps
// within the same basic block (always < 1MB), reconstruct the instruction
// with the correct imm19 encoding.
#if defined(__aarch64__)
inline void PatchArm64Bcond(CodeBuffer& buf, uint32_t patch_pos, uint32_t target_pos) noexcept {
    int32_t disp = static_cast<int32_t>(target_pos - (patch_pos + 4));
    uint32_t instr = buf.Load32(patch_pos);
    uint32_t imm19 = (static_cast<uint32_t>(disp) >> 2) & 0x7FFFF;
    buf.Patch32(patch_pos, (instr & 0xFF00001Fu) | (imm19 << 5));
}
/// ARM64: patch a forward B (unconditional) instruction.
/// B has a 26-bit imm26 field (±128MB), sufficient for any method-local jump.
inline void PatchArm64B(CodeBuffer& buf, uint32_t patch_pos, uint32_t target_pos) noexcept {
    int32_t disp = static_cast<int32_t>(target_pos - (patch_pos + 4));
    uint32_t instr = buf.Load32(patch_pos);
    uint32_t imm26 = (static_cast<uint32_t>(disp) >> 2) & 0x3FFFFFF;
    buf.Patch32(patch_pos, (instr & 0xFC000000u) | imm26);
}
#endif

// Internal class that drives code generation.
class NativeCodeGenerator {
public:
    NativeCodeGenerator(const interpreter::RegisterMethod& rm, const CompileConfig& config, ISehHandler& seh)
        : rm_(rm), config_(config), encoder_(buf_), enc_(encoder_), seh_(seh) {
        is_tier0_ = (config_.compile_tier == CompileTier::kQuick);
    }

    JitMethod* Generate() noexcept;

private:
    const interpreter::RegisterMethod& rm_;
    CompileConfig config_;
    CodeBuffer buf_;
#if defined(__aarch64__)
    Arm64Encoder encoder_; // Concrete ARM64 encoder writing to buf_
#else
    X64Encoder encoder_; // Concrete x64 encoder writing to buf_
#endif
    IEncoder& enc_; // Interface reference to encoder_
    ISehHandler& seh_;

    // Slot patch displacement offset within the call instruction placeholder.
    // x64: call [rip+disp32] (FF 15 <dd dd dd dd>) — disp32 starts at byte 2.
    // ARM64: LDR X17, #imm19 — imm19 field starts at byte 0 (bits [23:5]).
#if defined(__aarch64__)
    static constexpr uint32_t kSlotPatchDispOff = 0; // ARM64 LDR X17, #imm19 starts at byte 0 (imm19 in bits [23:5])
#else
    static constexpr uint32_t kSlotPatchDispOff = 2; // x64 call [rip+disp32]: FF 15 <dd dd dd dd>
#endif

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
        uint32_t patch_offset; // offset of JMP rel32 displacement field
    };
    std::vector<ColdPatch> cold_patches_;

    // Slot-based call tracking for call-site indirection.
    // Records which call instructions should use slot-based (call [rip+off])
    // emission instead of mov rax, imm64; call rax.
    struct SlotPatch {
        uint32_t patch_offset;    // buffer offset of the disp32 in call [rip+disp32]
        uint32_t call_site_index; // index in call_sites_ for this call
        void* target_fn;          // target function pointer to write into slot
    };
    std::vector<SlotPatch> slot_patches_;
    uint32_t slot_count_ = 0;      // total number of slots reserved
    uint32_t slot_count_used_ = 0; // number of slots actually used (≤ slot_count_)

    // Jump table patch records (for Switch with >=4 cases).
    struct JumpTablePatch {
        uint32_t table_entry_offset; // buffer offset of this .int32 entry
        uint32_t table_base;         // buffer offset of the table start
        uint32_t target_instr;       // target instruction index
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
    static constexpr uint8_t kTypeVoid = 0;
    static constexpr uint8_t kTypeInt32 = 1;
    static constexpr uint8_t kTypeInt64 = 2;
    static constexpr uint8_t kTypeFloat32 = 3;
    static constexpr uint8_t kTypeFloat64 = 4;
    static constexpr uint8_t kTypeObjectRef = 5;

    std::vector<uint8_t> vreg_types_;
    inline void SetVregType(uint32_t vreg, uint8_t type) noexcept {
        if (vreg < vreg_types_.size())
            vreg_types_[vreg] = type;
    }
    void PropagateTypes(const interpreter::RegisterInstruction& instr) noexcept;
    // ───────────────────────────────────────────────────────────────────

    // Position of the deopt_return shared epilogue label.
    uint32_t deopt_return_pos_ = 0;

    // ── Register caching V1 ──────────────────────────────────────────────
    // Maps hot virtual registers to callee-saved GPRs.
    // Dirty-bit tracking avoids unnecessary stack writes.
#if defined(__aarch64__)
    // ARM64: X19-X28 (all 10 callee-saved GPRs)
    static constexpr uint8_t kCacheableRegs[10] = {19, 20, 21, 22, 23, 24, 25, 26, 27, 28};
    static constexpr uint32_t kMaxCacheRegs = 10;
    static constexpr uint32_t kPhysRegCount = 32;
#else
#if defined(_WIN32) || defined(_WIN64)
    // Win64: R12-R15 (all callee-saved except RDI which is pushed
    // explicitly in prologue for REP STOSQ zero-init)
    static constexpr uint8_t kCacheableRegs[4] = {12, 13, 14, 15};
    static constexpr uint32_t kMaxCacheRegs = 4;
#else
    // Linux SysV: R12-R15 only (RDI is caller-saved, excluded from cache)
    static constexpr uint8_t kCacheableRegs[4] = {12, 13, 14, 15};
    static constexpr uint32_t kMaxCacheRegs = 4;
#endif
    static constexpr uint32_t kPhysRegCount = 16;
#endif
    static constexpr uint8_t kNotCached = 0xFF;

    // vreg → phys reg# (kNotCached if not cached)
    uint8_t cached_x64_for_vreg_[interpreter::kGPRegisters];
    // phys reg → vreg (kNotCached if not used as cache)
    uint8_t phys_to_cached_vreg_[kPhysRegCount];
    // Bit i set = cache slot i is in use (maps to kCacheableRegs[i])
    uint32_t cached_slots_used_ = 0;
    // Bit i set = cache slot i is dirty (needs spill)
    uint32_t cached_dirty_mask_ = 0;
    // Number of cacheable regs actually selected
    uint32_t num_cache_regs_ = 0;
    // Alignment adjustment (0 or 8 bytes) when num_cache_regs_ is odd
    // Keeps RSP 16-byte aligned per Win64 ABI (x64 only — ARM64 STP is always 16 bytes).
    int32_t frame_align_adj_ = 0;

    // ── Graph-coloring register allocation (V2) ──────────────────────────
    // Replaces V1 frequency-based caching with Chaitin-Briggs coloring.
    // Results are mutually exclusive with V1: when active, cached_slots_used_=0.
    GraphColoringResult gcr_;
    bool has_graph_coloring_ = false;
    // Callee-saved GPRs selected by graph coloring (subset of kCacheableRegs)
    uint8_t callee_gpr_regs_[kMaxCacheRegs];
    // phys reg → vreg (0xFF = not colored); indexed by physical register number
    uint8_t phys_to_colored_vreg_[kPhysRegCount];
    // Pointer to current callee-saved register list (callee_gpr_regs_ or kCacheableRegs)
    const uint8_t* callee_saved_regs_ = kCacheableRegs;

    // ── FPR (XMM) coloring ────────────────────────────────────────────────
    uint8_t callee_xmm_regs_[10]; // XMM6-XMM15 max
    uint8_t callee_xmm_fi_[10];   // FPR vreg index (fi) for each callee_xmm_regs_[slot]
    uint32_t num_fpr_callee_ = 0;
    int32_t xmm_save_size_ = 0;

    // ── Prologue tracking (for .pdata/.xdata unwind info) ──────────────────
    // Byte offsets from function entry for each prologue instruction.
    // Set during prologue emission (lines ~2825-2833).
    uint32_t prologue_push_offsets_[14] {};  // Offsets: [0]=STP FP/LR, [4..13]=cache reg STPs
    uint32_t prologue_sub_rsp_offset_ = 0;   // Offset of sub rsp, K
    uint32_t prologue_set_fpreg_offset_ = 0; // Offset of mov rbp, rsp
    uint32_t prologue_total_bytes_ = 0;      // Total prologue size in bytes
    uint8_t push_reg_nums_[11] {};           // Register numbers in push/STP order
    uint32_t num_push_regs_ = 0;             // Number of push regs (3 + num_cache_regs_)
    uint32_t prologue_sub_rsp_size_ = 0;     // K value in sub rsp, K

    // .eh_frame DWARF CFI offset (Linux x64), 0 = not emitted.
    uint32_t eh_frame_offset_ = 0;

    // Pointer to JitMethod::stale for HotUpdate inline PIC stale checking.
    // Set during Generate() before instruction emission; read by EmitInstruction
    // to embed the address as an immediate for runtime stale flag checks.
    void* stale_flag_ptr_ = nullptr;

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

    // Bitmask of FPR vregs colored to caller-saved XMM registers.
    // Same strategy as caller_colored_mask_ but for XMM0-XMM5 on x64
    // (V0-V7 on ARM64). EmitCallWithSpill reloads after each call.
    uint64_t caller_fpr_colored_mask_ = 0;

    // True if any IR instruction in the method can clobber caller-saved
    // registers at runtime: a managed/runtime call (Call/CallVirt/CallBridge/
    // Calli), a safepoint poll (enable_safepoint_polls), or an overflow-checked
    // op (AddOvf/SubOvf/MulOvf/ConvOvf*) whose deopt path reads caller-colored
    // vreg stack slots.  When false, caller-colored vregs never need the
    // write-through in StoreGpr/StoreFpr — their value stays purely in the
    // colored register (caller-saved) across the whole call-free method,
    // eliminating the per-op stack roundtrip (T2.1 A1).
    bool has_caller_clobber_ = true;

    // LocAlloc: extra frame bytes (bump counter + reserve) when method uses LocAlloc.
    uint32_t localloc_extra_ = 0;

    // Frame extension for tree-optimizer-created vregs.
    // The GPR file area only covers vregs 0-63 (kGprCount * 8 = 512 bytes).
    // Tree-created vregs start at 64; when they exceed kFrameSize, this
    // extension shifts saved-register / XMM-save / localloc areas upward
    // to prevent StoreGpr(vreg) from overwriting saved ret_buf at [SP+kFrameSize].
    uint32_t frame_size_extra_ = 0;

    void SelectCacheableRegs() noexcept;
    void SpillCachedRegs() noexcept;
    void SpillGcRefCachedRegs() noexcept;
    void EmitCallWithSpill(uint8_t reg) noexcept;
    template <typename T>
    uint32_t EmitRuntimeHelperCall(T* target_fn) noexcept {
        return EmitRuntimeHelperCallImpl(reinterpret_cast<void*>(target_fn));
    }
    uint32_t EmitRuntimeHelperCallImpl(void* target_fn) noexcept;

    void LoadGpr(uint8_t x64_reg, uint32_t vreg) noexcept;
    void StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept;
    void LoadFpr(uint8_t xmm_reg, uint32_t vreg) noexcept;
    void StoreFpr(uint8_t xmm_reg, uint32_t vreg) noexcept;
    void EmitSafepointPoll() noexcept;
    void EmitInlineDirtyCard(uint8_t obj_reg) noexcept;
    void EmitGprArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;
    void EmitFprArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;
    void EmitBitwise(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;
    void EmitShift(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2, int32_t imm) noexcept;
    void ResolveBranches() noexcept;
    bool EmitInstruction(const interpreter::RegisterInstruction& instr) noexcept;
    bool EmitSimd(const interpreter::RegisterInstruction& instr, uint8_t simd_op, uint8_t elem_type,
                  uint16_t simd_imm) noexcept;
    void EmitDeoptSequence(uint32_t instr_pc, uint32_t osr_resume_pc = 0) noexcept;
    void DumpCode() noexcept;
    void RecordGcPoint(uint32_t native_offset) noexcept;

    /// Returns true when an OOM or other unrecoverable error has occurred.
    /// Emit helpers check buf_.failed() internally; this is a combined check
    /// so Generate() can bail out early after any major emit section.
    bool CheckFailed() noexcept {
        if (buf_.failed())
            failed_ = true;
        return failed_;
    }
};

void NativeCodeGenerator::LoadGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LoadGpr");
    if (vreg >= interpreter::kGPRegisters) {
        enc_.EmitMovRM(x64_reg, AT::kStackReg, static_cast<int32_t>(GprOff(vreg)));
        return;
    }

    // Graph coloring V2: colored vreg → direct reg-to-reg move
    if (has_graph_coloring_) {
        uint8_t colored_x64 = gcr_.gpr_color[vreg];
        if (colored_x64 != 0xFF) {
            if (x64_reg != colored_x64)
                enc_.EmitMovRR(x64_reg, colored_x64);
            return;
        }
    }
    // V1 cache hit
    if (config_.enable_register_caching) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != kNotCached) {
            if (x64_reg != cached)
                enc_.EmitMovRR(x64_reg, cached);
            return;
        }
    }
    // Load from stack
    enc_.EmitMovRM(x64_reg, AT::kStackReg, static_cast<int32_t>(GprOff(vreg)));
}

void NativeCodeGenerator::StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StoreGpr");
    if (vreg >= interpreter::kGPRegisters) {
        enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), x64_reg);
        return;
    }

    // Graph coloring V2: colored vreg → direct reg-to-reg move (no stack write)
    if (has_graph_coloring_) {
        uint8_t colored_x64 = gcr_.gpr_color[vreg];
        if (colored_x64 != 0xFF) {
            if (x64_reg != colored_x64)
                enc_.EmitMovRR(colored_x64, x64_reg);
            // Caller-colored vregs: write through to stack so the stack slot
            // holds the correct value even if argument setup clobbers the
            // colored register before EmitCallWithSpill's pre-call spill.
            // T2.1 A1: on a call-free method (no calls, no safepoints, no
            // overflow-check deopt sites), nothing can clobber the caller-saved
            // register, so the value stays purely resident — skip the store.
            if ((caller_colored_mask_ & (1ULL << vreg)) && has_caller_clobber_)
                enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), colored_x64);
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
                    if (kCacheableRegs[slot] == cached)
                        break;
                }
                if (slot < kMaxCacheRegs)
                    cached_dirty_mask_ |= (1u << slot);
                return;
            }
            enc_.EmitMovRR(cached, x64_reg);
            uint32_t slot = 0;
            for (; slot < kMaxCacheRegs; ++slot) {
                if (kCacheableRegs[slot] == cached)
                    break;
            }
            if (slot < kMaxCacheRegs)
                cached_dirty_mask_ |= (1u << slot);
            return;
        }
    }
    // Not cached/spilled: write through to stack
    enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), x64_reg);
}

void NativeCodeGenerator::LoadFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    // Graph coloring V2: colored FPR → direct reg-to-reg move
    if (has_graph_coloring_ && vreg >= kGprCount) {
        uint32_t fi = vreg - kGprCount;
        if (fi < 32) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (xmm_reg != colored_xmm)
                    enc_.EmitMovdqaRR(xmm_reg, colored_xmm);
                return;
            }
        }
    }
    // Fallback: load 128-bit from stack via movdqa
    enc_.EmitMovdqaRM(xmm_reg, AT::kStackReg, static_cast<int32_t>(FprOff(vreg)));
}

void NativeCodeGenerator::StoreFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    // Graph coloring V2: colored FPR → direct reg-to-reg move (128-bit movdqa, no stack write)
    if (has_graph_coloring_ && vreg >= kGprCount) {
        uint32_t fi = vreg - kGprCount;
        if (fi < 32) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (xmm_reg != colored_xmm)
                    enc_.EmitMovdqaRR(colored_xmm, xmm_reg);
                // Caller-colored FPRs: write through to stack so the stack slot
                // holds the correct value even if argument setup clobbers the
                // colored register before EmitCallWithSpill's pre-call spill.
                // T2.1 A1: skip on a call-free method (no caller-clobber sites).
                if ((caller_fpr_colored_mask_ & (1ULL << fi)) && has_caller_clobber_)
                    enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(vreg)), colored_xmm);
                return;
            }
        }
    }
    // Fallback: write 128-bit to stack via movdqa
    enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(vreg)), xmm_reg);
}

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

void NativeCodeGenerator::EmitGprArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitGprArithmetic");
    // Div/Rem have implicit eax/edx/ecx register requirements.
    bool has_implicit =
        (opc == IROpCode::Div || opc == IROpCode::Rem || opc == IROpCode::DivUn || opc == IROpCode::RemUn);
    uint8_t op_reg = AT::kScratchA;
    uint8_t src2_reg = AT::kScratchB;
    if (has_graph_coloring_ && !has_implicit) {
        if (dst < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[dst];
            if (c != 0xFF)
                op_reg = c;
        }
        if (src2 != UINT32_MAX && src2 < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[src2];
            if (c != 0xFF)
                src2_reg = c;
        }
    }
    // When op_reg and src2_reg share the same colored register (dst and src2
    // assigned the same x64 register by graph coloring) and src1 != src2,
    // loading src1 into op_reg destroys src2's value.  Load src2 into a
    // scratch register FIRST, then load src1, so the computation reads the
    // correct src2 value even after op_reg overwrites the shared register.
    bool src2_loaded = false;
    if (has_graph_coloring_ && !has_implicit && op_reg == src2_reg && src1 != src2) {
        src2_reg = (op_reg == AT::kScratchB) ? AT::kScratchA : AT::kScratchB;
        LoadGpr(src2_reg, src2);
        src2_loaded = true;
    }
    LoadGpr(op_reg, src1);
    if (src2 != UINT32_MAX && !src2_loaded)
        LoadGpr(src2_reg, src2);
    if (opc == IROpCode::Add) {
        enc_.EmitAdd32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Sub) {
        enc_.EmitSub32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Mul) {
        enc_.EmitImul32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Neg) {
#if defined(__aarch64__)
        // ARM64: 32-bit NEG (NEG Wd,Wm) doesn't zero-extend upper 32 bits of Xd,
        // unlike x64 where 32-bit register destination always zero-extends to 64 bits.
        // Use 64-bit NEG so the full register holds the sign-extended result.
        enc_.EmitNeg(op_reg);
#else
        enc_.EmitNeg32(op_reg);
#endif
    } else if (opc == IROpCode::Div || opc == IROpCode::Rem) {
#if defined(__aarch64__)
        // ARM64: SDIV 32-bit signed.  WscratchA = dividend, WscratchB = divisor (pre-loaded).
        enc_.EmitMovRR(AT::kScratchC, AT::kScratchA); // save dividend
        ::chaos::il2cpp::jit::EmitSdiv32(buf_, AT::kScratchA, AT::kScratchA,
                                         AT::kScratchB); // quotient = dividend / divisor
        if (opc == IROpCode::Rem) {
            ::chaos::il2cpp::jit::EmitMul32(buf_, AT::kScratchB, AT::kScratchA,
                                            AT::kScratchB); // WscratchB = quotient * divisor
            enc_.EmitSub32RR(AT::kScratchC, AT::kScratchB); // remainder = original_dividend - quotient*divisor
            enc_.EmitMovRR(op_reg, AT::kScratchC);
        }
#else
        EmitREXB(buf_, false, 0);
        buf_.EmitByte(0x99); // cdq: sign-extend eax→edx:eax
        EmitREX(buf_, false, 7, AT::kScratchB);
        buf_.EmitByte(0xF7);
        buf_.EmitByte(ModRM(3, 7, AT::kScratchB)); // idiv ecx
        if (opc == IROpCode::Rem)
            enc_.EmitMovRR(op_reg, AT::kScratchC);
#endif
    } else if (opc == IROpCode::DivUn || opc == IROpCode::RemUn) {
#if defined(__aarch64__)
        // ARM64: UDIV 32-bit unsigned
        enc_.EmitMovRR(AT::kScratchC, AT::kScratchA); // save dividend
        ::chaos::il2cpp::jit::EmitUdiv32(buf_, AT::kScratchA, AT::kScratchA,
                                         AT::kScratchB); // quotient = dividend / divisor
        if (opc == IROpCode::RemUn) {
            ::chaos::il2cpp::jit::EmitMul32(buf_, AT::kScratchB, AT::kScratchA,
                                            AT::kScratchB); // WscratchB = quotient * divisor
            enc_.EmitSub32RR(AT::kScratchC, AT::kScratchB); // remainder = original_dividend - quotient*divisor
            enc_.EmitMovRR(op_reg, AT::kScratchC);
        }
#else
        enc_.EmitXor32ZR(AT::kScratchC); // xor edx, edx
        EmitREX(buf_, false, 6, AT::kScratchB);
        buf_.EmitByte(0xF7);
        buf_.EmitByte(ModRM(3, 6, AT::kScratchB)); // div ecx
        if (opc == IROpCode::RemUn)
            enc_.EmitMovRR(op_reg, AT::kScratchC);
#endif
    }
    StoreGpr(op_reg, dst);
}

void NativeCodeGenerator::EmitFprArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
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
    if (opc == IROpCode::Add)
        enc_.EmitAddSDRR(op_xmm, src2_xmm);
    else if (opc == IROpCode::Sub)
        enc_.EmitSubSDRR(op_xmm, src2_xmm);
    else if (opc == IROpCode::Mul)
        enc_.EmitMulSDRR(op_xmm, src2_xmm);
    else if (opc == IROpCode::Div)
        enc_.EmitDivSDRR(op_xmm, src2_xmm);
    StoreFpr(op_xmm, dst);
}

void NativeCodeGenerator::EmitBitwise(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitBitwise");
    uint8_t op_reg = AT::kScratchA;
    uint8_t src2_reg = AT::kScratchB;
    if (has_graph_coloring_) {
        if (dst < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[dst];
            if (c != 0xFF)
                op_reg = c;
        }
        if (src2 != UINT32_MAX && src2 < interpreter::kGPRegisters) {
            uint8_t c = gcr_.gpr_color[src2];
            if (c != 0xFF)
                src2_reg = c;
        }
    }
    // Same collision guard as EmitGprArithmetic: when op_reg and src2_reg
    // share a color and src1 != src2, load src2 into a scratch register first.
    bool src2_loaded = false;
    if (has_graph_coloring_ && op_reg == src2_reg && src1 != src2) {
        src2_reg = (op_reg == AT::kScratchB) ? AT::kScratchA : AT::kScratchB;
        LoadGpr(src2_reg, src2);
        src2_loaded = true;
    }
    LoadGpr(op_reg, src1);
    if (opc == IROpCode::Not)
        enc_.EmitNot32(op_reg);
    else if (opc == IROpCode::And) {
        if (src2 != UINT32_MAX && !src2_loaded) {
            LoadGpr(src2_reg, src2);
        }
        enc_.EmitAnd32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Or) {
        if (src2 != UINT32_MAX && !src2_loaded) {
            LoadGpr(src2_reg, src2);
        }
        enc_.EmitOr32RR(op_reg, src2_reg);
    } else if (opc == IROpCode::Xor) {
        if (src2 != UINT32_MAX && !src2_loaded) {
            LoadGpr(src2_reg, src2);
        }
        enc_.EmitXor32RR(op_reg, src2_reg);
    }
    StoreGpr(op_reg, dst);
}

// ── Shift with proper 32-bit semantics ─────────────────────────────────
// RegisterExecute uses int32_t/uint32_t for shift operations, which means:
//   Shr (signed):   (int32_t)RAX >> CL  → sign-extend to 64-bit
//   ShrUn (unsigned): (uint32_t)RAX >> CL → zero-extend to 64-bit
// x64 32-bit ops automatically zero-extend to 64 bits.
void NativeCodeGenerator::EmitShift(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2, int32_t imm) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitShift");
    // For variable shifts (src2 present), shift count must be in CL (AT::kScratchB),
    // but the destination can be any register. For immediate shifts, any GPR works.
    uint8_t op_reg = AT::kScratchA;
    if (has_graph_coloring_ && dst < interpreter::kGPRegisters) {
        uint8_t c = gcr_.gpr_color[dst];
        if (c != 0xFF)
            op_reg = c;
    }
    // Collision guard (mirrors EmitGprArithmetic): a variable shift computes
    // "dst = src1 << src2", where src2 is the shift-count vreg that must land
    // in CL (AT::kScratchB).  When src2's graph color equals op_reg
    // (e.g. in-place "dst = src1 << dst", where dst and src2 are the same vreg
    // and therefore share a color), loading src1 into op_reg first would
    // clobber src2's value before the shift count is captured into CL.  Load
    // the shift count into CL FIRST, then load src1 into op_reg, so the count
    // is read from the true src2 value, not from the just-loaded src1.
    bool load_src1_first = true;
    if (has_graph_coloring_ && src2 != UINT32_MAX && src2 < interpreter::kGPRegisters) {
        uint8_t src2_color = gcr_.gpr_color[src2];
        if (src2_color != 0xFF && src2_color == op_reg)
            load_src1_first = false;
    }
#if defined(__aarch64__)
    if (src2 != UINT32_MAX) {
        if (!load_src1_first)
            LoadGpr(AT::kScratchB, src2);
        LoadGpr(op_reg, src1);
        if (load_src1_first)
            LoadGpr(AT::kScratchB, src2);
        if (opc == IROpCode::Shl) {
            enc_.EmitShlRCL(op_reg);
        } else if (opc == IROpCode::Shr) {
            enc_.EmitSarRCL(op_reg); // signed → arithmetic
        } else if (opc == IROpCode::ShrUn) {
            enc_.EmitShrRCL(op_reg); // unsigned → logical
        }
    } else {
        uint8_t shift = static_cast<uint8_t>(imm & 0x1F);
        if (opc == IROpCode::Shl) {
            enc_.EmitShlRI(op_reg, shift);
        } else if (opc == IROpCode::Shr) {
            enc_.EmitSarRI(op_reg, shift);
        } else if (opc == IROpCode::ShrUn) {
            enc_.EmitShrRI(op_reg, shift);
        }
    }
#else
    if (src2 != UINT32_MAX) {
        if (!load_src1_first)
            LoadGpr(AT::kScratchB, src2);
        LoadGpr(op_reg, src1);
        if (load_src1_first)
            LoadGpr(AT::kScratchB, src2);
        EmitREXB(buf_, false, op_reg); // REX.B for extended destination register
        if (opc == IROpCode::Shl) {
            buf_.EmitByte(0xD3);
            buf_.EmitByte(ModRM(3, 4, op_reg));
        } else if (opc == IROpCode::Shr) {
            buf_.EmitByte(0xD3);
            buf_.EmitByte(ModRM(3, 7, op_reg));
        } else if (opc == IROpCode::ShrUn) {
            buf_.EmitByte(0xD3);
            buf_.EmitByte(ModRM(3, 5, op_reg));
        }
    } else {
        uint8_t shift = static_cast<uint8_t>(imm & 0x1F);
        if (opc == IROpCode::Shl) {
            EmitREX(buf_, false, 4, op_reg);
            buf_.EmitByte(0xC1);
            buf_.EmitByte(ModRM(3, 4, op_reg));
            buf_.EmitByte(shift);
        } else if (opc == IROpCode::Shr) {
            EmitREX(buf_, false, 7, op_reg);
            buf_.EmitByte(0xC1);
            buf_.EmitByte(ModRM(3, 7, op_reg));
            buf_.EmitByte(shift);
        } else if (opc == IROpCode::ShrUn) {
            EmitREX(buf_, false, 5, op_reg);
            buf_.EmitByte(0xC1);
            buf_.EmitByte(ModRM(3, 5, op_reg));
            buf_.EmitByte(shift);
        }
    }
#endif
    StoreGpr(op_reg, dst);
}

void NativeCodeGenerator::ResolveBranches() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::ResolveBranches");
#if defined(__aarch64__)
    for (auto& bp : branch_patches_) {
        if (bp.target_instr >= instr_offsets_.size()) {
            // Out of bounds target — patch to B #4 (skip next instruction, i.e. no-op).
            buf_.Patch32(bp.patch_offset, 0x14000001u); // B #4
            continue;
        }
        uint32_t target_off = instr_offsets_[bp.target_instr];
        uint32_t current_off = bp.patch_offset;
        int64_t disp = static_cast<int64_t>(target_off) - static_cast<int64_t>(current_off);
        uint32_t instr = buf_.Load32(bp.patch_offset);
        uint8_t opcode = static_cast<uint8_t>(instr >> 24);
        if (opcode == 0x54) {
            // B.cond (condition code at bits [0:4], imm19 at bits [5:23])
            int64_t max_disp = 1048576; // ±1MB
            if (disp < -max_disp || disp > max_disp) {
                // Out of range — emit trampoline B at end of buffer
                uint32_t tramp_off = buf_.pos();
                int64_t tramp_disp = static_cast<int64_t>(target_off) - static_cast<int64_t>(tramp_off + 4);
                uint32_t tramp_imm26 = (static_cast<uint32_t>(tramp_disp) >> 2) & 0x3FFFFFF;
                buf_.Emit32(0x14000000u | tramp_imm26); // B target
                // Redirect original B.cond to trampoline
                disp = static_cast<int64_t>(tramp_off) - current_off;
            }
            uint32_t imm19 = (static_cast<uint32_t>(disp) >> 2) & 0x7FFFF;
            uint32_t new_instr = (instr & 0xFF00001Fu) | (imm19 << 5);
            buf_.Patch32(bp.patch_offset, new_instr);
        } else {
            // B (unconditional, 26-bit imm26, ±128MB range)
            uint32_t imm26 = (static_cast<uint32_t>(disp) >> 2) & 0x3FFFFFF;
            uint32_t new_instr = (instr & 0xFC000000u) | imm26;
            buf_.Patch32(bp.patch_offset, new_instr);
        }
    }
#else
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
#endif
    uint32_t deopt_ret_off = deopt_return_pos_;
    for (auto& djp : deopt_jump_patches_) {
        int32_t disp = static_cast<int32_t>(deopt_ret_off - (djp.patch_offset + 4));
        buf_.Patch32(djp.patch_offset, static_cast<uint32_t>(disp));
    }
    // Patch jump table entries: each stores target_off - table_base
    for (auto& jtp : jump_table_patches_) {
        uint32_t target_off = jtp.target_instr < instr_offsets_.size() ? instr_offsets_[jtp.target_instr] : 0;
        int32_t disp = static_cast<int32_t>(target_off - jtp.table_base);
        buf_.Patch32(jtp.table_entry_offset, static_cast<uint32_t>(disp));
    }
}

void NativeCodeGenerator::EmitDeoptSequence(uint32_t instr_pc, uint32_t osr_resume_pc) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitDeoptSequence");
    uint32_t deopt_pos = buf_.pos();
    if (config_.enable_deopt) {
        // Spill any dirty cached regs so the stack frame is consistent
        if (config_.enable_register_caching && cached_slots_used_)
            SpillCachedRegs();

        uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            DeoptValue dv;
            dv.reg_index = vr;
            // Use inferred type from vreg_types_ (kType* constants match ValueTag exactly)
            dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                             ? vreg_types_[vr]
                             : static_cast<uint8_t>(interpreter::ValueTag::Int64);
            dv.is_spilled = true;
            dv.spill_offset = static_cast<int16_t>(GprOff(vr));
            deopt_values_.push_back(dv);
        }
        for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
            DeoptValue dv;
            dv.reg_index = vr;
            dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
            dv.is_spilled = true;
            dv.spill_offset = static_cast<int16_t>(FprOff(vr));
            deopt_values_.push_back(dv);
        }
        uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
        DeoptEntry entry;
        entry.native_offset = deopt_pos;
        entry.instr_pc = instr_pc;
        entry.osr_resume_pc = osr_resume_pc;
        entry.num_values = n_vals;
        entry.values_offset = val_start;
        deopt_entries_.push_back(entry);

        // Emit CALL to DeoptSaveFrameState(RSP) — saves all register values
        // and type tags from the stack frame to t_deopt_state before we
        // write kDeoptMagic and return to InterpreterEntryDirect.
        enc_.EmitMovRR(AT::kScratchB, AT::kStackReg); // RCX = codegen_rsp
        enc_.EmitSubRI(AT::kStackReg, 32);            // shadow space for Win64 callee
        EmitRuntimeHelperCall(::DeoptSaveFrameState);
        enc_.EmitAddRI(AT::kStackReg, 32); // restore shadow space
    }
    enc_.EmitMovImm64(AT::kScratchA, kDeoptMagic);
    // kRetBuf (RSI/X4) was set to the local GPR file in the prologue, not the
    // original ABI ret_buf from the caller.  Load the saved ret_buf from the
    // stack frame ([kFrameReg - 16] = saved RSI on x64, saved X1 on ARM64)
    // and write kDeoptMagic through it so ExecuteNative can read it.
#if defined(__aarch64__)
    // ARM64: saved ABI ret_buf (X1) at [X29 - 16].
    EmitLdur64(buf_, AT::kScratchC, AT::kFrameReg, -16);
    EmitStr64(buf_, AT::kScratchA, AT::kScratchC, 0);
#else
    enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16);
    enc_.EmitMovMR(AT::kScratchC, 0, AT::kScratchA);
#endif
    uint32_t patch_off = buf_.pos() + 1;
    enc_.EmitJmpRel32(0);
    deopt_jump_patches_.push_back({patch_off});
}

// ── Register caching V1 ──────────────────────────────────────────────────
void NativeCodeGenerator::SelectCacheableRegs() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::SelectCacheableRegs");
    if (!config_.enable_register_caching)
        return;

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
    std::memset(phys_to_cached_vreg_, kNotCached, sizeof(phys_to_cached_vreg_));
    cached_slots_used_ = 0;
    cached_dirty_mask_ = 0;
    num_cache_regs_ = 0;

    // Select top-k vregs by frequency. Since kMaxCacheRegs is small (4), a
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
        if (best_vreg == kNotCached || best_freq == 0)
            break;

        uint8_t x64r = kCacheableRegs[slot];
        cached_x64_for_vreg_[best_vreg] = x64r;
        phys_to_cached_vreg_[x64r] = static_cast<uint8_t>(best_vreg);
        cached_slots_used_ |= (1u << slot);
        ++num_cache_regs_;
    }
}

void NativeCodeGenerator::SpillCachedRegs() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::SpillCachedRegs");
    if (!config_.enable_register_caching || cached_slots_used_ == 0)
        return;
    if (cached_dirty_mask_ == 0)
        return;

    // For each dirty slot, write the cached x64 register to the vreg's stack slot
    uint32_t dirty = cached_dirty_mask_;
    for (uint32_t slot = 0; slot < kMaxCacheRegs && dirty; ++slot) {
        if (!(dirty & (1u << slot)))
            continue;
        dirty &= ~(1u << slot);
        uint8_t x64r = static_cast<uint8_t>(kCacheableRegs[slot]);
        uint32_t vreg = phys_to_cached_vreg_[x64r];
        if (vreg != kNotCached) {
            enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), x64r);
        }
    }
    cached_dirty_mask_ = 0;
}

void NativeCodeGenerator::SpillGcRefCachedRegs() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::SpillGcRefCachedRegs");
    if (!config_.enable_register_caching || cached_slots_used_ == 0)
        return;
    if (cached_dirty_mask_ == 0)
        return;

    // Only spill cache slots holding GC object references. Non-GC ref values
    // (int32, int64, float, etc.) stay in registers across safepoints since
    // the GC only needs to scan object references on the stack.
    uint32_t dirty = cached_dirty_mask_;
    for (uint32_t slot = 0; slot < kMaxCacheRegs && dirty; ++slot) {
        if (!(dirty & (1u << slot)))
            continue;
        dirty &= ~(1u << slot);
        uint8_t x64r = static_cast<uint8_t>(kCacheableRegs[slot]);
        uint32_t vreg = phys_to_cached_vreg_[x64r];
        if (vreg != kNotCached && vreg < vreg_types_.size() && vreg_types_[vreg] == kTypeObjectRef) {
            enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), x64r);
            cached_dirty_mask_ &= ~(1u << slot);
        }
    }
}

void NativeCodeGenerator::EmitCallWithSpill(uint8_t reg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitCallWithSpill");
    if (config_.enable_register_caching && cached_slots_used_)
        SpillCachedRegs();
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
                if (caller_colored_mask_ & (1ULL << vr))
                    continue;
                enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vr)), colored_x64);
            }
        }
        // Spill colored FPRs (same logic: StoreFpr with graph coloring skips
        // the stack write).  Caller-colored FPRs are excluded — StoreFpr
        // already does write-through for them.
        for (uint32_t fi = 0; fi < 32; ++fi) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (caller_fpr_colored_mask_ & (1ULL << fi))
                    continue;
                enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)), colored_xmm);
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
                enc_.EmitMovRM(colored_x64, AT::kStackReg, static_cast<int32_t>(GprOff(vr)));
            }
            mask >>= 1;
        }
    }
    // Post-call reload: restore caller-saved colored FPRs.
    if (has_graph_coloring_ && caller_fpr_colored_mask_) {
        uint64_t mask = caller_fpr_colored_mask_;
        for (uint32_t fi = 0; mask; ++fi) {
            if (mask & 1) {
                uint8_t colored_xmm = gcr_.fpr_color[fi];
                enc_.EmitMovdqaRM(colored_xmm, AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)));
            }
            mask >>= 1;
        }
    }
}

uint32_t NativeCodeGenerator::EmitRuntimeHelperCallImpl(void* target_fn) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::RuntimeHelperCall");
    if (config_.enable_register_caching && cached_slots_used_)
        SpillCachedRegs();

    // On Linux (System V ABI), convert args from Win64 positions (RCX, RDX, R8, R9)
    // to SysV positions (RDI, RSI, RDX, RCX, R8, R9). The JIT engine always sets up
    // args in Win64 positions internally.
#if !defined(_WIN32) && !defined(_WIN64) && defined(__x86_64__)
    // EmitMovRR(dst, src) — dst and src are x86-64 register numbers
    enc_.EmitMovRR(7, 1); // RDI = RCX  (arg1 fixup — RCX→RDI)
    enc_.EmitMovRR(6, 2); // RSI = RDX  (arg2 fixup — RDX→RSI)
    enc_.EmitMovRR(2, 8); // RDX = R8   (arg3 fixup — R8→RDX)
    // R9 stays in R9 (SysV arg6 = R9, same as Win64 arg4 — no 4-arg helpers currently)
#endif

    if (has_graph_coloring_) {
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            uint8_t colored_x64 = gcr_.gpr_color[vr];
            if (colored_x64 != 0xFF) {
                if (caller_colored_mask_ & (1ULL << vr))
                    continue;
                enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vr)), colored_x64);
            }
        }
        // Spill colored FPRs (same logic: StoreFpr skips stack write).
        for (uint32_t fi = 0; fi < 32; ++fi) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (caller_fpr_colored_mask_ & (1ULL << fi))
                    continue;
                enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)), colored_xmm);
            }
        }
    }
    uint32_t call_start = buf_.pos();
    enc_.EmitCallRipRel(0);
    slot_patches_.push_back({call_start + kSlotPatchDispOff, UINT32_MAX, target_fn});
    slot_count_used_++;
    if (has_graph_coloring_ && caller_colored_mask_) {
        uint64_t mask = caller_colored_mask_;
        for (uint32_t vr = 0; mask; ++vr) {
            if (mask & 1) {
                uint8_t colored_x64 = gcr_.gpr_color[vr];
                enc_.EmitMovRM(colored_x64, AT::kStackReg, static_cast<int32_t>(GprOff(vr)));
            }
            mask >>= 1;
        }
    }
    // Post-call reload: restore caller-saved colored FPRs.
    if (has_graph_coloring_ && caller_fpr_colored_mask_) {
        uint64_t mask = caller_fpr_colored_mask_;
        for (uint32_t fi = 0; mask; ++fi) {
            if (mask & 1) {
                uint8_t colored_xmm = gcr_.fpr_color[fi];
                enc_.EmitMovdqaRM(colored_xmm, AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)));
            }
            mask >>= 1;
        }
    }
    return call_start;
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
            if (!instr.has_dst())
                return false;
            // Load int32_t zero-extended to 64-bit, matching x64 mov r32,imm32
            // and interpreter WRITE_REG semantics.
#if defined(__aarch64__)
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>(instr.imm.i4));
#else
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>(instr.imm.i4));
#endif
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdcI8: {
            if (!instr.has_dst())
                return false;
            int64_t val = instr.imm.i8;
            if (val >= INT32_MIN && val <= INT32_MAX)
                enc_.EmitMovRI32(AT::kScratchA, static_cast<int32_t>(val));
            else
                enc_.EmitMovImm64(AT::kScratchA, static_cast<uint64_t>(val));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdcR4: {
            if (!instr.has_dst())
                return false;
            // Load float constant, promote to double (matching RegisterExecute's
            // WriteFloat64(static_cast<double>(v), dst) semantics).
            float v;
            std::memcpy(&v, &instr.imm.i4, sizeof(v));
            uint32_t bits;
            std::memcpy(&bits, &v, sizeof(bits));
            enc_.EmitMovRIImm32(AT::kScratchA, bits);
            enc_.EmitMovdXrm(0, AT::kScratchA);
            enc_.EmitCvtss2sd(0, 0); // promote float→double
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::LdcR8: {
            if (!instr.has_dst())
                return false;
            uint64_t bits;
            std::memcpy(&bits, &instr.imm.r8, sizeof(bits));
            enc_.EmitMovImm64(AT::kScratchA, bits);
            enc_.EmitMovqXrm(0, AT::kScratchA);
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::LdNull: {
            if (!instr.has_dst())
                return false;
            enc_.EmitXorZR(AT::kScratchA);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdArg: {
            if (!instr.has_dst())
                return false;
            uint32_t arg_idx = instr.imm.operand_index;
            enc_.EmitMovRM(AT::kScratchA, AT::kArgsBuf, static_cast<int32_t>(arg_idx * 8));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdLoc: {
            if (!instr.has_dst())
                return false;
            uint32_t local_idx = instr.imm.operand_index;
            uint32_t vreg = kLocalRegBase + local_idx;
            if (vreg < kGprCount) {
                LoadGpr(AT::kScratchA, vreg);
                StoreGpr(AT::kScratchA, instr.dst_reg());
            }
            return true;
        }

        case IROpCode::StLoc: {
            if (!instr.has_src1())
                return false;
            uint32_t local_idx = instr.imm.operand_index;
            uint32_t vreg = kLocalRegBase + local_idx;
            if (vreg < kGprCount) {
                LoadGpr(AT::kScratchA, instr.src1_reg());
                StoreGpr(AT::kScratchA, vreg);
                if (instr.has_dst())
                    StoreGpr(AT::kScratchA, instr.dst_reg());
            }
            return true;
        }

        case IROpCode::StArg: {
            if (!instr.has_src1())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitMovMR(AT::kArgsBuf, static_cast<int32_t>(instr.imm.operand_index * 8), AT::kScratchA);
            return true;
        }

        case IROpCode::Ret: {
            // Spill cached regs before reading return value
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            if (instr.has_src1()) {
                LoadGpr(AT::kScratchA, instr.src1_reg());
#if defined(__aarch64__)
                // Load saved ABI ret_buf (X1) from [X29, #-16], saved right after
                // MOV X29, SP in the prologue.  Write the return value through it
                // (matching x64 saved-RSI reload in the Ret handler).
                EmitLdur64(buf_, AT::kScratchC, AT::kFrameReg, -16); // LDR XscratchC, [X29, #-16]
                EmitStr64(buf_, AT::kScratchA, AT::kScratchC, 0);    // STR Xretval, [XscratchC]
#else
                // kRetBuf (RSI) is caller-saved and may have been clobbered by EnterCooperativeMode
                enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16);
                enc_.EmitMovMR(AT::kScratchC, 0, AT::kScratchA);
#endif
            }
            // Restore callee-saved XMMs
            for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
                int32_t off = static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + si * 16);
                enc_.EmitMovUPRM(callee_xmm_regs_[si], AT::kStackReg, off);
            }
#if defined(__aarch64__)
            enc_.EmitAddRI(AT::kStackReg, static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ +
                                                               xmm_save_size_ + localloc_extra_));
            for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
                EmitLdp64Post(buf_, callee_saved_regs_[slot - 1], 0, kARM64_SP,
                              16);                    // LDP Xreg, X0, [SP], #16 (X0=scratch, never XZR)
            EmitLdp64Post(buf_, 1, 0, kARM64_SP, 16); // LDP X1, X0, [SP], #16 (discard saved ABI ret_buf & scratch)
            EmitLdp64Post(buf_, AT::kFrameReg, 30, kARM64_SP, 16); // LDP X29, X30, [SP], #16
            enc_.EmitRet();
#else
            enc_.EmitAddRI(AT::kStackReg, static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ +
                                                               xmm_save_size_ + localloc_extra_));
            for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
                enc_.EmitPop(callee_saved_regs_[slot - 1]);
            enc_.EmitPop(kRDI);
            enc_.EmitPop(AT::kRetBuf);
            enc_.EmitPop(AT::kArgsBuf);
            enc_.EmitPop(AT::kFrameReg);
            enc_.EmitRet();
#endif
            return true;
        }

        case IROpCode::Add:
        case IROpCode::Sub:
        case IROpCode::Mul:
        case IROpCode::Div:
        case IROpCode::Rem:
        case IROpCode::Neg:
        case IROpCode::DivUn:
        case IROpCode::RemUn: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            if (instr.src1_reg() < vreg_types_.size() && vreg_types_[instr.src1_reg()] >= kTypeFloat32)
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
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
#if defined(__aarch64__)
            // ARM64 abs(Wd): CMP Wsrc, #0 → CSNEG Wd, Wsrc, Wsrc, GE
            // Save original in scratchC, negate to scratchA, conditionally select
            enc_.EmitMovRR(AT::kScratchC, AT::kScratchA);          // copy val
            enc_.EmitNeg32(AT::kScratchA);                         // scratchA = -val
            enc_.EmitCmp32RR(AT::kScratchC, 31);                   // CMP WscratchC, WZR (31)
            enc_.EmitCmovcc(kCC_GE, AT::kScratchA, AT::kScratchC); // if ≥0, restore original
#else
            buf_.EmitByte(0x99);                            // cdq
            enc_.EmitXor32RR(AT::kScratchA, AT::kScratchC); // eax ^= edx
            enc_.EmitSub32RR(AT::kScratchA, AT::kScratchC); // eax -= edx
#endif
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Min: {
            // int32 min: dst = (src1 > src2) ? src2 : src1 → cmovg
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            LoadGpr(AT::kScratchB, instr.src2_reg());
            enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
            enc_.EmitCmovcc(kCC_G, AT::kScratchA, AT::kScratchB); // if a > b, a = b
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Max: {
            // int32 max: dst = (src1 < src2) ? src2 : src1 → cmovl
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            LoadGpr(AT::kScratchB, instr.src2_reg());
            enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
            enc_.EmitCmovcc(kCC_L, AT::kScratchA, AT::kScratchB); // if a < b, a = b
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::AddOvf:
        case IROpCode::SubOvf:
        case IROpCode::MulOvf: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            uint8_t op_reg = AT::kScratchA;
            uint8_t src2_reg = AT::kScratchB;
            if (has_graph_coloring_) {
                if (instr.dst_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.dst_reg()];
                    if (c != 0xFF)
                        op_reg = c;
                }
                if (instr.has_src2() && instr.src2_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src2_reg()];
                    if (c != 0xFF)
                        src2_reg = c;
                }
            }
            LoadGpr(op_reg, instr.src1_reg());
            if (instr.has_src2())
                LoadGpr(src2_reg, instr.src2_reg());
            // Use 32-bit operations so x64 OF flag reflects 32-bit
            // signed overflow, matching IL add.ovf/sub.ovf/mul.ovf semantics.
#if defined(__aarch64__)
            if (opc == IROpCode::AddOvf) {
                // ADDS Wd, Wn, Wm — sets V flag on signed 32-bit overflow.
                EmitAdds32(buf_, op_reg, op_reg, src2_reg);
            } else if (opc == IROpCode::SubOvf) {
                // SUBS Wd, Wn, Wm — sets V flag on signed 32-bit overflow.
                EmitSubs32(buf_, op_reg, op_reg, src2_reg);
            } else {
                // MulOvf: SMULL 64-bit product + SXTW + CMP sets Z flag.
                // No overflow if lower 32 bits sign-extend to match full 64-bit result.
                EmitSmull(buf_, AT::kScratchC, op_reg, src2_reg);
                EmitSxtw(buf_, AT::kScratchB, AT::kScratchC);
                EmitCmp64(buf_, AT::kScratchC, AT::kScratchB);
            }
#else
            if (opc == IROpCode::AddOvf) {
                EmitREX(buf_, false, op_reg, src2_reg);
                buf_.EmitByte(0x03);
                buf_.EmitByte(ModRM(3, op_reg, src2_reg));
            } else if (opc == IROpCode::SubOvf) {
                EmitREX(buf_, false, op_reg, src2_reg);
                buf_.EmitByte(0x2B);
                buf_.EmitByte(ModRM(3, op_reg, src2_reg));
            } else {
                EmitREX(buf_, false, op_reg, src2_reg);
                buf_.EmitByte(0x0F);
                buf_.EmitByte(0xAF);
                buf_.EmitByte(ModRM(3, op_reg, src2_reg));
            }
#endif
            {
                uint32_t jno_pos = buf_.pos();
#if defined(__aarch64__)
                if (opc == IROpCode::MulOvf) {
                    // SMULL+SXTW+CMP sets Z flag — B.EQ skips deopt on no overflow.
                    EmitBCond(buf_, kARM64_EQ, 0);
                } else {
                    enc_.EmitJccRel32(kCC_NO, 0);
                }
#else
                enc_.EmitJccRel32(kCC_NO, 0);
#endif
                // Find the nearest backward branch target (loop header) for OSR
                // resume on overflow deoptimization.  Scanning backward from the
                // current instruction, the first backward branch found is the
                // nearest back-edge; its target is the loop header to resume at.
                uint32_t osr_pc = 0;
                for (uint32_t si = current_instr_index_; si > 0; --si) {
                    uint32_t scan_idx = si - 1;
                    const auto& scan_instr = rm_.instructions[scan_idx];
                    if (scan_instr.is_branch() && scan_instr.imm.branch_target < scan_idx) {
                        osr_pc = scan_instr.imm.branch_target;
                        break;
                    }
                }
                EmitDeoptSequence(current_instr_index_, osr_pc);
                uint32_t no_overflow = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, jno_pos, no_overflow);
#else
                // JccRel32 is 6 bytes: 0F 8x + 4-byte offset at jno_pos+2.
                // Displacement is from end of instruction (jno_pos + 6).
                int32_t disp = static_cast<int32_t>(no_overflow - (jno_pos + 6));
                buf_.Patch32(jno_pos + 2, static_cast<uint32_t>(disp));
#endif
#if defined(__aarch64__)
                // ARM64 MulOvf: SMULL result is in scratchC — move low 32 bits to op_reg.
                if (opc == IROpCode::MulOvf) {
                    EmitOrr32(buf_, op_reg, 31, AT::kScratchC);
                }
#endif
            }
            StoreGpr(op_reg, instr.dst_reg());
            return true;
        }

        case IROpCode::And:
        case IROpCode::Or:
        case IROpCode::Xor:
        case IROpCode::Not: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            EmitBitwise(opc, instr.dst_reg(), instr.src1_reg(), instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
            return true;
        }

        case IROpCode::Shl:
        case IROpCode::Shr:
        case IROpCode::ShrUn: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            EmitShift(opc, instr.dst_reg(), instr.src1_reg(), instr.has_src2() ? instr.src2_reg() : UINT32_MAX,
                      instr.has_imm() ? instr.imm.i4 : 0);
            return true;
        }

        case IROpCode::Br: {
            uint32_t target = instr.imm.branch_target;
            if (target < current_instr_index_)
                EmitSafepointPoll();
            uint32_t patch_off
#if defined(__aarch64__)
                = buf_.pos();
#else
                = buf_.pos() + 1;
#endif
            enc_.EmitJmpRel32(0);
            branch_patches_.push_back({patch_off, target});
            return true;
        }

        case IROpCode::Beq:
        case IROpCode::BneUn: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            uint8_t cmp_a = AT::kScratchA, cmp_b = AT::kScratchB;
            if (has_graph_coloring_) {
                if (instr.src1_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src1_reg()];
                    if (c != 0xFF)
                        cmp_a = c;
                }
                if (instr.src2_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src2_reg()];
                    if (c != 0xFF)
                        cmp_b = c;
                }
                if (cmp_a == cmp_b)
                    cmp_b = (cmp_a == AT::kScratchA) ? AT::kScratchB : AT::kScratchA;
            }
            LoadGpr(cmp_a, instr.src1_reg());
            LoadGpr(cmp_b, instr.src2_reg());
            enc_.EmitCmpRR(cmp_a, cmp_b);
            uint8_t jcc = (instr.op_code() == IROpCode::Beq) ? kCC_E : kCC_NE;
            uint32_t patch_off
#if defined(__aarch64__)
                = buf_.pos();
#else
                = buf_.pos() + 2;
#endif
            enc_.EmitJccRel32(jcc, 0);
            branch_patches_.push_back({patch_off, instr.imm.branch_target});
            return true;
        }

        case IROpCode::Blt:
        case IROpCode::Bgt:
        case IROpCode::Ble:
        case IROpCode::Bge:
        case IROpCode::BltUn:
        case IROpCode::BgtUn:
        case IROpCode::BleUn:
        case IROpCode::BgeUn: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            uint8_t cmp_a = AT::kScratchA, cmp_b = AT::kScratchB;
            if (has_graph_coloring_) {
                if (instr.src1_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src1_reg()];
                    if (c != 0xFF)
                        cmp_a = c;
                }
                if (instr.src2_reg() < interpreter::kGPRegisters) {
                    uint8_t c = gcr_.gpr_color[instr.src2_reg()];
                    if (c != 0xFF)
                        cmp_b = c;
                }
                if (cmp_a == cmp_b)
                    cmp_b = (cmp_a == AT::kScratchA) ? AT::kScratchB : AT::kScratchA;
            }
            LoadGpr(cmp_a, instr.src1_reg());
            LoadGpr(cmp_b, instr.src2_reg());
            enc_.EmitCmp32RR(cmp_a, cmp_b);
            uint8_t jcc = CmpToJccSigned(instr.op_code());
            uint32_t patch_off
#if defined(__aarch64__)
                = buf_.pos();
#else
                = buf_.pos() + 2;
#endif
            enc_.EmitJccRel32(jcc, 0);
            branch_patches_.push_back({patch_off, instr.imm.branch_target});
            return true;
        }

        case IROpCode::BrTrue:
        case IROpCode::BrFalse: {
            if (!instr.has_src1())
                return false;
            uint32_t target = instr.imm.branch_target;
            if (target < current_instr_index_)
                EmitSafepointPoll();
            uint8_t test_reg = AT::kScratchA;
            if (has_graph_coloring_ && instr.src1_reg() < interpreter::kGPRegisters) {
                uint8_t c = gcr_.gpr_color[instr.src1_reg()];
                if (c != 0xFF)
                    test_reg = c;
            }
            LoadGpr(test_reg, instr.src1_reg());
            enc_.EmitTestRR(test_reg, test_reg);
            uint8_t jcc = (instr.op_code() == IROpCode::BrTrue) ? kCC_NE : kCC_E;
            uint32_t patch_off
#if defined(__aarch64__)
                = buf_.pos();
#else
                = buf_.pos() + 2;
#endif
            enc_.EmitJccRel32(jcc, 0);
            branch_patches_.push_back({patch_off, target});
            return true;
        }

        // ── Switch: jump table (>=4 cases) or linear chain (<4 cases) ────
        case IROpCode::Switch: {
            if (!instr.has_src1())
                return false;
            uint32_t target_count = static_cast<uint32_t>((instr.header >> 48) & 0x7FFF);
            auto* targets = static_cast<const uint32_t*>(instr.imm.ptr);
            if (targets == nullptr || target_count == 0)
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());

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
                enc_.EmitCmpRI(AT::kScratchA, static_cast<int32_t>(target_count));
                uint32_t default_patch_off
#if defined(__aarch64__)
                    = buf_.pos();
#else
                    = buf_.pos() + 2;
#endif
                enc_.EmitJccRel32(kCC_AE, 0);
                branch_patches_.push_back({default_patch_off, targets[target_count]});

                // LEA r10, [rip + 10] — points to table start (10 bytes from LEA end)
                // LEA is 7 bytes. movsxd(4) + add(3) + jmp(3) = 10 bytes after LEA.
                uint32_t lea_pos = buf_.pos();
#if defined(__aarch64__)
                // ARM64 jump table: ADR Xtmp, table; LDRSW Xidx, [Xtmp, Xidx, LSL #2]; ADD Xtmp, Xtmp, Xidx; BR Xtmp
                // ADR X10, #(offset to table) — points to table entries
                // We'll compute offset after emitting LDRSW+ADD+BR (12 bytes)
                enc_.EmitAdr(AT::kTmpReg, 12); // skip ADR(4)+LDR(4)+BR(4) → points to table
                buf_.Emit32(0xF860780Au | (AT::kTmpReg << 5) | AT::kScratchA); // LDRSW Xidx, [X10, Xidx, LSL #2]
                //           ^-- actually 0xF8607800 | (Xtmp << 5) | Xidx
                buf_.Emit32(0x8B0C000Au | (AT::kScratchA << 16) | (AT::kTmpReg << 5) |
                            AT::kTmpReg); // ADD X10, X10, Xidx
                enc_.EmitBr(AT::kTmpReg);
#else
                enc_.EmitLeaRipRel(AT::kTmpReg, 10);
                // MOVSXD rax, [r10 + rax*4] — load sign-extended table entry
                // REX: W=1, R=0(rax), X=0(rax), B=1(r10) → 0x49
                buf_.EmitByte(0x49);
                buf_.EmitByte(0x63);                               // MOVSXD opcode
                buf_.EmitByte(ModRM(0, AT::kScratchA, 4));         // rm=4 = SIB escape
                buf_.EmitByte(SIB(2, AT::kScratchA, AT::kTmpReg)); // scale=4, index=rax, base=r10
                // ADD r10, rax
                enc_.EmitAddRR(AT::kTmpReg, AT::kScratchA);
                // JMP r10
                enc_.EmitJmpReg(AT::kTmpReg);
#endif

                // Emit jump table entries (placeholder values, patched by ResolveBranches)
                uint32_t table_pos = buf_.pos();
                for (uint32_t i = 0; i < target_count; ++i) {
                    buf_.Emit32(0); // placeholder
                    jump_table_patches_.push_back({buf_.pos() - 4, table_pos, targets[i]});
                }
            } else {
                // ── Linear chain for small switches (< 4 cases) ──────────
                for (uint32_t i = 0; i < target_count; ++i) {
                    enc_.EmitCmpRI(AT::kScratchA, static_cast<int32_t>(i));
                    uint32_t patch_off
#if defined(__aarch64__)
                        = buf_.pos();
#else
                        = buf_.pos() + 2;
#endif
                    enc_.EmitJccRel32(kCC_E, 0);
                    branch_patches_.push_back({patch_off, targets[i]});
                }
                // No match: jump to default target at targets[target_count]
                uint32_t default_patch_off
#if defined(__aarch64__)
                    = buf_.pos();
#else
                    = buf_.pos() + 1;
#endif
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
                if ((flags == 2 || flags == 4) && current_instr_index_ >= try_start && current_instr_index_ < try_end) {
                    has_finally_covering = true;
                    break;
                }
            }

            if (has_finally_covering) {
                // Leave crossing a finally/fault boundary: call JitLeaveHelper
                // to resolve byte offsets at runtime and find the innermost handler.
                enc_.EmitMovRIImm32(AT::kScratchB, target);               // arg1: target_instr_idx
                enc_.EmitMovRIImm32(AT::kScratchC, current_instr_index_); // arg2: current_instr_idx
                EmitRuntimeHelperCall(JitLeaveHelper);                    // RAX = handler addr or 0
                enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);            // test rax, rax
                uint32_t jz_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_E, 0);    // jz .normal_jmp
                enc_.EmitJmpReg(AT::kScratchA); // jmp rax → handler
                // .normal_jmp: fall through to normal leave JMP
                uint32_t normal_pos = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, jz_pos, normal_pos);
#else
                int32_t jz_disp = static_cast<int32_t>(normal_pos - (jz_pos + 6));
                buf_.Patch32(jz_pos + 2, static_cast<uint32_t>(jz_disp));
#endif
                // Normal JMP to leave target (JitLeaveHelper returned 0 or
                // finally chain completed and returned to leave path).
                uint32_t patch_off
#if defined(__aarch64__)
                    = buf_.pos();
#else
                    = buf_.pos() + 1;
#endif
                enc_.EmitJmpRel32(0);
                branch_patches_.push_back({patch_off, target});
            } else {
                // No finally/fault covering — normal JMP (current behavior).
                if (target < current_instr_index_)
                    EmitSafepointPoll();
                uint32_t patch_off
#if defined(__aarch64__)
                    = buf_.pos();
#else
                    = buf_.pos() + 1;
#endif
                enc_.EmitJmpRel32(0);
                branch_patches_.push_back({patch_off, target});
            }
            return true;
        }

        case IROpCode::EndFinally: {
            // SEH V3: call JitEndFinallyHelper to advance the finally/fault
            // unwind chain.  Returns next handler address (non-zero) or 0
            // (continue normally).
            EmitRuntimeHelperCall(JitEndFinallyHelper);
            enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
            uint32_t jz_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_E, 0);    // jz .continue
            enc_.EmitJmpReg(AT::kScratchA); // jmp rax → next handler/leave target
            // .continue: normal fall-through
            uint32_t continue_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, jz_pos, continue_pos);
#else
            int32_t jz_disp = static_cast<int32_t>(continue_pos - (jz_pos + 6));
            buf_.Patch32(jz_pos + 2, static_cast<uint32_t>(jz_disp));
#endif
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
                LoadGpr(AT::kScratchA, instr.src1_reg()); // EAX = filter result
            } else {
                enc_.EmitMovImm64(AT::kScratchA, 0); // defensive: treat as reject
            }
            enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
            // jne .accept  (filter result != 0 → fall through to handler)
            uint32_t jne_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_NE, 0);
            // Reject path: dispatch via JitEndFinallyHelper.
            EmitRuntimeHelperCall(JitEndFinallyHelper);
            enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
            uint32_t jz_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_E, 0);    // jz .accept (no further handler)
            enc_.EmitJmpReg(AT::kScratchA); // jmp to next handler
            // .accept: fall through to the handler block (sequentially after EndFilter).
            uint32_t accept_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, jne_pos, accept_pos);
            PatchArm64Bcond(buf_, jz_pos, accept_pos);
#else
            int32_t jne_disp = static_cast<int32_t>(accept_pos - (jne_pos + 6));
            buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(jne_disp));
            int32_t jz_disp = static_cast<int32_t>(accept_pos - (jz_pos + 6));
            buf_.Patch32(jz_pos + 2, static_cast<uint32_t>(jz_disp));
#endif
            return true;
        }

        case IROpCode::Throw:
        case IROpCode::Rethrow: {
            // Deoptimize when register allocation is inconsistent
            // (should not happen — the interpreter verifier ensures correct
            //  src1 count for Throw/Rethrow; this is a safety check).
            if (!instr.has_src1())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg()); // RCX = exception object
            // JMP to cold section (patched after epilogue emission).
            // Cold section contains ChaosJitRaiseException call + INT3 safety net.
            // This keeps hot-path code contiguous for better icache behavior.
            uint32_t jmp_off = buf_.pos();
            enc_.EmitJmpRel32(0);
            cold_patches_.push_back({jmp_off + 1});
            return true;
        }

        case IROpCode::Ceq:
        case IROpCode::Clt:
        case IROpCode::Cgt: {
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            // Use fixed scratch registers RAX/RCX, NOT graph-colored registers,
            // because the setcc sequence (MovRIImm32 + Setcc) destroys the
            // register used as cmp_a.  Using src1's colored register would
            // silently corrupt src1's value for any subsequent read of src1
            // through LoadGpr (which returns the colored register, not the stack).
            uint8_t cmp_a = AT::kScratchA, cmp_b = AT::kScratchB;
            LoadGpr(cmp_a, instr.src1_reg());
            LoadGpr(cmp_b, instr.src2_reg());
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

        case IROpCode::Conv_I4:
        case IROpCode::Conv_I8:
        case IROpCode::ConvI:
        case IROpCode::ConvU:
        case IROpCode::ConvOvfI:
        case IROpCode::ConvOvfI8: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            // Int32-converting opcodes: truncate to 32-bit via mov eax,eax (zero-extends).
            // Int64 opcodes (Conv_I8, ConvOvfI8): keep full 64-bit value.
            bool is_int32_op = (instr.op_code() != IROpCode::Conv_I8 && instr.op_code() != IROpCode::ConvOvfI8);
            if (is_int32_op) {
#if !defined(__aarch64__)
                buf_.EmitByte(0x89);
                buf_.EmitByte(0xC0); // mov eax, eax (zero-extend 32→64)
#endif
            }
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::ConvOvfI4: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            // Check overflow: sign-extend 32-bit truncation back to 64-bit and compare.
            // If original ≠ sign-extend(truncate(original)), the value doesn't fit int32.
            enc_.EmitMovRR(AT::kScratchB, AT::kScratchA); // rcx = rax (copy original)
#if !defined(__aarch64__)
            buf_.EmitByte(0x89);
            buf_.EmitByte(0xC1); // mov ecx, eax (truncate to 32-bit)
#endif
            enc_.EmitMovsxd(AT::kScratchB, AT::kScratchB); // movsxd rcx, ecx (sign-extend 32→64)
            enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);  // cmp rax, rcx
            {
                uint32_t jne_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_NE, 0); // jne → deopt (overflow)
                EmitDeoptSequence(current_instr_index_);
                uint32_t no_overflow = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, jne_pos, no_overflow);
#else
                int32_t disp = static_cast<int32_t>(no_overflow - (jne_pos + 6));
                buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(disp));
#endif
            }
            // Result: truncated 32-bit value in EAX (zero-extended to 64-bit in RAX)
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::ConvOvfU:
        case IROpCode::ConvOvfU4:
        case IROpCode::ConvOvfU8: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            if (instr.op_code() == IROpCode::ConvOvfU4) {
                // Check value fits in uint32: truncate + compare
                enc_.EmitMovRR(AT::kScratchB, AT::kScratchA);
#if !defined(__aarch64__)
                buf_.EmitByte(0x89);
                buf_.EmitByte(0xC1); // mov ecx, eax (32-bit truncate)
#endif
                enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
                uint32_t jne_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_NE, 0);
                EmitDeoptSequence(current_instr_index_);
                uint32_t no_overflow = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, jne_pos, no_overflow);
#else
                int32_t disp = static_cast<int32_t>(no_overflow - (jne_pos + 6));
                buf_.Patch32(jne_pos + 2, static_cast<uint32_t>(disp));
#endif
            } else {
                // ConvOvfU / ConvOvfU8: check sign bit
                enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
                uint32_t js_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_S, 0);
                EmitDeoptSequence(current_instr_index_);
                uint32_t non_neg = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, js_pos, non_neg);
#else
                int32_t disp = static_cast<int32_t>(non_neg - (js_pos + 6));
                buf_.Patch32(js_pos + 2, static_cast<uint32_t>(disp));
#endif
            }
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Conv_R4: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            // int32→float: read GPR[src1], truncate to int32, convert to float.
            // Matches RegisterExecute: float v = static_cast<float>(static_cast<int32_t>(gpr[src1]));
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitMovsxd(AT::kScratchA, AT::kScratchA); // movsxd rax, eax (sign-extend 32→64)
            enc_.EmitCvtsi2sd(0, AT::kScratchA);           // cvtsi2sd xmm0, rax (int64→double, full 64-bit slot)
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::Conv_R8: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            // int32→double: read GPR[src1], truncate to int32, convert to double.
            // Matches RegisterExecute: double v = static_cast<double>(static_cast<int32_t>(gpr[src1]));
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitMovsxd(AT::kScratchA, AT::kScratchA); // movsxd rax, eax (sign-extend 32→64)
            enc_.EmitCvtsi2sd(0, AT::kScratchA);           // cvtsi2sd xmm0, rax (int64→double)
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::ConvRUn: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            // uint32 → float64: load uint32 from GPR[src1], zero-extend to 64-bit,
            // convert to double, store to FPR[dst].  Matches interpreter VM:
            //   double result = static_cast<double>(static_cast<uint32_t>(gpr[src1]));
            LoadGpr(AT::kScratchA, instr.src1_reg());
#if !defined(__aarch64__)
            buf_.EmitByte(0x8B);
            buf_.EmitByte(0xC0); // mov eax, eax (zero-extend 32→64)
#endif
            enc_.EmitCvtsi2sd(0, AT::kScratchA); // cvtsi2sd xmm0, rax
            StoreFpr(0, instr.dst_reg());
            return true;
        }

        case IROpCode::Pop:
            return true;

        case IROpCode::Break:
            return true;

        case IROpCode::SizeOf: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>(instr.imm.i4));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdToken: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>(instr.imm.i4));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdFtn: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(instr.imm.ptr));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdStr: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(instr.imm.ptr));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdFld: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdFld);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::StFld: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            // Simple path -- always call CodegenStFld (full SATB barrier).
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            LoadGpr(AT::kExtraScratch0, instr.src2_reg());
            EmitRuntimeHelperCall(::CodegenStFld);
            return true;
            ;
        }

        case IROpCode::StFldBarrier: {
            // Inline g_bgc_is_marking check + conditional barrier.
            // Fast path (not marking): call CodegenStFldNoBarrier (store + card mark).
            // Slow path (marking): call CodegenStFld (full SATB pre-write barrier).
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());                   // obj
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset); // field_idx
            LoadGpr(AT::kExtraScratch0, instr.src2_reg());              // value
            // Inline g_bgc_is_marking check (preserves RCX/RDX/R8)
            enc_.EmitMovImm64(AT::kScratchA,
                              reinterpret_cast<uint64_t>(&chaos::il2cpp::runtime_core::g_bgc_is_marking));
#if defined(__aarch64__)
            // ARM64: LDRB Wtmp, [Xaddr]; CBZ Wtmp, skip_marking
            buf_.Emit32(0x39400000u | (AT::kScratchA << 5) | AT::kScratchC); // LDRB WscratchC, [XscratchA]
            buf_.Emit32(0x3400000Cu | (AT::kScratchC << 5)); // CBZ WscratchC, #0 (placeholder, patched later)
#else
            buf_.EmitByte(0x80); // cmp r/m8, imm8
            buf_.EmitByte(0x38); // ModRM: mod=00, reg=7, rm=0 → [rax]
            buf_.EmitByte(0x00); // imm8 = 0
#endif
            uint32_t marking_jmp_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_NE, 0); // jne .marking
            // Fast path: not marking → CodegenStFldNoBarrier
            EmitRuntimeHelperCall(::CodegenStFldNoBarrier);
            uint32_t done_jmp_pos = buf_.pos();
            enc_.EmitJmpRel32(0); // jmp .done
            // Slow path: marking → CodegenStFld (full SATB barrier)
            uint32_t marking_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, marking_jmp_pos, marking_pos);
#else
            buf_.Patch32(marking_jmp_pos + 2, marking_pos - (marking_jmp_pos + 6));
#endif
            // Reload args (spilled/colored regs may differ in slow path)
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            LoadGpr(AT::kExtraScratch0, instr.src2_reg());
            EmitRuntimeHelperCall(::CodegenStFld);
            uint32_t done_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64B(buf_, done_jmp_pos, done_pos);
#else
            buf_.Patch32(done_jmp_pos + 1, done_pos - (done_jmp_pos + 5));
#endif
            return true;
        }

        case IROpCode::NewObj: {
            if (!instr.has_dst())
                return false;
            uint32_t type_token = instr.imm.field_offset;
            uint32_t field_count = instr.imm.operand_index;
            if (field_count == 0)
                field_count = 1;
            // InterpreterObject size (64 bytes: SmallFieldArray 56 + type_token 4 + padding 4)
            static constexpr int32_t kObjSize = static_cast<int32_t>(sizeof(interpreter::InterpreterObject));

            if (false) {
                // ═══ TLAB inline allocation path ═══
                if (config_.enable_register_caching && cached_slots_used_)
                    SpillCachedRegs();
                EmitLoadTlsTlab(buf_); // rax = &tls_tlab

                enc_.EmitMovRM(AT::kScratchB, AT::kScratchA, 8);       // rcx = tls_tlab.current
                enc_.EmitLeaRM(AT::kArgsBuf, AT::kScratchB, kObjSize); // rbx = new current
                enc_.EmitMovRM(AT::kScratchC, AT::kScratchA, 16);      // rdx = tls_tlab.end
                enc_.EmitCmpRR(AT::kArgsBuf, AT::kScratchC);
                uint32_t newobj_ja_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_A, 0); // ja slow_path (patched later)

                // TLAB HIT: bump, zero-init, init struct
                enc_.EmitMovMR(AT::kScratchA, 8, AT::kArgsBuf); // tls_tlab.current = new_ptr

                // Zero-init 64 bytes (4 × movups)
                enc_.EmitXorpsRR(0, 0);                  // xorps xmm0, xmm0
                enc_.EmitMovUPSMR(AT::kScratchB, 0, 0);  // [rcx+0]
                enc_.EmitMovUPSMR(AT::kScratchB, 16, 0); // [rcx+16]
                enc_.EmitMovUPSMR(AT::kScratchB, 32, 0); // [rcx+32]
                enc_.EmitMovUPSMR(AT::kScratchB, 48, 0); // [rcx+48]

                // Init SmallFieldArray: fields_ptr_ = &inline_[0] (at offset 24)
                enc_.EmitLeaRM(AT::kArgsBuf, AT::kScratchB, 24);
                enc_.EmitMovMR(AT::kScratchB, 0, AT::kArgsBuf);  // fields.fields_ptr_ = &inline_[0]
                enc_.EmitMovRI32(AT::kArgsBuf, 2);               // rbx = kInlineCapacity
                enc_.EmitMovMR(AT::kScratchB, 16, AT::kArgsBuf); // fields.field_capacity_ = 2
                // field_count_ at offset 8 is already 0 from zero-init

                // Set type_token at offset 56 (4 bytes, upper 4 zero from zero-init)
                enc_.EmitMovRIImm32(AT::kArgsBuf, type_token);
                enc_.EmitMovMR(AT::kScratchB, 56, AT::kArgsBuf); // obj->type_token = type_token

                StoreGpr(AT::kScratchB, instr.dst_reg()); // result = obj pointer

                uint32_t newobj_jmp_done_pos = buf_.pos();
                enc_.EmitJmpRel32(0); // skip slow path

                // ═══ Slow path (TLAB miss) ═══
                uint32_t newobj_slow_pos = buf_.pos();
#if defined(__aarch64__)
                PatchArm64Bcond(buf_, newobj_ja_pos, newobj_slow_pos);
#else
                buf_.Patch32(newobj_ja_pos + 2, newobj_slow_pos - (newobj_ja_pos + 6));
#endif

                {
                    enc_.EmitMovRIImm32(AT::kScratchB, type_token);
                    enc_.EmitMovRIImm32(AT::kScratchC, field_count);
                    uint32_t call_pos = EmitRuntimeHelperCall(::CodegenNewObj);
                    {
                        uint32_t call_token = 0, call_module = 0;
                        if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                            const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                                config_.call_cache)[current_instr_index_];
                            call_token = cached.method_token;
                            call_module = cached.module_id;
                        }
                        call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
                    }
                    RecordGcPoint(call_pos);
                    StoreGpr(AT::kScratchA, instr.dst_reg());
                }

                uint32_t newobj_done_pos = buf_.pos();
#if defined(__aarch64__)
                PatchArm64B(buf_, newobj_jmp_done_pos, newobj_done_pos);
#else
                buf_.Patch32(newobj_jmp_done_pos + 1, newobj_done_pos - (newobj_jmp_done_pos + 5));
#endif
            } else {
                // Object too large for TLAB inline — direct GC allocation
                enc_.EmitMovRIImm32(AT::kScratchB, type_token);
                enc_.EmitMovRIImm32(AT::kScratchC, field_count);
                uint32_t call_pos = EmitRuntimeHelperCall(::CodegenNewObj);
                {
                    uint32_t call_token = 0, call_module = 0;
                    if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                        const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                            config_.call_cache)[current_instr_index_];
                        call_token = cached.method_token;
                        call_module = cached.module_id;
                    }
                    call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
                }
                RecordGcPoint(call_pos);
                StoreGpr(AT::kScratchA, instr.dst_reg());
            }
            return true;
        }

        case IROpCode::Box: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            // BoxedValue = 16 bytes (single InterpreterValue, no heap-allocated fields)
            static constexpr int32_t kBoxSize = static_cast<int32_t>(sizeof(interpreter::BoxedValue));

            // ═══ TLAB inline allocation path (Windows TLS / ARM64 Linux TPIDR_EL0) ═══
#if defined(_WIN32) || defined(_WIN64) || (defined(__linux__) && defined(__aarch64__))
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            EmitLoadTlsTlab(buf_); // rax/x0 = &tls_tlab

            enc_.EmitMovRM(AT::kScratchB, AT::kScratchA, 8);       // rcx/x1 = tls_tlab.current
            enc_.EmitLeaRM(AT::kArgsBuf, AT::kScratchB, kBoxSize); // rbx/x6 = new current
            enc_.EmitMovRM(AT::kScratchC, AT::kScratchA, 16);      // rdx/x2 = tls_tlab.end
            enc_.EmitCmpRR(AT::kArgsBuf, AT::kScratchC);
            uint32_t box_ja_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_A, 0); // ja slow_path

            enc_.EmitMovMR(AT::kScratchA, 8, AT::kArgsBuf); // tls_tlab.current = new_ptr
            LoadGpr(AT::kExtraScratch0, instr.src1_reg());  // r8 = value
            enc_.EmitXorpsRR(0, 0);                         // xorps xmm0, xmm0
            enc_.EmitMovUPSMR(AT::kScratchB, 0, 0);         // [rcx+0..15] = 0
            enc_.EmitMovRIImm32(AT::kArgsBuf, static_cast<uint32_t>(interpreter::ValueTag::Int64));
            enc_.EmitMovMR(AT::kScratchB, 0, AT::kArgsBuf);       // BoxedValue::value.tag = Int64
            enc_.EmitMovMR(AT::kScratchB, 8, AT::kExtraScratch0); // BoxedValue::value.i64 = value
            StoreGpr(AT::kScratchB, instr.dst_reg());             // result = boxed pointer

            uint32_t box_jmp_done_pos = buf_.pos();
            enc_.EmitJmpRel32(0); // skip slow path

            uint32_t box_slow_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, box_ja_pos, box_slow_pos);
#else
            buf_.Patch32(box_ja_pos + 2, box_slow_pos - (box_ja_pos + 6));
#endif
#else
            // Linux: no inline TLAB — always use runtime helper
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            uint32_t box_slow_pos = buf_.pos();
#endif

            {
                LoadGpr(AT::kScratchB, instr.src1_reg());
                enc_.EmitMovRIImm32(AT::kScratchC, static_cast<uint32_t>(interpreter::ValueTag::Int64));
                enc_.EmitMovRIImm32(AT::kExtraScratch0, instr.imm.field_offset);
                uint32_t call_pos = EmitRuntimeHelperCall(::CodegenBox);
                {
                    uint32_t call_token = 0, call_module = 0;
                    if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                        const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                            config_.call_cache)[current_instr_index_];
                        call_token = cached.method_token;
                        call_module = cached.module_id;
                    }
                    call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
                }
                RecordGcPoint(call_pos);
                StoreGpr(AT::kScratchA, instr.dst_reg());
            }

#if defined(_WIN32) || defined(_WIN64) || (defined(__linux__) && defined(__aarch64__))
            uint32_t box_done_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64B(buf_, box_jmp_done_pos, box_done_pos);
#else
            buf_.Patch32(box_jmp_done_pos + 1, box_done_pos - (box_jmp_done_pos + 5));
#endif
#endif
            return true;
        }

        case IROpCode::Unbox: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenUnbox);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdLen: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdLen);
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::NewArr: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            static constexpr int32_t kArrSize = static_cast<int32_t>(sizeof(interpreter::ArrayStorage));

            // ═══ TLAB inline allocation path (Windows TLS / ARM64 Linux TPIDR_EL0) ═══
#if defined(_WIN32) || defined(_WIN64) || (defined(__linux__) && defined(__aarch64__))
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            EmitLoadTlsTlab(buf_); // rax/x0 = &tls_tlab

            enc_.EmitMovRM(AT::kScratchB, AT::kScratchA, 8);       // rcx/x1 = tls_tlab.current
            enc_.EmitLeaRM(AT::kArgsBuf, AT::kScratchB, kArrSize); // rbx/x6 = new current
            enc_.EmitMovRM(AT::kScratchC, AT::kScratchA, 16);      // rdx/x2 = tls_tlab.end
            enc_.EmitCmpRR(AT::kArgsBuf, AT::kScratchC);
            uint32_t newarr_ja_pos = buf_.pos();
            enc_.EmitJccRel32(kCC_A, 0); // ja slow_path

            // TLAB HIT: bump
            enc_.EmitMovMR(AT::kScratchA, 8, AT::kArgsBuf); // tls_tlab.current = new_ptr

            // CodegenNewArrTlab(mem=rcx, length=rdx) — placement new + elements.resize
            LoadGpr(AT::kScratchC, instr.src1_reg()); // rdx = length
            EmitRuntimeHelperCall(::CodegenNewArrTlab);
            StoreGpr(AT::kScratchA, instr.dst_reg());

            uint32_t newarr_jmp_done_pos = buf_.pos();
            enc_.EmitJmpRel32(0); // skip slow path

            // ═══ Slow path (TLAB miss) ═══
            uint32_t newarr_slow_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64Bcond(buf_, newarr_ja_pos, newarr_slow_pos);
#else
            buf_.Patch32(newarr_ja_pos + 2, newarr_slow_pos - (newarr_ja_pos + 6));
#endif
#else
            // Linux x64: no inline TLAB — always use runtime helper
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            uint32_t newarr_slow_pos = buf_.pos();
#endif

            LoadGpr(AT::kScratchB, instr.src1_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenNewArr);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());

#if defined(_WIN32) || defined(_WIN64) || (defined(__linux__) && defined(__aarch64__))
            uint32_t newarr_done_pos = buf_.pos();
#if defined(__aarch64__)
            PatchArm64B(buf_, newarr_jmp_done_pos, newarr_done_pos);
#else
            buf_.Patch32(newarr_jmp_done_pos + 1, newarr_done_pos - (newarr_jmp_done_pos + 5));
#endif
#endif
            return true;
        }

        case IROpCode::LdElem:
        case IROpCode::LdElemA: {
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdElem);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::StElem: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            if (instr.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3)
                LoadGpr(AT::kExtraScratch0, instr.src3_reg());
            EmitRuntimeHelperCall(::CodegenStElem);
            return true;
        }

        case IROpCode::LdElemNoChk:
        case IROpCode::LdElemANoChk: {
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdElemNoCheck);
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::StElemNoChk: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            if (instr.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3)
                LoadGpr(AT::kExtraScratch0, instr.src3_reg());
            EmitRuntimeHelperCall(::CodegenStElemNoCheck);
            return true;
        }

        case IROpCode::LdSFld: {
            if (!instr.has_dst())
                return false;
            enc_.EmitMovRIImm32(AT::kScratchB, instr.imm.field_offset);
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdSFld);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::StSFld: {
            if (!instr.has_src1())
                return false;
            enc_.EmitMovRIImm32(AT::kScratchB, instr.imm.field_offset);
            LoadGpr(AT::kScratchC, instr.src1_reg());
            EmitRuntimeHelperCall(::CodegenStSFld);
            return true;
        }

        case IROpCode::CastClass: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            EmitRuntimeHelperCall(::CodegenCastClass);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::IsInst: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            EmitRuntimeHelperCall(::CodegenIsInst);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdVirtFtn: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRIImm32(AT::kScratchC, instr.imm.field_offset);
            EmitRuntimeHelperCall(::CodegenLdVirtFtn);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::InitObj: {
            if (!instr.has_src1())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            EmitRuntimeHelperCall(::CodegenInitObj);
            return true;
        }

        case IROpCode::StObj: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            EmitRuntimeHelperCall(::CodegenStObj);
            return true;
        }

        case IROpCode::StInd: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            LoadGpr(AT::kScratchB, instr.src2_reg());
            enc_.EmitMovMR(AT::kScratchA, 0, AT::kScratchB);
            return true;
        }

        case IROpCode::LdInd: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            enc_.EmitMovRM(AT::kScratchA, AT::kScratchB, 0);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdObj: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            uint32_t call_pos = EmitRuntimeHelperCall(::CodegenLdObj);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Cpblk: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            if (instr.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3)
                LoadGpr(AT::kExtraScratch0, instr.src3_reg());
            EmitRuntimeHelperCall(::CodegenCpblk);
            return true;
        }

        case IROpCode::InitBlk: {
            if (!instr.has_src1() || !instr.has_src2())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg());
            LoadGpr(AT::kScratchC, instr.src2_reg());
            if (instr.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3)
                LoadGpr(AT::kExtraScratch0, instr.src3_reg());
            EmitRuntimeHelperCall(::CodegenInitBlk);
            return true;
        }

        case IROpCode::LocAlloc: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchB, instr.src1_reg()); // size
            if (localloc_extra_ > 0) {
                // Stack allocation from pre-allocated frame reserve.
                // RDX = base address of localloc reserve.
                enc_.EmitLeaRM(
                    AT::kScratchC, AT::kStackReg,
                    static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_ + 8));
                // R8 = bump counter pointer (8 bytes, at kFrameSize + align_adj + xmm_save).
                enc_.EmitLeaRM(
                    AT::kExtraScratch0, AT::kStackReg,
                    static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_));
                EmitRuntimeHelperCall(reinterpret_cast<void*>(::CodegenLocAlloc));
            } else {
                // Fallback: heap allocation (no stack reserve — rare edge case).
                EmitRuntimeHelperCall(reinterpret_cast<void*>(::CodegenLocAlloc));
            }
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Dup: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdArgA: {
            if (!instr.has_dst())
                return false;
            uint32_t arg_idx = instr.imm.operand_index;
            enc_.EmitLeaRM(AT::kScratchA, AT::kArgsBuf, static_cast<int32_t>(arg_idx * 8));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::LdLocA: {
            if (!instr.has_dst())
                return false;
            uint32_t local_idx = instr.imm.operand_index;
            uint32_t vreg = kLocalRegBase + local_idx;
            enc_.EmitLeaRM(AT::kScratchA, AT::kStackReg, static_cast<int32_t>(GprOff(vreg)));
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Call:
        case IROpCode::CallBridge: {
            if (!instr.has_src1())
                return false;
            uint32_t first_arg_reg = instr.src1_reg();
            uint32_t arg_count = instr.call_arg_count();
            void* target_fn = instr.imm.ptr;
            if (target_fn == nullptr)
                return false;
            static constexpr uint8_t kArgRegs[] = {AT::kScratchB, AT::kScratchC, AT::kExtraScratch0,
                                                   AT::kExtraScratch1};
            uint32_t max_scratch = arg_count < 4 ? arg_count : 4;
            for (uint32_t i = 0; i < max_scratch; ++i)
                LoadGpr(kArgRegs[i], first_arg_reg + i);
            for (uint32_t i = 4; i < arg_count; ++i) {
                LoadGpr(AT::kScratchA, first_arg_reg + i);
                enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>((i - 4) * 8), AT::kScratchA);
            }
            // Spill cached/colored registers before call (same as EmitCallWithSpill preamble)
            if (config_.enable_register_caching && cached_slots_used_)
                SpillCachedRegs();
            if (has_graph_coloring_) {
                for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                    uint8_t colored_x64 = gcr_.gpr_color[vr];
                    if (colored_x64 != 0xFF) {
                        if (caller_colored_mask_ & (1ULL << vr))
                            continue;
                        enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vr)), colored_x64);
                    }
                }
                // Spill colored FPRs (same logic: StoreFpr skips stack write).
                for (uint32_t fi = 0; fi < 32; ++fi) {
                    uint8_t colored_xmm = gcr_.fpr_color[fi];
                    if (colored_xmm != 0xFF) {
                        if (caller_fpr_colored_mask_ & (1ULL << fi))
                            continue;
                        enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)), colored_xmm);
                    }
                }
            }
            // Slot-based call: emit call [rip+0] placeholder, record SlotPatch.
            // The slot table is emitted after all instructions, before buf_.Seal().
            uint32_t call_start = buf_.pos();
            enc_.EmitCallRipRel(0); // 6 bytes: FF 15 <disp32>
            uint32_t call_pos = call_start;
            uint32_t slot_patch_offset = call_start + kSlotPatchDispOff; // disp32 starts at byte 2 of call [rip+disp32]
            slot_patches_.push_back({slot_patch_offset, static_cast<uint32_t>(call_sites_.size()), target_fn});
            slot_count_used_++;
#if defined(__aarch64__)
            // ARM64 ABI returns values in X0, but StoreGpr below expects kScratchA (X9).
            enc_.EmitMovRR(AT::kScratchA, 0); // MOV X9, X0
#endif
            // Post-call reload (same as EmitCallWithSpill postamble)
            if (has_graph_coloring_ && caller_colored_mask_) {
                uint64_t mask = caller_colored_mask_;
                for (uint32_t vr = 0; mask; ++vr) {
                    if (mask & 1) {
                        uint8_t colored_x64 = gcr_.gpr_color[vr];
                        enc_.EmitMovRM(colored_x64, AT::kStackReg, static_cast<int32_t>(GprOff(vr)));
                    }
                    mask >>= 1;
                }
            }
            // Post-call reload: restore caller-saved colored FPRs.
            if (has_graph_coloring_ && caller_fpr_colored_mask_) {
                uint64_t mask = caller_fpr_colored_mask_;
                for (uint32_t fi = 0; mask; ++fi) {
                    if (mask & 1) {
                        uint8_t colored_xmm = gcr_.fpr_color[fi];
                        enc_.EmitMovdqaRM(colored_xmm, AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)));
                    }
                    mask >>= 1;
                }
            }
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);
            if (config_.enable_deopt) {
                uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
                for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                                     ? vreg_types_[vr]
                                     : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                    deopt_values_.push_back(dv);
                }
                for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                    deopt_values_.push_back(dv);
                }
                uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
                DeoptEntry entry;
                entry.native_offset = call_pos;
                entry.instr_pc = current_instr_index_;
                entry.num_values = n_vals;
                entry.values_offset = val_start;
                deopt_entries_.push_back(entry);
            }
            if (instr.has_dst())
                StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Calli: {
            if (!instr.has_src1())
                return false;
            uint32_t first_arg_reg = instr.src1_reg();
            uint32_t arg_count = instr.call_arg_count();
            uint32_t func_ptr_vreg = instr.imm.operand_index;

            LoadGpr(AT::kScratchA, func_ptr_vreg);
            enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
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
            static constexpr uint8_t kArgRegs[] = {AT::kScratchB, AT::kScratchC, AT::kExtraScratch0,
                                                   AT::kExtraScratch1};
            uint32_t max_scratch = arg_count < 4 ? arg_count : 4;
            for (uint32_t i = 0; i < max_scratch; ++i)
                LoadGpr(kArgRegs[i], first_arg_reg + i);
            for (uint32_t i = 4; i < arg_count; ++i) {
                LoadGpr(AT::kScratchA, first_arg_reg + i);
                enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>((i - 4) * 8), AT::kScratchA);
            }

            LoadGpr(AT::kScratchA, func_ptr_vreg);
            uint32_t call_pos = buf_.pos();
            EmitCallWithSpill(AT::kScratchA);
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    call_token = cached.method_token;
                    call_module = cached.module_id;
                }
                call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module});
            }
            RecordGcPoint(call_pos);

            if (config_.enable_deopt) {
                uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
                for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                                     ? vreg_types_[vr]
                                     : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                    deopt_values_.push_back(dv);
                }
                for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                    deopt_values_.push_back(dv);
                }
                uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
                DeoptEntry entry;
                entry.native_offset = call_pos;
                entry.instr_pc = current_instr_index_;
                entry.num_values = n_vals;
                entry.values_offset = val_start;
                deopt_entries_.push_back(entry);
            }
            if (instr.has_dst())
                StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::CallVirt:
        case IROpCode::CallVirtConstrained: {
            if (!instr.has_src1())
                return false;
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
            std::vector<uint32_t> inline_miss_jumps; // jne positions (per slot)
            std::vector<uint32_t> inline_done_jumps; // jmp .done positions (per slot)
            uint32_t inline_null_jmp_pos = 0;

            if (pic_slot_count > 0) {
                // Load receiver from register file (via LoadGpr for correct cache/coloring)
                LoadGpr(AT::kScratchA, first_arg_reg);
                enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
                inline_null_jmp_pos = buf_.pos();
                enc_.EmitJccRel32(kCC_E, 0); // je .use_c_helper (null receiver)

                // Load type_token from object at offset 56 (= sizeof(SmallFieldArray))
                // RCX holds type_token across ALL slot checks — loaded once here.
                enc_.EmitMovRM(AT::kScratchB, AT::kScratchA, 56);

                // Generate type check chain for up to pic_slot_count slots
                for (uint32_t si = 0; si < pic_slot_count; ++si) {
                    // Compare RCX (type_token) with expected type for this slot
                    enc_.EmitMovRIImm32(AT::kScratchA, inline_expected_types[si]);
                    enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
                    // Miss jump — patched after all slot code is emitted
                    inline_miss_jumps.push_back(buf_.pos());
                    enc_.EmitJccRel32(kCC_NE, 0); // jne .next (or .use_c_helper if last)

                    // ── Slot hit: load args and call ──
                    // Load Win64 calling convention registers from register file.
                    // RCX is overwritten with first arg (type_token no longer needed).
                    if (arg_count >= 1)
                        LoadGpr(AT::kScratchB, first_arg_reg);
                    if (arg_count >= 2)
                        LoadGpr(AT::kScratchC, first_arg_reg + 1);
                    if (arg_count >= 3)
                        LoadGpr(AT::kExtraScratch0, first_arg_reg + 2);
                    if (arg_count >= 4)
                        LoadGpr(AT::kExtraScratch1, first_arg_reg + 3);

                    // Direct call to pre-resolved AOT function pointer
                    uint32_t call_pos = buf_.pos();
                    EmitRuntimeHelperCall(inline_direct_fns[si]);

                    // Record call site for hotpatch tracking and GC point
                    {
                        uint32_t call_token = 0, call_module = 0;
                        if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                            const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                                config_.call_cache)[current_instr_index_];
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
                            DeoptValue dv;
                            dv.reg_index = vr;
                            dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                                             ? vreg_types_[vr]
                                             : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                            dv.is_spilled = true;
                            dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                            deopt_values_.push_back(dv);
                        }
                        for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                            DeoptValue dv;
                            dv.reg_index = vr;
                            dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                            dv.is_spilled = true;
                            dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                            deopt_values_.push_back(dv);
                        }
                        uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
                        DeoptEntry entry;
                        entry.native_offset = call_pos;
                        entry.instr_pc = current_instr_index_;
                        entry.num_values = n_vals;
                        entry.values_offset = val_start;
                        deopt_entries_.push_back(entry);
                    }

                    // Store return value before deopt check (deopt clobbers RAX)
                    if (instr.has_dst())
                        StoreGpr(AT::kScratchA, instr.dst_reg());

                    // Check ret_buf[0] for kDeoptMagic
                    // kRetBuf may have been clobbered -- reload from stack save slot.
                    enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16);
                    enc_.EmitMovRM(AT::kScratchB, AT::kScratchC, 0);
                    enc_.EmitMovImm64(AT::kScratchA, kDeoptMagic);
                    enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
                    uint32_t inline_jne_patch_off = buf_.pos() + 2;
                    enc_.EmitJccRel32(kCC_NE, 0); // jne .check_stale

                    // Deopt path: jump to common deopt trampoline
                    {
                        uint32_t inline_deopt_patch = buf_.pos() + 1;
                        enc_.EmitJmpRel32(0);
                        deopt_jump_patches_.push_back({inline_deopt_patch});
                    }

                    // .check_stale — also check HotUpdate stale flag
                    {
                        // Patch the jne from ret_buf check to land here (not stale → skip deopt)
                        uint32_t check_stale_off = buf_.pos();
                        int32_t disp = static_cast<int32_t>(check_stale_off - (inline_jne_patch_off + 4));
                        buf_.Patch32(inline_jne_patch_off, static_cast<uint32_t>(disp));

                        // Load stale flag (atomic<bool>, 0 or 1) from pre-computed pointer
                        enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(stale_flag_ptr_));
                        enc_.EmitMovRM(AT::kScratchA, AT::kScratchA, 0);
                        enc_.EmitTestRR(AT::kScratchA, AT::kScratchA);
                        uint32_t stale_done_patch = buf_.pos() + 2;
                        enc_.EmitJccRel32(kCC_E, 0); // je .slot_done (not stale)

                        // Stale: jump to common deopt trampoline
                        uint32_t stale_deopt_patch = buf_.pos() + 1;
                        enc_.EmitJmpRel32(0);
                        deopt_jump_patches_.push_back({stale_deopt_patch});

                        // .slot_done (normal return — skip deopt path)
                        uint32_t slot_done_off = buf_.pos();
                        {
                            int32_t done_disp = static_cast<int32_t>(slot_done_off - (stale_done_patch + 4));
                            buf_.Patch32(stale_done_patch, static_cast<uint32_t>(done_disp));
                        }

                    } // end check_stale + slot_done block

                    // Jump past C helper path
                    inline_done_jumps.push_back(buf_.pos());
                    enc_.EmitJmpRel32(0); // jmp .done
                } // end for-loop body (slot si)

                // ── Patch all slot miss jumps ──
                // For slot si, the miss jump target is right after this slot's
                // jmp .done instruction (= the next slot's check, or .use_c_helper
                // for the last slot).
                uint32_t c_helper_start = buf_.pos();
                for (size_t si = 0; si < inline_miss_jumps.size(); ++si) {
                    uint32_t target;
                    if (si + 1 < inline_done_jumps.size()) {
                        // Miss goes to next slot's check (right after this slot's jmp .done)
                        target = inline_done_jumps[si] + 5; // jmp rel32 = 5 bytes
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
            enc_.EmitLeaRM(AT::kScratchB, AT::kStackReg, static_cast<int32_t>(kCallVirtArgsOff));
            enc_.EmitLeaRM(AT::kScratchA, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
            enc_.EmitMovMR(AT::kScratchB, 0, AT::kScratchA); // gpr_base
            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(config_.pic_dispatch_data));
            enc_.EmitMovMR(AT::kScratchB, 8, AT::kScratchA); // pic_data
            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(config_.dispatch_ctx));
            enc_.EmitMovMR(AT::kScratchB, 16, AT::kScratchA); // dispatch_ctx
            enc_.EmitMovRIImm32(AT::kScratchA, current_instr_index_);
            enc_.EmitMovMR(AT::kScratchB, 24, AT::kScratchA); // instruction_idx
            enc_.EmitMovRIImm32(AT::kScratchA, arg_count);
            enc_.EmitMovMR(AT::kScratchB, 28, AT::kScratchA); // arg_count
            enc_.EmitMovRIImm32(AT::kScratchA, first_arg_reg);
            enc_.EmitMovMR(AT::kScratchB, 32, AT::kScratchA); // first_arg_reg

            // method_token at offset 36 (from call_cache or 0)
            {
                uint32_t mt = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
                    mt = cached.method_token;
                }
                enc_.EmitMovRIImm32(AT::kScratchA, mt);
                enc_.EmitMovMR(AT::kScratchB, 36, AT::kScratchA); // method_token
            }

            enc_.EmitMovImm64(AT::kScratchA, reinterpret_cast<uint64_t>(instr.imm.ptr));
            enc_.EmitMovMR(AT::kScratchB, 40, AT::kScratchA); // call_target
            enc_.EmitMovRIImm32(AT::kScratchA, instr.has_dst() ? 1 : 0);
            enc_.EmitMovMR(AT::kScratchB, 48, AT::kScratchA); // has_dst
            enc_.EmitMovRIImm32(AT::kScratchA, static_cast<uint32_t>((instr.header >> 63) & 1));
            enc_.EmitMovMR(AT::kScratchB, 52, AT::kScratchA);  // is_instance_call
            enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16); // reload kRetBuf from stack save
            enc_.EmitMovMR(AT::kScratchB, 56, AT::kScratchC);  // ret_buf = saved kRetBuf

            // call CodegenCallVirt
            uint32_t call_pos = buf_.pos();
            EmitRuntimeHelperCall(reinterpret_cast<void*>(::CodegenCallVirt));
            {
                uint32_t call_token = 0, call_module = 0;
                if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
                    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(
                        config_.call_cache)[current_instr_index_];
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
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = (vr < static_cast<uint32_t>(vreg_types_.size()))
                                     ? vreg_types_[vr]
                                     : static_cast<uint8_t>(interpreter::ValueTag::Int64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                    deopt_values_.push_back(dv);
                }
                for (uint32_t vr = kGprCount; vr < kGprCount + kFprCount; ++vr) {
                    DeoptValue dv;
                    dv.reg_index = vr;
                    dv.value_tag = static_cast<uint8_t>(interpreter::ValueTag::Float64);
                    dv.is_spilled = true;
                    dv.spill_offset = static_cast<int16_t>(FprOff(vr));
                    deopt_values_.push_back(dv);
                }
                uint32_t n_vals = static_cast<uint32_t>(deopt_values_.size()) - val_start;
                DeoptEntry entry;
                entry.native_offset = call_pos;
                entry.instr_pc = current_instr_index_;
                entry.num_values = n_vals;
                entry.values_offset = val_start;
                deopt_entries_.push_back(entry);
            }
            // Check ret_buf[0] for kDeoptMagic.
            // kRetBuf may have been clobbered -- reload from stack save slot.
            enc_.EmitMovRM(AT::kScratchC, AT::kFrameReg, -16);
            enc_.EmitMovRM(AT::kScratchB, AT::kScratchC, 0);
            enc_.EmitMovImm64(AT::kScratchA, kDeoptMagic);
            enc_.EmitCmpRR(AT::kScratchA, AT::kScratchB);
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

            if (instr.has_dst())
                StoreGpr(AT::kScratchA, instr.dst_reg());

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

        // ── SIMD operations ──────────────────────────────────────────────
        case IROpCode::Simd: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            int64_t meta = instr.imm.i8;
            uint8_t simd_op = static_cast<uint8_t>(meta & 0xFF);
            uint8_t elem_type = static_cast<uint8_t>((meta >> 8) & 0xFF);
            uint16_t simd_imm = static_cast<uint16_t>((meta >> 16) & 0xFFFF);
            return EmitSimd(instr, simd_op, elem_type, simd_imm);
        }

        // ── FMA (3-operand fused multiply-add) ──────────────────────────
        case IROpCode::SimdFma: {
            if (!instr.has_src1() || !instr.has_src2() || !instr.has_src3() || !instr.has_dst())
                return false;
            int64_t meta = instr.imm.i8;
            uint8_t fma_op = static_cast<uint8_t>(meta & 0xFF);
            uint8_t elem_type = static_cast<uint8_t>((meta >> 8) & 0xFF);

            uint8_t xmm_src1 = 1; // XMM1
            uint8_t xmm_src2 = 2; // XMM2
            uint8_t xmm_acc = 0;  // XMM0 (accumulator/dest)

            LoadFpr(xmm_src1, instr.src1_reg());
            LoadFpr(xmm_src2, instr.src2_reg());
            LoadFpr(xmm_acc, instr.src3_reg());

            // VEX 3-operand: acc = src1 * src2 + acc (231 form)
            switch (fma_op) {
                case 0:                 // kSimdFmaAdd
                    if (elem_type == 4) // kElemFloat32 → PS
                        enc_.EmitVfmadd231psRR(xmm_acc, xmm_src1, xmm_src2);
                    else // kElemFloat64 → PD
                        enc_.EmitVfmadd231pdRR(xmm_acc, xmm_src1, xmm_src2);
                    break;
                case 1: // kSimdFmaSub
                    if (elem_type == 4)
                        enc_.EmitVfmsub231psRR(xmm_acc, xmm_src1, xmm_src2);
                    else
                        enc_.EmitVfmsub231pdRR(xmm_acc, xmm_src1, xmm_src2);
                    break;
                case 2: // kSimdFmaNegAdd (negate multiply-add: a * b - c)
                    if (elem_type == 4)
                        enc_.EmitVfnmadd231psRR(xmm_acc, xmm_src1, xmm_src2);
                    else
                        enc_.EmitVfnmadd231pdRR(xmm_acc, xmm_src1, xmm_src2);
                    break;
                case 3: // kSimdFmaNegSub (negate multiply-sub: -(a * b) - c)
                    if (elem_type == 4)
                        enc_.EmitVfnmsub231psRR(xmm_acc, xmm_src1, xmm_src2);
                    else
                        enc_.EmitVfnmsub231pdRR(xmm_acc, xmm_src1, xmm_src2);
                    break;
                default:
                    return false;
            }

            StoreFpr(instr.dst_reg(), xmm_acc);
            return true;
        }

        // ── POPCNT / LZCNT (bit manipulation intrinsics) ────────────────
        case IROpCode::Popcnt: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitPopcntRR(AT::kScratchA, AT::kScratchA);
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        case IROpCode::Lzcnt: {
            if (!instr.has_src1() || !instr.has_dst())
                return false;
            LoadGpr(AT::kScratchA, instr.src1_reg());
            enc_.EmitLzcntRR(AT::kScratchA, AT::kScratchA);
            StoreGpr(AT::kScratchA, instr.dst_reg());
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

static void OptimizeInstructions(CHAOS_IL2CPP_VECTOR(::chaos::il2cpp::interpreter::RegisterInstruction) & instrs,
                                 std::vector<uint8_t>& removed_mask, bool has_seh) noexcept {
    using IROpCode = ::chaos::il2cpp::interpreter::IROpCode;
    uint32_t n = static_cast<uint32_t>(instrs.size());
    if (n == 0)
        return;

    // ── Pass 1: Use count ────────────────────────────────────────────
    uint32_t use_count[64] = {};
    for (uint32_t i = 0; i < n; ++i) {
        const auto& ri = instrs[i];
        if (ri.has_src1() && ri.src1_reg() < 64)
            ++use_count[ri.src1_reg()];
        if (ri.has_src2() && ri.src2_reg() < 64)
            ++use_count[ri.src2_reg()];
        if ((ri.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3) && ri.src3_reg() < 64)
            ++use_count[ri.src3_reg()];
    }

    // ── Header builder for folded instructions ───────────────────────
    auto MakeHdr = [](IROpCode opc, uint8_t dst, uint8_t src1, uint8_t src2, uint8_t flags) -> uint64_t {
        return static_cast<uint64_t>(opc) | (static_cast<uint64_t>(dst) << 16) | (static_cast<uint64_t>(src1) << 24) |
               (static_cast<uint64_t>(src2) << 32) | (static_cast<uint64_t>(flags) << 40);
    };

    // Foldable pure-arithmetic opcodes (no side effects, no overflow).
    auto IsFoldable = [](IROpCode opc) -> bool {
        return opc == IROpCode::Add || opc == IROpCode::Sub || opc == IROpCode::Mul || opc == IROpCode::And ||
               opc == IROpCode::Or || opc == IROpCode::Xor || opc == IROpCode::Shl || opc == IROpCode::Shr;
    };

    auto FoldBinary = [](IROpCode opc, int32_t a, int32_t b) -> int32_t {
        switch (opc) {
            case IROpCode::Add:
                return a + b;
            case IROpCode::Sub:
                return a - b;
            case IROpCode::Mul:
                return a * b;
            case IROpCode::And:
                return a & b;
            case IROpCode::Or:
                return a | b;
            case IROpCode::Xor:
                return a ^ b;
            case IROpCode::Shl:
                return static_cast<int32_t>(static_cast<uint32_t>(a) << (static_cast<uint32_t>(b) & 0x1F));
            case IROpCode::Shr:
                return a >> (static_cast<uint32_t>(b) & 0x1F);
            default:
                return 0;
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
                if (prev.op_code() == IROpCode::LdcI4) {
                    // Check for backward branches from current_idx onward that
                    // target an instruction between the def (j-1) and current_idx.
                    // Such back-edges skip the def on loop re-entry, making the
                    // "constant" stale.
                    for (uint32_t k = current_idx; k < instrs.size(); ++k) {
                        const auto& bi = instrs[k];
                        auto bopc = bi.op_code();
                        if (bi.has_imm() &&
                            (bopc == IROpCode::Br || bopc == IROpCode::BrTrue || bopc == IROpCode::BrFalse ||
                             bopc == IROpCode::Beq || bopc == IROpCode::BneUn || bopc == IROpCode::Blt ||
                             bopc == IROpCode::BltUn || bopc == IROpCode::Bgt || bopc == IROpCode::BgtUn ||
                             bopc == IROpCode::Ble || bopc == IROpCode::BleUn)) {
                            uint32_t target = bi.imm.branch_target;
                            // target < k = backward branch; target between def and
                            // current_idx means loop re-entry skips the LdcI4 def.
                            if (target < k && target >= j - 1 && target <= current_idx) {
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
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = result;
                did_opt = true;
                continue;
            }
        }

        // (a2) Constant folding — unary Neg/Not with constant src
        if (opc == IROpCode::Neg && ri.has_dst() && ri.has_src1()) {
            const int32_t* v = FindDefLdcI4(i, ri.src1_reg());
            if (v) {
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = -*v;
                did_opt = true;
                continue;
            }
        }
        if (opc == IROpCode::Not && ri.has_dst() && ri.has_src1()) {
            const int32_t* v = FindDefLdcI4(i, ri.src1_reg());
            if (v) {
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = ~(*v);
                did_opt = true;
                continue;
            }
        }

        // (b) DCE — unused LdcI4
        if (opc == IROpCode::LdcI4 && ri.has_dst()) {
            uint8_t dst = ri.dst_reg();
            if (dst < 64 && use_count[dst] == 0) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }
        }

        // (c) BrFalse simplification
        if (opc == IROpCode::BrFalse && ri.has_src1()) {
            const int32_t* v = FindDefLdcI4(i, ri.src1_reg());
            if (v) {
                uint32_t target = ri.imm.branch_target;
                if (*v == 0) {
                    // src == 0 → branch IS taken → unconditional Br
                    ri.header =
                        MakeHdr(IROpCode::Br, 0, 0, 0,
                                ::chaos::il2cpp::interpreter::kRegIsBranch | ::chaos::il2cpp::interpreter::kRegHasImm);
                    ri.imm.branch_target = target;
                } else {
                    // src != 0 → branch NOT taken → remove (fall through)
                    removed_mask[i] = 1;
                }
                did_opt = true;
            }
        }

        // (d) Unbox elimination: Box(dst_b, src) + Unbox(dst_u, dst_b) → Dup(dst_u, src)
        if (opc == IROpCode::Unbox && ri.has_src1()) {
            uint8_t unbox_src = ri.src1_reg();
            uint8_t unbox_dst = ri.has_dst() ? ri.dst_reg() : 0;
            for (uint32_t j = i; j > 0; --j) {
                auto& prev = instrs[j - 1];
                if (prev.has_dst() && prev.dst_reg() == unbox_src) {
                    if (prev.op_code() == IROpCode::Box && prev.has_src1() && unbox_src < 64 &&
                        use_count[unbox_src] == 1) // Box dst used only by this Unbox
                    {
                        uint8_t box_src1 = prev.src1_reg();
                        prev.header = MakeHdr(
                            IROpCode::Dup, unbox_dst, box_src1, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                        prev.imm.i4 = 0;
                        removed_mask[i] = 1;
                        did_opt = true;
                    }
                    break;
                }
            }
        }

        // (e) Dead store elimination: consecutive StLoc to same local
        if (opc == IROpCode::StLoc && ri.has_src1() && i > 0) {
            auto& prev = instrs[i - 1];
            if (prev.op_code() == IROpCode::StLoc && prev.imm.operand_index == ri.imm.operand_index &&
                !removed_mask[i - 1]) {
                // Previous StLoc writes to the same local vreg and is immediately
                // overwritten — safe to remove regardless of use_count.
                removed_mask[i - 1] = 1;
                did_opt = true;
            }
        }

        // (e2) Extended dead store: non-adjacent StLoc to same local (EBB-safe scan)
        if (opc == IROpCode::StLoc && ri.has_src1() && i > 0) {
            uint32_t local_idx = ri.imm.operand_index;
            const uint32_t kMaxScan = 50;
            for (uint32_t j = i; j > 0 && (i - j) < kMaxScan; --j) {
                uint32_t idx = j - 1;
                if (removed_mask[idx])
                    continue;
                auto& prev = instrs[idx];
                auto prev_opc = prev.op_code();
                // Stop at branches, calls, or terminators (different execution path)
                if (prev_opc == IROpCode::Br || prev_opc == IROpCode::BrTrue || prev_opc == IROpCode::BrFalse ||
                    prev_opc == IROpCode::Beq || prev_opc == IROpCode::Blt || prev_opc == IROpCode::Bgt ||
                    prev_opc == IROpCode::Ble || prev_opc == IROpCode::Bge || prev_opc == IROpCode::BneUn ||
                    prev_opc == IROpCode::BgeUn || prev_opc == IROpCode::BgtUn || prev_opc == IROpCode::BleUn ||
                    prev_opc == IROpCode::BltUn || prev_opc == IROpCode::Switch || prev_opc == IROpCode::Call ||
                    prev_opc == IROpCode::CallVirt)
                    break;
                if (prev_opc == IROpCode::StLoc && prev.imm.operand_index == local_idx) {
                    removed_mask[idx] = 1; // previous StLoc is dead
                    did_opt = true;
                    break;
                }
                if (prev_opc == IROpCode::LdLoc && prev.src1_reg() == local_idx) {
                    break; // LdLoc reads this local — not dead
                }
            }
        }

        // (f) Copy propagation: Dup elimination + forwarding
        if (opc == IROpCode::Dup && ri.has_dst() && ri.has_src1()) {
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
                    if (removed_mask[k])
                        continue;
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
                    break; // only forward to the first non-removed user
                }
            }
        }

        // (g) Redundant LdLoc elimination
        if (opc == IROpCode::LdLoc && ri.has_dst() && ri.has_src1()) {
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
                if (prev.op_code() == IROpCode::StLoc && !removed_mask[i - 1] && prev.has_dst() &&
                    prev.dst_reg() == local_vreg && prev.has_src1()) {
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
                const uint32_t kMaxScan = 50;
                // Scan backward for the defining StLoc
                for (uint32_t j = i; j > 0 && (i - j) < kMaxScan; --j) {
                    uint32_t idx = j - 1;
                    if (removed_mask[idx])
                        continue;
                    auto& prev = instrs[idx];
                    auto prev_opc = prev.op_code();
                    if (prev_opc == IROpCode::StLoc && prev.imm.operand_index == local_vreg && prev.has_src1()) {
                        uint8_t store_src = prev.src1_reg();
                        // Scan forward from LdLoc for the single user
                        for (uint32_t k = i + 1; k < n; ++k) {
                            if (removed_mask[k])
                                continue;
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
                    if (prev_opc == IROpCode::Br || prev_opc == IROpCode::BrTrue || prev_opc == IROpCode::BrFalse ||
                        prev_opc == IROpCode::Beq || prev_opc == IROpCode::Blt || prev_opc == IROpCode::Bgt ||
                        prev_opc == IROpCode::Ble || prev_opc == IROpCode::Bge || prev_opc == IROpCode::BneUn ||
                        prev_opc == IROpCode::BgeUn || prev_opc == IROpCode::BgtUn || prev_opc == IROpCode::BleUn ||
                        prev_opc == IROpCode::BltUn || prev_opc == IROpCode::Switch || prev_opc == IROpCode::Call ||
                        prev_opc == IROpCode::CallVirt)
                        break;
                }
            }
        }

        // (h) Branch-to-branch forwarding + dead Br elimination
        if ((opc == IROpCode::Br || opc == IROpCode::BrFalse || opc == IROpCode::BrTrue) && ri.has_imm()) {
            uint32_t target = ri.imm.branch_target;

            // (h1) Dead unconditional Br: branch to next instruction → fall-through
            if (opc == IROpCode::Br && target == i + 1) {
                removed_mask[i] = 1;
                did_opt = true;
                continue;
            }

            // (h2) Br/BrFalse/BrTrue forwarding: resolve Br → Br chains
            uint32_t resolved = target;
            uint32_t max_hop = 16;
            while (resolved < n && max_hop > 0) {
                auto& target_instr = instrs[resolved];
                if (target_instr.op_code() == IROpCode::Br && target_instr.has_imm() && !removed_mask[resolved]) {
                    uint32_t next_target = target_instr.imm.branch_target;
                    if (next_target == resolved)
                        break; // self-loop guard
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
                if (opc == IROpCode::Br && resolved == i + 1) {
                    removed_mask[i] = 1;
                }
            }
        }

        // (i) Arithmetic identity intrinsics: eliminate redundant operations
        if (ri.has_dst() && ri.has_src1()) {
            bool identity_opt = false;

            if (opc == IROpCode::Add || opc == IROpCode::Sub || opc == IROpCode::Or || opc == IROpCode::Xor ||
                opc == IROpCode::Shl || opc == IROpCode::Shr || opc == IROpCode::ShrUn) {
                // x op 0 → x (src2 is constant 0)
                if (ri.has_src2()) {
                    const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
                    if (v2 && *v2 == 0) {
                        ri.header = MakeHdr(
                            IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    }
                }
            }

            if (!identity_opt && opc == IROpCode::Mul && ri.has_src2()) {
                const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
                if (v2) {
                    if (*v2 == 1) {
                        // x * 1 → x
                        ri.header = MakeHdr(
                            IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    } else if (*v2 == 0) {
                        // x * 0 → 0
                        ri.header = MakeHdr(
                            IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    }
                }
            }

            if (!identity_opt && opc == IROpCode::And && ri.has_src2()) {
                if (ri.src1_reg() == ri.src2_reg()) {
                    // x & x → x
                    ri.header =
                        MakeHdr(IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                                ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                } else {
                    // x & 0 → 0 (src2 is constant 0)
                    const int32_t* v2 = FindDefLdcI4(i, ri.src2_reg());
                    if (v2 && *v2 == 0) {
                        ri.header = MakeHdr(
                            IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                        ri.imm.i4 = 0;
                        identity_opt = true;
                    }
                }
            }

            if (!identity_opt && opc == IROpCode::Or && ri.has_src2()) {
                if (ri.src1_reg() == ri.src2_reg()) {
                    // x | x → x
                    ri.header =
                        MakeHdr(IROpCode::Dup, ri.dst_reg(), ri.src1_reg(), 0,
                                ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasSrc1);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                }
            }

            if (!identity_opt && opc == IROpCode::Xor && ri.has_src2()) {
                if (ri.src1_reg() == ri.src2_reg()) {
                    // x ^ x → 0
                    ri.header =
                        MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                    ri.imm.i4 = 0;
                    identity_opt = true;
                }
            }

            if (!identity_opt && opc == IROpCode::Sub) {
                if (ri.has_src2() && ri.src1_reg() == ri.src2_reg()) {
                    // x - x → 0
                    ri.header =
                        MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                                ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
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
            if (opc == IROpCode::DivUn || opc == IROpCode::RemUn) {
                uint8_t src2_reg = ri.src2_reg();
                if (src2_reg < 64 && use_count[src2_reg] == 1) {
                    const int32_t* v2 = FindDefLdcI4(i, src2_reg);
                    if (v2 && *v2 > 0) {
                        uint32_t shift = 0;
                        uint32_t uv = static_cast<uint32_t>(*v2);
                        if (uv && (uv & (uv - 1)) == 0) {
                            while ((uv >> shift) > 1)
                                ++shift;
                            if (opc == IROpCode::DivUn) {
                                // DivUn by 2^k → ShrUn by k (logical shift, unsigned)
                                ri.header = MakeHdr(IROpCode::ShrUn, ri.dst_reg(), ri.src1_reg(), src2_reg,
                                                    ::chaos::il2cpp::interpreter::kRegHasDst |
                                                        ::chaos::il2cpp::interpreter::kRegHasSrc1 |
                                                        ::chaos::il2cpp::interpreter::kRegHasSrc2);
                                for (uint32_t j = i; j > 0; --j) {
                                    auto& prev = instrs[j - 1];
                                    if (prev.has_dst() && prev.dst_reg() == src2_reg &&
                                        prev.op_code() == IROpCode::LdcI4) {
                                        prev.imm.i4 = static_cast<int32_t>(shift);
                                        break;
                                    }
                                }
                                did_opt = true;
                                continue;
                            } else {
                                // RemUn by 2^k → And with (2^k - 1)
                                uint32_t mask = (1u << shift) - 1u;
                                ri.header = MakeHdr(IROpCode::And, ri.dst_reg(), ri.src1_reg(), src2_reg,
                                                    ::chaos::il2cpp::interpreter::kRegHasDst |
                                                        ::chaos::il2cpp::interpreter::kRegHasSrc1 |
                                                        ::chaos::il2cpp::interpreter::kRegHasSrc2);
                                for (uint32_t j = i; j > 0; --j) {
                                    auto& prev = instrs[j - 1];
                                    if (prev.has_dst() && prev.dst_reg() == src2_reg &&
                                        prev.op_code() == IROpCode::LdcI4) {
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
            if (opc == IROpCode::Ceq) {
                // x == x → 1
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = 1;
                did_opt = true;
                continue;
            }
            if (opc == IROpCode::Clt || opc == IROpCode::Cgt) {
                // x < x → 0,  x > x → 0
                ri.header =
                    MakeHdr(IROpCode::LdcI4, ri.dst_reg(), 0, 0,
                            ::chaos::il2cpp::interpreter::kRegHasDst | ::chaos::il2cpp::interpreter::kRegHasImm);
                ri.imm.i4 = 0;
                did_opt = true;
                continue;
            }
        }

        // (l) Branch-to-switch conversion: consecutive Beq (same src1 vs different LdcI4) → Switch
        if (opc == IROpCode::Beq && ri.has_src1() && ri.has_src2() && !removed_mask[i]) {
            uint8_t switch_reg = ri.src1_reg();
            uint32_t max_cases = (n - i) / 2; // each case = Beq (+ optional LdcI4)
            if (max_cases > 64)
                max_cases = 64;

            // Collect cases: (value_k, target_k) pairs
            struct BeqCase {
                int32_t value;
                uint32_t target;
                uint32_t beq_idx;
            };
            BeqCase cases[64];
            uint32_t case_count = 0;
            bool valid_chain = true;
            uint32_t default_target = n; // fall-through

            for (uint32_t j = i; j < n && case_count < max_cases;) {
                if (j > i) {
                    // Skip any LdcI4 that defines the Beq's src2
                    if (instrs[j].op_code() == IROpCode::LdcI4 && instrs[j].has_dst() && !removed_mask[j]) {
                        ++j;
                        continue;
                    }
                }
                auto& cur = instrs[j];
                if (cur.op_code() != IROpCode::Beq || !cur.has_src1() || !cur.has_src2() ||
                    cur.src1_reg() != switch_reg || removed_mask[j]) {
                    // Check for trailing Br (default target)
                    if (cur.op_code() == IROpCode::Br && cur.has_imm() && case_count >= 3) {
                        default_target = cur.imm.branch_target;
                        removed_mask[j] = 1; // remove Br, Switch handles default
                        did_opt = true;
                    }
                    if (case_count < 3)
                        valid_chain = false;
                    break;
                }
                // Find the LdcI4 defining this Beq's src2 (scan backward from j)
                uint8_t src2 = cur.src2_reg();
                const int32_t* val = FindDefLdcI4(j, src2);
                if (val == nullptr) {
                    valid_chain = false;
                    break;
                }
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
                    static_cast<uint64_t>(IROpCode::Switch) | (static_cast<uint64_t>(0) << 16) |   // no dst
                    (static_cast<uint64_t>(switch_reg) << 24) | (static_cast<uint64_t>(0) << 32) | // no src2
                    (static_cast<uint64_t>(::chaos::il2cpp::interpreter::kRegHasSrc1 |
                                           ::chaos::il2cpp::interpreter::kRegHasImm |
                                           ::chaos::il2cpp::interpreter::kRegIsBranch)
                     << 40) |
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

    if (!did_opt)
        removed_mask.clear();
}

JitMethod* NativeCodeGenerator::Generate() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::Generate");
    uint32_t n_instrs = static_cast<uint32_t>(rm_.instructions.size());
    if (n_instrs == 0)
        return nullptr;
    instr_offsets_.resize(n_instrs, 0);

    // Initialize type inference state for all GPR vregs
    vreg_types_.assign(kGprCount, kTypeVoid);

    // Scan for LocAlloc: if used, enlarge frame to include stack reserve.
    // This avoids heap allocation (no leak, freed on method return).
    localloc_extra_ = 0;
    for (const auto& instr : rm_.instructions) {
        if (instr.op_code() == IROpCode::LocAlloc) {
            localloc_extra_ = kLocAllocBumpAndReserve;
            break;
        }
    }

    // Pre-scan: count managed call instructions to reserve slot table entries.
    // slot_count_ = number of Call/CallBridge instructions (not Calli, not CallVirt).
    slot_count_ = 0;
    // T2.1 A1: determine whether any instruction can clobber caller-saved
    // registers (and thus require the caller-colored write-through in
    // StoreGpr/StoreFpr).  A managed/runtime helper call, an enabled safepoint
    // poll, an overflow-checked op (deopt), or ANY object/heap/memory op
    // (allocation, field/element access, box/cast, write barrier — all of
    // which either emit a runtime helper call, can trigger a GC that scans the
    // frame, or write through a write barrier) can clobber/require caller-saved
    // values on the stack.  Only a strictly scalar, call-free method (pure
    // arithmetic/bitwise/shift/compare/convert on Int32/Int64/Float) is safe
    // to keep purely register-resident.
    bool can_clobber_caller = config_.enable_safepoint_polls && config_.safepoint_fn != nullptr;
    for (const auto& instr : rm_.instructions) {
        auto opc = instr.op_code();
        if (opc == IROpCode::Call || opc == IROpCode::CallBridge || opc == IROpCode::CallVirt ||
            opc == IROpCode::Calli) {
            slot_count_ += (opc == IROpCode::Call || opc == IROpCode::CallBridge);
            can_clobber_caller = true;
            continue;
        }
        // Whitelist of ops that are pure scalar (Int/FP) register-resident
        // safe: no heap, no GC, no call, no overflow-deopt, no memory RMW.
        switch (opc) {
            case IROpCode::LdcI4:
            case IROpCode::LdcI8:
            case IROpCode::LdcR4:
            case IROpCode::LdcR8:
            case IROpCode::Add:
            case IROpCode::Sub:
            case IROpCode::Mul:
            case IROpCode::Div:
            case IROpCode::Rem:
            case IROpCode::DivUn:
            case IROpCode::RemUn:
            case IROpCode::Neg:
            case IROpCode::Not:
            case IROpCode::And:
            case IROpCode::Or:
            case IROpCode::Xor:
            case IROpCode::Shl:
            case IROpCode::Shr:
            case IROpCode::ShrUn:
            case IROpCode::Ceq:
            case IROpCode::Clt:
            case IROpCode::Cgt:
            case IROpCode::Conv_I4:
            case IROpCode::Conv_I8:
            case IROpCode::ConvI:
            case IROpCode::ConvU:
            case IROpCode::Conv_R4:
            case IROpCode::Conv_R8:
            case IROpCode::ConvRUn:
            case IROpCode::Abs:
            case IROpCode::Min:
            case IROpCode::Max:
            case IROpCode::Dup:
            case IROpCode::Pop:
            case IROpCode::Ret:
                break; // safe; keep resident
            default:
                can_clobber_caller = true; // object/heap/call/memory/br — not resident-safe
                break;
        }
    }
    has_caller_clobber_ = can_clobber_caller;
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
        std::memset(phys_to_colored_vreg_, 0xFF, sizeof(phys_to_colored_vreg_));
        for (uint32_t vr = 0; vr < kGprCount; ++vr) {
            uint8_t x64r = gcr_.gpr_color[vr];
            if (x64r != 0xFF) {
                has_graph_coloring_ = true;
                phys_to_colored_vreg_[x64r] = static_cast<uint8_t>(vr);
            }
        }
        if (has_graph_coloring_) {
            // Print final coloring after filter for diagnostics
            std::printf("    [GC] %u GPR callee colors (%u instrs):", num_cache_regs_, n_instrs);
            for (uint32_t dv = 0; dv < kGprCount; ++dv) {
                if (gcr_.gpr_color[dv] != 0xFF)
                    std::printf(" r%u→x%u", dv, gcr_.gpr_color[dv]);
            }
            std::printf(" | caller_mask=0x%llx filtered_mask=0x%llx\n", (unsigned long long)caller_colored_mask_,
                        (unsigned long long)filtered_vreg_mask_);

            // V3: keep all colored registers, build caller_colored_mask_ for
            // caller-saved regs.  EmitCallWithSpill reloads these after each
            // runtime helper call.  Only callee-saved regs are saved/restored
            // in prologue/epilogue (PUSH/POP on x64, STP/LDP on ARM64).
            bool seen[32] = {};
            num_cache_regs_ = 0;
            filtered_vreg_mask_ = 0;
            caller_colored_mask_ = 0;
            caller_fpr_colored_mask_ = 0;
            for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                uint8_t x64r = gcr_.gpr_color[vr];
                if (x64r != 0xFF) {
#if defined(__aarch64__)
                    // ARM64 callee-saved: X19-X28
                    bool is_callee = (x64r >= 19 && x64r <= 28);
#else
                    // x64 callee-saved: R12-R15 (SysV — RDI is caller-saved)
                    bool is_callee = (x64r >= 12 && x64r <= 15);
#endif
                    if (is_callee) {
                        if (!seen[x64r] && num_cache_regs_ < kMaxCacheRegs) {
                            seen[x64r] = true;
                            callee_gpr_regs_[num_cache_regs_++] = x64r;
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
                        // Caller-saved register.
                        // Check for duplicate physical register (same issue as
                        // callee-saved): the post-call reload iterates through
                        // caller_colored_mask_ and reloads each vreg from its
                        // stack slot — when two vregs share the same register,
                        // the second reload overwrites the first, corrupting
                        // the value for subsequent instructions that read it.
                        if (!seen[x64r]) {
                            seen[x64r] = true;
                            caller_colored_mask_ |= (1ULL << vr);
                        } else {
                            gcr_.gpr_color[vr] = 0xFF; // duplicate → stack spill
                        }
                    }
                }
            }
            cached_slots_used_ = 0;
            cached_dirty_mask_ = 0;
            // Clear V1 cache mappings so spilled vregs fall through to stack
            std::memset(cached_x64_for_vreg_, 0xFF, sizeof(cached_x64_for_vreg_));
            callee_saved_regs_ = callee_gpr_regs_;

            // ── FPR callee-saved filter ────────────────────────────────
            // x64 Win64: XMM6-XMM15 are callee-saved.
            // ARM64: V8-V15 are callee-saved.
            num_fpr_callee_ = 0;
            bool xmm_seen[16] = {};
            for (uint32_t fi = 0; fi < 32; ++fi) {
                uint8_t xmm = gcr_.fpr_color[fi];
                if (xmm != 0xFF) {
#if defined(__aarch64__)
                    // ARM64 NEON: V8-V15 callee-saved
                    bool fpr_callee = (xmm >= 8 && xmm <= 15);
#else
                    // x64: XMM6-XMM15 callee-saved
                    bool fpr_callee = (xmm >= 6 && xmm <= 15);
#endif
                    if (fpr_callee) {
                        if (!xmm_seen[xmm] && num_fpr_callee_ < 10) {
                            xmm_seen[xmm] = true;
                            callee_xmm_regs_[num_fpr_callee_] = xmm;
                            callee_xmm_fi_[num_fpr_callee_] = static_cast<uint8_t>(fi);
                            ++num_fpr_callee_;
                        } else {
                            gcr_.fpr_color[fi] = 0xFF; // already claimed → spill
                        }
                    } else {
                        // Caller-saved FPR: keep the color assignment.
                        // EmitCallWithSpill will reload from stack after each call.
                        uint32_t fpr_vreg = fi;
                        caller_fpr_colored_mask_ |= (1ULL << fpr_vreg);
                    }
                }
            }
            xmm_save_size_ = static_cast<int32_t>(num_fpr_callee_) * 16;
        } else {
            SelectCacheableRegs(); // V1 fallback
        }
    } else {
        SelectCacheableRegs();
    }
#if defined(__aarch64__)
    // ARM64: STP is always 16 bytes — frame_align_adj_ not needed.
    frame_align_adj_ = 0;
#else
    // x64: after CALL (RSP -= 8), each PUSH/N pushes by 8.
    // Alignment: RSP_pre_call - 8 - 8*num_push_regs ≡ 0 (mod 16)
    // → num_push_regs must be ODD for alignment.
    // With 4 base pushes (rbp, rbx, rsi, rdi), num_push_regs = 4 + num_cache_regs_.
    // So pad 8 when num_push_regs is EVEN → pad when num_cache_regs_ is even.
    frame_align_adj_ = ((num_cache_regs_ + 4) % 2) * 8;
#endif

    // Prologue — push/STP callee-saved regs, establish frame pointer
#if defined(__aarch64__)
    // ARM64 prologue: STP X29, X30 (FP+LR), MOV X29, SP, save ABI ret_buf
    // (X1) below FP/LR, then cacheable regs, then SUB SP, #N.
    prologue_push_offsets_[0] = buf_.pos();
    EmitStp64Pre(buf_, AT::kFrameReg, 30, kARM64_SP, -16); // STP X29, X30, [SP, #-16]!
    prologue_set_fpreg_offset_ = buf_.pos();
    enc_.EmitAddRI(AT::kFrameReg, AT::kStackReg, 0); // MOV X29, SP
    // Save ABI ret_buf (X1) at [X29-16] so Ret handler can write return value
    // through it (matching x64 saved-RSI approach).  X29 stays at the saved
    // X29/X30 pair (from the STP above); X1 sits one slot below at [X29-16].
    EmitStp64Pre(buf_, 1, 0, kARM64_SP, -16); // STP X1, X0, [SP, #-16]! (X0=scratch, never XZR)
    // Save additional callee-saved regs used for register caching (Phase 3+)
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot) {
        prologue_push_offsets_[4 + slot] = buf_.pos();
        EmitStp64Pre(buf_, callee_saved_regs_[slot], 0, kARM64_SP,
                     -16); // STP Xreg, X0, [SP, #-16]! (X0=scratch, never XZR)
    }
    prologue_sub_rsp_offset_ = buf_.pos();
    prologue_sub_rsp_size_ =
        static_cast<uint32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_ + localloc_extra_);
    enc_.EmitSubRI(AT::kStackReg, static_cast<int32_t>(prologue_sub_rsp_size_));
    enc_.EmitAddRI(AT::kArgsBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff)); // X3 = SP + GPR file
    enc_.EmitAddRI(AT::kRetBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff));  // X4 = SP + GPR file
    prologue_total_bytes_ = buf_.pos() - prologue_push_offsets_[0];
    // Build push register list (ARM64 unwind — Phase 4)
    push_reg_nums_[0] = AT::kFrameReg;
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        push_reg_nums_[1 + slot] = callee_saved_regs_[slot];
    num_push_regs_ = 1 + num_cache_regs_;
#else
    prologue_push_offsets_[0] = buf_.pos();
    enc_.EmitPush(AT::kFrameReg);
    prologue_set_fpreg_offset_ = buf_.pos();
    enc_.EmitAddRI(AT::kFrameReg, AT::kStackReg, 0); // frame pointer chain for GC stack walking
    prologue_push_offsets_[1] = buf_.pos();
    enc_.EmitPush(AT::kArgsBuf);
    prologue_push_offsets_[2] = buf_.pos();
    enc_.EmitPush(AT::kRetBuf);
    prologue_push_offsets_[3] = buf_.pos();
    enc_.EmitPush(kRDI); // x64 ABI: RDI is callee-saved; used by REP STOSQ below
    // Push additional callee-saved regs used for register caching
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        prologue_push_offsets_[4 + slot] = buf_.pos(), enc_.EmitPush(callee_saved_regs_[slot]);
    prologue_sub_rsp_offset_ = buf_.pos();
    prologue_sub_rsp_size_ =
        static_cast<uint32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_ + localloc_extra_);
    enc_.EmitSubRI(AT::kStackReg, static_cast<int32_t>(prologue_sub_rsp_size_));
    // Set up register convention: RBX = args_buf, RSI = ret_buf.
    // Both point to the GPR file in the local frame (not from caller RCX/RDX),
    // since JIT mode calls these functions as void() with no arguments.
    enc_.EmitLeaRM(AT::kArgsBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
    enc_.EmitLeaRM(AT::kRetBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
    prologue_total_bytes_ = buf_.pos() - prologue_push_offsets_[0];
    // Build push register list for unwind info: rbp, rbx, rsi, rdi, cached regs
    push_reg_nums_[0] = AT::kFrameReg;
    push_reg_nums_[1] = AT::kArgsBuf;
    push_reg_nums_[2] = AT::kRetBuf;
    push_reg_nums_[3] = kRDI;
    for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
        push_reg_nums_[4 + slot] = callee_saved_regs_[slot];
    num_push_regs_ = 4 + num_cache_regs_;
#endif

    // Save callee-saved XMM registers (used by graph coloring)
    // Stored in the area just below the regular frame (at RSP + kFrameSize + frame_size_extra_ + frame_align_adj_).
    for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
        int32_t off = static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + si * 16);
        enc_.EmitMovUPSMR(AT::kStackReg, off, callee_xmm_regs_[si]);
    }

    // Zero-initialize all GPR stack slots — reading an uninitialized slot
    // produces garbage.  Done BEFORE colored-reg zeroing to use RDI as scratch.
    enc_.EmitXorZR(AT::kScratchA);
#if defined(__aarch64__)
    // ARM64: zero GPR stack slots using STP XZR, XZR pairs (halves instruction
    // count vs. STR-per-slot).  Each STP zeroes 2 × 8 = 16 bytes.
    // kGprFileOff is always 0, so pairs cover [0, kGprCount * 8) in stride 16.
    // Register 31 = XZR (zero register) for STP source; kARM64_SP = 31 = SP base.
    for (uint32_t i = 0; i < kGprCount; i += 2) {
        EmitStp64(buf_, 31, 31, kARM64_SP, static_cast<int32_t>((kGprFileOff + i * 8) / 16));
    }
#else
    enc_.EmitLeaRM(kRDI, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
    enc_.EmitMovRIImm32(AT::kScratchB, kGprCount);
    buf_.EmitByte(0xF3);
    buf_.EmitByte(0x48);
    buf_.EmitByte(0xAB);
#endif

    // Zero the localloc bump pointer (RAX is still 0 from xor above)
    if (localloc_extra_ > 0) {
        enc_.EmitMovMR(AT::kStackReg,
                       static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_),
                       AT::kScratchA);
    }

    // Initialize colored callee-saved regs to 0 (re-zeros RDI if colored,
    // since REP STOSQ above left RDI pointing past the GPR file).
    if (has_graph_coloring_) {
        for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
            enc_.EmitXorRR(callee_saved_regs_[slot], callee_saved_regs_[slot]);
    }

    // Zero caller-colored registers — LoadGpr reads them directly
    // for vregs colored to caller-saved regs, bypassing the zeroed stack.
    // Without this, a read-before-write vreg in a caller-saved reg returns
    // garbage from the caller, while RegisterExecute returns 0.
    if (caller_colored_mask_) {
#if defined(__aarch64__)
        // ARM64: zero all caller-saved GPRs in the coloring pool (X5-X17)
        for (uint32_t x64r = 5; x64r <= 17; ++x64r)
            enc_.EmitXorRR(x64r, x64r);
#else
        // x64: zero R8-R11
        for (uint32_t x64r = AT::kExtraScratch0; x64r <= AT::kExtraScratch2; ++x64r)
            enc_.EmitXorRR(x64r, x64r);
#endif
    }

    // ── GC mode switch: EnterCooperativeMode ──────────────────────────
    // Switch the thread to cooperative GC mode before any instruction
    // that allocates or accesses the managed heap.  This must happen
    // after the frame is set up (stack walking works) but before the
    // first managed object access.
    if (config_.cooperative_fn != nullptr) {
#if !defined(__aarch64__)
        enc_.EmitSubRI(AT::kStackReg, 32); // shadow space for Win64 ABI
#endif
        uint32_t call_start = buf_.pos();
        enc_.EmitCallRipRel(0);
        slot_patches_.push_back(
            {call_start + kSlotPatchDispOff, UINT32_MAX, reinterpret_cast<void*>(config_.cooperative_fn)});
        slot_count_used_++;
        uint32_t call_pos = call_start;
#if !defined(__aarch64__)
        enc_.EmitAddRI(AT::kStackReg, 32);
#endif
        // Record as GC point so stack is walkable during mode switch
        call_sites_.push_back({UINT32_MAX, call_pos});
        RecordGcPoint(call_pos);
    }

    // Bail out early if any emit above hit an OOM
    if (CheckFailed())
        return nullptr;

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
            if (tree::OptimizeWithTreeIR(opt_instrs, tree_opt, false, rm_.max_regs, config_.enable_inlining,
                                         &inline_results)) {
                opt_instrs = std::move(tree_opt);
                n_instrs = static_cast<uint32_t>(opt_instrs.size());

                // Resize vreg_types_ to cover tree-optimizer-created vregs
                // (Linearizer starts at tree::Linearizer::kBaseVReg = 64, which
                //  is >= kGprCount, so the original vreg_types_[kGprCount] sizing
                //  would silently discard type info for these new vregs).
                {
                    uint32_t opt_max = kGprCount;
                    for (const auto& ri : opt_instrs) {
                        if (ri.has_dst() && ri.dst_reg() > opt_max)
                            opt_max = ri.dst_reg();
                        if (ri.has_src1() && ri.src1_reg() > opt_max)
                            opt_max = ri.src1_reg();
                        if (ri.has_src2() && ri.src2_reg() > opt_max)
                            opt_max = ri.src2_reg();
                    }
                    opt_max = std::max(opt_max + 1, static_cast<uint32_t>(kGprCount));
                    if (opt_max > vreg_types_.size())
                        vreg_types_.resize(opt_max, kTypeVoid);
                }

                // Extend the stack frame if tree-created vregs exceed kFrameSize.
                // Without this, StoreGpr(vreg>=136) writes past the frame boundary,
                // overwriting the saved ABI ret_buf pointer at [SP+kFrameSize].
                {
                    uint32_t max_vreg = kGprCount - 1;
                    for (const auto& ri : opt_instrs) {
                        if (ri.has_dst() && ri.dst_reg() > max_vreg)
                            max_vreg = ri.dst_reg();
                        if (ri.has_src1() && ri.src1_reg() > max_vreg)
                            max_vreg = ri.src1_reg();
                        if (ri.has_src2() && ri.src2_reg() > max_vreg)
                            max_vreg = ri.src2_reg();
                    }
                    uint32_t gpr_bytes = static_cast<uint32_t>(max_vreg + 1) * 8;
                    if (gpr_bytes > kFrameSize)
                        frame_size_extra_ = gpr_bytes - kFrameSize;
                }

                // Re-count call slots: inlining may have removed call instructions.
                // Save prologue/runtime helper slot patches that were added before
                // the optimizer (e.g. EnterCooperativeMode call) — they'll be
                // re-appended after the clear.
                auto saved_prologue_patches = std::move(slot_patches_);
                slot_count_ = 0;
                for (const auto& instr : opt_instrs) {
                    auto opc = instr.op_code();
                    if (opc == IROpCode::Call || opc == IROpCode::CallBridge) {
                        slot_count_++;
                    }
                }
                slot_patches_.clear();
                slot_patches_.reserve(slot_count_ + saved_prologue_patches.size());
                // Re-add prologue/runtime helper patches (cooperative_fn, etc.)
                for (auto& sp : saved_prologue_patches) {
                    slot_patches_.push_back(std::move(sp));
                }
                slot_count_used_ = static_cast<uint32_t>(saved_prologue_patches.size());
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
    if (!is_tier0_ && config_.enable_liveness && n_instrs > 0) {
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
            if (inst.flags() & ::chaos::il2cpp::interpreter::kRegHasSrc3) {
                uint8_t src3 = inst.src3_reg();
                if (src3 < kGprCount)
                    use[i] |= (1ULL << src3);
            }
            // Calli: func_ptr vreg in imm.operand_index is an implicit source
            if (inst.op_code() == IROpCode::Calli && inst.imm.operand_index < kGprCount) {
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

                if (opc == IROpCode::Switch) {
                    uint32_t n_targets = inst.imm.i4;
                    if (n_targets > 0 && inst.imm.ptr) {
                        const uint32_t* targets = static_cast<const uint32_t*>(inst.imm.ptr);
                        uint32_t limit = n_targets < 256 ? n_targets : 256;
                        for (uint32_t ti = 0; ti < limit; ++ti) {
                            if (targets[ti] < n_instrs)
                                new_live_out |= live_in_[targets[ti]];
                        }
                    }
                } else if (opc == IROpCode::Br || opc == IROpCode::Leave) {
                    uint32_t target = inst.imm.branch_target;
                    if (target < n_instrs)
                        new_live_out |= live_in_[target];
                } else if (opc == IROpCode::BrTrue || opc == IROpCode::BrFalse || opc == IROpCode::Beq ||
                           opc == IROpCode::BneUn || opc == IROpCode::Blt || opc == IROpCode::Bgt ||
                           opc == IROpCode::Ble || opc == IROpCode::Bge || opc == IROpCode::BltUn ||
                           opc == IROpCode::BgtUn || opc == IROpCode::BleUn || opc == IROpCode::BgeUn) {
                    uint32_t target = inst.imm.branch_target;
                    if (target < n_instrs)
                        new_live_out |= live_in_[target];
                    if (static_cast<uint32_t>(i) + 1 < n_instrs)
                        new_live_out |= live_in_[i + 1];
                } else if (opc == IROpCode::Ret || opc == IROpCode::Throw || opc == IROpCode::Rethrow ||
                           opc == IROpCode::EndFinally || opc == IROpCode::EndFilter) {
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
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "Liveness computed for %u instructions, use_liveness=%d", n_instrs,
                                 (int)use_liveness_);
    }

    // Pre-allocate JitMethod early so instruction emission can embed
    // the address of its stale flag for HotUpdate inline PIC checking.
    auto* nm = static_cast<JitMethod*>(CHAOS_IL2CPP_MALLOC(sizeof(JitMethod)));
    if (nm == nullptr)
        return nullptr;
    std::memset(nm, 0, sizeof(*nm));
    ::new (nm) JitMethod();
    stale_flag_ptr_ = &nm->stale;

    // Emit instructions
    for (uint32_t i = 0; i < n_instrs; ++i) {
        instr_offsets_[i] = buf_.pos();
        current_instr_index_ = i;
        const auto& instr = opt_instrs[i];
        if (!removed_mask.empty() && removed_mask[i])
            continue;
        if (!EmitInstruction(instr)) {
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "Compile: unsupported opcode {} at pc={}, emitting deopt",
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
    if (CheckFailed())
        return nullptr;

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
#if !defined(__aarch64__)
        enc_.EmitSubRI(AT::kStackReg, 32); // shadow space for Win64 ABI
#endif
        uint32_t call_start = buf_.pos();
        enc_.EmitCallRipRel(0);
        slot_patches_.push_back(
            {call_start + kSlotPatchDispOff, UINT32_MAX, reinterpret_cast<void*>(config_.preemptive_fn)});
        slot_count_used_++;
        uint32_t call_pos = call_start;
#if !defined(__aarch64__)
        enc_.EmitAddRI(AT::kStackReg, 32);
#endif
        call_sites_.push_back({UINT32_MAX, call_pos});
        RecordGcPoint(call_pos);
    }

    // Restore callee-saved XMMs before deallocating frame
    for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
        int32_t off = static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + si * 16);
        enc_.EmitMovUPRM(callee_xmm_regs_[si], AT::kStackReg, off);
    }
#if defined(__aarch64__)
    enc_.EmitAddRI(AT::kStackReg, static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ +
                                                       xmm_save_size_ + localloc_extra_));
    for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
        EmitLdp64Post(buf_, callee_saved_regs_[slot - 1], 0, kARM64_SP,
                      16);                                 // LDP Xreg, X0, [SP], #16 (X0=scratch, never XZR)
    EmitLdp64Post(buf_, 1, 0, kARM64_SP, 16);              // LDP X1, X0, [SP], #16 (pop saved ret_buf)
    EmitLdp64Post(buf_, AT::kFrameReg, 30, kARM64_SP, 16); // LDP X29, X30, [SP], #16
    enc_.EmitRet();
#else
    enc_.EmitAddRI(AT::kStackReg, static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ +
                                                       xmm_save_size_ + localloc_extra_));
    for (uint32_t slot = num_cache_regs_; slot > 0; --slot)
        enc_.EmitPop(callee_saved_regs_[slot - 1]);
    enc_.EmitPop(kRDI);
    enc_.EmitPop(AT::kRetBuf);
    enc_.EmitPop(AT::kArgsBuf);
    enc_.EmitPop(AT::kFrameReg);
    enc_.EmitRet();
#endif

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
        uint32_t call_start = buf_.pos();
        enc_.EmitCallRipRel(0);
        slot_patches_.push_back(
            {call_start + kSlotPatchDispOff, UINT32_MAX, reinterpret_cast<void*>(ChaosJitRaiseException)});
        slot_count_used_++;
        uint32_t call_pos = call_start;
#if defined(__aarch64__)
        buf_.Emit32(0xD4200000); // BRK #0 (ARM64 breakpoint)
#else
        buf_.EmitByte(0xCC); // INT3 safety net
#endif
        int32_t disp = static_cast<int32_t>(cold_target - (cp.patch_offset + 4));
        buf_.Patch32(cp.patch_offset, static_cast<uint32_t>(disp));
    }

    if (CheckFailed())
        return nullptr;
    if (buf_.pos() == 0 || instr_offsets_.empty())
        return nullptr;

    // Resolve branches (including deopt jump patches)
    ResolveBranches();
    if (CheckFailed())
        return nullptr;

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
        uint32_t max_idx = n_instrs; // instr_offsets_ has n_instrs + 1 entries (sentinel at end)
        uint32_t emitted_count = 0;
        buf_.Emit32(count);
        for (const auto& clause : rm_.seh_clauses) {
            uint32_t try_start = static_cast<uint32_t>(clause.try_start_idx);
            uint32_t try_end = static_cast<uint32_t>(clause.try_end_idx);
            uint32_t handler_start = static_cast<uint32_t>(clause.handler_start_idx);
            const auto cflags_u32 = static_cast<uint32_t>(clause.flags);
            const bool is_filter = (cflags_u32 == static_cast<uint32_t>(interpreter::SEHFlags::Filter));
            // For filter clauses, the dispatched offset is the filter function;
            // the actual handler follows the filter sequentially.
            uint32_t dispatch_idx = handler_start;
            // Validate indices: skip malformed clauses (defensive, not a crash).
            if (try_start >= max_idx || try_end > max_idx || dispatch_idx >= max_idx || handler_start >= max_idx) {
                CHAOS_IL2CPP_LOG_WARN_M(
                    "codegen",
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
    uint32_t code_body_size = buf_.pos(); // Function body ends before metadata
    bool has_seh = !rm_.seh_clauses.empty();
#if defined(_WIN64)
    // Pad to 4-byte alignment: Win64 requires UNWIND_INFO to be DWORD-aligned.
    // The code body may end at any byte alignment; emit NOP padding to the
    // next 4-byte boundary so the UNWIND_INFO starts aligned.
    while (buf_.pos() % 4 != 0) {
        buf_.EmitByte(0x90); // NOP padding
    }
    if (!is_tier0_ && prologue_total_bytes_ > 0 && num_push_regs_ > 0) {
        unwind_data_offset =
            EmitUnwindInfo(buf_, prologue_total_bytes_, prologue_sub_rsp_size_, num_push_regs_, push_reg_nums_,
                           prologue_push_offsets_, prologue_sub_rsp_offset_, prologue_set_fpreg_offset_, has_seh);
        if (unwind_data_offset > 0) {
        }
    }
#endif

    // ── Emit .eh_frame DWARF CFI (Linux) ─────────────────────────────────
#if defined(__linux__)
    if (!is_tier0_ && prologue_total_bytes_ > 0 && num_push_regs_ > 0) {
        uint32_t cie_off = EmitDwarfCie(buf_);
        EmitDwarfFde(buf_, cie_off, code_body_size, num_push_regs_, push_reg_nums_);
        eh_frame_offset_ = cie_off;
        buf_.Emit32(0); // .eh_frame zero-length end marker
    }
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
#if defined(__aarch64__)
            EmitStp64Pre(buf_, AT::kFrameReg, 30, kARM64_SP, -16); // STP X29, X30, [SP, #-16]!
            enc_.EmitAddRI(AT::kFrameReg, AT::kStackReg, 0);
            for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
                EmitStp64Pre(buf_, callee_saved_regs_[slot], 0, kARM64_SP,
                             -16); // STP Xreg, X0, [SP, #-16]! (X0=scratch, never XZR)
#else
            enc_.EmitPush(AT::kFrameReg);
            enc_.EmitMovRR(AT::kFrameReg, AT::kStackReg); // frame pointer chain
            enc_.EmitPush(AT::kArgsBuf);
            enc_.EmitPush(AT::kRetBuf);
            enc_.EmitPush(kRDI); // x64 ABI: RDI is callee-saved; matches main prologue
            for (uint32_t slot = 0; slot < num_cache_regs_; ++slot)
                enc_.EmitPush(callee_saved_regs_[slot]);
#endif
            enc_.EmitSubRI(AT::kStackReg, static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ +
                                                               xmm_save_size_ + localloc_extra_));
            // Zero the localloc bump pointer for OSR entry
            if (localloc_extra_ > 0) {
                enc_.EmitXorRR(AT::kScratchA, AT::kScratchA);
                enc_.EmitMovMR(AT::kStackReg,
                               static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_),
                               AT::kScratchA);
            }
            // using rep movsq (RSI=source, RDI=dest, RCX=count)
#if defined(__aarch64__)
            // ARM64: copy RegisterFile to stack frame (96 quadwords)
            // kScratchB (X1/RCX) = RegisterFile* (source, preserved)
            // dest = SP + kGprFileOff
            for (uint32_t i = 0; i < kGprCount + kFprCount; ++i) {
                ::chaos::il2cpp::jit::EmitLdr64(buf_, AT::kScratchA, AT::kScratchB, static_cast<uint16_t>(i));
                ::chaos::il2cpp::jit::EmitStr64(buf_, AT::kScratchA, AT::kStackReg,
                                                static_cast<uint16_t>((kGprFileOff + i * 8) / 8));
            }
#else
            enc_.EmitLeaRM(kRDI, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
            enc_.EmitMovRR(AT::kRetBuf, AT::kScratchB);
            enc_.EmitMovRIImm32(AT::kScratchB, kGprCount + kFprCount); // 96
            buf_.EmitByte(0xF3);                                       // REP prefix
            buf_.EmitByte(0x48);                                       // REX.W
            buf_.EmitByte(0xA5);                                       // MOVSQ
#endif

            // Set up register convention: RBX=args_buf, RSI=ret_buf
            enc_.EmitLeaRM(AT::kArgsBuf, AT::kStackReg, static_cast<int32_t>(kGprFileOff));
            enc_.EmitMovRR(AT::kRetBuf, AT::kScratchC);

            // Initialize cached/colored registers from the stack frame
            if (config_.enable_register_caching) {
                for (uint32_t slot = 0; slot < num_cache_regs_; ++slot) {
                    uint8_t x64_reg = callee_saved_regs_[slot];
                    if (has_graph_coloring_) {
                        uint32_t vreg = phys_to_colored_vreg_[x64_reg];
                        if (vreg != 0xFF) {
                            enc_.EmitMovRM(x64_reg, AT::kArgsBuf, static_cast<int32_t>(vreg * 8));
                        }
                    } else {
                        uint32_t vreg = phys_to_cached_vreg_[x64_reg];
                        if (vreg != kNotCached) {
                            enc_.EmitMovRM(x64_reg, AT::kArgsBuf, static_cast<int32_t>(vreg * 8));
                        }
                    }
                }
                cached_dirty_mask_ = 0; // clean after initialization
            }

            // Zero caller-colored registers — matches regular prologue.
            // OSR entry only loads callee-saved regs from the stack (above);
            // caller-colored regs have no stack slot, so without this they
            // contain garbage from the caller, while RegisterExecute returns 0.
            if (caller_colored_mask_) {
#if defined(__aarch64__)
                for (uint32_t x64r = 5; x64r <= 17; ++x64r)
                    enc_.EmitXorRR(x64r, x64r);
#else
                for (uint32_t x64r = AT::kExtraScratch0; x64r <= AT::kExtraScratch2; ++x64r)
                    enc_.EmitXorRR(x64r, x64r);
#endif
            }

            // Initialize callee-saved XMM registers from RegisterFile FPR copy.
            // Must also save to the XMM save area so the epilogue (Ret/deopt_return)
            // can restore them correctly.
            for (uint32_t si = 0; si < num_fpr_callee_; ++si) {
                uint8_t xmm_reg = callee_xmm_regs_[si];
                uint32_t fi = callee_xmm_fi_[si];
                int32_t fpr_off = static_cast<int32_t>(kFprFileOff + fi * kFprSlotSize);
                enc_.EmitMovSDRM(xmm_reg, AT::kStackReg, fpr_off);
                // Duplicate to XMM save area so Ret/deopt_return epilogue can restore
                int32_t save_off = static_cast<int32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + si * 16);
                enc_.EmitMovUPSMR(AT::kStackReg, save_off, xmm_reg);
            }

            // ── GC mode switch: EnterCooperativeMode (OSR entry) ───────
            // OSR resumes mid-execution in a hot loop.  The thread must be
            // in cooperative mode before the first managed object access.
            if (config_.cooperative_fn != nullptr) {
#if !defined(__aarch64__)
                enc_.EmitSubRI(AT::kStackReg, 32);
#endif
                uint32_t call_start = buf_.pos();
                enc_.EmitCallRipRel(0);
                slot_patches_.push_back(
                    {call_start + kSlotPatchDispOff, UINT32_MAX, reinterpret_cast<void*>(config_.cooperative_fn)});
                slot_count_used_++;
                uint32_t call_pos = call_start;
#if !defined(__aarch64__)
                enc_.EmitAddRI(AT::kStackReg, 32);
#endif
                call_sites_.push_back({UINT32_MAX, call_pos});
                RecordGcPoint(call_pos);
            }

            // Resolve loop header target and jump there.
            // Since OSR always restarts at the backward branch target
            // (loop header), we call OsrResolveLoopHeader() which reads
            // t_deopt_state.osr_resume_pc and resolves it through the
            // persisted instr_offsets table to an absolute native address.
#if !defined(__aarch64__)
            enc_.EmitSubRI(AT::kStackReg, 32); // shadow space for Win64
#endif
            EmitRuntimeHelperCall(reinterpret_cast<void*>(::OsrResolveLoopHeader));
#if !defined(__aarch64__)
            enc_.EmitAddRI(AT::kStackReg, 32); // restore shadow space
#endif

            // RAX now holds the target native address (or nullptr if resolution failed).
            // Null-check RAX before jumping: if null, return to caller (fall back to
            // interpreter).  The caller initialized ret_buf[0]=0, so a bare ret here
            // signals "OSR failed — continue interpreting".
#if defined(__aarch64__)
            // ARM64: CBNZ XscratchA, #4 (skip RET if non-null)
            buf_.Emit32(0xB5000020u | AT::kScratchA);
            enc_.EmitRet();
#else
            EmitTestRR(buf_, AT::kScratchA, AT::kScratchA);
            buf_.EmitByte(0x75); // JNE rel8
            buf_.EmitByte(0x01); // skip 1 byte (the RET)
            buf_.EmitByte(0xC3); // RET
#endif
            // RAX non-null: jump to resolved loop header address.
            // Re-zero caller-colored regs clobbered by the call.
            if (caller_colored_mask_) {
#if defined(__aarch64__)
                for (uint32_t x64r = 5; x64r <= 17; ++x64r)
                    enc_.EmitXorRR(x64r, x64r);
#else
                for (uint32_t x64r = AT::kExtraScratch0; x64r <= AT::kExtraScratch2; ++x64r)
                    enc_.EmitXorRR(x64r, x64r);
#endif
            }
            enc_.EmitJmpReg(AT::kScratchA);
        }
    }

    if (CheckFailed())
        return nullptr;

    // ── Emit call-site slot table ─────────────────────────────────────────
    // Slot table embedded in the RX code buffer for call [rip+off] indirection.
    // Each entry is a void* pointer to the target function.  Patch all call
    // instruction RIP-relative displacements now that we know the slot table
    // position.  The table is in RX memory; ReverseSlotMap::UpdateAll uses
    // VirtualProtect to write to it during hotpatch.
    // Sync slot_count_ to actual total (managed + runtime helper slots).
    slot_count_ = static_cast<uint32_t>(slot_patches_.size());
    slot_count_used_ = slot_count_;

    uint32_t slot_table_offset = 0;
    if (slot_count_used_ > 0) {
        slot_table_offset = buf_.pos();
        for (uint32_t si = 0; si < slot_patches_.size(); ++si) {
            auto& sp = slot_patches_[si];
            buf_.Emit64(reinterpret_cast<uint64_t>(sp.target_fn));
#if defined(__aarch64__)
            // ARM64: LDR X17, #imm19 — encode the PC-relative offset.
            // LDR loads from PC + imm19*4, where PC = address of the LDR instruction.
            int64_t ldr_addr = static_cast<int64_t>(sp.patch_offset);
            int64_t slot_addr = static_cast<int64_t>(slot_table_offset + si * 8);
            int64_t disp_bytes = slot_addr - ldr_addr;
            int32_t imm19 = static_cast<int32_t>(disp_bytes / 4);
            uint32_t instr = buf_.Load32(sp.patch_offset);
            instr = (instr & 0xFF00001Fu) | ((static_cast<uint32_t>(imm19) & 0x7FFFFu) << 5);
            buf_.Patch32(sp.patch_offset, instr);
#else
            // Patch the RIP-relative displacement: slot_entry - (call_next_addr)
            uint32_t call_next = sp.patch_offset + 4; // FF 15 <disp32> = 6 bytes
            int32_t disp = static_cast<int32_t>((slot_table_offset + si * 8) - call_next);
            buf_.Patch32(sp.patch_offset, static_cast<uint32_t>(disp));
#endif
        }
    }

    // ── Emit JitDebugInfo footer (before Seal, while buffer is RW) ──────
    uint32_t debug_info_offset = 0;
    if (!is_tier0_) {
        debug_info_offset = buf_.pos();

        // 1. Emit instr_offsets array (uint32_t[])
        uint32_t offsets_off = debug_info_offset;
        for (auto off : instr_offsets_) {
            buf_.Emit32(off);
        }

        // 2. Emit method name placeholder: "T4_{module_id}_{token}"
        uint32_t name_off = buf_.pos();
        char name_buf[128];
        int name_len =
            std::snprintf(name_buf, sizeof(name_buf), "T4_%08X_%08X", config_.method_module_id, config_.method_token);
        buf_.EmitBytes(name_buf, static_cast<uint32_t>(name_len + 1));

        // 3. Emit JitDebugInfo header
        JitDebugInfo di;
        debug_info_offset = buf_.pos(); // Record offset of the struct itself
        di.magic = JitDebugInfo::kMagic;
        di.version = JitDebugInfo::kVersion;
        di.code_size = buf_.pos();
        di.instr_offset_count = static_cast<uint32_t>(instr_offsets_.size());
        di.instr_offsets_off = offsets_off;
        di.method_name_off = name_off;
        di.method_name_len = static_cast<uint32_t>(name_len);
        buf_.EmitBytes(&di, sizeof(di));
    }

    // Seal code buffer — returns nullptr on OOM or failure
    if (CheckFailed())
        return nullptr;
    uint32_t code_bytes = buf_.pos();
    void* code = buf_.Seal();
    if (code == nullptr)
        return nullptr;

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "Compile: {} instrs, {} bytes code, {} call sites", n_instrs, code_bytes,
                             call_sites_.size());

    // Build JitMethod (pre-allocated as nm_ before instruction emission)
    nm->code = code;
    nm->code_size = code_bytes;
    nm->instr_count = n_instrs;
    nm->seh_table_offset = seh_offset;
    nm->osr_entry_offset = osr_entry;
    nm->debug_info_offset = debug_info_offset;

    // Wire up call-site slot table (embedded in the RX code buffer).
    // call_site_slots points into the sealed buffer after buf_.Seal().
    // Slot entries are written with VirtualProtect during hotpatch.
    if (slot_count_used_ > 0) {
        nm->call_site_slots = reinterpret_cast<void**>(static_cast<uint8_t*>(code) + slot_table_offset);
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
                nm->inlined_callees[ri].callee_token = inline_results.callee_tokens[ri];
                nm->inlined_callees[ri].snapshot_version = inline_results.snapshot_versions[ri];
                g_inline_reverse_map.Add(inline_results.callee_tokens[ri], nm);
            }
            nm->inlined_callee_count = inline_results.count;
        }
    }

    // Persist instruction offset table for OSR loop header resolution.
    if (!instr_offsets_.empty()) {
        nm->instr_offset_count = static_cast<uint32_t>(instr_offsets_.size());
        nm->instr_offsets = static_cast<uint32_t*>(CHAOS_IL2CPP_MALLOC(nm->instr_offset_count * sizeof(uint32_t)));
        if (nm->instr_offsets) {
            std::memcpy(nm->instr_offsets, instr_offsets_.data(), nm->instr_offset_count * sizeof(uint32_t));
        }
    }

    if (!call_sites_.empty()) {
        nm->call_site_count = static_cast<uint32_t>(call_sites_.size());
        nm->call_sites = static_cast<CallSiteInfo*>(CHAOS_IL2CPP_MALLOC(nm->call_site_count * sizeof(CallSiteInfo)));
        if (nm->call_sites)
            std::memcpy(nm->call_sites, call_sites_.data(), nm->call_site_count * sizeof(CallSiteInfo));
    }
    if (!deopt_entries_.empty()) {
        nm->deopt_entry_count = static_cast<uint32_t>(deopt_entries_.size());
        nm->deopt_entries = static_cast<DeoptEntry*>(CHAOS_IL2CPP_MALLOC(nm->deopt_entry_count * sizeof(DeoptEntry)));
        if (nm->deopt_entries)
            std::memcpy(nm->deopt_entries, deopt_entries_.data(), nm->deopt_entry_count * sizeof(DeoptEntry));
    }
    if (!deopt_values_.empty()) {
        uint32_t n = static_cast<uint32_t>(deopt_values_.size());
        nm->deopt_values = static_cast<DeoptValue*>(CHAOS_IL2CPP_MALLOC(n * sizeof(DeoptValue)));
        if (nm->deopt_values)
            std::memcpy(nm->deopt_values, deopt_values_.data(), n * sizeof(DeoptValue));
    }
    if (!gc_points_.empty()) {
        nm->gc_point_count = static_cast<uint32_t>(gc_points_.size());
        nm->gc_points = static_cast<GcPoint*>(CHAOS_IL2CPP_MALLOC(nm->gc_point_count * sizeof(GcPoint)));
        if (nm->gc_points)
            std::memcpy(nm->gc_points, gc_points_.data(), nm->gc_point_count * sizeof(GcPoint));
    }

    // Serialize GcSlotMapV0
    if (!slot_map_entries_.empty()) {
        uint32_t num_slots = static_cast<uint32_t>(slot_map_entries_.size());
        uint32_t map_size = sizeof(GcSlotMapV0) + num_slots * sizeof(uint32_t);
        auto* map_data = static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(map_size));
        if (map_data) {
            auto* sm = reinterpret_cast<GcSlotMapV0*>(map_data);
            sm->frame_size = static_cast<uint32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_);
            sm->num_gc_slots = num_slots;
            std::memcpy(sm->slots, slot_map_entries_.data(), num_slots * sizeof(uint32_t));
            nm->slot_map_data = map_data;
            nm->slot_map_size = map_size;
            nm->gc_slot_map = reinterpret_cast<GcSlotMapV0*>(map_data);
        }
    }

    // T2.2-A: Serialize per-safepoint precise root map (GcPointMapV0).
    // For each GC point (already liveness-filtered ObjectRef vregs at that
    // point), emit a GcSafepointV0 with its exact live stack slots so the
    // scanner can report only the roots live at the return offset — replacing
    // the whole-method union above with per-safepoint precision.
    if (!gc_points_.empty() && nm->code_size > 0) {
        // Compute total size: header + per-safepoint (GcSafepointV0 fixed part
        // + num_gc_slots stack-slot encodings + num_live_regs register encodings).
        auto popcount = [](uint32_t m) noexcept {
            uint32_t n = 0;
            while (m) {
                n += (m & 1u);
                m >>= 1;
            }
            return n;
        };
        uint32_t total = sizeof(GcPointMapV0);
        for (const auto& gp : gc_points_) {
            uint32_t nregs = popcount(gp.live_reg_mask);
            total += sizeof(GcSafepointV0) + (gp.slot_count + nregs) * sizeof(uint32_t);
        }
        auto* pm = static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(total));
        if (pm) {
            auto* gm = reinterpret_cast<GcPointMapV0*>(pm);
            gm->code_size = nm->code_size;
            gm->num_gprs = static_cast<uint32_t>(kPhysRegCount);
            uint32_t count = static_cast<uint32_t>(gc_points_.size());
            gm->num_safepoints = count;
            uint8_t* cursor = pm + sizeof(GcPointMapV0);
            for (uint32_t i = 0; i < count; ++i) {
                const GcPoint& gp = gc_points_[i];
                auto* sp = reinterpret_cast<GcSafepointV0*>(cursor);
                sp->native_offset = gp.native_offset;
                // Precise stack slots for this safepoint (Task A), plus live
                // volatile-register roots (Task B) as register encodings.
                sp->num_gc_slots = gp.slot_count;
                sp->num_live_regs = popcount(gp.live_reg_mask);
                uint32_t* slit = sp->slots;
                for (uint32_t si = 0; si < gp.slot_count; ++si) {
                    // GcSlot may represent a stack spill slot (offset/8) or a
                    // register.  Reconstruct the GcSlotMap-style offset-only
                    // encoding for stack roots.
                    if (gp.slots[si].kind == GcSlotKind::Stack) {
                        uint32_t byte_off = static_cast<uint32_t>(gp.slots[si].index) * 8u;
                        *slit++ = CHAOS_GC_SLOT_ENCODE(byte_off, CHAOS_GC_SLOT_KIND_OBJECT);
                    }
                }
                // T2.2-B: serialize the live volatile-register root mask as
                // register encodings (physical reg index | object kind).
                for (uint32_t phys = 0; phys < 12; ++phys) {
                    if (gp.live_reg_mask & (1u << phys)) {
                        *slit++ = CHAOS_GC_REG_ENCODE(phys, CHAOS_GC_SLOT_KIND_OBJECT);
                    }
                }
                cursor += sizeof(GcSafepointV0) + (gp.slot_count + sp->num_live_regs) * sizeof(uint32_t);
            }
            (void)gm;
            nm->gc_point_map_data = pm;
            nm->gc_point_map_size = total;
        }
    }

    nm->rbp_to_rsp_offset =
        16 + num_cache_regs_ * 8 +
        static_cast<uint32_t>(kFrameSize + frame_size_extra_ + frame_align_adj_ + xmm_save_size_ + localloc_extra_);

    // ── Allocate Win64 RUNTIME_FUNCTION for .pdata registration ─────────
#if defined(_WIN64)
    if (unwind_data_offset > 0) {
        nm->runtime_function = AllocRuntimeFunction(unwind_data_offset, code_body_size);
    }
#elif defined(__linux__)
    // Store .eh_frame DWARF CFI offset for __register_frame in RegisterNativeCodeSection.
    nm->eh_frame_offset = eh_frame_offset_;
#endif

    CHAOS_IL2CPP_LOG_INFO_M("codegen", "Generate: method compiled, code_size={}, code={:p}, slots={}", nm->code_size,
                            static_cast<void*>(nm->code), slot_count_used_);
    return nm;
}

JitMethod* Compile(const ::chaos::il2cpp::interpreter::RegisterMethod& rm, const CompileConfig& config) noexcept {
    if (rm.instructions.empty())
        return nullptr;
    ISehHandler& seh = GetSehHandler();
    NativeCodeGenerator gen(rm, config, seh);
    return gen.Generate();
}

bool CanCompile(const ::chaos::il2cpp::interpreter::RegisterMethod& rm) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (rm.instructions.empty())
        return false;
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
    return true; // All opcodes accepted — unsupported ones deopt at runtime.
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
    CHAOS_IL2CPP_FREE(gc_point_map_data);
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
    if (eh_frame_registered && code != nullptr) {
        const void* eh_frame = static_cast<const uint8_t*>(code) + eh_frame_offset;
        __deregister_frame(eh_frame);
    }
#endif
    if (code != nullptr && !code_managed_externally) {
        chaos::il2cpp::pal::PalVirtualFree(code, code_size);
    }
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

// ── NativeCodeGenerator::EmitSimd — SIMD operation dispatch ───────────
//
// Dispatches on (simd_op, elem_type) to emit the correct SSE/SSE2/SSSE3/SSE4.1
// instruction sequence.  src1/src2 vregs are loaded from the stack frame into
// physical XMM registers, the operation is emitted, and the result is stored.
bool NativeCodeGenerator::EmitSimd(const interpreter::RegisterInstruction& instr, uint8_t simd_op, uint8_t elem_type,
                                   uint16_t simd_imm) noexcept {
    // Map virtual src/dst to physical XMM registers
    uint8_t xmm_dst = 0;  // XMM0 for SIMD operations
    uint8_t xmm_src1 = 1; // XMM1
    uint8_t xmm_src2 = 2; // XMM2 (for binary ops)

    // Load source operands from stack frame into XMM registers
    LoadFpr(xmm_src1, instr.src1_reg());
    if (instr.has_src2())
        LoadFpr(xmm_src2, instr.src2_reg());

    switch (simd_op) {
        // ── Arithmetic ──────────────────────────────────────────────────
        case 1: // kSimdAdd
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPaddbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPaddwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPadddRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPaddqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitVAddpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVAddpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPaddbRR(xmm_src1, xmm_src2);
                        break;
                    case 1:
                        enc_.EmitPaddwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPadddRR(xmm_src1, xmm_src2);
                        break;
                    case 3:
                        enc_.EmitPaddqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFadd4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFadd2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        case 2: // kSimdSub
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPsubbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPsubwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPsubdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPsubqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitVSubpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVSubpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPsubbRR(xmm_src1, xmm_src2);
                        break;
                    case 1:
                        enc_.EmitPsubwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPsubdRR(xmm_src1, xmm_src2);
                        break;
                    case 3:
                        enc_.EmitPsubqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFsub4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFsub2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        case 3: // kSimdMul
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPmullwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPmuludqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitVMulpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVMulpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 1:
                        enc_.EmitPmullwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPmuludqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFmul4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFmul2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        // ── Bitwise ────────────────────────────────────────────────────
        case 4: // kSimdAnd
            if (kUseVexEncoding)
                EmitVPandRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else
                enc_.EmitPandRR(xmm_src1, xmm_src2);
            break;
        case 5: // kSimdOr
            if (kUseVexEncoding)
                EmitVPorRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else
                enc_.EmitPorRR(xmm_src1, xmm_src2);
            break;
        case 6: // kSimdXor
            if (kUseVexEncoding)
                EmitVPxorRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else
                enc_.EmitPxorRR(xmm_src1, xmm_src2);
            break;
        case 7: // kSimdAndNot
            if (kUseVexEncoding)
                EmitVPandnRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else {
#if defined(__aarch64__)
                // BIC = rn & ~rm.  PANDN(src1, src2) = ~src1 & src2 = BIC(src2, src1).
                // So BIC(xmm_src1, xmm_src2, xmm_src1) → xmm_src2 & ~xmm_src1.
                EmitBic16B(buf_, xmm_src1, xmm_src2, xmm_src1);
#else
                enc_.EmitPandnRR(xmm_src1, xmm_src2);
#endif
            }
            break;

        // ── Compare (integer + float/double) ─────────────────────────────
        case 8: // kSimdEq
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPcmpeqbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPcmpeqwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPcmpeqdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPcmpeqqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitVCmppsRR(buf_, xmm_dst, xmm_src1, xmm_src2, 0);
                        break; // EQ_OQ
                    case 5:
                        EmitVCmppdRR(buf_, xmm_dst, xmm_src1, xmm_src2, 0);
                        break; // EQ_OQ
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPcmpeqbRR(xmm_src1, xmm_src2);
                        break;
                    case 1:
                        enc_.EmitPcmpeqwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPcmpeqdRR(xmm_src1, xmm_src2);
                        break;
                    case 3:
                        enc_.EmitPcmpeqqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFcmeq4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFcmeq2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        case 9: // kSimdGt
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPcmpgtbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPcmpgtwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPcmpgtdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPcmpgtqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    // SSE compare has no direct GT predicate; use LT_OS with swapped operands:
                    // vcmpps dest, src2, src1, 1  =>  src2 < src1  =>  src1 > src2
                    case 4:
                        EmitVCmppsRR(buf_, xmm_dst, xmm_src2, xmm_src1, 1);
                        break;
                    case 5:
                        EmitVCmppdRR(buf_, xmm_dst, xmm_src2, xmm_src1, 1);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPcmpgtbRR(xmm_src1, xmm_src2);
                        break;
                    case 1:
                        enc_.EmitPcmpgtwRR(xmm_src1, xmm_src2);
                        break;
                    case 2:
                        enc_.EmitPcmpgtdRR(xmm_src1, xmm_src2);
                        break;
                    case 3:
                        enc_.EmitPcmpgtqRR(xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFcmgt4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFcmgt2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            }
            break;

        // ── Shuffle ─────────────────────────────────────────────────────
        case 10: // kSimdShuffle (pshufd — dword shuffle with imm8)
            if (kUseVexEncoding)
                EmitVPshufdRR(buf_, xmm_dst, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
            else
                enc_.EmitPshufdRR(xmm_src1, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
            break;

        case 11: // kSimdShuffleB (pshufb — SSSE3 byte shuffle)
            if (elem_type != 0)
                return false;
            if (kUseVexEncoding)
                EmitVPshufbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
            else
                EmitPshufbRR(buf_, xmm_src1, xmm_src2);
            break;

        // ── Unpack / Interleave ─────────────────────────────────────────
        case 12: // kSimdUnpackLo
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPunpcklbwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPunpcklwdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPunpckldqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPunpcklqdqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 0:
                        EmitPunpcklbwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitPunpcklwdRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPunpckldqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitPunpcklqdqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        case 13: // kSimdUnpackHi
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPunpckhbwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitVPunpckhwdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPunpckhdqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPunpckhqdqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 0:
                        EmitPunpckhbwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitPunpckhwdRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPunpckhdqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitPunpckhqdqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        // ── Pack with signed saturation ─────────────────────────────────
        case 14: // kSimdPackS
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPacksswbRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPackssdwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 1:
                        EmitPacksswbRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPackssdwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        // ── Absolute value ──────────────────────────────────────────────
        case 15: // kSimdAbs
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 0:
                        EmitVPabsbRR(buf_, xmm_dst, xmm_src1);
                        break;
                    case 1:
                        EmitVPabswRR(buf_, xmm_dst, xmm_src1);
                        break;
                    case 2:
                        EmitVPabsdRR(buf_, xmm_dst, xmm_src1);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
                    case 0:
                        enc_.EmitPabsbRR(xmm_src1, xmm_src1);
                        break;
                    case 1:
                        enc_.EmitPabswRR(xmm_src1, xmm_src1);
                        break;
                    case 2:
                        enc_.EmitPabsdRR(xmm_src1, xmm_src1);
                        break;
#if defined(__aarch64__)
                    case 4:
                        EmitFabs4S(buf_, xmm_src1, xmm_src1);
                        break;
                    case 5:
                        EmitFabs2D(buf_, xmm_src1, xmm_src1);
                        break;
#endif
                    default:
                        return false;
                }
            }
            break;

        // ── Packed shift by XMM count ───────────────────────────────────
        case 16: // kSimdShl (shift left logical)
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPsllwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPslldRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPsllqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 1:
                        EmitPsllwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPslldRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitPsllqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        case 17: // kSimdShr (shift right logical)
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPsrlwRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPsrldRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitVPsrlqRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 1:
                        EmitPsrlwRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPsrldRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 3:
                        EmitPsrlqRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        case 18: // kSimdSar (shift right arithmetic)
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 1:
                        EmitVPsrawRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitVPsradRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if !defined(__aarch64__)
                switch (elem_type) {
                    case 1:
                        EmitPsrawRR(buf_, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitPsradRR(buf_, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
#else
                return false;
#endif
            }
            break;

        // ── Extract element to GPR ──────────────────────────────────────
        case 19: { // kSimdExtract
            // Load XMM source (already in xmm_src1 from the top of this function).
            // Extract the element at index simd_imm into RAX, then store to reg file.
            switch (elem_type) {
                case 0:
                    if (kUseVexEncoding)
                        EmitVPextrbRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitUmovB(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm));
#else
                        EmitPextrbRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
                case 1:
                    if (kUseVexEncoding)
                        EmitVPextrwRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitUmovH(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm));
#else
                        EmitPextrwRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
                case 2:
                    if (kUseVexEncoding)
                        EmitVPextrdRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitUmovS(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm));
#else
                        EmitPextrdRR(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
#if defined(__aarch64__)
                case 3:
                    EmitUmovD(buf_, AT::kScratchA, xmm_src1, static_cast<uint8_t>(simd_imm));
                    break;
#endif
                default:
                    return false;
            }
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        // ── Insert scalar from GPR ──────────────────────────────────────
        case 20: { // kSimdInsert
            // Load destination XMM, load source GPR, insert, store back.
            LoadFpr(xmm_src1, instr.dst_reg()); // re-load dest as source
            uint32_t gpr_vreg = instr.src2_reg();
            LoadGpr(AT::kScratchA, gpr_vreg);
            switch (elem_type) {
                case 0:
                    if (kUseVexEncoding)
                        EmitVPinsrbRR(buf_, xmm_src1, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitInsB(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm));
#else
                        EmitPinsrbRR(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
                case 1:
                    if (kUseVexEncoding)
                        EmitVPinsrwRR(buf_, xmm_src1, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitInsH(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm));
#else
                        EmitPinsrwRR(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
                case 2:
                    if (kUseVexEncoding)
                        EmitVPinsrdRR(buf_, xmm_src1, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
                    else
#if defined(__aarch64__)
                        EmitInsS(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm));
#else
                        EmitPinsrdRR(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm & 0xFF));
#endif
                    break;
#if defined(__aarch64__)
                case 3:
                    EmitInsD(buf_, xmm_src1, AT::kScratchA, static_cast<uint8_t>(simd_imm));
                    break;
#endif
                default:
                    return false;
            }
            StoreFpr(instr.dst_reg(), xmm_src1);
            return true;
        }

        // ── Move byte mask to GPR ───────────────────────────────────────
        case 21: { // kSimdMoveMask
            if (elem_type != 0)
                return false;
#if !defined(__aarch64__)
            if (kUseVexEncoding)
                EmitVPmovmskbRR(buf_, AT::kScratchA, xmm_src1);
            else
                EmitPmovmskbRR(buf_, AT::kScratchA, xmm_src1);
#else
            return false;
#endif
            StoreGpr(AT::kScratchA, instr.dst_reg());
            return true;
        }

        // ── Zero XMM ────────────────────────────────────────────────────
        case 24: // kSimdZero
            if (kUseVexEncoding)
                EmitVPxorRR(buf_, xmm_dst, xmm_dst, xmm_dst);
            else
                enc_.EmitPxorRR(xmm_dst, xmm_dst);
            StoreFpr(instr.dst_reg(), xmm_dst);
            return true;

        // ── Load/Store (movdqa) ─────────────────────────────────────────
        case 22: { // kSimdLoad — load from memory at address in src1
            uint32_t addr_vreg = instr.src1_reg();
            // Load address into RAX from the register file
            LoadGpr(AT::kScratchA, addr_vreg);
            if (kUseVexEncoding) {
                buf_.EmitVEX_66_0F(xmm_dst, 0, xmm_dst);
                buf_.EmitByte(0x6F); // vmovdqa
                buf_.EmitByte(ModRM(0, xmm_dst, AT::kScratchA));
            } else {
                enc_.EmitMovdqaRM(xmm_dst, AT::kScratchA, 0);
            }
            StoreFpr(instr.dst_reg(), xmm_dst);
            return true;
        }

        case 23: { // kSimdStore — store to memory at address in src1
            uint32_t addr_vreg = instr.src1_reg();
            LoadGpr(AT::kScratchA, addr_vreg);
            if (kUseVexEncoding) {
                buf_.EmitVEX_66_0F(xmm_src1, 0, xmm_src1);
                buf_.EmitByte(0x7F); // vmovdqa store
                buf_.EmitByte(ModRM(0, xmm_src1, AT::kScratchA));
            } else {
                enc_.EmitMovdqaMR(AT::kScratchA, 0, xmm_src1);
            }
            return true;
        }

        case 25: // kSimdMin
            if (kUseVexEncoding) {
                switch (elem_type) {
                    // VEX integer SIMD min/max encoder functions not yet available (x64 future work).
                    case 4:
                        EmitVMinpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVMinpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
#if defined(__aarch64__)
                    case 0:
                        EmitSmin16B(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitSmin8H(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitSmin4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFmin4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFmin2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
#endif
                    default:
                        return false;
                }
            }
            break;

        case 26: // kSimdMax
            if (kUseVexEncoding) {
                switch (elem_type) {
                    // VEX integer SIMD min/max encoder functions not yet available (x64 future work).
                    case 4:
                        EmitVMaxpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVMaxpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
                switch (elem_type) {
#if defined(__aarch64__)
                    case 0:
                        EmitSmax16B(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 1:
                        EmitSmax8H(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 2:
                        EmitSmax4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 4:
                        EmitFmax4S(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitFmax2D(buf_, xmm_src1, xmm_src1, xmm_src2);
                        break;
#endif
                    default:
                        return false;
                }
            }
            break;

        case 27: // kSimdDiv
            if (kUseVexEncoding) {
                switch (elem_type) {
                    case 4:
                        EmitVDivpsRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    case 5:
                        EmitVDivpdRR(buf_, xmm_dst, xmm_src1, xmm_src2);
                        break;
                    default:
                        return false;
                }
            } else {
#if defined(__aarch64__)
                // ARM64 NEON has no packed FDIV.  Use Newton-Raphson reciprocal:
                //   xmm_dst = FRECPE(denom)              → 1/denom (initial estimate)
                //   xmm_src2 = FRECPS(denom, xmm_dst)     → 2 - denom * xmm_dst (refinement step)
                //   xmm_dst = FMUL(xmm_dst, xmm_src2)     → refined reciprocal
                //   xmm_src1 = FMUL(numer, xmm_dst)       → numer/denom in xmm_src1 (for StoreFpr)
                switch (elem_type) {
                    case 4: // float32x4
                        EmitFrecpe4S(buf_, xmm_dst, xmm_src2);
                        EmitFrecps4S(buf_, xmm_src2, xmm_src2, xmm_dst);
                        EmitFmul4S(buf_, xmm_dst, xmm_dst, xmm_src2);
                        EmitFmul4S(buf_, xmm_src1, xmm_src1, xmm_dst);
                        break;
                    case 5: // float64x2
                        EmitFrecpe2D(buf_, xmm_dst, xmm_src2);
                        EmitFrecps2D(buf_, xmm_src2, xmm_src2, xmm_dst);
                        EmitFmul2D(buf_, xmm_dst, xmm_dst, xmm_src2);
                        EmitFmul2D(buf_, xmm_src1, xmm_src1, xmm_dst);
                        break;
                    default:
                        return false;
                }
#else
                // ARM64 NEON has no packed float division
                return false;
#endif
            }
            break;

        default:
            return false; // unsupported SIMD operation
    }

    // Store result: VEX→XMM0, legacy→XMM1
    StoreFpr(instr.dst_reg(), kUseVexEncoding ? xmm_dst : xmm_src1);
    return true;
}

} // namespace chaos::il2cpp::jit