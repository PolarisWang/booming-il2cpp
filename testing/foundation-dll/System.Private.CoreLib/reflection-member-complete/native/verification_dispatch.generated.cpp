// verification_dispatch.generated.cpp — auto-generated
// Assembly: ReflectionMemberCompleteSubjects, Methods: 22, Mode: JIT

#include <cstdint>
#include <chrono>
#include <chaos/native_types.h>
#include <runtime_core.h>
#include <chaos/eh.h>
#include <chaos/hotpatch_dispatch.h>

using chaos::il2cpp::runtime_core::ChaosDispatchMethod;
using chaos::il2cpp::runtime_core::ChaosDispatchMethodAllModules;

extern "C" const int kAotMethodCount;
extern "C" const int kSubjectEntryCount;
extern "C" const int kSubjectSlotMap[];
// kSubjectEntryCount/kSubjectSlotMap defined in native-aot.generated.cpp (DispatchEntryCode template)

// kDefaultArgThunks defined in native-aot.generated.cpp
extern "C" void (*kDefaultArgThunks[])() noexcept;

extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept;
extern "C" int32_t GetHotpatchEntryCount() noexcept;

// Subject entry count: 22 (from manifest)


// ── RunFactAll: dispatch ALL methods across ALL registered modules ────
// Uses ChaosDispatchMethodAllModules from hotpatch_dispatch.h which iterates
// every module in HotpatchNameRegistry.  Returns total failure count.
extern "C" CHAOS_IL2CPP_INT32 RunFactAll() {
    return ChaosDispatchMethodAllModules(nullptr);
}

// ── RunBenchmark: timing loop via ChaosDispatchMethod ───────────────────
extern "C" double RunBenchmark(int entry_index, int iterations) {
    if (entry_index < 0 || entry_index >= kAotMethodCount)
        return -1.0;
    auto* entries = GetHotpatchEntries();
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
        ChaosDispatchMethod(entries, kAotMethodCount, entry_index, nullptr);
    }
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// ── RunHotpatchAll: dispatch ALL methods across ALL modules (post-patch) ──
extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() {
    return ChaosDispatchMethodAllModules(nullptr);
}

// ── RunHotpatchBenchmark: timing loop via ChaosDispatchMethod (post-patch) ──
extern "C" double RunHotpatchBenchmark(int entry_index, int iterations) {
    if (entry_index < 0 || entry_index >= kAotMethodCount)
        return -1.0;
    auto* entries = GetHotpatchEntries();
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
        ChaosDispatchMethod(entries, kAotMethodCount, entry_index, nullptr);
    }
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}
