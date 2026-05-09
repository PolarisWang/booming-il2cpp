// Auto-generated runtime entry point for chaos-il2cpp AOT output.
// Supports three modes:
//   (no args)          - Fact mode: run all methods, print Passed N/M
//   N                  - Legacy single-entry mode (backward compat)
//   --benchmark N [I]  - Benchmark mode: run method N for I iterations, print JSON timing
//   --hotupdate        - HotUpdate mode: run patch lifecycle, print JSON results
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <chrono>
#include "codegen_bridge.h"
#include "runtime_abi.h"

extern "C" std::int32_t RunNativeAot(std::int32_t);

enum class RunMode { Fact, Benchmark, HotUpdate };

int main(int argc, char** argv) {
    auto* bridge = chaos_codegen_get_bridge_v0();
    if (!bridge) return -1;
    bridge->bootstrap_runtime();

    RunMode mode = RunMode::Fact;
    int entry_index = 0;
    int iterations = 10000;

    // Simple argv parsing (self-contained, no getopt dependency)
    if (argc >= 3 && std::strcmp(argv[1], "--benchmark") == 0) {
        mode = RunMode::Benchmark;
        entry_index = std::atoi(argv[2]);
        if (argc >= 4) iterations = std::atoi(argv[3]);
    } else if (argc >= 2 && std::strcmp(argv[1], "--hotupdate") == 0) {
        mode = RunMode::HotUpdate;
    } else if (argc >= 2) {
        entry_index = std::atoi(argv[1]);  // backward compat: numeric entry index
    }

    switch (mode) {
    case RunMode::Fact: {
        int result = RunNativeAot(entry_index);
        printf("Passed: %d/%d\n", result == 0 ? 1 : 0, 1);
        return result;
    }
    case RunMode::Benchmark: {
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < iterations; i++) {
            RunNativeAot(entry_index);
        }
        auto end = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
        double ops_per_sec = iterations / (elapsed_ms / 1000.0);
        printf("{\"elapsedMilliseconds\":%.3f,\"opsPerSecond\":%.1f,\"iterations\":%d,\"methodIndex\":%d}\n",
               elapsed_ms, ops_per_sec, iterations, entry_index);
        std::fflush(stdout);
        return 0;
    }
    case RunMode::HotUpdate: {
        // HotUpdate: RunNativeAot with the entry class's Run dispatcher.
        // Patch lifecycle is managed externally (C++ bootstrap phase).
        // Run all methods and collect results.
        int failures = 0;
        int total = 0;
        // For simplicity, run entry_index=0 which triggers full suite
        // in the generated Program.Main(). HotUpdate runner knows its own
        // method table and will output JSON from C# side.
        int result = RunNativeAot(0);
        printf("{\"passedMethods\":%d,\"failedMethods\":%d,\"totalMethods\":%d}\n",
               result == 0 ? 1 : 0, result, 1);
        std::fflush(stdout);
        return result;
    }
    }
}