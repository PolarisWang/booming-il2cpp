// pal_cpu_win32.cpp — Windows CPU feature detection (CPUID / IsProcessorFeaturePresent)

#include <chaos/pal/pal_cpu.h>

#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#include <windows.h>
#include <processthreadsapi.h>
#endif

namespace chaos::il2cpp::pal {

static uint64_t DetectCpuFeatures() noexcept {
    uint64_t features = 0;

#if defined(__x86_64__) || defined(_M_AMD64)
    int info[4] = {};

#if defined(_MSC_VER)
    __cpuidex(info, 1, 0);
#else
    __get_cpuid(1, &info[0], &info[1], &info[2], &info[3]);
#endif
    // Leaf 1: ECX
    if (info[2] & (1 << 20)) features |= (1ULL << static_cast<uint32_t>(CpuFeature::kSse41));
    if (info[2] & (1 << 23)) features |= (1ULL << static_cast<uint32_t>(CpuFeature::kPopcnt));
    if (info[2] & (1 << 25)) features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAes));
    // Leaf 1: EDX
    if (info[3] & (1 << 26)) features |= (1ULL << static_cast<uint32_t>(CpuFeature::kSse2));

    // Check OSXSAVE before querying AVX features (leaf 7)
    bool has_avx = false;
    if (info[2] & (1 << 27)) {  // OSXSAVE
#if defined(_MSC_VER)
        uint64_t xcr0 = _xgetbv(0);
#else
        uint32_t xcr0_low = 0, xcr0_high = 0;
        __asm__ volatile("xgetbv" : "=a"(xcr0_low), "=d"(xcr0_high) : "c"(0));
        uint64_t xcr0 = (static_cast<uint64_t>(xcr0_high) << 32) | xcr0_low;
#endif
        if ((xcr0 & 6) == 6) {  // XMM + YMM state save enabled
            has_avx = true;
            features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAvx));
        }
        if ((xcr0 & 0xE0) == 0xE0) {  // OPMASK + ZMM upper + ZMM lower
            features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAvx512F));
        }

        // Leaf 7 (subleaf 0)
#if defined(_MSC_VER)
        __cpuidex(info, 7, 0);
#else
        __get_cpuid_count(7, 0, &info[0], &info[1], &info[2], &info[3]);
#endif
        if (info[1] & (1 << 5))  features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAvx2));
        if (info[1] & (1 << 3))  features |= (1ULL << static_cast<uint32_t>(CpuFeature::kBmi1));
        if (info[1] & (1 << 8))  features |= (1ULL << static_cast<uint32_t>(CpuFeature::kBmi2));
        if (info[2] & (1 << 1))  features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAvx512Vbmi));
    }

    if (!has_avx) {
        // Leaf 1 check for AVX (ECX bit 28) — only valid if OS saves YMM
        // (handled above via xgetbv). If OSXSAVE is disabled, AVX is unavailable.
        // Leaf 7 queries above skipped without OSXSAVE.
    }
#elif defined(__aarch64__) || defined(_M_ARM64)
    // Windows ARM64: use IsProcessorFeaturePresent
    if (::IsProcessorFeaturePresent(PF_ARM_NEON_INSTRUCTIONS_AVAILABLE))
        features |= (1ULL << static_cast<uint32_t>(CpuFeature::kNeon));
    // ARM64 AES: check via ISProcessorFeaturePresent or reg read
    if (::IsProcessorFeaturePresent(PF_ARM_V8_INSTRUCTIONS_AVAILABLE))
        features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAes));
#endif

    return features;
}

bool PalHasCpuFeature(CpuFeature feature) noexcept {
    static const uint64_t s_features = DetectCpuFeatures();
    return (s_features & (1ULL << static_cast<uint32_t>(feature))) != 0;
}

uint64_t PalGetCpuFeatures() noexcept {
    static const uint64_t s_features = DetectCpuFeatures();
    return s_features;
}

}  // namespace chaos::il2cpp::pal
