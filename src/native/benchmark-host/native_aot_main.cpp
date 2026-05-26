#include <chrono>
#include <cstdint>
#include <cstring>

#include <chaos/common.h>
#include <chaos/profile.h>

#ifndef CHAOS_NATIVE_AOT_ENTRY
#define CHAOS_NATIVE_AOT_ENTRY RunNativeAot
#endif

extern "C" int CHAOS_NATIVE_AOT_ENTRY(CHAOS_IL2CPP_INT32 entryIndex);

namespace {

int ParseIterations(int argc, char** argv) {
    int iterations = 1;
    for (int index = 1; index + 1 < argc; index += 2) {
        if (CHAOS_IL2CPP_STRCMP(argv[index], "--iterations") != 0) {
            continue;
        }

        const int parsed = CHAOS_IL2CPP_ATOI(argv[index + 1]);
        if (parsed > 0) {
            iterations = parsed;
        }
    }

    return iterations;
}

CHAOS_IL2CPP_INT32 ParseEntryIndex(int argc, char** argv) {
    CHAOS_IL2CPP_INT32 entry_index = 0;
    for (int index = 1; index + 1 < argc; index += 2) {
        if (CHAOS_IL2CPP_STRCMP(argv[index], "--entry-index") != 0) {
            continue;
        }

        entry_index = static_cast<CHAOS_IL2CPP_INT32>(CHAOS_IL2CPP_ATOI(argv[index + 1]));
    }

    return entry_index;
}

const char* ParseSubjectId(int argc, char** argv) {
    for (int index = 1; index + 1 < argc; index += 2) {
        if (CHAOS_IL2CPP_STRCMP(argv[index], "--subject-id") != 0) {
            continue;
        }
        return argv[index + 1];
    }
    return nullptr;
}

long long NormalizeChecksum(long long checksum) {
    const long long normalized = checksum % 10000LL;
    return normalized < 0 ? -normalized : normalized;
}

}  // namespace

int main(int argc, char** argv) {
    const int iterations = ParseIterations(argc, argv);
    const CHAOS_IL2CPP_INT32 entry_index = ParseEntryIndex(argc, argv);
    const char* subject_id = ParseSubjectId(argc, argv);

    // Warmup: pay one-time runtime initialization cost (GC, type system, vtables, etc.)
    // before starting the timed measurement. This ensures per-call reported time
    // reflects actual execution cost, not one-time init.
    CHAOS_NATIVE_AOT_ENTRY(entry_index);

    // Reset profile accumulators so warmup allocations/initializations are excluded.
    CHAOS_IL2CPP_PROFILE_RESET();

    const auto started = CHAOS_IL2CPP_CHRONO_STEADY_CLOCK::now();
    long long checksum = 0;
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("BenchmarkLoop");
        for (int index = 0; index < iterations; ++index) {
            checksum += static_cast<long long>(CHAOS_NATIVE_AOT_ENTRY(entry_index));
        }
    }
    CHAOS_IL2CPP_PROFILE_DUMP();
    const auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        CHAOS_IL2CPP_CHRONO_STEADY_CLOCK::now() - started);
    const double total_ms = static_cast<double>(elapsed_ns.count()) / 1000000.0;
    // Report total batch time (all iterations), matching managed benchmark convention.
    // The dashboard normalizes across modes by comparing total time for the same
    // iteration count (managed and native both use _perf_harness_iterations = 100).
    const double ops_per_second = total_ms > 0.0 ? (static_cast<double>(iterations) / (total_ms / 1000.0)) : 0.0;

    if (subject_id != nullptr) {
        CHAOS_IL2CPP_LOG_WRITE_RAW_M(
            "{{\"elapsedMilliseconds\":{0:.6f},\"opsPerSecond\":{1:.6f},\"checksum\":{2},\"iterations\":{3},\"subjectId\":\"{4}\"}}\n",
            total_ms,
            ops_per_second,
            NormalizeChecksum(checksum),
            iterations,
            subject_id);
    } else {
        CHAOS_IL2CPP_LOG_WRITE_RAW_M(
            "{{\"elapsedMilliseconds\":{0:.6f},\"opsPerSecond\":{1:.6f},\"checksum\":{2},\"iterations\":{3}}}\n",
            total_ms,
            ops_per_second,
            NormalizeChecksum(checksum),
            iterations);
    }
    CHAOS_IL2CPP_LOG_FLUSH_STDOUT();
    return 0;
}
