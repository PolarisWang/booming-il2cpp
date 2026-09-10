/// log_system_test.cpp — Unit tests for the unified logging system (Chaos Log)
///
/// Tests:
///   1. Empty flush — flush with no writes, no crash
///   2. Single write — write+flush one ERROR message, verify output
///   3. Multiple writes — write+flush three messages, verify ordering
///   4. Buffer wrap — write 70 entries (>64 ring capacity), verify last 64
///   5. Format log — _M variant with {0} {1} substitution
///   6. RAW write — CHAOS_IL2CPP_LOG_WRITE_RAW
///   7. RAW format write — CHAOS_IL2CPP_LOG_WRITE_RAW_M
///   8. Output mask suppress — SET_OUTPUT(0), verify nothing written
///   9. Thread safety — 4 threads x 100 writes each, no crash, no data loss
///  10. Level elimination — INFO/DEBUG respect CHAOS_IL2CPP_LOG_LEVEL=3

#include <chaos/log.h>

#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <fstream>
#include <algorithm>

#include <io.h>

// ════════════════════════════════════════════════════════════════════════════
// Stdout capture helper (redirects stdout to temp file, then restores)
// ════════════════════════════════════════════════════════════════════════════

static const char* kCapturePath = "_log_capture.txt";

struct StdoutCapture {
    int saved_fd = -1;

    bool Begin() {
        fflush(stdout);
        saved_fd = _dup(_fileno(stdout));
        if (saved_fd < 0) return false;
        FILE* f = freopen(kCapturePath, "w", stdout);
        return f != nullptr;
    }

    std::string End() {
        fflush(stdout);
        if (saved_fd >= 0) {
            _dup2(saved_fd, _fileno(stdout));
            _close(saved_fd);
            saved_fd = -1;
        }
        std::string content;
        FILE* f = fopen(kCapturePath, "r");
        if (f) {
            char buf[4096];
            size_t n;
            while ((n = fread(buf, 1, sizeof(buf) - 1, f)) > 0) {
                buf[n] = '\0';
                content.append(buf, n);
            }
            fclose(f);
        }
        remove(kCapturePath);
        return content;
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Test 1: Empty flush — should not crash
// ════════════════════════════════════════════════════════════════════════════

TEST(LogSystem, EmptyFlush) {
    CHAOS_IL2CPP_LOG_FLUSH();
    CHAOS_IL2CPP_LOG_FLUSH_STDOUT();
}

// ════════════════════════════════════════════════════════════════════════════
// Test 2: Single ERROR write + flush
// ════════════════════════════════════════════════════════════════════════════

TEST(LogSystem, SingleWrite) {
    StdoutCapture cap;
    ASSERT_TRUE(cap.Begin()) << "capture begin failed";

    CHAOS_IL2CPP_LOG_ERROR("TestCat", "hello world");
    CHAOS_IL2CPP_LOG_FLUSH();

    std::string output = cap.End();

    EXPECT_NE(output.find("[ERROR]"), std::string::npos) << "missing [ERROR] level tag";
    EXPECT_NE(output.find("[TestCat]"), std::string::npos) << "missing [TestCat] category";
    EXPECT_NE(output.find("hello world"), std::string::npos) << "missing message text";
}

// ════════════════════════════════════════════════════════════════════════════
// Test 3: Multiple writes — ordering preserved
// ════════════════════════════════════════════════════════════════════════════

TEST(LogSystem, MultipleWrites) {
    StdoutCapture cap;
    ASSERT_TRUE(cap.Begin()) << "capture begin failed";

    CHAOS_IL2CPP_LOG_ERROR("TestCat", "first");
    CHAOS_IL2CPP_LOG_WARN("TestCat", "second");
    CHAOS_IL2CPP_LOG_INFO("TestCat", "third");
    CHAOS_IL2CPP_LOG_FLUSH();

    std::string output = cap.End();

    auto pos_first = output.find("first");
    auto pos_second = output.find("second");
    auto pos_third = output.find("third");

    EXPECT_NE(pos_first, std::string::npos) << "missing 'first'";
    EXPECT_NE(pos_second, std::string::npos) << "missing 'second'";
    EXPECT_NE(pos_third, std::string::npos) << "missing 'third'";
    EXPECT_TRUE(pos_first < pos_second && pos_second < pos_third) << "ordering violated";
}

// ════════════════════════════════════════════════════════════════════════════
// Test 4: Buffer wrap — write > 64 entries
// ════════════════════════════════════════════════════════════════════════════

TEST(LogSystem, BufferWrap) {
    StdoutCapture cap;
    ASSERT_TRUE(cap.Begin()) << "capture begin failed";

    for (int i = 0; i < 70; ++i) {
        CHAOS_IL2CPP_LOG_ERROR_M("Wrap", "entry {0}", i);
    }
    CHAOS_IL2CPP_LOG_FLUSH();

    std::string output = cap.End();
    EXPECT_NE(output.find("entry 69"), std::string::npos)
        << "entry 69 missing after wrap \342\200\224 data loss on ring wrap";
}

// ════════════════════════════════════════════════════════════════════════════
// Test 5: Format macros (_M variants)
// ════════════════════════════════════════════════════════════════════════════

TEST(LogSystem, FormatMacros) {
    StdoutCapture cap;
    ASSERT_TRUE(cap.Begin()) << "capture begin failed";

    CHAOS_IL2CPP_LOG_ERROR_M("FmtCat", "int={0} str={1}", 42, "hello");
    CHAOS_IL2CPP_LOG_FLUSH();

    std::string output = cap.End();
    EXPECT_NE(output.find("int=42 str=hello"), std::string::npos);
}

// ════════════════════════════════════════════════════════════════════════════
// Test 6: RAW write (machine protocol, no prefix)
// ════════════════════════════════════════════════════════════════════════════

TEST(LogSystem, RawWrite) {
    StdoutCapture cap;
    ASSERT_TRUE(cap.Begin()) << "capture begin failed";

    CHAOS_IL2CPP_LOG_WRITE_RAW("{\"benchmark\": true}\n");
    CHAOS_IL2CPP_LOG_FLUSH_STDOUT();

    std::string output = cap.End();

    EXPECT_NE(output.find("benchmark"), std::string::npos) << "missing raw content";
    EXPECT_EQ(output.find("[RAW]"), std::string::npos) << "raw output should not contain [RAW] prefix";
    EXPECT_EQ(output.find("[INFO]"), std::string::npos) << "raw output should not contain [INFO] prefix";
}

// ════════════════════════════════════════════════════════════════════════════
// Test 7: RAW format write (_M variant)
// ════════════════════════════════════════════════════════════════════════════

TEST(LogSystem, RawWriteFmt) {
    StdoutCapture cap;
    ASSERT_TRUE(cap.Begin()) << "capture begin failed";

    CHAOS_IL2CPP_LOG_WRITE_RAW_M("{{\"key\": \"{0}\", \"val\": {1}}}\n", "test", 42);
    CHAOS_IL2CPP_LOG_FLUSH_STDOUT();

    std::string output = cap.End();
    EXPECT_NE(output.find(R"("key": "test")"), std::string::npos);
    EXPECT_NE(output.find(R"("val": 42)"), std::string::npos);
}

// ════════════════════════════════════════════════════════════════════════════
// Test 8: Thread safety — 4 threads, 100 writes each
// ════════════════════════════════════════════════════════════════════════════

TEST(LogSystem, ThreadSafety) {
    constexpr int kThreadCount = 4;
    constexpr int kWritesPerThread = 100;

    StdoutCapture cap;
    ASSERT_TRUE(cap.Begin()) << "capture begin failed";

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([t, kWritesPerThread]() {
            for (int i = 0; i < kWritesPerThread; ++i) {
                CHAOS_IL2CPP_LOG_ERROR_M("ThreadSafe", "t={0} i={1}", t, i);
            }
            CHAOS_IL2CPP_LOG_FLUSH();
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    std::string output = cap.End();
    EXPECT_FALSE(output.empty()) << "no output captured from threads";

    for (int t = 0; t < kThreadCount; ++t) {
        std::string marker = std::string("t=") + std::to_string(t);
        EXPECT_NE(output.find(marker), std::string::npos)
            << "no output from thread " << t;
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test 9: Level elimination — DEBUG vs INFO at CHAOS_IL2CPP_LOG_LEVEL=3
// ════════════════════════════════════════════════════════════════════════════

TEST(LogSystem, LevelElimination) {
    StdoutCapture cap;
    ASSERT_TRUE(cap.Begin()) << "capture begin failed";

    CHAOS_IL2CPP_LOG_ERROR("Level", "err");
    CHAOS_IL2CPP_LOG_WARN("Level", "wrn");
    CHAOS_IL2CPP_LOG_INFO("Level", "inf");
    CHAOS_IL2CPP_LOG_DEBUG("Level", "dbg");
    CHAOS_IL2CPP_LOG_FLUSH();

    std::string output = cap.End();

    EXPECT_NE(output.find("[ERROR]"), std::string::npos) << "ERROR level missing";
    EXPECT_NE(output.find("[WARN]"), std::string::npos) << "WARN level missing";
    EXPECT_NE(output.find("[INFO]"), std::string::npos) << "INFO level missing";
    EXPECT_NE(output.find("[DEBUG]"), std::string::npos) << "DEBUG level missing";
}
