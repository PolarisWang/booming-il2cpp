#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef CHAOS_NATIVE_AOT_ENTRY
#define CHAOS_NATIVE_AOT_ENTRY RunNativeAot
#endif

extern "C" int CHAOS_NATIVE_AOT_ENTRY(std::int32_t entryIndex);

namespace {

int ParseIterations(int argc, char** argv) {
    int iterations = 1;
    for (int index = 1; index + 1 < argc; index += 2) {
        if (std::strcmp(argv[index], "--iterations") != 0) {
            continue;
        }

        const int parsed = std::atoi(argv[index + 1]);
        if (parsed > 0) {
            iterations = parsed;
        }
    }

    return iterations;
}

std::int32_t ParseEntryIndex(int argc, char** argv) {
    std::int32_t entry_index = 0;
    for (int index = 1; index + 1 < argc; index += 2) {
        if (std::strcmp(argv[index], "--entry-index") != 0) {
            continue;
        }

        entry_index = static_cast<std::int32_t>(std::atoi(argv[index + 1]));
    }

    return entry_index;
}

long long NormalizeChecksum(long long checksum) {
    const long long normalized = checksum % 10000LL;
    return normalized < 0 ? -normalized : normalized;
}

}  // namespace

int main(int argc, char** argv) {
    const int iterations = ParseIterations(argc, argv);
    const std::int32_t entry_index = ParseEntryIndex(argc, argv);
    const auto started = std::chrono::steady_clock::now();
    long long checksum = 0;
    for (int index = 0; index < iterations; ++index) {
        checksum += static_cast<long long>(CHAOS_NATIVE_AOT_ENTRY(entry_index));
    }
    const auto elapsed = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - started);
    const double seconds = elapsed.count() / 1000.0;
    const double ops_per_second = seconds > 0.0 ? (static_cast<double>(iterations) / seconds) : 0.0;

    std::printf(
        "{\"elapsedMilliseconds\":%.6f,\"opsPerSecond\":%.6f,\"checksum\":%lld,\"iterations\":%d}\n",
        elapsed.count(),
        ops_per_second,
        NormalizeChecksum(checksum),
        iterations);
    return 0;
}
