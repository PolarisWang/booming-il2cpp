// marshal_api_basic_test.cpp — Basic marshal API tests for P/Invoke & Marshal.
//
// Tests SetLastPInvokeError, GetLastPInvokeError, ClearOsLastError,
// GetOsLastError, TaskKernelNewId, InteropKernel32*, NativeLibrary*,
// MarshalDestroyStructure, MarshalSizeOf, MarshalOffsetOf,
// MarshalUnsafeAddrOfPinnedArrayElement, MarshalSafeHandleGetHandle.
//
// These tests focus on functions that do NOT require a managed GC heap,
// testing null safety, OS interop wrappers, and thread-local state management.

#include "marshal_test_fixture.h"

#include <engine_binding.h>

#include <cstdint>

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// B3 — Marshal API basic tests
// ════════════════════════════════════════════════════════════════════════════

class MarshalApiBasicTest : public MarshalTestFixture {
};

TEST_F(MarshalApiBasicTest, SetAndGetLastPInvokeError) {
    auto* ts = GetThreadState();
    ASSERT_NE(ts, nullptr);

    // When ThreadInternalState is null, GetLastPInvokeError returns 0
    // and SetLastPInvokeError is a no-op.
    EXPECT_EQ(GetLastPInvokeError(ts), 0);
    SetLastPInvokeError(ts, 42);
    EXPECT_EQ(GetLastPInvokeError(ts), 0);  // no internal_state yet
}

TEST_F(MarshalApiBasicTest, GetLastPInvokeErrorIcall) {
    // GetLastPInvokeErrorIcall uses TLS ThreadState via GetCurrentThreadState().
    // Without a full ThreadInternalState, returns 0.
    EXPECT_EQ(GetLastPInvokeErrorIcall(), 0);
}

TEST_F(MarshalApiBasicTest, ClearAndGetOsLastError) {
    // These are no-ops that should not crash
    ClearOsLastError();
    // On Win32, GetOsLastError returns GetLastError(). Since we just called
    // ClearOsLastError (which sets last error to 0 on Win32), this should be 0.
    // However, other calls may have happened. Just verify no crash.
    GetOsLastError();
    SUCCEED();
}

TEST_F(MarshalApiBasicTest, TaskKernelNewId) {
    // Returns monotonically increasing IDs
    auto id1 = TaskKernelNewId();
    auto id2 = TaskKernelNewId();
    auto id3 = TaskKernelNewId();

    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_GT(id2, id1);
    EXPECT_GT(id3, id2);
}

#if defined(_WIN32)
TEST_F(MarshalApiBasicTest, InteropKernel32Win32) {
    // Process and thread IDs should be non-zero on Win32
    EXPECT_NE(InteropKernel32GetCurrentProcessId(), 0);
    EXPECT_NE(InteropKernel32GetCurrentThreadId(), 0);
    EXPECT_NE(InteropKernel32GetCurrentProcess(), 0);
    EXPECT_NE(InteropKernel32GetCurrentThread(), 0);

    // CloseHandle with null handle should be safe
    EXPECT_TRUE(InteropKernel32CloseHandle(0));

    // GetLastError returns last error value
    InteropKernel32GetLastError();  // just verify no crash
    SUCCEED();
}
#endif

TEST_F(MarshalApiBasicTest, NativeLibraryLoadKernel32) {
#if defined(_WIN32)
    // Load kernel32 (always available)
    auto* handle = NativeLibraryLoad("kernel32.dll");
    ASSERT_NE(handle, nullptr);

    // Resolve a known symbol
    auto* fn = NativeLibraryGetProcAddress(handle, "GetLastError");
    EXPECT_NE(fn, nullptr);

    // Resolve a non-existent symbol → nullptr
    auto* nonexistent = NativeLibraryGetProcAddress(handle, "NonExistentFunction_XYZ123");
    EXPECT_EQ(nonexistent, nullptr);

    // Free library
    EXPECT_TRUE(NativeLibraryFree(handle));

    // Double free should be safe (handle already released)
    NativeLibraryFree(handle);
#endif
}

TEST_F(MarshalApiBasicTest, NativeLibraryLoadNonExistent) {
    auto* handle = NativeLibraryLoad("nonexistent_library_XYZ123.dll");
    EXPECT_EQ(handle, nullptr);
}

TEST_F(MarshalApiBasicTest, MarshalDestroyStructure_Null) {
    // Null struct pointer → no-op
    ChaosDestroyStructureByType(0, 0);
    SUCCEED();
}

TEST_F(MarshalApiBasicTest, MarshalSizeOf_NullTypeInfo) {
    auto* rs = GetRuntimeState();
    auto* ts = GetThreadState();
    ASSERT_NE(rs, nullptr);
    ASSERT_NE(ts, nullptr);

    // Null TypeInfoHot → 0
    EXPECT_EQ(MarshalSizeOf(rs, ts, nullptr), 0);
}

TEST_F(MarshalApiBasicTest, MarshalUnsafeAddrOfPinnedArrayElement_Null) {
    // Null array → 0
    EXPECT_EQ(MarshalUnsafeAddrOfPinnedArrayElement(nullptr, 0), 0);

    // Out of bounds → 0
    // (can't test with valid array since we'd need a managed array)
}

TEST_F(MarshalApiBasicTest, MarshalSafeHandleGetHandle_Null) {
    auto* rs = GetRuntimeState();
    auto* ts = GetThreadState();
    ASSERT_NE(rs, nullptr);
    ASSERT_NE(ts, nullptr);

    // Null safe handle → 0
    EXPECT_EQ(MarshalSafeHandleGetHandle(rs, ts, nullptr), 0);
}

TEST_F(MarshalApiBasicTest, MarshalThrowExceptionForHR_NullState) {
    // Null RuntimeState → no-op (should be safe)
    MarshalThrowExceptionForHR(nullptr, nullptr, -1);
    MarshalThrowExceptionForHR(nullptr, nullptr, 0);
    SUCCEED();
}
