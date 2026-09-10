// com_factory_test.cpp — CcwFactoryFn registry unit tests.
//
// Tests RegisterCcwFactory, UnregisterCcwFactory, FindCcwFactory,
// and CreateCcwForStableId for thread-safe runtime CCW factory management.

#include "marshal_test_fixture.h"

#include <com_factory.h>

#include <atomic>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// F1 — Factory registry tests
// ════════════════════════════════════════════════════════════════════════════

class ComFactoryTest : public MarshalTestFixture {
protected:
    // A factory that returns a distinctive value based on stable_id for verification.
    static CHAOS_IL2CPP_INTPTR TestFactoryOne(void* /*obj*/, void* /*rs*/) noexcept {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0001);
    }

    static CHAOS_IL2CPP_INTPTR TestFactoryTwo(void* /*obj*/, void* /*rs*/) noexcept {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0002);
    }
};

TEST_F(ComFactoryTest, RegisterAndFind) {
    constexpr CHAOS_IL2CPP_UINT64 kTestId = 0x123456789ABCDEF0ull;

    RegisterCcwFactory(kTestId, TestFactoryOne);
    auto factory = FindCcwFactory(kTestId);
    EXPECT_NE(factory, nullptr);
    EXPECT_EQ(factory(nullptr, nullptr), static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0001));

    // Cleanup.
    UnregisterCcwFactory(kTestId);
}

TEST_F(ComFactoryTest, RegisterAndFindDifferentIds) {
    constexpr CHAOS_IL2CPP_UINT64 kIdA = 0xAAAAAAAAAAAAAAAAull;
    constexpr CHAOS_IL2CPP_UINT64 kIdB = 0xBBBBBBBBBBBBBBBBull;

    RegisterCcwFactory(kIdA, TestFactoryOne);
    RegisterCcwFactory(kIdB, TestFactoryTwo);

    auto fa = FindCcwFactory(kIdA);
    auto fb = FindCcwFactory(kIdB);
    EXPECT_NE(fa, nullptr);
    EXPECT_NE(fb, nullptr);
    EXPECT_NE(fa, fb);

    EXPECT_EQ(fa(nullptr, nullptr), static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0001));
    EXPECT_EQ(fb(nullptr, nullptr), static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0002));

    UnregisterCcwFactory(kIdA);
    UnregisterCcwFactory(kIdB);
}

TEST_F(ComFactoryTest, FindUnregisteredReturnsNull) {
    constexpr CHAOS_IL2CPP_UINT64 kUnknownId = 0xDEADBEEFDEADBEEFull;
    EXPECT_EQ(FindCcwFactory(kUnknownId), nullptr);
}

TEST_F(ComFactoryTest, RegisterNullFactoryNoOp) {
    constexpr CHAOS_IL2CPP_UINT64 kTestId = 0x1111111111111111ull;
    // Registering nullptr should not add an entry.
    RegisterCcwFactory(kTestId, nullptr);
    EXPECT_EQ(FindCcwFactory(kTestId), nullptr);
}

TEST_F(ComFactoryTest, UnregisterNoOpOnUnknown) {
    constexpr CHAOS_IL2CPP_UINT64 kUnknownId = 0x2222222222222222ull;
    // Should not crash.
    UnregisterCcwFactory(kUnknownId);
    SUCCEED();
}

TEST_F(ComFactoryTest, RegisterReplaces) {
    constexpr CHAOS_IL2CPP_UINT64 kTestId = 0x3333333333333333ull;

    RegisterCcwFactory(kTestId, TestFactoryOne);
    EXPECT_EQ(FindCcwFactory(kTestId)(nullptr, nullptr),
              static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0001));

    // Replace with TestFactoryTwo.
    RegisterCcwFactory(kTestId, TestFactoryTwo);
    EXPECT_EQ(FindCcwFactory(kTestId)(nullptr, nullptr),
              static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0002));

    UnregisterCcwFactory(kTestId);
}

TEST_F(ComFactoryTest, RegisterUnregisterReRegister) {
    constexpr CHAOS_IL2CPP_UINT64 kTestId = 0x4444444444444444ull;

    RegisterCcwFactory(kTestId, TestFactoryOne);
    UnregisterCcwFactory(kTestId);
    EXPECT_EQ(FindCcwFactory(kTestId), nullptr);

    RegisterCcwFactory(kTestId, TestFactoryTwo);
    EXPECT_EQ(FindCcwFactory(kTestId)(nullptr, nullptr),
              static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0002));

    UnregisterCcwFactory(kTestId);
}

TEST_F(ComFactoryTest, CreateCcwForStableIdNoFactory) {
    constexpr CHAOS_IL2CPP_UINT64 kUnknownId = 0x5555555555555555ull;
    EXPECT_EQ(CreateCcwForStableId(kUnknownId, nullptr, nullptr), 0);
}

TEST_F(ComFactoryTest, CreateCcwForStableIdWithFactory) {
    constexpr CHAOS_IL2CPP_UINT64 kTestId = 0x6666666666666666ull;

    RegisterCcwFactory(kTestId, TestFactoryOne);
    auto result = CreateCcwForStableId(kTestId, nullptr, nullptr);
    EXPECT_EQ(result, static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0001));

    UnregisterCcwFactory(kTestId);
}

TEST_F(ComFactoryTest, ConcurrentRegisterFindUnregister) {
    constexpr int kThreadCount = 8;
    constexpr int kOpsPerThread = 1000;
    std::atomic<int> errors{0};

    // Register a base factory.
    constexpr CHAOS_IL2CPP_UINT64 kBaseId = 0x7777777777777777ull;
    RegisterCcwFactory(kBaseId, TestFactoryOne);
    EXPECT_NE(FindCcwFactory(kBaseId), nullptr);

    // Concurrent register/unregister of many distinct IDs.
    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([t, &errors]() {
            CHAOS_IL2CPP_UINT64 id = 0x8000000000000000ull + t;
            RegisterCcwFactory(id, TestFactoryOne);
            auto f = FindCcwFactory(id);
            if (f == nullptr) {
                errors.fetch_add(1, std::memory_order_relaxed);
            }
            UnregisterCcwFactory(id);
        });
    }

    for (auto& th : threads) th.join();

    // Base factory should still be registered after concurrent activity.
    EXPECT_NE(FindCcwFactory(kBaseId), nullptr);
    EXPECT_EQ(errors.load(), 0);

    UnregisterCcwFactory(kBaseId);
}
