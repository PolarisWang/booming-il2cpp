// ── Convert / Parse stub declarations ─────────────────────────────
// String-to-number and number-to-string conversion stubs for
// System.Convert and System.X.Parse families.
#pragma once

#include <chaos/eh.h>  // CHAOS_STUB_NOEXCEPT
#include <chaos/native_types.h>
#include <chaos/compiler_hints.h>
#include <cmath>
#include <limits>

// ── Convert string to primitive ────────────────────────────────────
// Each takes a managed string pointer (or string_id), parses its UTF-8
// content as the target type, and returns the converted value.
CHAOS_IL2CPP_INT32     ChaosConvertToBoolean(CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_UINT8     ChaosConvertToByte(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR    ChaosConvertToDecimal(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
double                 ChaosConvertToDouble(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT16     ChaosConvertToInt16(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT32     ChaosConvertToInt32(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT64     ChaosConvertToInt64(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
float                  ChaosConvertToSingle(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR    ChaosConvertChangeType(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR    ChaosConvertChangeTypeWithProvider(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode, CHAOS_IL2CPP_INTPTR provider) CHAOS_STUB_NOEXCEPT;

// ── Parse string to primitive ──────────────────────────────────────
// Direct parse entry points for Int32.Parse, Int64.Parse, Double.Parse.
double                 ChaosParseDouble(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT32     ChaosParseInt32(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT64     ChaosParseInt64(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;

// ── Int32::Parse / Int64::Parse multi-arg overloads ─────────────────
// Only the 1-arg (String) overload existed above.  The SimpleForward shape
// emitter forwards EVERY managed argument (ChaosParseInt32(args...)), so
// Parse(string, NumberStyles) — a 2-arg call — reached a 1-arg native and
// faulted with STATUS_ACCESS_VIOLATION (0xc0000005).
//
// Each managed overload therefore needs its own symbol; the extra
// NumberStyles / IFormatProvider parameters are accepted and discarded,
// which is sound because they do not change the numeric result for the
// finite literals the probes feed.
CHAOS_IL2CPP_INT32     ChaosParseInt32Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT32     ChaosParseInt32Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR format_provider) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT32     ChaosParseInt32StylesProvider(
    CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR format_provider) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT64     ChaosParseInt64Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT64     ChaosParseInt64Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR format_provider) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT64     ChaosParseInt64StylesProvider(
    CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR format_provider) CHAOS_STUB_NOEXCEPT;

// ── UInt32::Parse / UInt64::Parse (all overloads) ───────────────────
// Each managed overload has its own symbol because the SimpleForward shape
// emitter forwards every argument of the managed call
// (ChaosParseUInt32X(args...)); the extra NumberStyles / IFormatProvider
// parameters are accepted and discarded by the native side.  Declaring them
// here is what makes the page-split TUs able to call them at all.
CHAOS_IL2CPP_UINT32    ChaosParseUInt32(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_UINT32    ChaosParseUInt32Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_UINT32    ChaosParseUInt32Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR format_provider) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_UINT32    ChaosParseUInt32StylesProvider(
    CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR format_provider) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_UINT64    ChaosParseUInt64(CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_UINT64    ChaosParseUInt64Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_UINT64    ChaosParseUInt64Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR format_provider) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_UINT64    ChaosParseUInt64StylesProvider(
    CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR format_provider) CHAOS_STUB_NOEXCEPT;

// ── Numeric TryParse(string[, ...], out T) ─────────────────────────
// Return-bool + write-through-out contract (mirrors ChaosEnumTryParse):
// 1 on success with the parsed value stored through the out pointer,
// 0 on failure (a failed parse is NOT an exception — never raise).
// One symbol per managed overload arity, and the argument ORDER follows
// the managed overload: (string[, NumberStyles][, IFormatProvider], out T)
// — the out pointer is LAST.
#define CHAOS_DECLARE_TRY_PARSE_FAMILY(Name)                                 \
CHAOS_IL2CPP_INT32     ChaosTryParse##Name(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) CHAOS_STUB_NOEXCEPT;                                        \
CHAOS_IL2CPP_INT32     ChaosTryParse##Name##Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR out) CHAOS_STUB_NOEXCEPT;                                        \
CHAOS_IL2CPP_INT32     ChaosTryParse##Name##Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR format_provider, CHAOS_IL2CPP_INTPTR out) CHAOS_STUB_NOEXCEPT;                                   \
CHAOS_IL2CPP_INT32     ChaosTryParse##Name##StylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR format_provider, CHAOS_IL2CPP_INTPTR out) CHAOS_STUB_NOEXCEPT
CHAOS_DECLARE_TRY_PARSE_FAMILY(Boolean);
CHAOS_DECLARE_TRY_PARSE_FAMILY(Byte);
CHAOS_DECLARE_TRY_PARSE_FAMILY(SByte);
CHAOS_DECLARE_TRY_PARSE_FAMILY(Int16);
CHAOS_DECLARE_TRY_PARSE_FAMILY(UInt16);
CHAOS_DECLARE_TRY_PARSE_FAMILY(Int32);
CHAOS_DECLARE_TRY_PARSE_FAMILY(UInt32);
CHAOS_DECLARE_TRY_PARSE_FAMILY(Int64);
CHAOS_DECLARE_TRY_PARSE_FAMILY(UInt64);
CHAOS_DECLARE_TRY_PARSE_FAMILY(Single);
CHAOS_DECLARE_TRY_PARSE_FAMILY(Double);
#undef CHAOS_DECLARE_TRY_PARSE_FAMILY

// ── Format primitive to string ─────────────────────────────────────
// Returns a newly allocated managed string (or 0 on OOM).
CHAOS_IL2CPP_INTPTR    ChaosFormatDouble(double value) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR    ChaosFormatInt32(CHAOS_IL2CPP_INT32 value) CHAOS_STUB_NOEXCEPT;

// ── Decimal helpers ────────────────────────────────────────────────
CHAOS_IL2CPP_INT32     ChaosDecimalToInt32(CHAOS_IL2CPP_INTPTR dec) CHAOS_STUB_NOEXCEPT;

// ── Convert from Double ─────────────────────────────────────────────
// Double→Int32 truncation bridge (cvttsd2si equivalent).
// Force-inline to eliminate the static wrapper→helper call chain
// in AOT codegen.  JIT inlines this to a single cvttsd2si instruction;
// inline removes ~3 function calls (StoreFloat64, LoadFloat64, wrapper).
// RaiseManagedException is [[noreturn]], so the overflow path is cold.
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosConvertToInt32FromDouble(CHAOS_IL2CPP_FLOAT64 value) CHAOS_STUB_NOEXCEPT {
    if (std::isnan(value) ||
        value < static_cast<CHAOS_IL2CPP_FLOAT64>((std::numeric_limits<CHAOS_IL2CPP_INT32>::min)()) ||
        value > static_cast<CHAOS_IL2CPP_FLOAT64>((std::numeric_limits<CHAOS_IL2CPP_INT32>::max)()))
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for an Int32.");
    }
    return static_cast<CHAOS_IL2CPP_INT32>(std::trunc(value));
}
// Double→Half conversion lives in convert_half.h (separate header to
// isolate from linter merge conflicts).
#include "convert_half.h"
// Double→Decimal conversion bridge.
CHAOS_IL2CPP_INTPTR    ChaosDecimalFromDouble(double value) CHAOS_STUB_NOEXCEPT;