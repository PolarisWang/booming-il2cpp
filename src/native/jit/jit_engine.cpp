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

// ── Codename glossary (expansion for the terse tags used below) ─────────
// V1 : the original register-CACHING allocator (fixed kCacheableRegs, freq-based
//      hit tracking, write-through to the GPR stack file on spill).
// V2 : the Chaitin-Briggs GRAPH-COLORING allocator (jit_reg_alloc.h) that
//      replaces V1 when enable_register_caching is active; unchanged on tiers.
// A1 : T2.1 increment that eliminated the caller-colored write-through for
//      scalar-only, call-free methods (has_caller_clobber_ gate).
// T2.x : roadmap milestone tags (T2.1 register-residency, T2.2 precise
//      per-safepoint GC root maps, T2.3 spill/deopt contract + hot-update).
// T4 : the fully-JIT-compiled native code tier (JIT Tier 4), as opposed to
//      the interpreter (T1-T3 lowering).  "T4 code" = the emitted native
//      machine code; "T4 frame" = a stack frame running it.
// kQuick : CompileTier::kQuick — the quick (no-optimizer) compile tier.

} // namespace chaos::il2cpp::jit