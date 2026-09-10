/// gc_hotupdate_interaction_test.cpp — GC + hot-update interaction test
///
/// Verifies that method replacement table operations (Register/Resolve/Revert)
/// are safe in the presence of simulated GC activity.  The GC itself is not
/// required to be fully initialized — this test validates that the replacement
/// table's atomic operations and memory ordering are compatible with concurrent
/// access patterns that mirror GC safepoint behavior.

#include "method_replacement.h"

#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>
#include <cstdlib>

namespace {

// AOT symbol stubs (required by chaos_interpreter linkage).
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// Test replacement target.
void* ReplacementFunc1() { return reinterpret_cast<void*>(0x1); }
void* ReplacementFunc2() { return reinterpret_cast<void*>(0x2); }

}  // anonymous namespace

TEST(GcHotupdateInteraction, RegisterDuringConcurrentAccess) {
    using chaos::il2cpp::method_replacement::Register;
    using chaos::il2cpp::method_replacement::Resolve;
    using chaos::il2cpp::method_replacement::RevertAll;

    RevertAll();

    // Simulate concurrent GC-like reads while registering replacements.
    std::atomic<bool> stop{false};

    // Reader thread: repeatedly calls Resolve (like a GC safepoint scan would).
    std::thread reader([&stop]() {
        while (!stop.load(std::memory_order_acquire)) {
            // Resolve various tokens — most will fail, which is fine.
            for (uint32_t token = 0x06000001; token <= 0x06000010; ++token) {
                chaos::il2cpp::method_replacement::Resolve(token);
            }
        }
    });

    // Writer thread: registers replacements.
    for (uint32_t token = 0x06000001; token <= 0x06000010; ++token) {
        EXPECT_TRUE(Register(token, reinterpret_cast<void*>(&ReplacementFunc1)));
    }

    // Stop the reader and join.
    stop.store(true, std::memory_order_release);
    reader.join();

    // Verify replacements are still valid.
    for (uint32_t token = 0x06000001; token <= 0x06000010; ++token) {
        void* result = chaos::il2cpp::method_replacement::Resolve(token);
        EXPECT_NE(result, nullptr);
    }

    RevertAll();
}

TEST(GcHotupdateInteraction, RevertDuringConcurrentAccess) {
    using chaos::il2cpp::method_replacement::Register;
    using chaos::il2cpp::method_replacement::Resolve;
    using chaos::il2cpp::method_replacement::Revert;
    using chaos::il2cpp::method_replacement::RevertAll;

    RevertAll();

    // Register replacements first.
    constexpr uint32_t kTokenCount = 20;
    for (uint32_t token = 0x06000001; token <= 0x06000001 + kTokenCount; ++token) {
        Register(token, reinterpret_cast<void*>(&ReplacementFunc1));
    }

    std::atomic<bool> stop{false};

    // Reader thread.
    std::thread reader([&stop]() {
        while (!stop.load(std::memory_order_acquire)) {
            for (uint32_t token = 0x06000001; token <= 0x06000001 + kTokenCount; ++token) {
                chaos::il2cpp::method_replacement::Resolve(token);
            }
        }
    });

    // Revert half the replacements while reader is active.
    for (uint32_t token = 0x06000001; token <= 0x06000001 + kTokenCount / 2; ++token) {
        EXPECT_TRUE(Revert(token));
    }

    stop.store(true, std::memory_order_release);
    reader.join();

    // Verify: first half should be reverted, second half still active.
    for (uint32_t token = 0x06000001; token <= 0x06000001 + kTokenCount / 2; ++token) {
        EXPECT_EQ(chaos::il2cpp::method_replacement::Resolve(token), nullptr);
    }
    for (uint32_t token = 0x06000001 + kTokenCount / 2 + 1;
         token <= 0x06000001 + kTokenCount; ++token) {
        EXPECT_NE(chaos::il2cpp::method_replacement::Resolve(token), nullptr);
    }

    RevertAll();
}

TEST(GcHotupdateInteraction, RevertAllDuringConcurrentAccess) {
    using chaos::il2cpp::method_replacement::Register;
    using chaos::il2cpp::method_replacement::Resolve;
    using chaos::il2cpp::method_replacement::RevertAll;
    using chaos::il2cpp::method_replacement::ActiveCount;

    RevertAll();

    // Register several replacements.
    for (uint32_t token = 0x06000001; token <= 0x06000010; ++token) {
        Register(token, reinterpret_cast<void*>(&ReplacementFunc1));
    }
    EXPECT_GT(ActiveCount(), 0);

    std::atomic<bool> stop{false};

    // Reader thread.
    std::thread reader([&stop]() {
        while (!stop.load(std::memory_order_acquire)) {
            for (uint32_t token = 0x06000001; token <= 0x06000010; ++token) {
                chaos::il2cpp::method_replacement::Resolve(token);
            }
        }
    });

    // Call RevertAll while reader is active.
    RevertAll();

    stop.store(true, std::memory_order_release);
    reader.join();

    // All replacements should be gone.
    EXPECT_EQ(ActiveCount(), 0);
    for (uint32_t token = 0x06000001; token <= 0x06000010; ++token) {
        EXPECT_EQ(chaos::il2cpp::method_replacement::Resolve(token), nullptr);
    }
}
