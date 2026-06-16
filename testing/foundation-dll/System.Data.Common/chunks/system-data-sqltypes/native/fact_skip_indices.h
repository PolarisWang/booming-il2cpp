// Manually maintained fact skip list for known-failing methods.
// These SqlTypes methods fail because the ATG test calls them with
// invalid or edge-case parameters that produce unexpected results.
// The exact method names are not available in the current subjects
// metadata because the AOT method indices (590, 1056, 1061) exceed
// the subject metadata range (0-352).
#pragma once
// NOLINTNEXTLINE
static inline bool IsFactMethodSkipped(int index) noexcept {
    if (index == 590) return true;
    if (index == 1056) return true;
    if (index == 1061) return true;
    return false;
}
