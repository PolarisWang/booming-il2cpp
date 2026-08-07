/// common_trace_test — Unit tests for trace.h ring buffer and macros.
///
/// Tests TraceRingBuffer write/flush/empty, cached_iso8601 timestamp,
/// and CHAOS_IL2CPP_LOG_TRACE macro output.
///
/// NOTE: The global g_trace_ring is shared across macro tests. Each macro
/// test flushes to a unique file and scans ALL lines to find its own entry.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/trace.h>
#include <cstdio>
#include <cstring>
#include <string>

using namespace chaos::il2cpp::common;

// Helper: flush trace ring to a file, read all lines, check if any line
// contains the expected substring. The file path should be unique per test.
static bool trace_flush_contains(const char* path, const char* expected) {
    CHAOS_IL2CPP_LOG_TRACE_FLUSH(path);
    auto* f = std::fopen(path, "r");
    if (!f) return false;
    char line[512];
    bool found = false;
    while (std::fgets(line, sizeof(line), f)) {
        if (std::strstr(line, expected)) { found = true; break; }
    }
    std::fclose(f);
    std::remove(path);
    return found;
}

// ═══════════════════════════════════════════════════════════════════════════
// TraceRingBuffer basic operations
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTrace, RingEmptyInitially) {
    detail::TraceRingBuffer buf;
    EXPECT_TRUE(buf.empty());
}

TEST(CommonTrace, RingNotEmptyAfterWrite) {
    detail::TraceRingBuffer buf;
    buf.write("hello");
    EXPECT_FALSE(buf.empty());
}

TEST(CommonTrace, RingFlushToFile) {
    detail::TraceRingBuffer buf;
    buf.write("line1");
    buf.write("line2");

    const char* tmp_path = "test_trace_flush.tmp";
    buf.flush(tmp_path);

    // Verify file content
    auto* f = std::fopen(tmp_path, "r");
    ASSERT_NE(nullptr, f);
    char line1[256], line2[256];
    ASSERT_NE(nullptr, std::fgets(line1, sizeof(line1), f));
    ASSERT_NE(nullptr, std::fgets(line2, sizeof(line2), f));
    std::fclose(f);
    std::remove(tmp_path);

    // fgets includes newline
    line1[std::strcspn(line1, "\n")] = '\0';
    line2[std::strcspn(line2, "\n")] = '\0';
    EXPECT_STREQ("line1", line1);
    EXPECT_STREQ("line2", line2);
}

TEST(CommonTrace, RingFlushEmpty) {
    // Flush on empty buffer should not crash or create a file
    detail::TraceRingBuffer buf;
    EXPECT_TRUE(buf.empty());
    buf.flush("test_empty_flush.tmp");
    // After flush, the file should NOT exist (no entries to flush)
    auto* f = std::fopen("test_empty_flush.tmp", "r");
    if (f) {
        std::fclose(f);
        std::remove("test_empty_flush.tmp");
        GTEST_FAIL() << "flush() should not create file for empty buffer";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// TraceRingBuffer wrap-around
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTrace, RingWrapAround) {
    detail::TraceRingBuffer buf;
    // Write more than capacity entries
    for (int i = 0; i < detail::kTraceRingCapacity + 10; i++) {
        buf.write("overflow");
    }
    EXPECT_FALSE(buf.empty());

    const char* tmp_path = "test_trace_wrap.tmp";
    buf.flush(tmp_path);

    // Should have written exactly kTraceRingCapacity entries
    auto* f = std::fopen(tmp_path, "r");
    ASSERT_NE(nullptr, f);
    int count = 0;
    char line[256];
    while (std::fgets(line, sizeof(line), f)) { count++; }
    std::fclose(f);
    std::remove(tmp_path);
    EXPECT_EQ(detail::kTraceRingCapacity, count);
}

// ═══════════════════════════════════════════════════════════════════════════
// cached_iso8601
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTrace, CachedIso8601Format) {
    const char* ts = detail::cached_iso8601();
    ASSERT_NE(nullptr, ts);
    EXPECT_EQ(19, std::strlen(ts));
    EXPECT_EQ('-', ts[4]);
    EXPECT_EQ('-', ts[7]);
    EXPECT_EQ('T', ts[10]);
    EXPECT_EQ(':', ts[13]);
    EXPECT_EQ(':', ts[16]);
}

TEST(CommonTrace, CachedIso8601Stable) {
    const char* ts1 = detail::cached_iso8601();
    const char* ts2 = detail::cached_iso8601();
    EXPECT_EQ(ts1, ts2);
}

// ═══════════════════════════════════════════════════════════════════════════
// CHAOS_IL2CPP_LOG_TRACE macro (CHECK build — trace enabled)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTrace, MacroTrace) {
    // Write a trace entry then flush to verify
    CHAOS_IL2CPP_LOG_TRACE("test", "op1", "value={0}", 42);
    EXPECT_TRUE(trace_flush_contains("test_trace_macro.tmp", "\"s\":\"test\""));
    EXPECT_TRUE(trace_flush_contains("test_trace_macro2.tmp", "\"o\":\"op1\""));
    EXPECT_TRUE(trace_flush_contains("test_trace_macro3.tmp", "value=42"));
    EXPECT_TRUE(trace_flush_contains("test_trace_macro4.tmp", "\"l\":\"cpp\""));
}

TEST(CommonTrace, MacroTraceNoExtra) {
    // Trace without extra fields — should produce valid JSON-like entry
    CHAOS_IL2CPP_LOG_TRACE("simple", "no_extra", "");
    EXPECT_TRUE(trace_flush_contains("test_trace_simple1.tmp", "\"s\":\"simple\""));
    EXPECT_TRUE(trace_flush_contains("test_trace_simple2.tmp", "\"o\":\"no_extra\""));
}

TEST(CommonTrace, MacroTraceMultiple) {
    // Multiple trace entries should each be flushable
    CHAOS_IL2CPP_LOG_TRACE("multi", "a", "i={0}", 1);
    CHAOS_IL2CPP_LOG_TRACE("multi", "b", "i={0}", 2);
    EXPECT_TRUE(trace_flush_contains("test_trace_multi1.tmp", "\"o\":\"a\""));
    EXPECT_TRUE(trace_flush_contains("test_trace_multi2.tmp", "\"o\":\"b\""));
}

// ═══════════════════════════════════════════════════════════════════════════
// g_trace_id default state
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTrace, TraceIdDefault) {
    // Before CHAOS_IL2CPP_LOG_TRACE_INIT, trace ID should be empty
    EXPECT_EQ('\0', detail::g_trace_id[0]);
}

// ═══════════════════════════════════════════════════════════════════════════
// CHAOS_IL2CPP_LOG_TRACE_INIT (environment dependent)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTrace, TraceInitNoEnv) {
    // Without CHAOS_TRACE_PATH/ID env vars, init should be safe no-op
    CHAOS_IL2CPP_LOG_TRACE_INIT();
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// Parallel trace write (wrap-around under concurrency)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTrace, ConcurrentWrite) {
    auto worker = []() {
        for (int i = 0; i < 100; i++) {
            CHAOS_IL2CPP_LOG_TRACE("stress", "write", "i={0}", i);
        }
    };
    std::thread t1(worker);
    std::thread t2(worker);
    t1.join();
    t2.join();

    // Flush should not crash
    const char* tmp_path = "test_trace_concurrent.tmp";
    CHAOS_IL2CPP_LOG_TRACE_FLUSH(tmp_path);
    std::remove(tmp_path);
    SUCCEED();
}
