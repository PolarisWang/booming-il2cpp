#include "code_generator.h"
#include "x64_encoder.h"
#include "code_buffer.h"
#include "codegen_helpers.h"

#include "../interpreter/ir_reg_alloc.h"
#include "../interpreter/interpreter_vm.h"

#include <codegen_bridge.h>
#include <chaos/log.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace chaos::il2cpp::codegen {

// ── Frame layout constants ─────────────────────────────────────────────────
// Stack frame (relative to RSP):
//   [rsp + 0 .. 32)       = Win64 shadow space (for callee calls)
//   [rsp + 32 .. 32 + gpr_count*8) = GPR file (virtual register 0..63)
//   [rsp + gpr_base .. gpr_base + fpr_count*8) = FPR file (virtual register 64..95)

static constexpr uint32_t kShadowSize = 32;
static constexpr uint32_t kGprCount   = interpreter::kGPRegisters;  // 64
static constexpr uint32_t kFprCount   = interpreter::kFPRegisters;  // 32
static constexpr uint32_t kGprFileSize  = kGprCount * 8;  // 512 bytes
static constexpr uint32_t kFprFileSize  = kFprCount * 8;  // 256 bytes
static constexpr uint32_t kGprFileOff   = kShadowSize;     // 32
static constexpr uint32_t kFprFileOff   = kGprFileOff + kGprFileSize;  // 544
static constexpr uint32_t kFrameSize    = kFprFileOff + kFprFileSize;   // 800 bytes

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
    // instr_offsets_[i] = buf_.pos() before emitting instruction i.
    std::vector<uint32_t> instr_offsets_;

    // Current instruction index in the Generate() loop.
    // Set before each EmitInstruction call for use in call site / deopt recording.
    uint32_t current_instr_index_ = 0;

    // Branch patch records: for forward branches where we don't yet know
    // the target offset.
    struct BranchPatch {
        uint32_t patch_offset;     // buf_ position of the 4-byte displacement
        uint32_t target_instr;     // target instruction index
    };
    std::vector<BranchPatch> branch_patches_;

    // Call site metadata
    std::vector<CallSiteInfo> call_sites_;

    // Deoptimization metadata
    std::vector<DeoptEntry> deopt_entries_;
    std::vector<DeoptValue> deopt_values_;

    // GC points (conservative scanning)
    std::vector<GcPoint> gc_points_;

    // ── V2: CallVirt + overflow + switch support ─────────────────────────

    // CallVirt deopt jump patches: after a CodegenCallVirt that signals deopt,
    // jump to the deopt trampoline (which just returns to entry_direct).
    std::vector<uint32_t> deopt_jumps_;  // patch offsets for je rel32

    // Overflow detection: after add/sub/mul with Ovf suffix, jo to trampoline.
    std::vector<uint32_t> overflow_jumps_;  // patch offsets for jo rel32

    // Switch jmp table records: table entries patched after ResolveBranches.
    struct SwitchRecord {
        uint32_t table_offset;      // buf_ position of the displacement table
        uint32_t target_count;      // number of case targets
        const uint32_t* targets;    // from instr.imm.ptr (branch_target indices)
    };
    std::vector<SwitchRecord> switch_records_;

    // ── V3: Safepoint interval counter ───────────────────────────────────
    uint32_t instr_since_safepoint_ = 0;

    // Epilogue position: for deopt/overflow trampoline jmp targets.
    uint32_t method_exit_label_ = 0;

    // ── V4: Register caching ────────────────────────────────────────────────
    // Maps vreg → x64 cached register (0xFF = not cached).
    // Only vregs < kGprCount can be cached (GPR virtual registers).
    uint8_t cached_x64_for_vreg_[64];   // vreg → x64 reg, 0xFF = none
    uint8_t x64_to_cached_vreg_[16];    // x64 reg → vreg, 0xFF = none
    uint32_t cached_vreg_count_ = 0;

    // ── GC ref vreg inference (T4 precise GC) ──────────────────────────────
    uint64_t gc_ref_vregs_ = 0;  // bitmask: vregs that ever hold a GC ref
    bool has_gc_ops_ = false;     // true if any instruction produces a GC ref
    void InferGcRefVregs() noexcept;

    // Select hot vregs for register caching (run once before code emission).
    void SelectCacheableRegs() noexcept;

    // Spill all cached register values back to their stack slots.
    // Must be called before any call, deopt point, or safepoint.
    void SpillCachedRegs() noexcept;

    // ── Emit helpers ─────────────────────────────────────────────────────

    // Load a GPR virtual register value into a scratch x64 register.
    // vreg must be < kGprCount.
    void LoadGpr(uint8_t x64_reg, uint32_t vreg) noexcept;

    // Store a scratch x64 register value into a GPR virtual register slot.
    void StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept;

    // Load an FPR virtual register value into an XMM scratch register.
    void LoadFpr(uint8_t xmm_reg, uint32_t vreg) noexcept;

    // Store an XMM scratch register value into an FPR virtual register slot.
    void StoreFpr(uint8_t xmm_reg, uint32_t vreg) noexcept;

    // Instruction emission. Returns false if opcode is unsupported.
    void EmitSafepointPoll() noexcept;

    // Emit GPR arithmetic (Add/Sub/Mul/Div/Rem/Neg/DivUn/RemUn).
    // Extracted to avoid MSVC C2597 nested-switch bug on `this->buf_`.
    void EmitGprArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;

    // Emit FPR arithmetic (Add/Sub/Mul/Div).
    // Extracted to avoid MSVC C2597 nested-switch bug on `this->buf_`.
    void EmitFprArithmetic(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;

    // Emit bitwise (And/Or/Xor/Not).
    // Extracted to avoid MSVC C2597 nested-switch bug on `this->buf_`.
    void EmitBitwise(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept;

    // Emit shifts (Shl/Shr/ShrUn).
    // Extracted to avoid MSVC C2597 nested-switch bug on `this->buf_`.
    void EmitShift(IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2, int32_t imm) noexcept;

    // Finalize branch fixups.
    void ResolveBranches() noexcept;

    // Instruction emission. Returns false if opcode is unsupported.
    bool EmitInstruction(const interpreter::RegisterInstruction& instr) noexcept;

    // Debug: dump generated code to log.
    void DumpCode() noexcept;

    // Record a GC point at the current buffer position.
    void RecordGcPoint(uint32_t native_offset) noexcept;
};

// ── Implementation ─────────────────────────────────────────────────────────

void NativeCodeGenerator::LoadGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    if (config_.enable_register_caching) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != 0xFF) {
            // Cache hit — copy from cached register to target register.
            EmitMovRR(buf_, x64_reg, cached);
            return;
        }
    }
    // Cache miss or caching disabled: load from stack.
    EmitMovRM(buf_, x64_reg, kRSP, static_cast<int32_t>(GprOff(vreg)));
}

void NativeCodeGenerator::StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    // Always write through to the stack frame (keeps stack consistent
    // for GC scanning, deopt, and spilling).
    EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vreg)), x64_reg);

    if (config_.enable_register_caching) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != 0xFF) {
            // Also update the cached register if different from x64_reg.
            if (cached != x64_reg) {
                EmitMovRR(buf_, cached, x64_reg);
            }
        }
    }
}

void NativeCodeGenerator::LoadFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    EmitMovSDRM(buf_, xmm_reg, kRSP, static_cast<int32_t>(FprOff(vreg)));
}

void NativeCodeGenerator::StoreFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    EmitMovSDMR(buf_, kRSP, static_cast<int32_t>(FprOff(vreg)), xmm_reg);
}

void NativeCodeGenerator::EmitSafepointPoll() noexcept {
    if (!config_.enable_safepoint_polls || config_.safepoint_fn == nullptr) return;

    // Spill cached registers before safepoint (GC conservative scan sees only
    // stack frame slots; cached R12-R15 must be written back first).
    SpillCachedRegs();

    // Inline fast check: load thread->suspend_seq and skip poll if 0.
    //   mov rax, imm64(thread_suspend_seq_ptr)  ; absolute address of suspend_seq
    //   cmp dword [rax], 0                       ; suspend_seq == 0?
    //   je .skip                                  ; fast path: no GC pending
    //   sub rsp, 32                               ; slow path: full call
    //   mov rax, imm64(safepoint_fn)
    //   call rax
    //   add rsp, 32
    // .skip:
    //
    // When thread_suspend_seq_ptr is null, fall back to unconditional call.
    if (config_.thread_suspend_seq_ptr != nullptr) {
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.thread_suspend_seq_ptr));
        EmitCmpMI(buf_, kRAX, 0, 0);  // cmp dword [rax], 0

        uint32_t je_patch = buf_.pos() + 2;  // offset of id32 in je rel32
        EmitJccRel32(buf_, kCC_E, 0);        // je .skip (placeholder)

        // Slow path (only reached when GC safepoint is active).
        EmitSubRI(buf_, kRSP, 32);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.safepoint_fn));
        uint32_t call_pos = buf_.pos();
        EmitCallReg(buf_, kRAX);
        call_sites_.push_back({UINT32_MAX, call_pos});
        RecordGcPoint(call_pos);
        EmitAddRI(buf_, kRSP, 32);

        // Patch the je to skip over the slow path.
        int32_t disp = static_cast<int32_t>(buf_.pos()) -
                       static_cast<int32_t>(je_patch + 4);
        buf_.Patch32(je_patch, static_cast<uint32_t>(disp));
    } else {
        // Fallback: unconditional call (legacy, when suspend_seq ptr not available).
        EmitSubRI(buf_, kRSP, 32);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.safepoint_fn));
        uint32_t call_pos = buf_.pos();
        EmitCallReg(buf_, kRAX);
        call_sites_.push_back({UINT32_MAX, call_pos});
        RecordGcPoint(call_pos);
        EmitAddRI(buf_, kRSP, 32);
    }
}

void NativeCodeGenerator::RecordGcPoint(uint32_t native_offset) noexcept {
    // Phase 3d: conservative GC point.
    // No precise slot information — the GC will conservatively scan the
    // entire native stack frame at this point.  Precise slot maps can be
    // added later by tracking live managed references across instructions.
    GcPoint gp;
    gp.native_offset = native_offset;
    gp.slot_count    = 0;
    gp.slots         = nullptr;
    gc_points_.push_back(gp);
}

// ── Register caching (V4) ───────────────────────────────────────────────

static constexpr uint8_t kCacheableX64Regs[] = {12, 13, 14, 15};  // R12-R15
static constexpr uint32_t kNumCacheableRegs  = 4;

void NativeCodeGenerator::SelectCacheableRegs() noexcept {
    if (!config_.enable_register_caching) return;
    if (rm_.instructions.empty()) return;

    std::memset(cached_x64_for_vreg_, 0xFF, sizeof(cached_x64_for_vreg_));
    std::memset(x64_to_cached_vreg_,  0xFF, sizeof(x64_to_cached_vreg_));

    // Count access frequency per vreg.
    uint32_t freq[64] = {};
    for (const auto& instr : rm_.instructions) {
        if (instr.has_dst()  && instr.dst_reg()  < 64) freq[instr.dst_reg()]++;
        if (instr.has_src1() && instr.src1_reg()  < 64) freq[instr.src1_reg()]++;
        if (instr.has_src2() && instr.src2_reg()  < 64) freq[instr.src2_reg()]++;
    }

    // Sort vregs by frequency descending (insertion sort on 64 elements).
    uint32_t sorted[64];
    for (uint32_t i = 0; i < 64; i++) sorted[i] = i;
    for (uint32_t i = 1; i < 64; i++) {
        uint32_t key = sorted[i];
        int32_t j = static_cast<int32_t>(i) - 1;
        while (j >= 0 && freq[sorted[j]] < freq[key]) {
            sorted[j + 1] = sorted[j];
            j--;
        }
        sorted[j + 1] = key;
    }

    // Assign top hot vregs to cacheable x64 registers.
    cached_vreg_count_ = 0;
    for (uint32_t i = 0; i < 64 && cached_vreg_count_ < kNumCacheableRegs; i++) {
        uint32_t vreg = sorted[i];
        if (freq[vreg] == 0) break;
        cached_x64_for_vreg_[vreg] = kCacheableX64Regs[cached_vreg_count_];
        x64_to_cached_vreg_[kCacheableX64Regs[cached_vreg_count_]] = static_cast<uint8_t>(vreg);
        cached_vreg_count_++;
    }
}

void NativeCodeGenerator::InferGcRefVregs() noexcept {
    uint64_t ref_mask = 0;
    has_gc_ops_ = false;
    for (const auto& instr : rm_.instructions) {
        auto opc = instr.op_code();
        bool produces_gc_ref = false;
        switch (opc) {
        // ALWAYS GC ref
        case IROpCode::NewObj:
        case IROpCode::Box:
        case IROpCode::LdStr:
        case IROpCode::LdNull:
            produces_gc_ref = true;
            break;
        // POSSIBLY GC ref — conservatively mark dst
        case IROpCode::LdFld:
        case IROpCode::LdArg:
        case IROpCode::LdLoc:
        case IROpCode::Call:
        case IROpCode::CallVirt:
        case IROpCode::CallVirtConstrained:
        case IROpCode::Calli:
        case IROpCode::CallBridge:
            produces_gc_ref = true;
            break;
        default:
            break;
        }
        if (produces_gc_ref && instr.has_dst() && instr.dst_reg() < kGprCount) {
            ref_mask |= (1ULL << instr.dst_reg());
            has_gc_ops_ = true;
        }
    }
    // Only mark vregs 0-15 as conservative GC refs when the method has at
    // least one GC-producing instruction.  This ensures purely arithmetic
    // methods produce no slot map (falling back to conservative scanning).
    if (has_gc_ops_) {
        uint32_t conservative_end = rm_.max_regs < 16 ? rm_.max_regs : 16;
        for (uint32_t vr = 0; vr < conservative_end; ++vr) {
            ref_mask |= (1ULL << vr);
        }
    }
    gc_ref_vregs_ = ref_mask;
}

void NativeCodeGenerator::SpillCachedRegs() noexcept {
    if (!config_.enable_register_caching || cached_vreg_count_ == 0) return;
    for (uint32_t i = 0; i < cached_vreg_count_; i++) {
        uint8_t x64_reg = kCacheableX64Regs[i];
        uint8_t vreg = x64_to_cached_vreg_[x64_reg];
        if (vreg == 0xFF) continue;
        EmitMovMR(buf_, kRSP, static_cast<int32_t>(GprOff(vreg)), x64_reg);
    }
}

void NativeCodeGenerator::EmitGprArithmetic(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    LoadGpr(kRAX, src1);
    if (src2 != UINT32_MAX) {
        LoadGpr(kRCX, src2);
    }
    if (opc == IROpCode::Add) {
        EmitAddRR(buf_, kRAX, kRCX);
    } else if (opc == IROpCode::Sub) {
        EmitSubRR(buf_, kRAX, kRCX);
    } else if (opc == IROpCode::Mul) {
        EmitImulRR(buf_, kRAX, kRCX);
    } else if (opc == IROpCode::Neg) {
        EmitNeg(buf_, kRAX);
    } else if (opc == IROpCode::Div || opc == IROpCode::Rem) {
        EmitREXB(buf_, true, 0);
        buf_.EmitByte(0x99);  // CQO
        EmitREX(buf_, true, 7, kRCX);
        buf_.EmitByte(0xF7);
        buf_.EmitByte(ModRM(3, 7, kRCX));
        if (opc == IROpCode::Rem) {
            EmitMovRR(buf_, kRAX, kRDX);
        }
    } else if (opc == IROpCode::DivUn || opc == IROpCode::RemUn) {
        EmitXorZR(buf_, kRDX);
        EmitREX(buf_, true, 6, kRCX);
        buf_.EmitByte(0xF7);
        buf_.EmitByte(ModRM(3, 6, kRCX));
        if (opc == IROpCode::RemUn) {
            EmitMovRR(buf_, kRAX, kRDX);
        }
    }
    StoreGpr(kRAX, dst);
}

void NativeCodeGenerator::EmitFprArithmetic(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    LoadFpr(0, src1);
    if (src2 != UINT32_MAX && src2 >= kGprCount) {
        LoadFpr(1, src2);
    }
    if (opc == IROpCode::Add) {
        EmitAddSDRR(buf_, 0, 1);
    } else if (opc == IROpCode::Sub) {
        EmitSubSDRR(buf_, 0, 1);
    } else if (opc == IROpCode::Mul) {
        EmitMulSDRR(buf_, 0, 1);
    } else if (opc == IROpCode::Div) {
        EmitDivSDRR(buf_, 0, 1);
    }
    StoreFpr(0, dst);
}

void NativeCodeGenerator::EmitBitwise(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2) noexcept {
    LoadGpr(kRAX, src1);
    if (opc == IROpCode::Not) {
        EmitNot(buf_, kRAX);
    } else if (opc == IROpCode::And) {
        if (src2 != UINT32_MAX) { LoadGpr(kRCX, src2); EmitAndRR(buf_, kRAX, kRCX); }
    } else if (opc == IROpCode::Or) {
        if (src2 != UINT32_MAX) { LoadGpr(kRCX, src2); EmitOrRR(buf_, kRAX, kRCX); }
    } else if (opc == IROpCode::Xor) {
        if (src2 != UINT32_MAX) { LoadGpr(kRCX, src2); EmitXorRR(buf_, kRAX, kRCX); }
    }
    StoreGpr(kRAX, dst);
}

void NativeCodeGenerator::EmitShift(
    IROpCode opc, uint32_t dst, uint32_t src1, uint32_t src2, int32_t imm) noexcept {
    LoadGpr(kRAX, src1);
    if (src2 != UINT32_MAX) {
        // Shift amount must be in CL (low byte of RCX)
        LoadGpr(kRCX, src2);
        if (opc == IROpCode::Shl) {
            EmitShlRCL(buf_, kRAX);
        } else if (opc == IROpCode::Shr) {
            EmitShrRCL(buf_, kRAX);
        } else if (opc == IROpCode::ShrUn) {
            EmitSarRCL(buf_, kRAX);
        }
    } else {
        uint8_t shift = static_cast<uint8_t>(imm & 0x3F);
        if (opc == IROpCode::Shl) {
            EmitShlRI(buf_, kRAX, shift);
        } else if (opc == IROpCode::Shr) {
            EmitShrRI(buf_, kRAX, shift);
        } else if (opc == IROpCode::ShrUn) {
            EmitSarRI(buf_, kRAX, shift);
        }
    }
    StoreGpr(kRAX, dst);
}

void NativeCodeGenerator::ResolveBranches() noexcept {
    for (auto& bp : branch_patches_) {
        if (bp.target_instr >= instr_offsets_.size()) {
            // Target out of range — patch with 0 (safety: jmp to next instr).
            buf_.Patch32(bp.patch_offset, 0);
            continue;
        }
        uint32_t target_off = instr_offsets_[bp.target_instr];
        uint32_t current_off = bp.patch_offset + 4;  // after the displacement
        int32_t disp = static_cast<int32_t>(target_off - current_off);
        buf_.Patch32(bp.patch_offset, static_cast<uint32_t>(disp));
    }
}

bool NativeCodeGenerator::EmitInstruction(const interpreter::RegisterInstruction& instr) noexcept {
    using IROpCode = interpreter::IROpCode;
    auto opc = instr.op_code();
    // Local refs to work around MSVC C2597 in deep control flow.
    auto& _buf = this->buf_;
    auto& _instr = instr;
    auto& _config = this->config_;
    auto& _rm = this->rm_;
    auto& _call_sites = this->call_sites_;
    auto& _branch_patches = this->branch_patches_;
    auto& _deopt_entries = this->deopt_entries_;
    auto& _deopt_values = this->deopt_values_;
    switch (opc) {

    // ── Load constant ───────────────────────────────────────────────────
    case IROpCode::LdcI4: {
        if (!instr.has_dst()) return false;
        EmitMovRIImm32(buf_, kRAX, static_cast<uint32_t>(instr.imm.i4));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdcI8: {
        if (!instr.has_dst()) return false;
        int64_t val = instr.imm.i8;
        if (val >= INT32_MIN && val <= INT32_MAX) {
            EmitMovRI32(buf_, kRAX, static_cast<int32_t>(val));
        } else {
            EmitMovImm64(buf_, kRAX, static_cast<uint64_t>(val));
        }
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::LdcR4: {
        if (!instr.has_dst()) return false;
        float v;
        std::memcpy(&v, &instr.imm.i4, sizeof(v));
        uint32_t bits;
        std::memcpy(&bits, &v, sizeof(bits));
        // Load float bits into GPR via RAX, then movd to XMM0, store
        EmitMovRIImm32(buf_, kRAX, bits);
        EmitMovdXrm(buf_, 0, kRAX);
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::LdcR8: {
        if (!instr.has_dst()) return false;
        uint64_t bits;
        std::memcpy(&bits, &instr.imm.r8, sizeof(bits));
        EmitMovImm64(buf_, kRAX, bits);
        EmitMovqXrm(buf_, 0, kRAX);
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    case IROpCode::LdNull: {
        if (!instr.has_dst()) return false;
        EmitXorZR(buf_, kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    // ── Load argument ───────────────────────────────────────────────────
    case IROpCode::LdArg: {
        if (!instr.has_dst()) return false;
        uint32_t arg_idx = instr.imm.operand_index;
        // Load argument from args_buf at offset arg_idx * 8.
        // args_buf is the first parameter (RCX on entry).
        // Need to read from the arg buffer — but we don't have args_buf in a register.
        //
        // At function entry, args_buf is in RCX.  We save it to RBX
        // in the prologue.  Access: mov rax, [rbx + arg_idx * 8]
        EmitMovRM(buf_, kRAX, kRBX, static_cast<int32_t>(arg_idx * 8));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    // ── Load/Store local ────────────────────────────────────────────────
    case IROpCode::LdLoc: {
        if (!instr.has_dst()) return false;
        uint32_t local_idx = instr.imm.operand_index;
        // Locals are stored at vreg = 8 + local_idx (convention: r8-r15 = locals)
        uint32_t vreg = kLocalRegBase + local_idx;
        if (vreg < kGprCount) {
            LoadGpr(kRAX, vreg);
            StoreGpr(kRAX, instr.dst_reg());
        }
        return true;
    }

    case IROpCode::StLoc: {
        if (!instr.has_src1()) return false;
        uint32_t local_idx = instr.imm.operand_index;
        uint32_t vreg = kLocalRegBase + local_idx;
        if (vreg < kGprCount) {
            LoadGpr(kRAX, instr.src1_reg());
            StoreGpr(kRAX, vreg);
            if (instr.has_dst()) {
                StoreGpr(kRAX, instr.dst_reg());
            }
        }
        return true;
    }

    // ── Return ──────────────────────────────────────────────────────────
    case IROpCode::Ret: {
        if (instr.has_src1()) {
            // Load return value from src1 and store to [ret_buf].
            // ret_buf is the second parameter (RDX on entry). Saved to RSI in prologue.
            LoadGpr(kRAX, instr.src1_reg());
            EmitMovMR(buf_, kRSI, 0, kRAX);  // ret_buf[0] = rax
        }
        // Epilogue: deallocate frame, restore callee-saved regs, ret.
        method_exit_label_ = buf_.pos();  // V2: trampoline jump target
        // Spill cached registers before epilogue (for methods that reach Ret
        // without going through a call site).
        SpillCachedRegs();
        EmitAddRI(buf_, kRSP, static_cast<int32_t>(kFrameSize));
        // Pop cached registers in reverse push order (LIFO).
        if (cached_vreg_count_ > 0) {
            for (uint32_t i = cached_vreg_count_; i > 0; i--) {
                EmitPop(buf_, kCacheableX64Regs[i - 1]);
            }
        }
        EmitPop(buf_, kRSI);
        EmitPop(buf_, kRBX);
        EmitPop(buf_, kRBP);
        EmitRet(buf_);
        return true;
    }

    // ── Overflow-checked arithmetic (V2: jo → deopt trampoline) ─────────
    case IROpCode::AddOvf:
    case IROpCode::SubOvf:
    case IROpCode::MulOvf: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        if (instr.src1_reg() >= kGprCount) return false;  // FPR not supported
        LoadGpr(kRAX, instr.src1_reg());
        if (instr.has_src2()) {
            LoadGpr(kRCX, instr.src2_reg());
        }
        if (opc == IROpCode::AddOvf) {
            EmitAddRR(buf_, kRAX, kRCX);
        } else if (opc == IROpCode::SubOvf) {
            EmitSubRR(buf_, kRAX, kRCX);
        } else {
            EmitImulRR(buf_, kRAX, kRCX);
        }
        // If overflow (OF=1), jump to deopt trampoline.
        uint32_t jo_off = buf_.pos() + 2;
        EmitJccRel32(buf_, kCC_O, 0);  // jo .overflow_trampoline
        overflow_jumps_.push_back(jo_off);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    // ── Arithmetic (GPR) ────────────────────────────────────────────────
    case IROpCode::Add:
    case IROpCode::Sub:
    case IROpCode::Mul:
    case IROpCode::Div:
    case IROpCode::Rem:
    case IROpCode::Neg:
    case IROpCode::DivUn:
    case IROpCode::RemUn: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        if (instr.src1_reg() >= kGprCount) {
            // FPR arithmetic — use extracted method.
            EmitFprArithmetic(opc, instr.dst_reg(), instr.src1_reg(),
                              instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
        } else {
            // GPR arithmetic — use extracted method.
            EmitGprArithmetic(opc, instr.dst_reg(), instr.src1_reg(),
                              instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
        }
        return true;
    }

    // ── Bitwise ─────────────────────────────────────────────────────────
    case IROpCode::And:
    case IROpCode::Or:
    case IROpCode::Xor:
    case IROpCode::Not: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        EmitBitwise(opc, instr.dst_reg(), instr.src1_reg(),
                    instr.has_src2() ? instr.src2_reg() : UINT32_MAX);
        return true;
    }

    // ── Shifts ──────────────────────────────────────────────────────────
    case IROpCode::Shl:
    case IROpCode::Shr:
    case IROpCode::ShrUn: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        EmitShift(opc, instr.dst_reg(), instr.src1_reg(),
                  instr.has_src2() ? instr.src2_reg() : UINT32_MAX,
                  instr.has_imm() ? instr.imm.i4 : 0);
        return true;
    }

    // ── Unconditional branch ────────────────────────────────────────────
    case IROpCode::Br: {
        uint32_t target = instr.imm.branch_target;
        // Detect backward branch (loop back edge) → insert safepoint poll.
        if (target < current_instr_index_) {
            EmitSafepointPoll();
            instr_since_safepoint_ = 0;
        }
        uint32_t patch_off = buf_.pos() + 1;  // offset of displacement in jmp rel32
        EmitJmpRel32(buf_, 0);  // placeholder
        branch_patches_.push_back({patch_off, target});
        return true;
    }

    // ── Conditional branch (compare two registers) ──────────────────────
    case IROpCode::Beq:
    case IROpCode::BneUn:
    case IROpCode::Blt: case IROpCode::Bgt:
    case IROpCode::Ble: case IROpCode::Bge:
    case IROpCode::BltUn: case IROpCode::BgtUn:
    case IROpCode::BleUn: case IROpCode::BgeUn: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        // Detect backward branch (loop back edge) → safepoint poll.
        if (instr.imm.branch_target < current_instr_index_) {
            EmitSafepointPoll();
            instr_since_safepoint_ = 0;
        }
        LoadGpr(kRAX, instr.src1_reg());
        LoadGpr(kRCX, instr.src2_reg());
        EmitCmpRR(buf_, kRAX, kRCX);
        uint8_t jcc = CmpToJccSigned(instr.op_code());
        uint32_t patch_off = buf_.pos() + 2;  // offset after 0F 8x
        EmitJccRel32(buf_, jcc, 0);
        branch_patches_.push_back({patch_off, instr.imm.branch_target});
        return true;
    }

    // ── Conditional branch (compare with 0) ─────────────────────────────
    case IROpCode::BrTrue:
    case IROpCode::BrFalse: {
        if (!instr.has_src1()) return false;
        // Detect backward branch (loop back edge) → safepoint poll.
        if (instr.imm.branch_target < current_instr_index_) {
            EmitSafepointPoll();
            instr_since_safepoint_ = 0;
        }
        LoadGpr(kRAX, instr.src1_reg());
        EmitCmpRI(buf_, kRAX, 0);
        uint8_t jcc = (instr.op_code() == IROpCode::BrTrue) ? kCC_NE : kCC_E;
        uint32_t patch_off = buf_.pos() + 2;
        EmitJccRel32(buf_, jcc, 0);
        branch_patches_.push_back({patch_off, instr.imm.branch_target});
        return true;
    }

    // ── Comparison (result in dst register) ─────────────────────────────
    case IROpCode::Ceq:
    case IROpCode::Clt:
    case IROpCode::Cgt: {
        if (!instr.has_src1() || !instr.has_src2() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        LoadGpr(kRCX, instr.src2_reg());
        EmitCmpRR(buf_, kRAX, kRCX);
        uint8_t cc;
        if (opc == IROpCode::Ceq) {
            cc = kCC_E;
        } else if (opc == IROpCode::Clt) {
            cc = kCC_L;
        } else if (opc == IROpCode::Cgt) {
            cc = kCC_G;
        } else {
            return false;
        }
        // xor eax, eax; setcc al → RAX = 0 or 1
        EmitXorZR(buf_, kRAX);
        EmitSetcc(buf_, cc, kRAX);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    // ── Type conversions ────────────────────────────────────────────────
    case IROpCode::Conv_I4:
    case IROpCode::Conv_I8:
    case IROpCode::ConvI:
    case IROpCode::ConvU: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        // For Conv_I4: truncate to 32-bit (mov eax, eax zero-extends)
        if (instr.op_code() == IROpCode::Conv_I4) {
            // Clear upper 32 bits: mov eax, eax (32-bit operation zero-extends)
            buf_.EmitByte(0x89);  // 89 C0 = mov eax, eax (no REX, 32-bit)
            buf_.EmitByte(0xC0);
        }
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Conv_R4:
    case IROpCode::Conv_R8:
    case IROpCode::ConvRUn: {
        // Float-to-float conversion: load from src, convert, store to dst
        // For simplicity in Phase 3b, treat as FPR copy (no conversion).
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadFpr(0, instr.src1_reg());
        StoreFpr(0, instr.dst_reg());
        return true;
    }

    // ── Overflow-checked conversions (V2: range check → deopt) ─────────
    case IROpCode::ConvOvfI4:
    case IROpCode::ConvOvfI: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        // Check signed 32-bit range: [INT32_MIN, INT32_MAX]
        // Lower: src >= INT32_MIN (0xFFFFFFFF80000000 as signed compare)
        EmitMovImm64(buf_, kRCX, 0xFFFFFFFF80000000ULL);
        EmitCmpRR(buf_, kRAX, kRCX);
        overflow_jumps_.push_back(buf_.pos() + 2);
        EmitJccRel32(buf_, kCC_L, 0);  // jl .overflow
        // Upper: src <= INT32_MAX (0x7FFFFFFF)
        EmitMovImm64(buf_, kRCX, 0x7FFFFFFFULL);
        EmitCmpRR(buf_, kRAX, kRCX);
        overflow_jumps_.push_back(buf_.pos() + 2);
        EmitJccRel32(buf_, kCC_G, 0);  // jg .overflow
        // Truncate to 32-bit
        buf_.EmitByte(0x89); buf_.EmitByte(0xC0);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::ConvOvfU4:
    case IROpCode::ConvOvfU: {
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        // Check unsigned 32-bit range: [0, UINT32_MAX]
        EmitCmpRI(buf_, kRAX, 0);
        overflow_jumps_.push_back(buf_.pos() + 2);
        EmitJccRel32(buf_, kCC_L, 0);  // jl .overflow
        EmitMovRIImm32(buf_, kRCX, -1);  // RCX = 0xFFFFFFFF
        EmitCmpRR(buf_, kRAX, kRCX);
        overflow_jumps_.push_back(buf_.pos() + 2);
        EmitJccRel32(buf_, kCC_A, 0);  // ja .overflow
        // Truncate to 32-bit
        buf_.EmitByte(0x89); buf_.EmitByte(0xC0);
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::ConvOvfI8:
    case IROpCode::ConvOvfU8: {
        // No overflow from narrower integer types — just pass through.
        if (!instr.has_src1() || !instr.has_dst()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    // ── Type conversions (continued) ────────────────────────────────
    case IROpCode::Pop: {
        // No-op in register-based IR (evaluation stack doesn't exist).
        // Pop exists in stack-based IR; in register IR it's a no-op.
        return true;
    }

    // ── LdStr (load string literal) ─────────────────────────────────────
    case IROpCode::LdStr: {
        // Returns a string pointer from string_operand.
        // For Phase 3b, this needs the runtime string table.
        // Return the string pointer as-is from the instruction metadata.
        if (!instr.has_dst()) return false;
        // Load string pointer into RAX and store to dst
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(instr.imm.ptr));
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    // ── LdFld / StFld (field access) ────────────────────────────────────
    case IROpCode::LdFld: {
        // Load field from object: dst = obj->fields[field_offset]
        // For Phase 3b, we need the runtime field access mechanism.
        // Emit a call to a runtime helper that does field loading.
        // This is complex. For now, fall back to RegisterExecute.
        return false;
    }

    case IROpCode::StFld: {
        return false;
    }

    // ── Call (direct AOT function call) ─────────────────────────────────
    case IROpCode::Call: {
        // Direct call with pre-resolved function pointer.
        // The instruction has: dst_reg, arg_count, direct_fn pointer.
        if (!instr.has_src1()) return false;

        uint32_t first_arg_reg = instr.src1_reg();
        uint32_t arg_count = instr.call_arg_count();
        void* target_fn = instr.imm.ptr;

        if (target_fn == nullptr) return false;

        // Shadow args go to stack (args 4-7 in Win64 convention).
        // The shadow space is at [rsp+0..32].
        // Load first 4 args into RCX, RDX, R8, R9.
        // For simplicity in Phase 3b, handle up to 8 args.
        static constexpr uint8_t kArgRegs[] = {kRCX, kRDX, kR8, kR9};
        uint32_t max_scratch = arg_count < 4 ? arg_count : 4;
        for (uint32_t i = 0; i < max_scratch; ++i) {
            LoadGpr(kArgRegs[i], first_arg_reg + i);
        }
        // Stack args: args 4+ go to shadow space at [rsp + (i-4)*8]
        for (uint32_t i = 4; i < arg_count; ++i) {
            // Load into RAX, then store to stack
            LoadGpr(kRAX, first_arg_reg + i);
            EmitMovMR(buf_, kRSP, static_cast<int32_t>((i - 4) * 8), kRAX);
        }

        // Spill cached registers before the call (GC safety).
        SpillCachedRegs();

        // Embed function pointer and call via register
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(target_fn));
        uint32_t call_pos = buf_.pos();
        EmitCallReg(buf_, kRAX);
        call_sites_.push_back({current_instr_index_, call_pos});
        RecordGcPoint(call_pos);

        // ── Deoptimization metadata for this call site ───────────────
        if (config_.enable_deopt) {
            uint32_t val_start = static_cast<uint32_t>(deopt_values_.size());
            // Record all GPR virtual registers (all values spilled to stack frame).
            for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                DeoptValue dv;
                dv.reg_index    = vr;
                dv.value_tag    = static_cast<uint8_t>(interpreter::ValueTag::Int64);
                dv.is_spilled   = true;
                dv.spill_offset = static_cast<int16_t>(GprOff(vr));
                deopt_values_.push_back(dv);
            }
            // Record all FPR virtual registers.
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
            entry.native_offset = call_pos;
            entry.instr_pc      = current_instr_index_;
            entry.num_values    = n_vals;
            deopt_entries_.push_back(entry);
        }

        // Store return value (in RAX) to dst register
        if (instr.has_dst()) {
            StoreGpr(kRAX, instr.dst_reg());
        }

        // For float-returning calls: preserve XMM0 if dst is an FPR.
        // For Phase 3b, assume GPR return (DirectFn returns uint64_t).

        return true;
    }

    // ── CallVirt / CallVirtConstrained (V2: PIC dispatch via helper) ─────
    case IROpCode::CallVirt:
    case IROpCode::CallVirtConstrained: {
        if (!instr.has_src1()) return false;
        uint32_t first_arg_reg = instr.src1_reg();
        uint32_t arg_count = instr.call_arg_count();

        // Build CodegenCallVirtArgs on the stack (64 bytes).
        EmitSubRI(buf_, kRSP, 64);

        // [rsp+0] = gpr_base: pointer to register file on T4 frame.
        uint32_t gpr_base_disp = 64 + kGprFileOff;
        EmitLeaRM(buf_, kRAX, kRSP, static_cast<int32_t>(gpr_base_disp));
        EmitMovMR(buf_, kRSP, 0, kRAX);

        // [rsp+8] = pic_data
        EmitMovImm64(buf_, kRAX,
            reinterpret_cast<uint64_t>(config_.pic_dispatch_data));
        EmitMovMR(buf_, kRSP, 8, kRAX);

        // [rsp+16] = dispatch_ctx (nullptr — unused in deopt path)
        EmitXorZR(buf_, kRAX);
        EmitMovMR(buf_, kRSP, 16, kRAX);

        // [rsp+24] = instruction_idx
        EmitMovRIImm32(buf_, kRAX, current_instr_index_);
        EmitMovMR(buf_, kRSP, 24, kRAX);

        // [rsp+28] = arg_count
        EmitMovRIImm32(buf_, kRAX, arg_count);
        EmitMovMR(buf_, kRSP, 28, kRAX);

        // [rsp+32] = first_arg_reg
        EmitMovRIImm32(buf_, kRAX, first_arg_reg);
        EmitMovMR(buf_, kRSP, 32, kRAX);

        // [rsp+40] = call_target (instr.imm.ptr)
        EmitMovImm64(buf_, kRAX,
            reinterpret_cast<uint64_t>(instr.imm.ptr));
        EmitMovMR(buf_, kRSP, 40, kRAX);

        // [rsp+48] = has_dst
        EmitMovRIImm32(buf_, kRAX, instr.has_dst() ? 1 : 0);
        EmitMovMR(buf_, kRSP, 48, kRAX);

        // [rsp+52] = is_instance_call (from header bit 63)
        uint32_t is_inst = (instr.header >> 63) & 1;
        EmitMovRIImm32(buf_, kRAX, is_inst);
        EmitMovMR(buf_, kRSP, 52, kRAX);

        // [rsp+56] = ret_buf (RSI = ret_buf from prologue)
        EmitMovMR(buf_, kRSP, 56, kRSI);

        // Spill cached registers before the helper call (GC safety).
        SpillCachedRegs();

        // Call CodegenCallVirt(&args): RCX = RSP
        EmitMovRR(buf_, kRCX, kRSP);
        EmitMovImm64(buf_, kRAX,
            reinterpret_cast<uint64_t>(CodegenCallVirt));
        uint32_t cpos = buf_.pos();
        EmitCallReg(buf_, kRAX);
        call_sites_.push_back({current_instr_index_, cpos});
        RecordGcPoint(cpos);

        // Pop CodegenCallVirtArgs from stack.
        EmitAddRI(buf_, kRSP, 64);

        // Check ret_buf[0] for kDeoptMagic.
        EmitMovRM(buf_, kRAX, kRSI, 0);
        EmitMovImm64(buf_, kRCX, kDeoptMagic);
        EmitCmpRR(buf_, kRAX, kRCX);
        uint32_t de_off = buf_.pos() + 2;
        EmitJccRel32(buf_, kCC_E, 0);  // je .deopt_trampoline
        deopt_jumps_.push_back(de_off);

        // Store return value.
        if (instr.has_dst()) {
            StoreGpr(kRAX, instr.dst_reg());
        }
        return true;
    }

    // ── Switch (V2: jmp table) ──────────────────────────────────────────
    case IROpCode::Switch: {
        if (!instr.has_src1()) return false;
        uint32_t target_count = instr.imm.operand_index;
        if (target_count == 0) return true;

        // Detect backward branch (loop back edge via any switch target) → safepoint.
        const uint32_t* switch_targets = static_cast<const uint32_t*>(instr.imm.ptr);
        for (uint32_t ti = 0; ti < target_count; ++ti) {
            if (switch_targets[ti] < current_instr_index_) {
                EmitSafepointPoll();
                instr_since_safepoint_ = 0;
                break;
            }
        }

        // Load index from src1_reg.
        LoadGpr(kRAX, instr.src1_reg());

        // Bounds check: if index < 0 OR index >= target_count → fallthrough.
        EmitCmpRI(buf_, kRAX, 0);
        uint32_t p_neg = buf_.pos();
        EmitJccRel32(buf_, kCC_L, 0);  // jl .after_switch

        EmitCmpRI(buf_, kRAX, static_cast<int32_t>(target_count));
        uint32_t p_oob = buf_.pos();
        EmitJccRel32(buf_, kCC_AE, 0);  // jae .after_switch

        // LEA RCX, [RIP + table_offset] — RIP-relative to the jmp table.
        uint32_t lea_pos = buf_.pos();
        EmitREX(buf_, true, kRCX, 0);
        buf_.EmitByte(0x8D);      // LEA opcode
        buf_.EmitByte(0x0D);      // ModRM: [rip + disp32]
        buf_.Emit32(0);           // placeholder disp32 — patched to table

        // MOV EAX, [RCX + RAX*4] — load displacement from table.
        EmitREX(buf_, false, 0, 4);
        buf_.EmitByte(0x8B);
        buf_.EmitByte(0x04);
        buf_.EmitByte(0x81);      // SIB: scale=4, index=rax, base=rcx

        // ADD RAX, RCX — compute absolute target (table_base + displacement).
        EmitAddRR(buf_, kRAX, kRCX);

        // JMP RAX.
        buf_.EmitByte(0xFF);
        buf_.EmitByte(0xE0);

        // Emit placeholder jmp table: target_count * uint32_t = 0.
        // These are patched after ResolveBranches.
        uint32_t table_pos = buf_.pos();
        for (uint32_t ti = 0; ti < target_count; ++ti) {
            buf_.Emit32(0);
        }
        uint32_t after_switch = buf_.pos();

        // Patch bounds-check jumps to after_switch.
        {
            int32_t d = static_cast<int32_t>(after_switch - (p_neg + 6));
            buf_.Patch32(p_neg + 2, static_cast<uint32_t>(d));
        }
        {
            int32_t d = static_cast<int32_t>(after_switch - (p_oob + 6));
            buf_.Patch32(p_oob + 2, static_cast<uint32_t>(d));
        }

        // Patch LEA RIP-relative offset from end of LEA (lea_pos+7) to table.
        {
            int32_t d = static_cast<int32_t>(table_pos - (lea_pos + 7));
            buf_.Patch32(lea_pos + 3, static_cast<uint32_t>(d));
        }

        // Record switch metadata for table patching (after ResolveBranches).
        switch_records_.push_back({table_pos, target_count,
            static_cast<const uint32_t*>(instr.imm.ptr)});

        return true;
    }

    // ── SEH opcodes ────────────────────────────────────────────────────
    case IROpCode::Throw: {
        // Load exception object (src1) into RCX, call CodegenThrow.
        if (!instr.has_src1()) return false;
        LoadGpr(kRCX, instr.src1_reg());
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(CodegenThrow));
        EmitCallReg(buf_, kRAX);
        // CodegenThrow never returns — ud2 for safety.
        buf_.EmitByte(0x0F); buf_.EmitByte(0x0B);  // ud2
        return true;
    }

    case IROpCode::Rethrow: {
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(CodegenRethrow));
        EmitCallReg(buf_, kRAX);
        buf_.EmitByte(0x0F); buf_.EmitByte(0x0B);  // ud2
        return true;
    }

    case IROpCode::EndFinally: {
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(CodegenEndFinally));
        EmitCallReg(buf_, kRAX);
        return true;
    }

    case IROpCode::EndFilter: {
        // EndFilter returns the filter result (src1) in RAX.
        if (!instr.has_src1()) return false;
        LoadGpr(kRAX, instr.src1_reg());
        EmitRet(buf_);
        return true;
    }

    // ── GC allocation opcodes (V5 + V3.5 TLAB inline) ────────────────────
    case IROpCode::NewObj: {
        // NewObj: allocate an InterpreterObject with type_token and field_count.
        // type_token is in imm.i4, field_count is in the secondary source.
        if (!instr.has_dst()) return false;
        uint32_t type_token = static_cast<uint32_t>(instr.imm.i4);

        // V3.5 TLAB inline fast path (V1: only when field_count is immediate).
        if (config_.tlab_current_loc != nullptr && config_.tlab_end_loc != nullptr &&
            !instr.has_src1()) {
            // Register usage:
            //   RAX = tlab_current_loc address / scratch
            //   RCX = bump ptr (allocated object)
            //   RDX = next bump ptr
            //   R8  = tlab_end value
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.tlab_current_loc));
            EmitMovRM(buf_, kRCX, kRAX, 0);        // rcx = bump ptr
            EmitLeaRM(buf_, kRDX, kRCX, static_cast<int32_t>(sizeof(interpreter::InterpreterObject)));
            EmitMovImm64(buf_, kR8, reinterpret_cast<uint64_t>(config_.tlab_end_loc));
            EmitMovRM(buf_, kR8, kR8, 0);           // r8 = end value
            EmitCmpRR(buf_, kRDX, kR8);              // next <= end?
            uint32_t ja_off = buf_.pos() + 2;        // disp32 of ja rel32
            EmitJccRel32(buf_, kCC_A, 0);            // ja .slow_path

            // Commit bump pointer.
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.tlab_current_loc));
            EmitMovMR(buf_, kRAX, 0, kRDX);          // [rax] = next

            // Zero-init entire object via XMM.
            EmitXorpsRR(buf_, 0, 0);                  // xmm0 = 0
            for (uint32_t off = 0; off < sizeof(interpreter::InterpreterObject); off += 8) {
                EmitMovSDMR(buf_, kRCX, static_cast<int32_t>(off), 0);
            }

            // Set up SmallFieldArray: fields_ptr_ points to inline_[0].
            EmitLeaRM(buf_, kRAX, kRCX,
                      static_cast<int32_t>(offsetof(interpreter::InterpreterObject, fields) +
                                           offsetof(interpreter::SmallFieldArray, inline_)));
            EmitMovMR(buf_, kRCX,
                      static_cast<int32_t>(offsetof(interpreter::InterpreterObject, fields) +
                                           offsetof(interpreter::SmallFieldArray, fields_ptr_)), kRAX);

            // field_capacity_ = kInlineCapacity (= 2).
            EmitMovMI32(buf_, kRCX,
                        static_cast<int32_t>(offsetof(interpreter::InterpreterObject, fields) +
                                             offsetof(interpreter::SmallFieldArray, field_capacity_)),
                        interpreter::SmallFieldArray::kInlineCapacity);

            // Store type_token.
            EmitMovMI32(buf_, kRCX,
                        static_cast<int32_t>(offsetof(interpreter::InterpreterObject, type_token)),
                        type_token);

            // Result in dst register.
            StoreGpr(kRCX, instr.dst_reg());

            // Jump over slow path.
            uint32_t jmp_off = buf_.pos() + 1;       // disp32 of jmp rel32
            EmitJmpRel32(buf_, 0);

            // .slow_path: patch ja to here.
            {
                int32_t disp = static_cast<int32_t>(buf_.pos()) -
                               static_cast<int32_t>(ja_off + 4);
                buf_.Patch32(ja_off, static_cast<uint32_t>(disp));
            }

            // Slow path: fallback to helper call.
            EmitMovRIImm32(buf_, kRDX, 0);            // field_count = 0
            EmitMovRIImm32(buf_, kRCX, type_token);
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(CodegenNewObj));
            SpillCachedRegs();
            {
                uint32_t cpos = buf_.pos();
                EmitCallReg(buf_, kRAX);
                call_sites_.push_back({current_instr_index_, cpos});
                RecordGcPoint(cpos);
            }
            StoreGpr(kRAX, instr.dst_reg());

            // .done: patch jmp to here.
            {
                int32_t disp = static_cast<int32_t>(buf_.pos()) -
                               static_cast<int32_t>(jmp_off + 4);
                buf_.Patch32(jmp_off, static_cast<uint32_t>(disp));
            }
            return true;
        }

        // Fallback: helper call (no TLAB config or register-sourced field_count).
        uint32_t field_count = 0;
        if (instr.has_src1()) {
            LoadGpr(kRDX, instr.src1_reg());
        } else {
            EmitMovRIImm32(buf_, kRDX, field_count);
        }
        EmitMovRIImm32(buf_, kRCX, type_token);
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(CodegenNewObj));
        SpillCachedRegs();
        {
            uint32_t cpos = buf_.pos();
            EmitCallReg(buf_, kRAX);
            call_sites_.push_back({current_instr_index_, cpos});
            RecordGcPoint(cpos);
        }
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    case IROpCode::Box: {
        // Box: wrap a raw value into a boxed InterpreterObject.
        // src1 = value, imm.i4 = type_token, tag is ValueTag::Int64 (bit-preserving).
        if (!instr.has_src1() || !instr.has_dst()) return false;

        // V3.5 TLAB inline fast path.
        if (config_.tlab_current_loc != nullptr && config_.tlab_end_loc != nullptr) {
            // Register usage:
            //   RAX = tlab_current_loc address / scratch
            //   RCX = bump ptr (allocated object)
            //   RDX = next bump ptr
            //   R8  = tlab_end value
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.tlab_current_loc));
            EmitMovRM(buf_, kRCX, kRAX, 0);        // rcx = bump ptr
            EmitLeaRM(buf_, kRDX, kRCX, static_cast<int32_t>(sizeof(interpreter::BoxedValue)));
            EmitMovImm64(buf_, kR8, reinterpret_cast<uint64_t>(config_.tlab_end_loc));
            EmitMovRM(buf_, kR8, kR8, 0);           // r8 = end value
            EmitCmpRR(buf_, kRDX, kR8);              // next <= end?
            uint32_t ja_off = buf_.pos() + 2;        // disp32 of ja rel32
            EmitJccRel32(buf_, kCC_A, 0);            // ja .slow_path

            // Commit bump pointer.
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.tlab_current_loc));
            EmitMovMR(buf_, kRAX, 0, kRDX);          // [rax] = next

            // Zero-init via XMM (sizeof(interpreter::BoxedValue) = size of InterpreterValue).
            EmitXorpsRR(buf_, 0, 0);                  // xmm0 = 0
            for (uint32_t off = 0; off < sizeof(interpreter::BoxedValue); off += 8) {
                EmitMovSDMR(buf_, kRCX, static_cast<int32_t>(off), 0);
            }

            // Store tag at InterpreterValue::tag (offset 0 within BoxedValue).
            uint8_t tag = static_cast<uint8_t>(
                chaos::il2cpp::interpreter::ValueTag::Int64);
            EmitMovMI32(buf_, kRCX, 0,
                        static_cast<uint32_t>(tag));

            // Store value from src1 register into InterpreterValue's union.
            LoadGpr(kRAX, instr.src1_reg());
            EmitMovMR(buf_, kRCX,
                      static_cast<int32_t>(offsetof(interpreter::InterpreterValue, i64)), kRAX);

            // Result in dst register.
            StoreGpr(kRCX, instr.dst_reg());

            // Jump over slow path.
            uint32_t jmp_off = buf_.pos() + 1;       // disp32 of jmp rel32
            EmitJmpRel32(buf_, 0);

            // .slow_path: patch ja to here.
            {
                int32_t disp = static_cast<int32_t>(buf_.pos()) -
                               static_cast<int32_t>(ja_off + 4);
                buf_.Patch32(ja_off, static_cast<uint32_t>(disp));
            }

            // Slow path: fallback to helper call.
            LoadGpr(kRCX, instr.src1_reg());                    // value
            EmitMovRIImm32(buf_, kRDX, static_cast<uint8_t>(     // tag
                chaos::il2cpp::interpreter::ValueTag::Int64));
            EmitMovRIImm32(buf_, kR8, static_cast<uint32_t>(instr.imm.i4));  // type_token
            EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(CodegenBox));
            SpillCachedRegs();
            {
                uint32_t cpos = buf_.pos();
                EmitCallReg(buf_, kRAX);
                call_sites_.push_back({current_instr_index_, cpos});
                RecordGcPoint(cpos);
            }
            StoreGpr(kRAX, instr.dst_reg());

            // .done: patch jmp to here.
            {
                int32_t disp = static_cast<int32_t>(buf_.pos()) -
                               static_cast<int32_t>(jmp_off + 4);
                buf_.Patch32(jmp_off, static_cast<uint32_t>(disp));
            }
            return true;
        }

        // Fallback: helper call (no TLAB config).
        LoadGpr(kRCX, instr.src1_reg());                    // value
        EmitMovRIImm32(buf_, kRDX, static_cast<uint8_t>(     // tag = Int64 (bit-preserving)
            chaos::il2cpp::interpreter::ValueTag::Int64));
        EmitMovRIImm32(buf_, kR8, static_cast<uint32_t>(instr.imm.i4));  // type_token
        EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(CodegenBox));
        SpillCachedRegs();
        {
            uint32_t cpos = buf_.pos();
            EmitCallReg(buf_, kRAX);
            call_sites_.push_back({current_instr_index_, cpos});
            RecordGcPoint(cpos);
        }
        StoreGpr(kRAX, instr.dst_reg());
        return true;
    }

    default:
        return false;
    }
}

NativeMethod* NativeCodeGenerator::Generate() noexcept {
    uint32_t n_instrs = static_cast<uint32_t>(rm_.instructions.size());
    if (n_instrs == 0) return nullptr;

    instr_offsets_.resize(n_instrs, 0);

    // ── Select hot vregs for register caching (V4) ─────────────────────
    SelectCacheableRegs();

    // ── Infer GC ref vregs (T4 precise GC) ────────────────────────────
    InferGcRefVregs();

    // ── Prologue ────────────────────────────────────────────────────────
    // Save callee-saved registers: RBP, RBX, RSI (Win64).
    // RBX holds args_buf pointer, RSI holds ret_buf pointer.
    // At entry: RCX = args_buf, RDX = ret_buf (Win64 calling convention)
    EmitPush(buf_, kRBP);
    EmitPush(buf_, kRBX);
    EmitPush(buf_, kRSI);
    // Push cached registers (R12-R15) if register caching is active.
    if (cached_vreg_count_ > 0) {
        for (uint32_t i = 0; i < cached_vreg_count_; i++) {
            EmitPush(buf_, kCacheableX64Regs[i]);
        }
    }
    EmitMovRR(buf_, kRBX, kRCX);   // RBX = args_buf
    EmitMovRR(buf_, kRSI, kRDX);   // RSI = ret_buf

    // Allocate stack frame: sub rsp, frame_size
    EmitSubRI(buf_, kRSP, static_cast<int32_t>(kFrameSize));

    // ── Emit instructions ───────────────────────────────────────────────
    for (uint32_t i = 0; i < n_instrs; ++i) {
        instr_offsets_[i] = buf_.pos();
        current_instr_index_ = i;
        const auto& instr = rm_.instructions[i];

        if (!EmitInstruction(instr)) {
            // Unsupported opcode — abort generation.
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "GenerateNativeCode: unsupported opcode {} at pc={}",
                static_cast<int>(instr.op_code()), i);
            return nullptr;
        }

        // Safepoint interval fallback: if no loop back edge was detected but
        // we've emitted too many instructions since the last poll, emit one.
        // This prevents GC starvation in forward-only control flow.
        ++instr_since_safepoint_;
        if (config_.safepoint_interval > 0 &&
            instr_since_safepoint_ >= config_.safepoint_interval) {
            EmitSafepointPoll();
            instr_since_safepoint_ = 0;
        }
    }

    // ── Epilogue (fallback — if Ret wasn't hit) ─────────────────────────
    // Add rsp, frame_size; pop rbp; ret
    // Actually if the method falls through without hitting Ret, we need to
    // fall back gracefully.  For now, emit a unreachable epilogue just in case.
    // (Most methods will return via a Ret instruction.)
    if (buf_.pos() == 0 || instr_offsets_.empty()) return nullptr;

    // ── Resolve branches ────────────────────────────────────────────────
    ResolveBranches();

    // ── V2: Resolve switch jmp tables ───────────────────────────────────
    // After branch resolution, all instr_offsets_ are final.
    // Patch each table entry: displacement = target_offset - table_offset.
    for (auto& sw : switch_records_) {
        for (uint32_t ti = 0; ti < sw.target_count; ++ti) {
            uint32_t target_instr = sw.targets[ti];
            if (target_instr < n_instrs) {
                uint32_t target_off = instr_offsets_[target_instr];
                int32_t disp = static_cast<int32_t>(
                    target_off - sw.table_offset);
                buf_.Patch32(sw.table_offset + ti * 4,
                             static_cast<uint32_t>(disp));
            }
        }
    }

    // ── V2: Deoptimization trampoline (for CallVirt PIC miss) ────────────
    // When CodegenCallVirt writes kDeoptMagic to ret_buf[0], the generated
    // code jumps here.  This trampoline just returns — entry_direct detects
    // kDeoptMagic in ret_buf[0] after the native entry returns and falls
    // back to RegisterExecute.
    for (auto& de_off : deopt_jumps_) {
        int32_t disp = static_cast<int32_t>(method_exit_label_ - (de_off + 4));
        buf_.Patch32(de_off, static_cast<uint32_t>(disp));
    }

    // ── V2: Overflow deoptimization trampoline ──────────────────────────
    if (!overflow_jumps_.empty()) {
        uint32_t ovf_tramp = buf_.pos();
        // Write kDeoptMagic to ret_buf[0] to signal deoptimization.
        EmitMovImm64(buf_, kRAX, static_cast<uint64_t>(kDeoptMagic));
        EmitMovMR(buf_, kRSI, 0, kRAX);
        // Jump to epilogue (shared with Ret).
        EmitJmpRel32(buf_, 0);
        uint32_t ovf_jmp_patch = buf_.pos() - 4;
        // Patch all overflow jumps to trampoline.
        for (auto& oj : overflow_jumps_) {
            int32_t disp = static_cast<int32_t>(ovf_tramp - (oj + 4));
            buf_.Patch32(oj, static_cast<uint32_t>(disp));
        }
        // Patch trampoline jmp to epilogue.
        {
            int32_t disp = static_cast<int32_t>(method_exit_label_ - (ovf_jmp_patch + 4));
            buf_.Patch32(ovf_jmp_patch, static_cast<uint32_t>(disp));
        }
    }

    // ── SEH clause table ────────────────────────────────────────────────
    // Emit after the main code, before Seal().  The table is appended to
    // the same code buffer (RX memory can contain R/O data).
    uint32_t code_bytes = buf_.pos();  // generated instruction bytes
    uint32_t seh_table_offset = 0;
    if (!rm_.seh_clauses.empty()) {
        seh_table_offset = buf_.pos();
        uint32_t n_clauses = static_cast<uint32_t>(rm_.seh_clauses.size());
        buf_.Emit32(n_clauses);
        for (const auto& clause : rm_.seh_clauses) {
            uint32_t flags       = static_cast<uint32_t>(clause.flags);
            uint32_t try_start   = clause.try_start_idx  < n_instrs
                                   ? instr_offsets_[clause.try_start_idx] : 0;
            uint32_t try_end     = clause.try_end_idx    < n_instrs
                                   ? instr_offsets_[clause.try_end_idx] : code_bytes;
            uint32_t handler_off = clause.handler_start_idx < n_instrs
                                   ? instr_offsets_[clause.handler_start_idx] : 0;
            uint32_t class_token = clause.class_token;
            buf_.Emit32(flags);
            buf_.Emit32(try_start);
            buf_.Emit32(try_end);
            buf_.Emit32(handler_off);
            buf_.Emit32(class_token);
        }
    }

    // ── Seal code buffer ────────────────────────────────────────────────
    uint32_t total_bytes = buf_.pos();  // code + SEH table
    void* code = buf_.Seal();
    if (code == nullptr) return nullptr;

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "GenerateNativeCode: {} instrs, {} bytes code ({} SEH), {} call sites",
        n_instrs, code_bytes, seh_table_offset ? (total_bytes - code_bytes) : 0,
        call_sites_.size());

    // ── GcSlotMapV0 emission (T4 precise GC) ──────────────────────────
    GcSlotMapV0* slot_map = nullptr;
    if (has_gc_ops_) {
        uint32_t n_slots = 0;
        uint64_t mask = gc_ref_vregs_;
        while (mask) { n_slots += (mask & 1); mask >>= 1; }
        size_t alloc_size = sizeof(GcSlotMapV0) + n_slots * sizeof(uint32_t);
        slot_map = static_cast<GcSlotMapV0*>(CHAOS_IL2CPP_MALLOC(alloc_size));
        if (slot_map != nullptr) {
            slot_map->frame_size = kFrameSize;
            slot_map->num_gc_slots = n_slots;
            uint32_t si = 0;
            for (uint32_t vr = 0; vr < kGprCount; ++vr) {
                if (gc_ref_vregs_ & (1ULL << vr)) {
                    slot_map->slots[si++] = CHAOS_GC_SLOT_ENCODE(
                        GprOff(vr), CHAOS_GC_SLOT_KIND_OBJECT);
                }
            }
        }
    }

    // ── Build NativeMethod ──────────────────────────────────────────────
    auto* nm = static_cast<NativeMethod*>(CHAOS_IL2CPP_MALLOC(sizeof(NativeMethod)));
    if (nm == nullptr) return nullptr;
    std::memset(nm, 0, sizeof(*nm));
    ::new (nm) NativeMethod();

    nm->code = code;
    nm->code_size = total_bytes;
    nm->instr_count = n_instrs;
    nm->seh_table_offset = seh_table_offset;
    nm->gc_slot_map = slot_map;

    // Allocate and fill call site info
    if (!call_sites_.empty()) {
        nm->call_site_count = static_cast<uint32_t>(call_sites_.size());
        nm->call_sites = static_cast<CallSiteInfo*>(
            CHAOS_IL2CPP_MALLOC(nm->call_site_count * sizeof(CallSiteInfo)));
        if (nm->call_sites) {
            std::memcpy(nm->call_sites, call_sites_.data(),
                        nm->call_site_count * sizeof(CallSiteInfo));
        }
    }

    // Allocate and fill deoptimization metadata
    if (!deopt_entries_.empty()) {
        nm->deopt_entry_count = static_cast<uint32_t>(deopt_entries_.size());
        nm->deopt_entries = static_cast<DeoptEntry*>(
            CHAOS_IL2CPP_MALLOC(nm->deopt_entry_count * sizeof(DeoptEntry)));
        if (nm->deopt_entries) {
            std::memcpy(nm->deopt_entries, deopt_entries_.data(),
                        nm->deopt_entry_count * sizeof(DeoptEntry));
        }
    }
    if (!deopt_values_.empty()) {
        uint32_t n = static_cast<uint32_t>(deopt_values_.size());
        nm->deopt_values = static_cast<DeoptValue*>(
            CHAOS_IL2CPP_MALLOC(n * sizeof(DeoptValue)));
        if (nm->deopt_values) {
            std::memcpy(nm->deopt_values, deopt_values_.data(),
                        n * sizeof(DeoptValue));
        }
    }

    // Allocate and fill GC point metadata
    if (!gc_points_.empty()) {
        nm->gc_point_count = static_cast<uint32_t>(gc_points_.size());
        nm->gc_points = static_cast<GcPoint*>(
            CHAOS_IL2CPP_MALLOC(nm->gc_point_count * sizeof(GcPoint)));
        if (nm->gc_points) {
            std::memcpy(nm->gc_points, gc_points_.data(),
                        nm->gc_point_count * sizeof(GcPoint));
        }
    }

    return nm;
}

// ── Public API ────────────────────────────────────────────────────────────

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

    // Check for unsupported opcodes.
    for (const auto& instr : rm.instructions) {
        switch (instr.op_code()) {
        // Supported opcodes
        case IROpCode::LdcI4:
        case IROpCode::LdcI8:
        case IROpCode::LdcR4:
        case IROpCode::LdcR8:
        case IROpCode::LdNull:
        case IROpCode::LdStr:
        case IROpCode::LdArg:
        case IROpCode::LdLoc:
        case IROpCode::StLoc:
        case IROpCode::Ret:
        case IROpCode::Pop:
        case IROpCode::Br:
        case IROpCode::BrTrue:
        case IROpCode::BrFalse:
        case IROpCode::Beq:
        case IROpCode::BneUn:
        case IROpCode::Blt:
        case IROpCode::Bgt:
        case IROpCode::Ble:
        case IROpCode::Bge:
        case IROpCode::BltUn:
        case IROpCode::BgtUn:
        case IROpCode::BleUn:
        case IROpCode::BgeUn:
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
        case IROpCode::Conv_I4:
        case IROpCode::Conv_I8:
        case IROpCode::Conv_R4:
        case IROpCode::Conv_R8:
        case IROpCode::ConvRUn:
        case IROpCode::ConvI:
        case IROpCode::ConvU:
            continue;  // OK

        // ── V2: CallVirt + Switch + overflow checking ─────────────────
        case IROpCode::CallVirt:
        case IROpCode::CallVirtConstrained:
            if (instr.imm.ptr != nullptr) continue;
            return false;

        case IROpCode::Switch:
        case IROpCode::AddOvf:
        case IROpCode::SubOvf:
        case IROpCode::MulOvf:
        case IROpCode::ConvOvfI:
        case IROpCode::ConvOvfI4:
        case IROpCode::ConvOvfI8:
        case IROpCode::ConvOvfU:
        case IROpCode::ConvOvfU4:
        case IROpCode::ConvOvfU8:
            continue;  // OK

        // ── V3: SEH opcodes ─────────────────────────────────────────────
        case IROpCode::Throw:
        case IROpCode::Rethrow:
        case IROpCode::EndFinally:
        case IROpCode::EndFilter:
            continue;  // OK

        // ── V5: GC allocation opcodes ───────────────────────────────────
        case IROpCode::NewObj:
        case IROpCode::Box:
            continue;  // OK (uses GcAllocate via helper call)

        case IROpCode::Call:
            if (instr.imm.ptr != nullptr) continue;  // Direct call supported
            return false;  // Indirect call not supported

        default:
            return false;  // Unsupported
        }
    }
    return true;
}

// ── NativeMethod destructor ───────────────────────────────────────────────

NativeMethod::~NativeMethod() noexcept {
    CHAOS_IL2CPP_FREE(call_sites);
    CHAOS_IL2CPP_FREE(deopt_entries);
    CHAOS_IL2CPP_FREE(deopt_values);
    CHAOS_IL2CPP_FREE(gc_points);
    CHAOS_IL2CPP_FREE(gc_slot_map);
    // Note: code is managed by CodeBuffer and freed separately.
    // For Phase 3b, we don't have a mechanism to free generated code yet.
    // Phase 3c will add a NativeMethodCache with proper cleanup.
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
