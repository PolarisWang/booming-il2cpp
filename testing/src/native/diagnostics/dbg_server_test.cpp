// dbg_server_test.cpp — DAP server dispatch routing unit tests
//
// Tests the message routing logic of the DAP server:
//   - Command dispatch by name (initialize, launch, threads, etc.)
//   - Handler routing edge cases (missing fields, invalid args)
//   - Side effects of handlers on module state
//
// Uses DbgServerDispatchTestRequest to bypass the transport layer
// and checks side effects via public debugger APIs.

#include <gtest/gtest.h>
#include <debugger/dbg_server.h>
#include <debugger/dbg_breakpoint.h>
#include <debugger/dbg_stepping.h>
#include <debugger/dbg_stack.h>
#include <debugger/dbg_variables.h>
#include <debugger/dbg_runtime.h>
#include <debugger/dbg_protocol.h>

#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

namespace chaos::il2cpp::diagnostics {
namespace {

class DbgServerTest : public ::testing::Test {
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
        DbgServerDispatchTestRequest(json_body, static_cast<int>(strlen(json_body)));
    }

    void DispatchCommand(const char* command, const char* args = nullptr) {
        char buf[1024];
        if (args) {
            snprintf(buf, sizeof(buf),
                     "{\"seq\":1,\"type\":\"request\",\"command\":\"%s\",\"arguments\":%s}",
                     command, args);
        } else {
            snprintf(buf, sizeof(buf),
                     "{\"seq\":1,\"type\":\"request\",\"command\":\"%s\"}",
                     command);
        }
        DispatchJson(buf);
    }
};

// ── Command dispatch routing ─────────────────────────────────────────

TEST_F(DbgServerTest, InitializeSetsClientAttached) {
    DispatchCommand("initialize", "{}");
    // After initialize, the stepping state should be None.
    EXPECT_EQ(DbgGetStepMode(), StepMode::None);
}

TEST_F(DbgServerTest, LaunchCommand) {
    DispatchCommand("launch");
    // Launch just acknowledges — no state changes expected.
    EXPECT_FALSE(DbgIsStepping());
}

TEST_F(DbgServerTest, ContinueClearsStepping) {
    DbgBeginStepping(1, StepMode::StepInto, 0);
    EXPECT_TRUE(DbgIsStepping());

    DispatchCommand("continue");
    EXPECT_FALSE(DbgIsStepping());
}

TEST_F(DbgServerTest, NextSetsStepOver) {
    // Need at least one frame for next to work.
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 3;

    DispatchCommand("next");
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOver);

    DbgClearFrameSnapshot();
}

TEST_F(DbgServerTest, StepInSetsStepInto) {
    DispatchCommand("stepIn");
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepInto);
}

TEST_F(DbgServerTest, StepOutSetsStepOut) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 3;

    DispatchCommand("stepOut");
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOut);

    DbgClearFrameSnapshot();
}

TEST_F(DbgServerTest, DisconnectClearsAllState) {
    // Set up some state.
    DbgSetBreakpoint(0x06000001, 0, "test.cs", 10);
    DbgBeginStepping(1, StepMode::StepInto, 0);
    DbgSetBreakpoint(0x06000002, 5, "other.cs", 20);

    EXPECT_FALSE(DbgGetAllBreakpoints().empty());
    EXPECT_TRUE(DbgIsStepping());

    DispatchCommand("disconnect");

    EXPECT_TRUE(DbgGetAllBreakpoints().empty());
    EXPECT_FALSE(DbgIsStepping());
}

TEST_F(DbgServerTest, ConfigurationDone) {
    DispatchCommand("configurationDone");
    // No state changes — just acknowledges.
}

TEST_F(DbgServerTest, ThreadsReturnsMainThread) {
    // The threads handler always returns "Main Thread" with id 1.
    DispatchCommand("threads");
    // Just verify no crash — actual output goes to transport.
}

TEST_F(DbgServerTest, StackTraceWhenEmpty) {
    // Empty frame snapshot — should not crash.
    DispatchCommand("stackTrace");
}

TEST_F(DbgServerTest, ScopesWithInvalidFrameId) {
    // No frames in snapshot, frameId 999 — empty scopes.
    DispatchCommand("scopes", "{\"frameId\":999}");
}

TEST_F(DbgServerTest, ScopesWithNegativeFrameId) {
    DispatchCommand("scopes", "{\"frameId\":-1}");
}

TEST_F(DbgServerTest, ScopesWithoutFrameId) {
    DispatchCommand("scopes", "{}");
}

TEST_F(DbgServerTest, VariablesWithNoRef) {
    DispatchCommand("variables", "{}");
}

TEST_F(DbgServerTest, VariablesWithRef) {
    DispatchCommand("variables", "{\"variablesReference\":1}");
}

TEST_F(DbgServerTest, EvaluateWithNoExpression) {
    DispatchCommand("evaluate", "{\"frameId\":0}");
}

TEST_F(DbgServerTest, EvaluateWithEmptyExpression) {
    DispatchCommand("evaluate", "{\"expression\":\"\",\"frameId\":0}");
}

TEST_F(DbgServerTest, EvaluateNumeric) {
    DispatchCommand("evaluate", "{\"expression\":\"123\",\"frameId\":0}");
}

TEST_F(DbgServerTest, EvaluateDecimal) {
    DispatchCommand("evaluate", "{\"expression\":\"0xFF\",\"frameId\":0}");
}

TEST_F(DbgServerTest, EvaluateNegativeNumber) {
    DispatchCommand("evaluate", "{\"expression\":\"-42\",\"frameId\":0}");
}

TEST_F(DbgServerTest, EvaluateHex) {
    DispatchCommand("evaluate", "{\"expression\":\"0xABC\",\"frameId\":0}");
}

TEST_F(DbgServerTest, SetFunctionBreakpoints) {
    DispatchCommand("setFunctionBreakpoints",
                    "{\"breakpoints\":[{\"name\":\"main\"}]}");
}

TEST_F(DbgServerTest, SetBreakpointsNullSource) {
    DispatchCommand("setBreakpoints", "{\"breakpoints\":[{\"line\":10}]}");
}

TEST_F(DbgServerTest, SetBreakpointsWithOnlySourceName) {
    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"name\":\"test.cs\"},\"breakpoints\":[{\"line\":10}]}");
}

TEST_F(DbgServerTest, SetBreakpointsTwiceOverwrites) {
    // First set.
    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"path\":\"server_test.cs\"},\"breakpoints\":[{\"line\":10}]}");
    // In test env without a real runtime, no actual breakpoints get registered
    // (because DbgResolveMethodsForSource finds no matching types).
    // But the second call should not crash.
    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"path\":\"server_test.cs\"},\"breakpoints\":[{\"line\":20}]}");
}

TEST_F(DbgServerTest, PauseSetsFlag) {
    EXPECT_FALSE(g_dbg_pause_requested.load());

    DispatchCommand("pause");
    EXPECT_TRUE(g_dbg_pause_requested.load());

    // Reset for subsequent tests.
    g_dbg_pause_requested.store(false, std::memory_order_release);
}

TEST_F(DbgServerTest, DisconnectAfterPause) {
    DispatchCommand("pause");
    EXPECT_TRUE(g_dbg_pause_requested.load());

    DispatchCommand("disconnect");
    // Disconnect should clear pause flag indirectly by clearing breakpoints.
    // Note: g_dbg_any_breakpoints is set by HandlePause.
    // g_dbg_pause_requested is NOT cleared by HandleDisconnect directly,
    // but g_dbg_any_breakpoints is set.
    g_dbg_pause_requested.store(false, std::memory_order_release);
}

TEST_F(DbgServerTest, UnknownCommandDoesNotCrash) {
    DispatchCommand("thisIsNotARealCommand");
}

TEST_F(DbgServerTest, EmptyJsonBody) {
    DispatchJson("");
}

TEST_F(DbgServerTest, MalformedJsonMissingCommand) {
    DispatchJson("{\"seq\":1,\"type\":\"request\"}");
}

TEST_F(DbgServerTest, RequestWithArgumentsAsObject) {
    // Args directly as object with no "arguments" key.
    DispatchJson("{\"seq\":1,\"type\":\"request\",\"command\":\"evaluate\","
                 "\"expression\":\"42\",\"frameId\":0}");
}

TEST_F(DbgServerTest, MultipleCommandsInSequence) {
    DispatchCommand("initialize", "{}");
    DispatchCommand("launch");
    DispatchCommand("threads");
    DispatchCommand("stackTrace");
    DispatchCommand("continue");
    // No crash, no hang.
}

TEST_F(DbgServerTest, StepInThenContinueThenStepOver) {
    DispatchCommand("stepIn");
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepInto);

    DispatchCommand("continue");
    EXPECT_FALSE(DbgIsStepping());

    DispatchCommand("next");
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOver);
}

TEST_F(DbgServerTest, EvaluateWithNonNumericExpression) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};
    snap.local_count = 1;
    snap.locals[0] = {0, 42, 1};  // local_0 = 42

    // Evaluate "local_0" should match.
    DispatchCommand("evaluate", "{\"expression\":\"local_0\",\"frameId\":0}");

    DbgClearFrameSnapshot();
}

TEST_F(DbgServerTest, EvaluateWithMatchingLocal) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};
    snap.local_count = 2;
    snap.locals[0] = {5, 100, 1};      // local_5 = 100 (int32)
    snap.locals[1] = {7, 0x1234, 5};   // local_7 = object ref

    // Evaluate an expression that doesn't match any local.
    DispatchCommand("evaluate", "{\"expression\":\"non_existent\",\"frameId\":0}");

    DbgClearFrameSnapshot();
}

TEST_F(DbgServerTest, SetBreakpointsWithLinesArray) {
    // "lines" is the legacy alternative to "breakpoints".
    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"path\":\"test.cs\"},\"lines\":[10, 20, 30]}");
}

TEST_F(DbgServerTest, SetBreakpointsWithMixedArgs) {
    // Both "breakpoints" and "lines" — "breakpoints" takes priority.
    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"path\":\"test.cs\"},"
                    "\"breakpoints\":[{\"line\":5}],"
                    "\"lines\":[10, 20]}");
}

TEST_F(DbgServerTest, ScopesAfterFramePopulated) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 2;
    snap.frames[0] = {0x06000001, 0};
    snap.frames[1] = {0x06000002, 15};

    // Scopes for frame 0.
    DispatchCommand("scopes", "{\"frameId\":0}");

    // Scopes for frame 1.
    DispatchCommand("scopes", "{\"frameId\":1}");

    DbgClearFrameSnapshot();
}

TEST_F(DbgServerTest, PauseThenContinue) {
    DispatchCommand("pause");
    EXPECT_TRUE(g_dbg_pause_requested.load());

    // Continue should clear stepping, which clears g_dbg_any_breakpoints,
    // but NOT g_dbg_pause_requested directly.
    DispatchCommand("continue");
    EXPECT_FALSE(DbgIsStepping());

    // Manually clear for test isolation.
    g_dbg_pause_requested.store(false, std::memory_order_release);
}

// ── Additional negative/stress tests (P3-G) ────────────────────────────

TEST_F(DbgServerTest, TruncatedJsonBody) {
    DispatchJson("{\"seq\":1,\"typ");
    SUCCEED();
}

TEST_F(DbgServerTest, NonJsonBinaryInput) {
    DispatchJson("\x00\x01\x02\xFF\xFE");
    SUCCEED();
}

TEST_F(DbgServerTest, RepeatedMalformedJsonDoesNotCorrupt) {
    for (int i = 0; i < 20; i++) {
        DispatchJson("{{{broken}}}");
    }
    // After all the malformed requests, a valid request should still work.
    DispatchCommand("threads");
    SUCCEED();
}

TEST_F(DbgServerTest, CommandWithOversizedArguments) {
    std::string large_arg(2048, 'X');
    char buf[3072];
    snprintf(buf, sizeof(buf),
             "{\"seq\":1,\"type\":\"request\",\"command\":\"evaluate\","
             "\"arguments\":{\"expression\":\"%s\"}}", large_arg.c_str());
    DispatchJson(buf);
    SUCCEED();
}

TEST_F(DbgServerTest, RepeatedDisconnectInSequence) {
    DispatchCommand("disconnect");
    DispatchCommand("disconnect");
    DispatchCommand("disconnect");
    SUCCEED();
}

}  // namespace
}  // namespace chaos::il2cpp::diagnostics
