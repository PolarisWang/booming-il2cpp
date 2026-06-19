// ── Misc stub declarations ─────────────────────────────────────
// Assorted stubs without a clear single domain: Array, Buffer,
// Type marshalling, Culture, GC, Environment, Console, Delegate.
#ifndef CHAOS_IL2CPP_MISC_STUBS_H_
#define CHAOS_IL2CPP_MISC_STUBS_H_
#pragma once

#include <chaos/compiler_hints.h>
#include <cstring>

// Array
void    ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept;

// Type marshalling — force-inline to eliminate call overhead for trivial casts.
// On x64, CHAOS_IL2CPP_INTPTR ≡ int64_t, so ChaosStoreInt64 is a no-op identity.
// Without forceinline, each call generates function prologue/epilogue (~10-20
// cycles) for what should be zero instructions.
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept {
    return static_cast<CHAOS_IL2CPP_INTPTR>(value);
}
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept {
    CHAOS_IL2CPP_INT32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT64  ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept {
    return static_cast<CHAOS_IL2CPP_INT64>(value);
}

// ── Float64 bit-cast helpers (force-inline for AOT hot path) ──
// These eliminate 2 out-of-line function calls per double operation
// in Convert::ToInt32(Double), Convert::ToDecimal(Double), etc.
// JIT inlines these to 0 instructions (same-register reuse); AOT
// codegen stores double bits in GPR via memcpy, requiring explicit
// store/load.  Inline avoids call/ret overhead (~10-20 cycles each).
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INTPTR ChaosStoreFloat64(CHAOS_IL2CPP_FLOAT64 value) noexcept {
    CHAOS_IL2CPP_INT64 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_FLOAT64 ChaosLoadFloat64(CHAOS_IL2CPP_INTPTR value) noexcept {
    CHAOS_IL2CPP_INT64 bits = static_cast<CHAOS_IL2CPP_INT64>(value);
    CHAOS_IL2CPP_FLOAT64 result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

// Buffer
CHAOS_IL2CPP_INT32  ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept;
void ChaosBufferMemmove(CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INTPTR src, CHAOS_IL2CPP_SIZE count) noexcept;
void ChaosBufferMemoryCopy(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT64 dest_size, CHAOS_IL2CPP_INT64 src_bytes) noexcept;
void ChaosBufferBlockCopy(CHAOS_IL2CPP_INTPTR src, CHAOS_IL2CPP_INT32 src_offset, CHAOS_IL2CPP_INTPTR dst, CHAOS_IL2CPP_INT32 dst_offset, CHAOS_IL2CPP_INT32 count) noexcept;

// Culture
CHAOS_IL2CPP_INTPTR ChaosCultureGetCurrent(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetInvariant(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCompareInfo(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDateTimeFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDisplayName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetNumberFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INT32 ChaosCompareInfoCompare(CHAOS_IL2CPP_INTPTR compare_info, CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept;
CHAOS_IL2CPP_INT32 ChaosCompareInfoIndexOf(CHAOS_IL2CPP_INTPTR compare_info, CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetTextInfo(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextInfoToLower(CHAOS_IL2CPP_INTPTR text_info, CHAOS_IL2CPP_INT32 c) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextInfoToUpper(CHAOS_IL2CPP_INTPTR text_info, CHAOS_IL2CPP_INT32 c) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextInfoGetCultureName(CHAOS_IL2CPP_INTPTR text_info) noexcept;

// ── Globalization stubs ──
CHAOS_IL2CPP_FLOAT64 ChaosCharUnicodeInfoGetNumericValue(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INT32  ChaosCharUnicodeInfoGetDigitValue(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INT32  ChaosCharUnicodeInfoGetDecimalDigitValue(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INT32  ChaosCharUnicodeInfoGetUnicodeCategory(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INT32  ChaosCompareInfoIsSortableString(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32  ChaosCompareInfoIsSortableInt(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDateTimeFormatInfoGetInstance(CHAOS_IL2CPP_INTPTR provider) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCultureInfo(CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCultureInfoBool(CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 tryFirst) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCultureInfoByIetfLanguageTag(CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureCreateSpecificCulture(CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCompareInfoGetCompareInfo(CHAOS_IL2CPP_INTPTR name) noexcept;

// GC
extern "C" {
void    ChaosGcCollect(CHAOS_IL2CPP_INT32 generation) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetGeneration(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetMaxGeneration(void) noexcept;
}

// Environment / Console
CHAOS_IL2CPP_INTPTR ChaosEnvironmentGetStackTrace(void) noexcept;
CHAOS_IL2CPP_INT32 chaos_current_managed_thread_id(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosConsoleGetError(void) noexcept;
void    ChaosConsoleWriteLine(CHAOS_IL2CPP_INTPTR value) noexcept;

// Delegate
void    ChaosDelegateInitialize(CHAOS_IL2CPP_INTPTR delegate_obj, CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR method_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDelegateGetTarget(CHAOS_IL2CPP_INTPTR delegate_obj) noexcept;
CHAOS_IL2CPP_INTPTR chaos_delegate_combine(CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept;
CHAOS_IL2CPP_INTPTR chaos_delegate_remove(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value) noexcept;
#endif  // CHAOS_IL2CPP_MISC_STUBS_H_
