#include <gtest/gtest.h>
#include <chaos/common.h>
#include <bootstrap.h>

#include <thread>
#include <vector>

// Stubs for AOT runtime symbols normally provided by codegen/entrypoint
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

using namespace chaos::il2cpp::bootstrap;

// Stress test: call bootstrap APIs concurrently from many threads.
// Verifies thread safety of pre-init state reads.
TEST(BootstrapStress, ConcurrentPreInitCalls) {
    constexpr int kThreadCount = 8;
    constexpr int kIterationsPerThread = 100;
    std::vector<std::thread> threads;

    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([t]() {
            for (int i = 0; i < kIterationsPerThread; ++i) {
                auto* state = PeekBootstrapState();
                EXPECT_NE(nullptr, state);
                EXPECT_FALSE(state->is_registered);
                EXPECT_FALSE(state->is_bootstrapped);

                auto* bridge = GetCodegenBridgeV0();
                EXPECT_NE(nullptr, bridge);
                EXPECT_GE(bridge->struct_size, sizeof(CodegenBridgeV0));

                auto* mptr = FindMethodPointerByToken(0);
                EXPECT_EQ(nullptr, mptr);

                auto* iptr = FindInvokerPointer(0);
                EXPECT_EQ(nullptr, iptr);

                auto* entry = FindRegisteredTypeCapabilityEntry(0);
                EXPECT_EQ(nullptr, entry);

                auto domain_id = GetAotDomainId();
                EXPECT_EQ(0u, domain_id);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }
}
