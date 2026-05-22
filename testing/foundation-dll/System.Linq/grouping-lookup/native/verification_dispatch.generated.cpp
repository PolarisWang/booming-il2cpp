// verification_dispatch.generated.cpp — auto-generated
// Assembly: GroupingLookupSubjects, Methods: 24
#include <cstdint>
#include <chrono>
#include <chaos/native_types.h>

extern "C" const int kAotMethodCount;
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32);

extern "C" const int kSubjectEntryCount;
extern "C" const int kSubjectEntryIndices[];
// (defined in native-aot.generated.cpp)


// ── RunFactAll: run every method via RunNativeAot, return failure count ──
extern "C" CHAOS_IL2CPP_INT32 RunFactAll() {
    int failed_count = 0;
    for (int i = 0; i < kAotMethodCount; i++) {
        try {
            RunNativeAot(i);
        } catch (...) {
            ++failed_count;
        }
    }
    return failed_count;
}

// ── RunBenchmark: timing loop via RunNativeAot ───────────────────
extern "C" double RunBenchmark(int entry_index, int iterations) {
    if (entry_index < 0 || entry_index >= kAotMethodCount)
        return -1.0;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
        RunNativeAot(entry_index);
    }
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// ── RunHotpatchAll: all-methods loop via RunNativeAot (post-patch) ──
extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() {
    int failed_count = 0;
    for (int i = 0; i < kAotMethodCount; i++) {
        try {
            RunNativeAot(i);
        } catch (...) {
            ++failed_count;
        }
    }
    return failed_count;
}

// ── RunHotpatchBenchmark: timing loop via RunNativeAot (post-patch) ──
extern "C" double RunHotpatchBenchmark(int entry_index, int iterations) {
    if (entry_index < 0 || entry_index >= kAotMethodCount)
        return -1.0;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
        RunNativeAot(entry_index);
    }
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}
