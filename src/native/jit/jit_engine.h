#ifndef CHAOS_IL2CPP_JIT_ENGINE_H_
#define CHAOS_IL2CPP_JIT_ENGINE_H_

// ── x64 native code generator ──────────────────────────────────────────────
//
// Translates a RegisterMethod (register-allocated IR) into x64 machine code.
// The generated code is a self-contained native function that reads arguments
// from args_buf, executes the method body, and writes the return value to
// ret_buf.
//
// Generated function signature:
//   void native_entry(void* args_buf, void* ret_buf);
//
// Phase 3b implementation: stack-slot register file with scratch-register
// computation.  Each virtual register has a fixed stack slot.  Arithmetic
// operations load from stack, compute via scratch registers, store to stack.

#include "IEncoder.h"
#include "ISehHandler.h"
#include "jit_method.h"
#include "jit_helpers.h"
#include "jit_reg_alloc.h"               // GraphColoringResult
#include "code_buffer.h"                 // CodeBuffer
#include "codegen_bridge.h"              // CodegenCallVirtArgs
#include "ArchTraits.h"                  // ArchTraits, Arch
#include <chaos/profile.h>               // CHAOS_IL2CPP_PROFILE_SCOPE
#include "../interpreter/ir_reg_alloc.h" // RegisterMethod, RegisterInstruction
#if defined(__aarch64__)
#include "Arm64Encoder.h" // Arm64Encoder
#else
#include "X64Encoder.h" // X64Encoder
#endif

#include <cstdint>
#include <cstddef>
#include <vector>

namespace chaos::il2cpp::jit {

// Architecture register-file alias (AT), shared by the codegen TUs split out
// of jit_engine.cpp (T2.4): maps AT::kScratchA/B/C etc. to the concrete
// ArchTraits specialization for the target architecture.
#if defined(__aarch64__)
using AT = ArchTraits<Arch::kARM64>;
#else
using AT = ArchTraits<Arch::kX64>;
#endif

// ── Stack frame layout (shared by the codegen TUs).  Moved from
// jit_engine.cpp during the T2.4 monolith split; all jit_codegen_*.cpp files
// reference these offsets/frame-size constants.

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

/// Compilation tier for JIT code generation.
/// Tier 0 produces code quickly with no optimizations (stack-only register access,
/// no liveness, no deopt metadata, no SEH).  Tier 1 is the full pipeline with
/// graph coloring, optimizer, liveness analysis, deopt, SEH, and OSR.
enum class CompileTier : uint8_t {
    kQuick = 0, // Quick JIT (<50µs target): stack-only, no optimizer/liveness/deopt/SEH
    kFull = 1,  // Standard JIT: full pipeline with graph coloring + optimizations
};

/// Configuration for native code generation.
struct CompileConfig {
    // If true, insert GC safepoint polls at loop back edges.
    bool enable_safepoint_polls = true;

    // If true, enable basic register caching (map hot GPRs to x64 regs).
    bool enable_register_caching = true;

    // If true, run the IR optimizer (constant folding + DCE) before codegen.
    bool enable_optimizer = true;

    // If true, enable method inlining during tree IR optimization.
    // Only applies when enable_optimizer is true and the method has no SEH.
    // Requires the tree IR pipeline (non-SEH methods only).
    bool enable_inlining = true;

    // If true, record deoptimization metadata at call sites.
    bool enable_deopt = true;

    // If true, perform register liveness analysis to refine GC/deopt metadata.
    bool enable_liveness = true;

    // Max instructions allowed before we emit a safepoint poll
    // (as a fallback when no loop back edges are detected).
    uint32_t safepoint_interval = 64;

    // Function pointer to SafepointPoll (runtime_core::threading::SafepointPoll).
    // Set by the caller (entry_direct.cpp) before calling Compile.
    // When non-null, EmitSafepointPoll emits a call through this pointer.
    void* safepoint_fn = nullptr;

    // PIC dispatch data from PatchMethod (for CallVirt fast path in T4 code).
    // Set by entry_direct.cpp before Compile.  When non-null,
    // CallVirt instructions use PIC chain lookup for direct dispatch.
    const void* pic_dispatch_data = nullptr;

    // Dispatch context pointer for InterpreterDispatchRaw fallback from
    // CodegenCallVirt.  Points to InterpreterDispatchContext on the caller's
    // stack, valid for the lifetime of the current RegisterExecute session.
    void* dispatch_ctx = nullptr;

    // CachedCallInfo[] from PatchMethod (one per instruction).
    // Used by EmitCall to populate CallSiteInfo.method_token for T4 call-site
    // hotpatch tracking.  Indexed by current_instr_index_.
    // nullptr = no call cache available (T4 test paths without PatchMethod).
    const void* call_cache = nullptr;
    uint32_t call_cache_count = 0;

    // This method's own AOT metadata token.
    // Used by RegisterNativeCodeSection to associate the generated code with its token
    // so method_replacement::Register can find and demote matching T4 entries.
    uint32_t method_token = 0;
    uint32_t method_module_id = 0;

    // ── Argument type tags for LdArg precision ──────────────────────────
    // Cached argument type tags from PatchMethod (ValueTag values).
    // When non-null, LdArg type inference uses these instead of conservative
    // ObjectRef, enabling precise GC slot maps for method arguments.
    // Indexed by `RegisterInstruction::imm::operand_index`.
    const uint8_t* arg_type_tags = nullptr;
    uint32_t arg_type_count = 0;

    // ── Precise GC: field type tags by field token (indexed by token value) ─
    // When non-null, LdFld/LdSFld type inference uses the per-field tag
    // instead of conservative ObjectRef, enabling precise GC slot maps.
    const uint8_t* field_type_tags = nullptr;
    uint32_t field_type_count = 0;

    // ── Precise GC: method return type tags by instruction index ──────────
    // When non-null, Call/CallVirt/CallBridge/Calli type inference uses the
    // per-instruction return tag as a fallback when call_cache ret_tag is
    // unavailable.  Indexed by current_instr_index_.
    const uint8_t* method_ret_tags = nullptr;
    uint32_t method_ret_tag_count = 0;

    // ── Per-instruction PIC data for inline monomorphic cache ──────────
    // Populated by entry_direct.cpp from the first PIC chain slot for each
    // CallVirt instruction.  When non-null and the entry for the current
    // instruction has a valid direct_fn, the code generator emits an inline
    // monomorphic check + direct call instead of going through CodegenCallVirt.
    // Indexed by current_instr_index_.
    const PerInstrPicData* per_instr_pic = nullptr;
    uint32_t per_instr_pic_count = 0;

    /// Compilation tier.  kFull (full pipeline) by default.
    CompileTier compile_tier = CompileTier::kFull;

    /// If true, enable PGO (Profile-Guided Optimization).  When enabled, Tier 0
    /// compilation does NOT patch direct_ptr — calls continue through the dispatch
    /// function which counts calls and triggers background Tier 1 recompilation
    /// when the call count exceeds kPgoTier1Threshold.
    bool enable_pgo = false;

    // ── GC mode switching ────────────────────────────────────────────────
    // Function pointer to EnterCooperativeMode (thread_state.h).
    // Set by the caller (entry_direct.cpp) before calling Compile.
    // When non-null, the prologue emits a call to switch the thread to
    // cooperative GC mode before accessing managed objects.
    // Required for GC safety: GcAllocate, GcAllocateAtomic, and all managed
    // heap access must happen in cooperative mode.
    void* cooperative_fn = nullptr;

    // Function pointer to EnterPreemptiveMode (thread_state.h).
    // Set by the caller (entry_direct.cpp) before calling Compile.
    // When non-null, the epilogue emits a call to switch the thread to
    // preemptive GC mode before returning to native code.
    // In preemptive mode, a safepoint request does NOT spin — the thread
    // acknowledges immediately and the GC does not wait for it.
    void* preemptive_fn = nullptr;
};

/// Generate native x64 code from a RegisterMethod.
///
/// @param rm       The register-allocated method to compile.
/// @param config   Code generation configuration.
///
/// @return JitMethod containing the generated code + metadata,
///         or nullptr on failure (unsupported opcodes, allocation error).
JitMethod* Compile(const interpreter::RegisterMethod& rm, const CompileConfig& config = CompileConfig()) noexcept;

/// Check if Compile can handle this RegisterMethod.
/// Returns false if the method contains unsupported opcodes.
bool CanCompile(const interpreter::RegisterMethod& rm) noexcept;

// ── NativeCodeGenerator ─────────────────────────────────────────────
// T4 machine-code generator.  Moved from jit_engine.cpp to jit_engine.h
// as the T2.4 monolith-split precursor: the class declaration (with all
// member state) lives here so the generated-code TUs (jit_codegen_*.cpp)
// can share it; method definitions remain in jit_engine.cpp (and split
// into per-module files during T2.4).

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
    void EmitIntegerArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;
    void EmitFloatingArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;
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

} // namespace chaos::il2cpp::jit

#endif // CHAOS_IL2CPP_JIT_ENGINE_H_
