#include "code_generator.h"
#include "x64_encoder.h"
#include "code_buffer.h"
#include "codegen_helpers.h"

#include "../interpreter/ir_reg_alloc.h"
#include "../interpreter/interpreter_vm.h"

#include <codegen_bridge.h>
#include <chaos/log.h>

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
    EmitMovRM(buf_, x64_reg, kRSP, static_cast<int32_t>(GprOff(vreg)));
}

void NativeCodeGenerator::StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
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

    // x64 safepoint sequence (calls through function pointer):
    //   sub rsp, 32                ; reserve shadow space for callee
    //   mov rax, <safepoint_fn>    ; function pointer
    //   call rax
    //   add rsp, 32                ; restore shadow space
    //
    // The SafepointPoll function checks thread->suspend_seq:
    //   - If 0 (no GC pending): returns immediately (~1ns)
    //   - If non-zero: acknowledges, spins/wait until GC completes
    //
    // For backward branch sites, the shadow-space overhead is acceptable
    // since loops are typically long-running.

    EmitSubRI(buf_, kRSP, 32);
    EmitMovImm64(buf_, kRAX, reinterpret_cast<uint64_t>(config_.safepoint_fn));
    uint32_t call_pos = buf_.pos();
    EmitCallReg(buf_, kRAX);
    // Record as a call site with sentinel instr index (non-method call).
    call_sites_.push_back({UINT32_MAX, call_pos});
    RecordGcPoint(call_pos);
    EmitAddRI(buf_, kRSP, 32);
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
        EmitAddRI(buf_, kRSP, static_cast<int32_t>(kFrameSize));
        EmitPop(buf_, kRSI);
        EmitPop(buf_, kRBX);
        EmitPop(buf_, kRBP);
        EmitRet(buf_);
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

    default:
        return false;
    }
}

NativeMethod* NativeCodeGenerator::Generate() noexcept {
    uint32_t n_instrs = static_cast<uint32_t>(rm_.instructions.size());
    if (n_instrs == 0) return nullptr;

    instr_offsets_.resize(n_instrs, 0);

    // ── Prologue ────────────────────────────────────────────────────────
    // Save callee-saved registers: RBP, RBX, RSI (Win64).
    // RBX holds args_buf pointer, RSI holds ret_buf pointer.
    // At entry: RCX = args_buf, RDX = ret_buf (Win64 calling convention)
    EmitPush(buf_, kRBP);
    EmitPush(buf_, kRBX);
    EmitPush(buf_, kRSI);
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
    }

    // ── Epilogue (fallback — if Ret wasn't hit) ─────────────────────────
    // Add rsp, frame_size; pop rbp; ret
    // Actually if the method falls through without hitting Ret, we need to
    // fall back gracefully.  For now, emit a unreachable epilogue just in case.
    // (Most methods will return via a Ret instruction.)
    if (buf_.pos() == 0 || instr_offsets_.empty()) return nullptr;

    // ── Resolve branches ────────────────────────────────────────────────
    ResolveBranches();

    // ── Seal code buffer ────────────────────────────────────────────────
    uint32_t code_bytes = buf_.pos();
    void* code = buf_.Seal();
    if (code == nullptr) return nullptr;

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "GenerateNativeCode: {} instrs, {} bytes code, {} call sites",
        n_instrs, code_bytes, call_sites_.size());

    // ── Build NativeMethod ──────────────────────────────────────────────
    auto* nm = static_cast<NativeMethod*>(CHAOS_IL2CPP_MALLOC(sizeof(NativeMethod)));
    if (nm == nullptr) return nullptr;
    std::memset(nm, 0, sizeof(*nm));
    ::new (nm) NativeMethod();

    nm->code = code;
    nm->code_size = code_bytes;
    nm->instr_count = n_instrs;

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
    if (!rm.seh_clauses.empty()) return false;  // SEH not supported in native code

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
