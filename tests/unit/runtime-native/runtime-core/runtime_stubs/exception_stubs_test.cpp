// exception_stubs_test.cpp — Unit tests for exception helpers (t1a-4)
//
// Tests all 4 exception helper functions by simulating managed Exception
// objects with zeroed memory blocks at expected field offsets.
//
// Links against CHAOS_COMMON_TEST_LIBS for ground lib resolution.
// exception_stubs.cpp is compiled directly (depends only on generated_code_compat.h).

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstring>

// ═══════════════════════════════════════════════════════════════════════════
// Simulated managed Exception layout (matching exception_stubs.cpp)
// ═══════════════════════════════════════════════════════════════════════════

// Field offsets matching exception_stubs.cpp:
//   kExcInnerException = 24
//   kExcHResult       = 40
//   kRweWrapped       = 64

// A simulated managed Exception: header + fields
struct alignas(8) SimException {
    unsigned char padding[24];        // 0-23: ThinLockableHeader(16) + _message(8)
    CHAOS_IL2CPP_INTPTR inner_exc;   // 24-31: _innerException
    unsigned char gap1[8];            // 32-39: _stackTrace
    CHAOS_IL2CPP_INT32 hresult;      // 40-43: _HResult
    unsigned char gap2[20];           // 44-63: padding (_data, _xptrs, _xcode)
    CHAOS_IL2CPP_INTPTR wrapped;     // 64-71: _wrappedException (RuntimeWrappedException)
};
static_assert(sizeof(SimException) >= 72, "SimException too small");

// ═══════════════════════════════════════════════════════════════════════════
// Forward declarations for the stub functions
// ═══════════════════════════════════════════════════════════════════════════

extern "C" {
CHAOS_IL2CPP_INTPTR ChaosExceptionGetBaseException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INTPTR ChaosExceptionGetInnerException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INT32  ChaosExceptionGetHresult(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR exc) noexcept;
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosExceptionGetHresult tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Exception_GetHresult, ReturnsHResult) {
    SimException exc{};
    exc.hresult = 0x80004002;  // COR_E_INVALIDCAST

    auto hr = ChaosExceptionGetHresult(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
    EXPECT_EQ(hr, 0x80004002);
}

TEST(Exception_GetHresult, ZeroHResult) {
    SimException exc{};

    auto hr = ChaosExceptionGetHresult(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
    EXPECT_EQ(hr, 0);
}

TEST(Exception_GetHresult, NullPointer) {
    EXPECT_EQ(ChaosExceptionGetHresult(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosExceptionGetInnerException tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Exception_GetInnerException, NoInner) {
    SimException exc{};
    exc.inner_exc = 0;

    auto inner = ChaosExceptionGetInnerException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
    EXPECT_EQ(inner, 0);
}

TEST(Exception_GetInnerException, HasInner) {
    SimException inner{};
    SimException outer{};
    outer.inner_exc = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&inner);

    auto result = ChaosExceptionGetInnerException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&outer));
    EXPECT_EQ(result, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&inner));
}

TEST(Exception_GetInnerException, NullPointer) {
    EXPECT_EQ(ChaosExceptionGetInnerException(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosExceptionGetBaseException tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Exception_GetBaseException, SingleException) {
    SimException exc{};
    exc.inner_exc = 0;

    auto base = ChaosExceptionGetBaseException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
    EXPECT_EQ(base, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
}

TEST(Exception_GetBaseException, TwoLevelChain) {
    SimException inner{};
    SimException outer{};
    outer.inner_exc = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&inner);
    inner.inner_exc = 0;

    auto base = ChaosExceptionGetBaseException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&outer));
    EXPECT_EQ(base, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&inner));
}

TEST(Exception_GetBaseException, ThreeLevelChain) {
    SimException root{};
    SimException middle{};
    SimException outer{};
    root.inner_exc = 0;
    middle.inner_exc = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&root);
    outer.inner_exc = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&middle);

    auto base = ChaosExceptionGetBaseException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&outer));
    EXPECT_EQ(base, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&root));
}

TEST(Exception_GetBaseException, NullPointer) {
    EXPECT_EQ(ChaosExceptionGetBaseException(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosRuntimewrappedGetWrappedException tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Exception_GetWrappedException, ReturnsWrapped) {
    SimException exc{};
    // Set wrapped field
    CHAOS_IL2CPP_INTPTR wrapped_val = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR*>(nullptr)) + 0xCAFE;
    exc.wrapped = wrapped_val;

    auto result = ChaosRuntimewrappedGetWrappedException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
    EXPECT_EQ(result, wrapped_val);
}

TEST(Exception_GetWrappedException, NoWrapped) {
    SimException exc{};
    exc.wrapped = 0;

    auto result = ChaosRuntimewrappedGetWrappedException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
    EXPECT_EQ(result, 0);
}

TEST(Exception_GetWrappedException, NullPointer) {
    EXPECT_EQ(ChaosRuntimewrappedGetWrappedException(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Integration: full cycle tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Exception_Integration, FullExceptionChain) {
    // Build: outer -> middle -> root (with HRESULT at each level)
    SimException root{};
    root.inner_exc = 0;
    root.hresult = 0x80004001;  // COR_E_NOTIMPLEMENTED

    SimException middle{};
    middle.inner_exc = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&root);
    middle.hresult = 0x80004002;  // COR_E_INVALIDCAST

    SimException outer{};
    outer.inner_exc = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&middle);
    outer.hresult = 0x80004003;  // E_POINTER

    // Base exception should be the root
    auto base = ChaosExceptionGetBaseException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&outer));
    EXPECT_EQ(base, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&root));

    // Inner of outer should be middle
    auto inner1 = ChaosExceptionGetInnerException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&outer));
    EXPECT_EQ(inner1, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&middle));

    // Inner of middle should be root
    auto inner2 = ChaosExceptionGetInnerException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&middle));
    EXPECT_EQ(inner2, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&root));

    // Inner of root should be 0
    auto inner3 = ChaosExceptionGetInnerException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&root));
    EXPECT_EQ(inner3, 0);

    // HRESULTs should be preserved
    EXPECT_EQ(ChaosExceptionGetHresult(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&outer)), 0x80004003);
    EXPECT_EQ(ChaosExceptionGetHresult(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&middle)), 0x80004002);
    EXPECT_EQ(ChaosExceptionGetHresult(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&root)), 0x80004001);
}

TEST(Exception_Integration, RuntimeWrappedException) {
    // Simulate a RuntimeWrappedException with no inner exception but with wrapped
    SimException inner{};
    SimException rwe{};
    rwe.inner_exc = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&inner);
    rwe.hresult = 0x80004003;
    rwe.wrapped = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&inner);  // wraps the inner

    // GetBaseException should walk to innermost
    auto base = ChaosExceptionGetBaseException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&rwe));
    EXPECT_EQ(base, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&inner));

    // GetWrappedException should return the wrapped object
    auto wrapped = ChaosRuntimewrappedGetWrappedException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&rwe));
    EXPECT_EQ(wrapped, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&inner));
}

// ═══════════════════════════════════════════════════════════════════════════
// Edge-case tests (t1b-6)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Exception_Edge, DeepChain) {
    // 100-level deep chain — verifies no stack overflow
    static constexpr int kDepth = 100;
    SimException excs[kDepth]{};
    for (int i = 0; i < kDepth - 1; ++i) {
        excs[i].inner_exc = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&excs[i + 1]);
    }
    excs[kDepth - 1].inner_exc = 0;

    auto base = ChaosExceptionGetBaseException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&excs[0]));
    EXPECT_EQ(base, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&excs[kDepth - 1]));
}

TEST(Exception_Edge, SingleLevelChainBaseReturnsSelf) {
    SimException exc{};
    exc.inner_exc = 0;
    auto base = ChaosExceptionGetBaseException(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
    EXPECT_EQ(base, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
}

TEST(Exception_Edge, WrappedExceptionNullHandle) {
    SimException exc{};
    exc.wrapped = 0;
    auto wrapped = ChaosRuntimewrappedGetWrappedException(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&exc));
    EXPECT_EQ(wrapped, 0);
}
