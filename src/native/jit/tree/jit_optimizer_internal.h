#pragma once
// jit_optimizer_internal.h — Shared helpers for the Tree-IR loop-optimization
// passes (jit_optimizer.cpp / jit_optimizer_loops.cpp).
//
// These were static (internal-linkage) helpers in the original single-TU
// jit_optimizer.cpp.  Split into two TUs, each pass file needs them; they are
// moved here as inline to keep a single definition across TUs without ODR.
//
// NOT part of the public API.  Do not include from outside jit/tree.

#include <cstdint>
#include <vector>

#include "interpreter/ir_reg_alloc.h"
#include "interpreter/generated/ir_opcodes.h"
#include "tree/jit_cfg.h"  // BBRange, LoopAnalysis, NaturalLoop, FindBasicBlocks

namespace chaos::il2cpp::jit::tree {
namespace internal {

// ── Helper: is this opcode pure arithmetic (safe for LICM hoisting)? ──
// Pure arithmetic has no side effects, no exceptions, and depends only
// on its src vregs.  Div/Rem/AddOvf are excluded (can throw).
inline bool IsPureArithmetic(interpreter::IROpCode opc) noexcept {
    switch (opc) {
        case interpreter::IROpCode::Add:
        case interpreter::IROpCode::Sub:
        case interpreter::IROpCode::Mul:
        case interpreter::IROpCode::And:
        case interpreter::IROpCode::Or:
        case interpreter::IROpCode::Xor:
        case interpreter::IROpCode::Shl:
        case interpreter::IROpCode::Shr:
        case interpreter::IROpCode::ShrUn:
        case interpreter::IROpCode::Neg:
        case interpreter::IROpCode::Not:
        case interpreter::IROpCode::Ceq:
        case interpreter::IROpCode::Clt:
        case interpreter::IROpCode::Cgt:
        case interpreter::IROpCode::Conv_I4:
        case interpreter::IROpCode::Conv_I8:
        case interpreter::IROpCode::Conv_R4:
        case interpreter::IROpCode::Conv_R8:
        case interpreter::IROpCode::ConvRUn:
        case interpreter::IROpCode::ConvI:
        case interpreter::IROpCode::ConvU:
        case interpreter::IROpCode::LdLen:
            return true;
        default:
            return false;
    }
}

// ── Loop-pass entry points (defined in jit_optimizer_loops.cpp) ──
void BuildVRegDefBlocks(const std::vector<interpreter::RegisterInstruction>& instrs,
                        const std::vector<BBRange>& bbs, uint32_t max_vreg,
                        std::vector<uint32_t>& out_def_blocks) noexcept;
void LicmHoist(std::vector<interpreter::RegisterInstruction>& out_instrs,
               const std::vector<uint32_t>& bb_starts, const std::vector<uint32_t>& bb_ends,
               const LoopAnalysis& analysis, const std::vector<uint32_t>& vreg_def_blocks,
               uint32_t& next_vreg) noexcept;
void IvStrengthReduce(std::vector<interpreter::RegisterInstruction>& out_instrs,
                      const std::vector<uint32_t>& bb_starts, const std::vector<uint32_t>& bb_ends,
                      const LoopAnalysis& analysis, uint32_t& next_vreg) noexcept;
void UnrollLoops(std::vector<interpreter::RegisterInstruction>& out_instrs,
                 const std::vector<uint32_t>& bb_starts, const std::vector<uint32_t>& bb_ends,
                 const LoopAnalysis& analysis, uint32_t& next_vreg,
                 uint32_t unroll_factor = 4) noexcept;

}  // namespace internal
}  // namespace chaos::il2cpp::jit::tree
