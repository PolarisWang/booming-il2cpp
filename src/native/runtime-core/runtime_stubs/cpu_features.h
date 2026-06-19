// ── Runtime CPU feature detection (CPUID-based) ────────────────
#ifndef CHAOS_IL2CPP_CPU_FEATURES_H_
#define CHAOS_IL2CPP_CPU_FEATURES_H_
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
inline bool HasAvx2() noexcept
{
    static bool s_checked = false;
    static bool s_has_avx2 = false;
    if (s_checked) [[likely]]
        return s_has_avx2;

    // Step 1: check CPUID leaf 1 for OSXSAVE support (ECX bit 27)
#if defined(_MSC_VER)
    int cpuinfo[4] = {};
    __cpuidex(cpuinfo, 1, 0);
    const bool has_osxsave = (cpuinfo[2] & (1 << 27)) != 0;
    if (!has_osxsave) { s_checked = true; s_has_avx2 = false; return false; }

    // Step 2: verify OS saves YMM registers via XGETBV
    const uint64_t xcr0 = _xgetbv(0);
    if ((xcr0 & 6) != 6) { s_checked = true; s_has_avx2 = false; return false; }

    // Step 3: check CPUID leaf 7 for AVX2 (EBX bit 5)
    __cpuidex(cpuinfo, 7, 0);
    s_has_avx2 = (cpuinfo[1] & (1 << 5)) != 0;
#elif defined(__GNUC__) || defined(__clang__)
    unsigned int eax, ebx, ecx, edx;
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    const bool has_osxsave = (ecx & (1 << 27)) != 0;
    if (!has_osxsave) { s_checked = true; s_has_avx2 = false; return false; }

    // XGETBV via inline asm (GCC/Clang < 12 may not have _xgetbv)
    uint32_t xcr0_low = 0, xcr0_high = 0;
    __asm__ volatile("xgetbv" : "=a"(xcr0_low), "=d"(xcr0_high) : "c"(0));
    const uint64_t xcr0 = (static_cast<uint64_t>(xcr0_high) << 32) | xcr0_low;
    if ((xcr0 & 6) != 6) { s_checked = true; s_has_avx2 = false; return false; }

    __get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx);
    s_has_avx2 = (ebx & (1 << 5)) != 0;
#else
    s_checked = true;
    s_has_avx2 = false;
#endif
    s_checked = true;
    return s_has_avx2;
}

}}} // namespace chaos::il2cpp::runtime_core

#endif // __x86_64__ || _M_AMD64
#endif  // CHAOS_IL2CPP_CPU_FEATURES_H_
