// osr_trigger.h — On-Stack Replacement (OSR) trigger management
//
// Separated from ir_reg_alloc.cpp for clearer architecture.
// OSR promotes hot interpreted methods from RegisterExecute to
// T4 native code mid-execution.
//
// Trigger criteria (from roadmap):
//   - Loop backward branches: promoted after kOsrLoopThreshold iterations
//   - Re-entry: promoted on kT3NativeThreshold call count

#ifndef CHAOS_IL2CPP_OSR_TRIGGER_H_
#define CHAOS_IL2CPP_OSR_TRIGGER_H_

#include <cstdint>

namespace chaos::il2cpp::interpreter {

struct RegisterFrame;
struct RegisterInstruction;

// ── OSR threshold ───────────────────────────────────────────────────────
// A method is promoted to T4 native code after this many backward branch
// iterations within RegisterExecute (loop hotness detection).
static constexpr uint32_t kOsrLoopThreshold = 100;

// ── TryOsrPromotion ─────────────────────────────────────────────────────
// Attempt to promote the currently executing method to T4 native code.
// If successful, frame.pc is set to the magic value 0xFFffFFffu and
// the return value is stored in frame.ret_val/frame.has_ret.
//
// Called from RegisterExecute's backward branch check.
void TryOsrPromotion(RegisterFrame& frame,
                     const RegisterInstruction* instrs,
                     uint32_t instr_count) noexcept;

}  // namespace chaos::il2cpp::interpreter

#endif  // CHAOS_IL2CPP_OSR_TRIGGER_H_
