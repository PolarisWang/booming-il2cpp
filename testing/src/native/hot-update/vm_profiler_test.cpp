/// vm_profiler_test.cpp — VM Profiler unit tests
///
/// Exercises: VmProfileScope creation / destruction, basic counting,
/// DumpProfilerToFile, ResetProfiler, and edge cases.

#include "register_vm_profiler.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vector>

// ── Tests ────────────────────────────────────────────────────────────────

TEST(VmProfiler, CreateScope) {
    using chaos::il2cpp::interpreter::VmProfileScope;

    // Creating and destroying a scope should not crash.
    {
        VmProfileScope scope(0x06000001);
        // scope destroyed at end of block
    }
}

TEST(VmProfiler, ScopeWithZeroKey) {
    using chaos::il2cpp::interpreter::VmProfileScope;

    // Zero method key should be handled safely.
    {
        VmProfileScope scope(0);
    }
}

TEST(VmProfiler, MultipleScopes) {
    using chaos::il2cpp::interpreter::VmProfileScope;

    // Multiple scopes for the same method should not crash.
    {
        VmProfileScope s1(0x06000001);
        VmProfileScope s2(0x06000001);
        VmProfileScope s3(0x06000001);
    }
}

TEST(VmProfiler, ResetProfiler) {
    using chaos::il2cpp::interpreter::ResetProfiler;
    using chaos::il2cpp::interpreter::VmProfileScope;

    {
        VmProfileScope s1(0x06000001);
        VmProfileScope s2(0x06000002);
    }

    // Reset should not crash.
    ResetProfiler();

    // After reset, creating new scopes should work.
    {
        VmProfileScope s3(0x06000003);
    }
}

TEST(VmProfiler, DumpProfilerToFileStdout) {
    using chaos::il2cpp::interpreter::DumpProfilerToFile;
    using chaos::il2cpp::interpreter::ResetProfiler;
    using chaos::il2cpp::interpreter::VmProfileScope;

    ResetProfiler();
    {
        VmProfileScope s1(0x06000001);
        VmProfileScope s2(0x06000002);
    }

    // Dump to stderr (null path).
    DumpProfilerToFile(nullptr);

    // Dump to empty string (stderr fallback).
    DumpProfilerToFile("");
}

TEST(VmProfiler, DumpProfilerToFile) {
    using chaos::il2cpp::interpreter::DumpProfilerToFile;
    using chaos::il2cpp::interpreter::ResetProfiler;
    using chaos::il2cpp::interpreter::VmProfileScope;

    ResetProfiler();
    {
        VmProfileScope s1(0x06000001);
        VmProfileScope s2(0x06000002);
    }

    // Dump to a temp file.
    const char* test_path = "vm_profiler_test_dump.txt";
    DumpProfilerToFile(test_path);

    // Verify the file was created and has content.
    FILE* fp = std::fopen(test_path, "r");
    ASSERT_NE(fp, nullptr);

    char buf[256];
    bool has_header = false;
    bool has_data = false;
    while (std::fgets(buf, sizeof(buf), fp) != nullptr) {
        if (std::strstr(buf, "VM Profiler") != nullptr) has_header = true;
        if (std::strstr(buf, "0x000000000600000") != nullptr) has_data = true;
    }
    std::fclose(fp);

    EXPECT_TRUE(has_header) << "Dump file should contain profiler header";
#if CHAOS_IL2CPP_VM_PROFILER_ENABLED
    EXPECT_TRUE(has_data) << "Dump file should contain method data";
#endif

    // Cleanup.
    std::remove(test_path);
}

TEST(VmProfiler, ConcurrentScopes) {
    using chaos::il2cpp::interpreter::ResetProfiler;
    using chaos::il2cpp::interpreter::VmProfileScope;

    ResetProfiler();

    // Create scopes from multiple threads.
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([t]() {
            for (int i = 0; i < 100; ++i) {
                VmProfileScope scope(
                    static_cast<uintptr_t>(0x06000010 + t * 10 + i));
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    // Should not crash. Reset after concurrent access.
    chaos::il2cpp::interpreter::ResetProfiler();
}

TEST(VmProfiler, MaxMethods) {
    using chaos::il2cpp::interpreter::DumpProfilerToFile;
    using chaos::il2cpp::interpreter::ResetProfiler;
    using chaos::il2cpp::interpreter::VmProfileScope;

    ResetProfiler();

    // Create scopes for many unique methods (up to profiler capacity).
    constexpr uint32_t kMethodCount = 500;
    for (uint32_t i = 0; i < kMethodCount; ++i) {
        VmProfileScope scope(static_cast<uintptr_t>(0x10000000 + i));
    }

    // Dump to verify no crash.
    DumpProfilerToFile("vm_profiler_mass_dump.txt");
    std::remove("vm_profiler_mass_dump.txt");

    ResetProfiler();
}

TEST(VmProfiler, ResetAfterDump) {
    using chaos::il2cpp::interpreter::DumpProfilerToFile;
    using chaos::il2cpp::interpreter::ResetProfiler;
    using chaos::il2cpp::interpreter::VmProfileScope;

    ResetProfiler();
    {
        VmProfileScope s1(0x06000001);
    }

    // Dump then reset.
    DumpProfilerToFile(nullptr);
    ResetProfiler();

    // Create another scope after reset.
    {
        VmProfileScope s2(0x06000002);
    }

    ResetProfiler();
}
