// com_benchmark_test.cpp — COM performance benchmark tests.
//
// Benchmarks RCW creation, CCW QI/AddRef/Release, and factory lookup
// to establish baseline performance metrics.
//
// LABEL: benchmark (excluded from default ctest run).

#include "marshal_test_fixture.h"

#include <com_rcw.h>
#include <com_ccw.h>
#include <com_factory.h>
#include <com_platform.h>

#include <chrono>
#include <cstdio>

using namespace chaos::il2cpp::com_rcw;
using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// B1 — COM benchmark tests
// ════════════════════════════════════════════════════════════════════════════

class ComBenchmarkTest : public MarshalTestFixture {
protected:
    static constexpr uint64_t kBenchIters = 100000;

    // Test CCW matching the committed ComCcw layout (x64).
using InterfaceEntry = chaos::il2cpp::com_ccw::ComCcwInterfaceEntry;

    struct ComCcwOld {
        void* vtable;
        std::atomic<CHAOS_IL2CPP_UINT32> refcount;
        CHAOS_IL2CPP_UINT64 gc_handle;
        void* runtime_state;
        CHAOS_IL2CPP_SIZE interface_count;
        CHAOS_IL2CPP_SIZE interface_capacity;
        void* outer_unknown;
        bool  is_aggregated;
        InterfaceEntry* interfaces;
        InterfaceEntry inline_interfaces[4];
        const void* typelib_data;
        void* cp_container;
    };

    // Helper: initialize a stack-local CCW for benchmarking.
    void InitCcw(ComCcwOld& ccw) noexcept {
        std::memset(&ccw, 0, sizeof(ccw));
        ccw.refcount = 1;  // prevent free on Release
        ccw.interface_count = 1;
        ccw.interface_capacity = 4;
        ccw.interfaces = &ccw.inline_interfaces[0];
        ccw.inline_interfaces[0].guid = kZeroGuid;
        ccw.inline_interfaces[0].ccw_ptr = &ccw;
    }

    static const CHAOS_IL2CPP_UINT8 kZeroGuid[16];
};

const CHAOS_IL2CPP_UINT8 ComBenchmarkTest::kZeroGuid[16] = {0};

TEST_F(ComBenchmarkTest, RcwFindOrCreate) {
#ifndef _WIN32
    void* fake_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(0xBEEF));
    auto* rcw = FindOrCreateRcw(fake_ptr);
    ASSERT_NE(rcw, nullptr);

    auto start = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 0; i < kBenchIters; ++i) {
        volatile auto* r = FindOrCreateRcw(
            reinterpret_cast<void*>(static_cast<uintptr_t>(0xBEEF + i)));
        if (r) { ReleaseRcw(r); }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::printf("[BENCHMARK] RcwFindOrCreate      %5" PRIu64 " iter  %8" PRIu64 " ns  %8" PRIu64 " ns/iter\n",
                kBenchIters, ns, ns / kBenchIters);

    ReleaseRcw(rcw);
#else
    GTEST_SKIP() << "RCW benchmark skipped on Win32 (needs real COM pointers)";
#endif
}

TEST_F(ComBenchmarkTest, CcwQueryInterface) {
    ComCcwOld ccw{};
    InitCcw(ccw);
    void* s = &ccw;

    auto start = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 0; i < kBenchIters; ++i) {
        void* ppv = nullptr;
        chaos::il2cpp::com_ccw::CcwQueryInterface(s, kZeroGuid, &ppv);
        if (ppv) chaos::il2cpp::com_ccw::CcwRelease(ppv);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::printf("[BENCHMARK] CcwQueryInterface    %5" PRIu64 " iter  %8" PRIu64 " ns  %8" PRIu64 " ns/iter\n",
                kBenchIters, ns, ns / kBenchIters);
}

TEST_F(ComBenchmarkTest, CcwAddRefRelease) {
    ComCcwOld ccw{};
    InitCcw(ccw);
    void* s = &ccw;

    auto start = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 0; i < kBenchIters; ++i) {
        chaos::il2cpp::com_ccw::CcwAddRef(s);
        chaos::il2cpp::com_ccw::CcwRelease(s);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::printf("[BENCHMARK] CcwAddRefRelease     %5" PRIu64 " iter  %8" PRIu64 " ns  %8" PRIu64 " ns/iter\n",
                kBenchIters, ns, ns / kBenchIters);
}

TEST_F(ComBenchmarkTest, FactoryLookup) {
    constexpr CHAOS_IL2CPP_UINT64 kBenchId = 0xCAFEBABECAFEBABEull;
    RegisterCcwFactory(kBenchId,
        [](void*, void*) -> CHAOS_IL2CPP_INTPTR {
            return static_cast<CHAOS_IL2CPP_INTPTR>(0xBEEF);
        });

    auto start = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 0; i < kBenchIters; ++i) {
        volatile auto f = FindCcwFactory(kBenchId);
        (void)f;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::printf("[BENCHMARK] FactoryLookup        %5" PRIu64 " iter  %8" PRIu64 " ns  %8" PRIu64 " ns/iter\n",
                kBenchIters, ns, ns / kBenchIters);

    UnregisterCcwFactory(kBenchId);
}

TEST_F(ComBenchmarkTest, PlatformNullGuard) {
    // Benchmark null-safety path of platform abstraction.
    auto start = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 0; i < kBenchIters; ++i) {
        com_platform::PlatformAddRef(nullptr);
        com_platform::PlatformRelease(nullptr);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::printf("[BENCHMARK] PlatformNullGuard    %5" PRIu64 " iter  %8" PRIu64 " ns  %8" PRIu64 " ns/iter\n",
                kBenchIters, ns, ns / kBenchIters);
}
