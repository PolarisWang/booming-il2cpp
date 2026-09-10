// ── Runtime CPU feature detection (CPUID-based) ────────────────
#pragma once

#include <chaos/native_types.h>

#if defined(__x86_64__) || defined(_M_AMD64)

#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#endif

namespace chaos { namespace il2cpp { namespace runtime_core {

/// Returns true if the CPU supports AVX2 (Advanced Vector Extensions 2).
/// Uses CPUID leaf 7, EBX bit 5 (AVX2), after verifying OSXSAVE support
/// and that the OS saves the YMM registers (XCR0 bits 1|2).
/// Static one-time initialization — negligible cost after first call.
/// Implemented in cpu_features.cpp to avoid C2084 when this header is
/// included from multiple translation units.
extern bool HasAvx2() noexcept;

}}} // namespace chaos::il2cpp::runtime_core

#endif // __x86_64__ || _M_AMD64
