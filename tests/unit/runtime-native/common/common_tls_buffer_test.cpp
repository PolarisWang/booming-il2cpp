/// common_tls_buffer_test — Unit tests for TlsBuffer.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/tls_buffer.h>
#include <cstring>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Basic access
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTlsBuffer, BasicAccess) {
    TlsBuffer<char, 64> buf;
    EXPECT_EQ(64u, buf.size());
    auto* data = buf.data();
    ASSERT_NE(nullptr, data);
    // Should be writable
    data[0] = 'A';
    EXPECT_EQ('A', data[0]);
}

// ═══════════════════════════════════════════════════════════════════════════
// Buffer isolation — each thread gets its own TLS view
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTlsBuffer, ThreadIsolation) {
    TlsBuffer<char, 64> buf;
    buf.data()[0] = 'M';  // main thread sets 'M'

    std::thread t([&]() {
        buf.data()[0] = 'T';  // worker thread sets 'T'
    });
    t.join();

    // Main thread should still see 'M' (TLS isolation)
    EXPECT_EQ('M', buf.data()[0]);
}

// ═══════════════════════════════════════════════════════════════════════════
// Multiple buffers
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTlsBuffer, MultipleBuffers) {
    TlsBuffer<int, 16> buf1;
    TlsBuffer<int, 32> buf2;
    EXPECT_EQ(16u, buf1.size());
    EXPECT_EQ(32u, buf2.size());

    buf1.data()[0] = 42;
    buf2.data()[0] = 99;
    EXPECT_EQ(42, buf1.data()[0]);
    EXPECT_EQ(99, buf2.data()[0]);
}
