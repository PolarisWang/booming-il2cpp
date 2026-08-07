#include <gtest/gtest.h>
#include <engine_bridge.h>

#include <thread>
#include <vector>

// Stubs for AOT runtime symbols normally provided by codegen/entrypoint
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// Stress test: call engine-bridge null-input validation APIs concurrently
// from many threads. Verifies thread safety of null-path handling.
TEST(EngineBridgeStress, ConcurrentNullInputCalls) {
    constexpr int kThreadCount = 8;
    constexpr int kIterationsPerThread = 50;
    std::vector<std::thread> threads;

    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([]() {
            for (int i = 0; i < kIterationsPerThread; ++i) {
                // Retain/release with null runtime
                EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT,
                    engine_retain_object(nullptr, 1u, ENGINE_HANDLE_KIND_WEAK));
                EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT,
                    engine_retain_object(nullptr, 1u, ENGINE_HANDLE_KIND_STRONG));
                EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT,
                    engine_release_object(nullptr, 1u, ENGINE_HANDLE_KIND_WEAK));

                // Transfer with null runtime
                EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT,
                    engine_transfer_ownership(nullptr, 1u, "from", "to"));

                // Callback registration with null runtime
                engine_callback_id_t cid = 0;
                EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT,
                    engine_register_callback(nullptr, "test", nullptr, nullptr, &cid));

                // Dispatch with null runtime
                EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT,
                    engine_dispatch_callback(nullptr, 1u, nullptr, 0u));

                // Lifecycle with null runtime
                EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT,
                    engine_reload(nullptr));
                EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT,
                    engine_scene_attach(nullptr, "test-scene"));

                // Init with null params
                engine_runtime_context_t* rt = nullptr;
                EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT,
                    engine_init(&rt, nullptr));

                // Shutdown null (no-op, should not crash)
                engine_shutdown(nullptr);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }
}
