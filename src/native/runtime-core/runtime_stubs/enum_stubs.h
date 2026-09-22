// ── Enum stub declarations ──────────────────────────────────────
#pragma once

#include <chaos/eh.h>  // CHAOS_STUB_NOEXCEPT
#include <chaos/native_types.h>

extern "C" {

// ── Internal helpers exposed for generated code inline shapes ──
// These are used by the InlineShapeDescriptor in RuntimeHelperShapeRegistry
// to avoid the extern "C" function call overhead on cache hits.
// Defined in enum_stubs.cpp (non-static, in the runtime_core namespace).
CHAOS_IL2CPP_INTPTR lookup_cached_enum_name(CHAOS_IL2CPP_INT64 value) CHAOS_STUB_NOEXCEPT;

CHAOS_IL2CPP_INT32 ChaosEnumIsDefined(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumGetName(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumGetNames(CHAOS_IL2CPP_INTPTR type) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumGetValues(CHAOS_IL2CPP_INTPTR type) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumParse(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumParseWithIgnoreCase(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 ignoreCase) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumFormat(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR format) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumFormatRaw(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT64 raw_value, CHAOS_IL2CPP_INTPTR format) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumToString(CHAOS_IL2CPP_INTPTR this_obj) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumToStringRaw(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT64 raw_value) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosEnumToStringWithFormat(CHAOS_IL2CPP_INTPTR this_obj, CHAOS_IL2CPP_INTPTR format_str) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT32 ChaosEnumTryParse(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR result) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INT32 ChaosEnumTryParseWithIgnoreCase(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 ignoreCase, CHAOS_IL2CPP_INTPTR result) CHAOS_STUB_NOEXCEPT;

}  // extern "C"
