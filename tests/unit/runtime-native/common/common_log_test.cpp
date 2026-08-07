/// common_log_test — Unit tests for log.h logging system.
///
/// Tests format helpers, timestamp formatting, stdout capture for log macros,
/// and raw write functionality.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <cstring>

// Need to include log.h explicitly to access log_internal helpers
#include <chaos/log.h>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Format helpers — format_simple
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLog, FormatSimple) {
    auto result = log_internal::format_simple("ERROR", "Test", "hello");
    // Format: [timestamp][ERROR][Test] hello
    EXPECT_NE(std::string::npos, result.find("[ERROR]"));
    EXPECT_NE(std::string::npos, result.find("[Test]"));
    EXPECT_NE(std::string::npos, result.find("hello"));
    // Should start with timestamp in brackets
    EXPECT_EQ('[', result[0]);
}

TEST(CommonLog, FormatSimpleEmptyMessage) {
    auto result = log_internal::format_simple("INFO", "Core", "");
    EXPECT_NE(std::string::npos, result.find("[INFO]"));
    EXPECT_NE(std::string::npos, result.find("[Core]"));
}

// ═══════════════════════════════════════════════════════════════════════════
// Format helpers — format_fmt
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLog, FormatFmtBasic) {
    auto result = log_internal::format_fmt("WARN", "GC", "heap at {0}%", 85);
    EXPECT_NE(std::string::npos, result.find("[WARN]"));
    EXPECT_NE(std::string::npos, result.find("[GC]"));
    EXPECT_NE(std::string::npos, result.find("heap at 85%"));
}

TEST(CommonLog, FormatFmtMultipleArgs) {
    auto result = log_internal::format_fmt("DEBUG", "Init",
        "loaded {0} types in {1}ms", 42, 15);
    EXPECT_NE(std::string::npos, result.find("loaded 42 types in 15ms"));
}

TEST(CommonLog, FormatFmtStringArg) {
    auto result = log_internal::format_fmt("ERROR", "Memory",
        "alloc failed at {0}", "0x1234");
    EXPECT_NE(std::string::npos, result.find("alloc failed at 0x1234"));
}

// ═══════════════════════════════════════════════════════════════════════════
// Cached timestamp
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLog, CachedTimestampFormat) {
    const char* ts = log_internal::cached_log_timestamp();
    ASSERT_NE(nullptr, ts);
    // Should look like "2026-05-23T12:34:56"
    EXPECT_EQ(19, std::strlen(ts));
    EXPECT_EQ('-', ts[4]);
    EXPECT_EQ('-', ts[7]);
    EXPECT_EQ('T', ts[10]);
    EXPECT_EQ(':', ts[13]);
    EXPECT_EQ(':', ts[16]);
}

TEST(CommonLog, CachedTimestampStableWithinMs) {
    // Two calls should return the same (cached) pointer
    const char* ts1 = log_internal::cached_log_timestamp();
    const char* ts2 = log_internal::cached_log_timestamp();
    EXPECT_EQ(ts1, ts2);
}

// ═══════════════════════════════════════════════════════════════════════════
// Log write — capture stdout via gtest internal API
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLog, LogWriteEmpty) {
    testing::internal::CaptureStdout();
    log_internal::log_write(std::string(""));
    std::string output = testing::internal::GetCapturedStdout();
    // Should output just a newline
    EXPECT_EQ("\n", output);
}

TEST(CommonLog, LogWriteContent) {
    testing::internal::CaptureStdout();
    log_internal::log_write(std::string("hello world"));
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ("hello world\n", output);
}

// ═══════════════════════════════════════════════════════════════════════════
// Raw write
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLog, WriteRaw) {
    testing::internal::CaptureStdout();
    log_internal::write_raw("raw data");
    std::string output = testing::internal::GetCapturedStdout();
    // write_raw does NOT add a newline
    EXPECT_EQ("raw data", output);
}

TEST(CommonLog, WriteRawFmt) {
    testing::internal::CaptureStdout();
    log_internal::write_raw_fmt("value={0}", 42);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ("value=42", output);
}

// ═══════════════════════════════════════════════════════════════════════════
// Flush stdout
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLog, FlushStdout) {
    // flush_stdout should not crash — call it and verify no side effects
    testing::internal::CaptureStdout();
    log_internal::flush_stdout();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.empty());
}

// ═══════════════════════════════════════════════════════════════════════════
// Log level macros — compile verification in CHECK mode
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLog, MacrosLogLevel) {
    // Verify CHAOS_IL2CPP_LOG_LEVEL is at least DEBUG (3) in CHECK build
    EXPECT_GE(CHAOS_IL2CPP_LOG_LEVEL, 3);
}

TEST(CommonLog, MacroErrorSimple) {
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_ERROR("Test", "error msg");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(std::string::npos, output.find("[ERROR]"));
    EXPECT_NE(std::string::npos, output.find("[Test]"));
    EXPECT_NE(std::string::npos, output.find("error msg"));
}

TEST(CommonLog, MacroErrorFormatted) {
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_ERROR_M("Math", "computed {0} = {1}", "result", 42);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(std::string::npos, output.find("[ERROR]"));
    EXPECT_NE(std::string::npos, output.find("[Math]"));
    EXPECT_NE(std::string::npos, output.find("computed result = 42"));
}

TEST(CommonLog, MacroWarnSimple) {
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_WARN("GC", "heap warning");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(std::string::npos, output.find("[WARN]"));
    EXPECT_NE(std::string::npos, output.find("[GC]"));
    EXPECT_NE(std::string::npos, output.find("heap warning"));
}

TEST(CommonLog, MacroInfoSimple) {
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_INFO("Init", "started");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(std::string::npos, output.find("[INFO]"));
    EXPECT_NE(std::string::npos, output.find("[Init]"));
    EXPECT_NE(std::string::npos, output.find("started"));
}

TEST(CommonLog, MacroDebugSimple) {
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_DEBUG("Trace", "debug msg");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(std::string::npos, output.find("[DEBUG]"));
    EXPECT_NE(std::string::npos, output.find("[Trace]"));
    EXPECT_NE(std::string::npos, output.find("debug msg"));
}

TEST(CommonLog, MacroWriteRaw) {
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_WRITE_RAW("protocol data\n");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ("protocol data\n", output);
}

TEST(CommonLog, MacroWriteRawFmt) {
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_WRITE_RAW_M("benchmark {0}={1}", "score", 99);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ("benchmark score=99", output);
}

TEST(CommonLog, MacroFlush) {
    // Just verify the macro compiles and doesn't crash
    CHAOS_IL2CPP_LOG_FLUSH();
    SUCCEED();
}

TEST(CommonLog, MacroFlushStdout) {
    // Just verify the macro compiles and doesn't crash
    CHAOS_IL2CPP_LOG_FLUSH_STDOUT();
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// Concurrent log write safety (basic smoke test)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLog, ConcurrentWrite) {
    // Two threads writing concurrently should not crash
    auto worker = [](int id) {
        for (int i = 0; i < 10; i++) {
            CHAOS_IL2CPP_LOG_INFO_M("Concurrent", "thread {0} iteration {1}", id, i);
        }
    };
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    t1.join();
    t2.join();
    SUCCEED();
}
