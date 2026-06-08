// pal_cpu.h — CPU feature detection (CPUID / HWCAP / sysctl)
#pragma once

#include <cstdint>

namespace chaos::il2cpp::pal {

enum class CpuFeature : uint32_t {
    kSse2    = 0,
    kSse41   = 1,
    kAvx     = 2,
    kAvx2    = 3,
    kBmi1    = 4,
    kBmi2    = 5,
    kPopcnt  = 6,
    kAes     = 7,
    kNeon    = 8,
    kSve     = 9,
    kAvx512F = 10,
    kAvx512Vbmi = 11,
};

/// Returns true if the current CPU supports the given feature.
bool PalHasCpuFeature(CpuFeature feature) noexcept;

/// Returns a bitmask of all supported features (1ULL << CpuFeature).
uint64_t PalGetCpuFeatures() noexcept;

}  // namespace chaos::il2cpp::pal
