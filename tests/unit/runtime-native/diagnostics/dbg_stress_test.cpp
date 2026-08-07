// dbg_stress_test.cpp — DAP debugger stress/no-crash tests
//
// High-frequency operations, concurrent access patterns, and
// resource-exhaustion scenarios (P3-G: stress tests).

#include <gtest/gtest.h>
#include <debugger/dbg_server.h>
#include <debugger/dbg_breakpoint.h>
#include <debugger/dbg_stepping.h>
#include <debugger/dbg_stack.h>
#include <debugger/dbg_variables.h>
#include <debugger/dbg_runtime.h>

#include <thread>
#include <vector>
#include <atomic>

namespace chaos::il2cpp::diagnostics {
namespace {

class DbgStressTest : public ::testing::Test {
protected:
    void SetUp() override {
        DbgServerInitialize();
        DbgBreakpointInitialize();
        DbgSteppingInitialize();
        DbgStackInitialize();
        DbgVariablesInitialize();
        DbgClearFrameSnapshot();
    }

    void TearDown() override {
        DbgVariablesShutdown();
        DbgSteppingShutdown();
        DbgStackShutdown();
        DbgBreakpointShutdown();
        DbgServerShutdown();
    }

    void DispatchJson(const char* json_body) {
        DbgServerDispatchTestRequest(json_body, static_cast<int>(std::strlen(json_body)));
    }

    void DispatchCommand(const char* command, const char* args = nullptr) {
        char buf[1024];
        if (args) {
            std::snprintf(buf, sizeof(buf),
                     "{\"seq\":1,\"type\":\"request\",\"command\":\"%s\",\"arguments\":%s}",
                     command, args);
        } else {
            std::snprintf(buf, sizeof(buf),
                     "{\"seq\":1,\"type\":\"request\",\"command\":\"%s\"}",
                     command);
        }
        DispatchJson(buf);
    }
};

// ── Rapid operation cycling ───────────────────────────────────────────

TEST_F(DbgStressTest, RapidSetClearBreakpoints) {
    for (int i = 0; i < 100; i++) {
        char args[256];
        std::snprintf(args, sizeof(args),
                 "{\"source\":{\"path\":\"stress_test.cs\"},"
                 "\"breakpoints\":[{\"line\":%d}]}", i + 1);
        DispatchCommand("setBreakpoints", args);
    }
    // No crash, no hang.
    SUCCEED();
}

TEST_F(DbgStressTest, RepeatedInitializeDisconnect) {
    for (int i = 0; i < 50; i++) {
        DispatchCommand("initialize", "{}");
        DispatchCommand("disconnect");
    }
    SUCCEED();
}

TEST_F(DbgStressTest, RapidStepSequence) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 3;

    for (int i = 0; i < 50; i++) {
        DispatchCommand("stepIn");
        DispatchCommand("continue");
    }
    DbgClearFrameSnapshot();
    SUCCEED();
}

TEST_F(DbgStressTest, ManyUnknownCommands) {
    for (int i = 0; i < 500; i++) {
        DispatchCommand("unknownCommandXyz");
    }
    SUCCEED();
}

TEST_F(DbgStressTest, RapidEvaluateCalls) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};
    snap.local_count = 1;
    snap.locals[0] = {0, 42, 1};

    for (int i = 0; i < 100; i++) {
        DispatchCommand("evaluate", "{\"expression\":\"42\",\"frameId\":0}");
    }
    DbgClearFrameSnapshot();
    SUCCEED();
}

// ── Concurrent access ─────────────────────────────────────────────────

TEST_F(DbgStressTest, ConcurrentDispatchThreads) {
    std::atomic<int> completed{0};
    auto worker = [&](const char* cmd, int iterations) {
        for (int i = 0; i < iterations; i++) {
            DispatchCommand(cmd);
        }
        completed++;
    };

    std::thread t1(worker, "threads", 50);
    std::thread t2(worker, "pause", 50);
    std::thread t3(worker, "continue", 50);
    t1.join();
    t2.join();
    t3.join();

    EXPECT_EQ(completed.load(), 3);
}

TEST_F(DbgStressTest, ConcurrentBreakpointAndStepping) {
    std::atomic<int> completed{0};
    auto bp_worker = [&]() {
        for (int i = 0; i < 50; i++) {
            DispatchCommand("setBreakpoints",
                            "{\"source\":{\"path\":\"concur.cs\"},"
                            "\"breakpoints\":[{\"line\":10}]}");
        }
        completed++;
    };
    auto step_worker = [&]() {
        for (int i = 0; i < 50; i++) {
            DispatchCommand("stepIn");
            DispatchCommand("continue");
        }
        completed++;
    };

    std::thread t1(bp_worker);
    std::thread t2(step_worker);
    t1.join();
    t2.join();

    EXPECT_EQ(completed.load(), 2);
}

TEST_F(DbgStressTest, BreakpointStressManySources) {
    for (int s = 0; s < 20; s++) {
        for (int b = 0; b < 10; b++) {
            char args[512];
            std::snprintf(args, sizeof(args),
                     "{\"source\":{\"path\":\"src_%d.cs\"},"
                     "\"breakpoints\":[{\"line\":%d}]}", s, b * 5 + 1);
            DispatchCommand("setBreakpoints", args);
        }
    }
    SUCCEED();
}

// ── Memory / resource patterns ────────────────────────────────────────

TEST_F(DbgStressTest, VeryLongCommandName) {
    std::string long_cmd(1024, 'A');
    char buf[2048];
    std::snprintf(buf, sizeof(buf),
             "{\"seq\":1,\"type\":\"request\",\"command\":\"%s\"}", long_cmd.c_str());
    DispatchJson(buf);
    SUCCEED();
}

TEST_F(DbgStressTest, VeryDeeplyNestedJson) {
    // Build deeply nested JSON to test parser recursion limits.
    std::string nested;
    for (int i = 0; i < 100; i++) {
        nested += "{\"a\":";
    }
    nested += "1";
    for (int i = 0; i < 100; i++) {
        nested += "}";
    }

    char buf[4096];
    std::snprintf(buf, sizeof(buf),
             "{\"seq\":1,\"type\":\"request\",\"command\":\"evaluate\",\"arguments\":%s}",
             nested.c_str());
    DispatchJson(buf);
    SUCCEED();
}

}  // namespace
}  // namespace chaos::il2cpp::diagnostics
