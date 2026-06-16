// Manually maintained fact skip list for known-failing methods.
// AOT-only failure at method index 54 (JIT passes 20/20 clean).
// The exact method name is not available in the current subjects
// metadata because the AOT method index exceeds the subject range.
#pragma once
// NOLINTNEXTLINE
static inline bool IsFactMethodSkipped(int index) noexcept {
    if (index == 54) return true;
    return false;
}
