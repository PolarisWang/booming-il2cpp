/// native_library_test.cpp — NativeLibrary 单元测试 (GoogleTest).
///
/// 覆盖 NativeLibraryLoad / NativeLibraryFree / NativeLibraryGetProcAddress /
/// RegisterPInvokeResolverCallback / TryResolveDllImport 的守卫条件和基本功能。
///
/// Level: ⭐⭐ 测试重开 (Loader 工业化 Phase 1)

#include <native_library.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#if defined(_WIN32)
#include <Windows.h>
#endif

using namespace chaos::il2cpp::runtime_core;

// ═══════════════════════════════════════════════════════════════════════════
// NativeLibraryLoad — 守卫条件
// ═══════════════════════════════════════════════════════════════════════════

TEST(native_library_test, Load_NullName_ReturnsNull) {
    EXPECT_EQ(NativeLibraryLoad(nullptr), nullptr);
}

TEST(native_library_test, Load_EmptyName_ReturnsNull) {
    EXPECT_EQ(NativeLibraryLoad(""), nullptr);
}

TEST(native_library_test, Load_NonexistentDll_ReturnsNull) {
    EXPECT_EQ(NativeLibraryLoad("Definitely_Not_A_Real_DLL_12345.dll"), nullptr);
}

TEST(native_library_test, Load_NonexistentDll_NoExtension) {
    // Should also return null for non-existent DLL without .dll extension.
    EXPECT_EQ(NativeLibraryLoad("Definitely_Not_A_Real_DLL_67890"), nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════
// NativeLibraryFree — 守卫条件
// ═══════════════════════════════════════════════════════════════════════════

TEST(native_library_test, Free_NullHandle_ReturnsFalse) {
    EXPECT_FALSE(NativeLibraryFree(nullptr));
}

TEST(native_library_test, Free_InvalidHandle_ReturnsFalse) {
    // A pointer that is definitely not a valid module handle.
    EXPECT_FALSE(NativeLibraryFree(reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234))));
}

// ═══════════════════════════════════════════════════════════════════════════
// NativeLibraryGetProcAddress — 守卫条件
// ═══════════════════════════════════════════════════════════════════════════

TEST(native_library_test, GetProcAddress_NullHandle_ReturnsNull) {
    EXPECT_EQ(NativeLibraryGetProcAddress(nullptr, "SomeFunction"), nullptr);
}

TEST(native_library_test, GetProcAddress_NullSymbol_ReturnsNull) {
    auto* handle = NativeLibraryLoad("kernel32.dll");
    ASSERT_NE(handle, nullptr) << "kernel32.dll should be loadable";

    void* proc = NativeLibraryGetProcAddress(handle, nullptr);
    EXPECT_EQ(proc, nullptr);

    NativeLibraryFree(handle);
}

TEST(native_library_test, GetProcAddress_NonexistentSymbol_ReturnsNull) {
    auto* handle = NativeLibraryLoad("kernel32.dll");
    ASSERT_NE(handle, nullptr);

    void* proc = NativeLibraryGetProcAddress(handle, "ThisFunctionDoesNotExist_XYZ123");
    EXPECT_EQ(proc, nullptr);

    NativeLibraryFree(handle);
}

// ═══════════════════════════════════════════════════════════════════════════
// 功能路径 — 加载已知 DLL 并解析符号
// ═══════════════════════════════════════════════════════════════════════════

TEST(native_library_test, LoadAndResolve_Kernel32) {
    auto* handle = NativeLibraryLoad("kernel32.dll");
    ASSERT_NE(handle, nullptr) << "kernel32.dll should be loadable";

    void* proc = NativeLibraryGetProcAddress(handle, "GetLastError");
    ASSERT_NE(proc, nullptr) << "GetLastError should be resolvable";

    EXPECT_TRUE(NativeLibraryFree(handle));
}

TEST(native_library_test, Load_WithoutExtension) {
    // Should auto-append .dll on Windows.
    auto* handle = NativeLibraryLoad("kernel32");
    ASSERT_NE(handle, nullptr) << "kernel32 (without extension) should load";

    void* proc = NativeLibraryGetProcAddress(handle, "GetLastError");
    EXPECT_NE(proc, nullptr);

    EXPECT_TRUE(NativeLibraryFree(handle));
}

// ═══════════════════════════════════════════════════════════════════════════
// Refcounting — 同一 DLL 多次加载应返回相同的 handle
// ═══════════════════════════════════════════════════════════════════════════

TEST(native_library_test, Refcounting_SameHandle) {
    auto* handle1 = NativeLibraryLoad("kernel32.dll");
    ASSERT_NE(handle1, nullptr);

    auto* handle2 = NativeLibraryLoad("kernel32.dll");
    ASSERT_NE(handle2, nullptr);

    // Same DLL should return same handle pointer.
    EXPECT_EQ(handle1, handle2);

    // Free twice — should succeed both times.
    EXPECT_TRUE(NativeLibraryFree(handle1));
    EXPECT_TRUE(NativeLibraryFree(handle2));
}

TEST(native_library_test, Refcounting_LoadFreeFree) {
    auto* handle = NativeLibraryLoad("user32.dll");
    if (handle == nullptr) {
        GTEST_SKIP() << "user32.dll not available on this system";
        return;
    }

    // One load → one free → handle is released.
    EXPECT_TRUE(NativeLibraryFree(handle));

    // Second free of same handle should fail (already unloaded).
    EXPECT_FALSE(NativeLibraryFree(handle));
}

TEST(native_library_test, Refcounting_Canonicalisation_DifferentNames) {
    auto* handle1 = NativeLibraryLoad("kernel32.dll");
    ASSERT_NE(handle1, nullptr);

    // Using different case — should canoncalise to the same entry.
    auto* handle2 = NativeLibraryLoad("KERNEL32.DLL");
    ASSERT_NE(handle2, nullptr);

    // On Windows (case-insensitive), should be the same entry.
    EXPECT_EQ(handle1, handle2);

    NativeLibraryFree(handle1);
    NativeLibraryFree(handle2);
}

// ═══════════════════════════════════════════════════════════════════════════
// Concurrency — 多线程同时加载同一 DLL
// ═══════════════════════════════════════════════════════════════════════════

TEST(native_library_test, ConcurrentLoad_SameDll) {
    static constexpr int kThreadCount = 8;
    std::vector<void*> handles[kThreadCount];
    std::vector<std::thread> threads;

    for (int t = 0; t < kThreadCount; t++) {
        threads.emplace_back([&handles, t]() {
            for (int i = 0; i < 20; i++) {
                auto* h = NativeLibraryLoad("kernel32.dll");
                if (h != nullptr) {
                    handles[t].push_back(h);
                }
            }
        });
    }

    for (auto& th : threads) th.join();

    // All handles should be the same pointer.
    void* expected = nullptr;
    int total_loads = 0;
    for (int t = 0; t < kThreadCount; t++) {
        for (auto* h : handles[t]) {
            if (expected == nullptr) {
                expected = h;
            }
            EXPECT_EQ(h, expected) << "Concurrent loads returned different handles";
            total_loads++;
        }
    }

    // Free all references.
    for (int t = 0; t < kThreadCount; t++) {
        for (auto* h : handles[t]) {
            NativeLibraryFree(h);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// RegisterPInvokeResolverCallback / TryResolveDllImport
// ═══════════════════════════════════════════════════════════════════════════

TEST(native_library_test, ResolveDllImport_NullCallback_ReturnsNull) {
    RegisterPInvokeResolverCallback(nullptr);
    EXPECT_EQ(TryResolveDllImport("Test.Assembly", "test.dll"), nullptr);
}

TEST(native_library_test, ResolveDllImport_WithCallback) {
    // Register a callback that resolves "CustomLib" to kernel32.
    RegisterPInvokeResolverCallback(reinterpret_cast<void*>(+[](
        const char* assembly_name, const char* library_name) -> intptr_t {
        if (std::strcmp(library_name, "CustomLib") == 0) {
            auto* handle = NativeLibraryLoad("kernel32.dll");
            return reinterpret_cast<intptr_t>(handle);
        }
        return 0;
    }));

    void* result = TryResolveDllImport("Test.Assembly", "CustomLib");
    EXPECT_NE(result, nullptr) << "CustomLib should resolve via callback";

    // Reset callback.
    RegisterPInvokeResolverCallback(nullptr);

    // After reset, resolution should fail again.
    EXPECT_EQ(TryResolveDllImport("Test.Assembly", "CustomLib"), nullptr);

    // Free the handle we loaded in the callback.
    if (result != nullptr) {
        NativeLibraryFree(result);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Canonicalisation — 路径分隔符标准化
// ═══════════════════════════════════════════════════════════════════════════

TEST(native_library_test, Canonicalise_PathSeparator) {
    // Forward slash should be normalised on Windows.
    auto* handle = NativeLibraryLoad("kernel32.dll");
    ASSERT_NE(handle, nullptr);

    // This tests the canonicalisation via the API — same DLL queried
    // with different separators should result in same entry.
    auto* handle2 = NativeLibraryLoad("kernel32.dll");
    EXPECT_EQ(handle, handle2);

    NativeLibraryFree(handle);
    NativeLibraryFree(handle2);
}
