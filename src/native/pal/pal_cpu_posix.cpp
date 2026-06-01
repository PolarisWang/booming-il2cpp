// pal_cpu_posix.cpp — POSIX CPU feature detection (CPUID / getauxval / sysctl)

#include <chaos/pal/pal_cpu.h>

#include <cstdint>

#if defined(__x86_64__)
#include <cpuid.h>
#elif defined(__aarch64__)
#if defined(__linux__)
#include <sys/auxv.h>
#include <asm/hwcap.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#include <cstring>
#endif
#endif

namespace chaos::il2cpp::pal {

static uint64_t DetectCpuFeatures() noexcept {
    uint64_t features = 0;

#if defined(__x86_64__)
    unsigned int eax, ebx, ecx, edx;

    // Leaf 1
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    if (edx & (1 << 26)) features |= (1ULL << static_cast<uint32_t>(CpuFeature::kSse2));
    if (ecx & (1 << 20)) features |= (1ULL << static_cast<uint32_t>(CpuFeature::kSse41));
    if (ecx & (1 << 23)) features |= (1ULL << static_cast<uint32_t>(CpuFeature::kPopcnt));
    if (ecx & (1 << 25)) features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAes));

    // AVX requires OSXSAVE + XCR0 YMM save
    if (ecx & (1 << 27)) {  // OSXSAVE
        uint32_t xcr0_low, xcr0_high;
        __asm__ volatile("xgetbv" : "=a"(xcr0_low), "=d"(xcr0_high) : "c"(0));
        uint64_t xcr0 = (static_cast<uint64_t>(xcr0_high) << 32) | xcr0_low;

        if ((xcr0 & 6) == 6) {  // XMM + YMM
            features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAvx));

            // Leaf 7 (subleaf 0)
            __get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx);
            if (ebx & (1 << 5))  features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAvx2));
            if (ebx & (1 << 3))  features |= (1ULL << static_cast<uint32_t>(CpuFeature::kBmi1));
            if (ebx & (1 << 8))  features |= (1ULL << static_cast<uint32_t>(CpuFeature::kBmi2));
        }

        if ((xcr0 & 0xE0) == 0xE0) {  // OPMASK + ZMM upper + ZMM lower
            features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAvx512F));
            if (ecx & (1 << 1)) {
                features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAvx512Vbmi));
            }
        }
    }

#elif defined(__aarch64__)
#if defined(__linux__)
    unsigned long hwcap = ::getauxval(AT_HWCAP);
#if defined(HWCAP_NEON)
    if (hwcap & HWCAP_NEON)
#endif
        features |= (1ULL << static_cast<uint32_t>(CpuFeature::kNeon));
    if (hwcap & HWCAP_AES)     features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAes));

    unsigned long hwcap2 = ::getauxval(AT_HWCAP2);
#if defined(HWCAP2_SVE)
    if (hwcap2 & HWCAP2_SVE)   features |= (1ULL << static_cast<uint32_t>(CpuFeature::kSve));
#endif
#elif defined(__APPLE__)
    // macOS/iOS: sysctlbyname interface for CPU features
    auto has_feature = [](const char* name) -> bool {
        int value = 0;
        size_t size = sizeof(value);
        return ::sysctlbyname(name, &value, &size, nullptr, 0) == 0 && value != 0;
    };
    if (has_feature("hw.optional.arm.FEAT_NEON") ||
        has_feature("hw.optional.neon")) {
        features |= (1ULL << static_cast<uint32_t>(CpuFeature::kNeon));
    }
    if (has_feature("hw.optional.arm.FEAT_AES") ||
        has_feature("hw.optional.arm.FEAT_AES")) {
        features |= (1ULL << static_cast<uint32_t>(CpuFeature::kAes));
    }
    if (has_feature("hw.optional.arm.FEAT_SVE")) {
        features |= (1ULL << static_cast<uint32_t>(CpuFeature::kSve));
    }
#endif
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
