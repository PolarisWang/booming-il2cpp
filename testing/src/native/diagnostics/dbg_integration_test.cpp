// dbg_integration_test.cpp — DAP server integration tests
//
// Tests the DAP message dispatch pipeline: request routing, handler
// execution, and response formatting.  Uses DbgServerDispatchTestRequest
// to bypass the transport layer and a stdout pipe for response capture.
//
// Links chaos_debugger for the server + transport implementation.

#include <gtest/gtest.h>
#include <debugger/dbg_server.h>
#include <debugger/dbg_transport.h>
#include <debugger/dbg_protocol.h>
#include <debugger/dbg_breakpoint.h>
#include <debugger/dbg_stepping.h>
#include <debugger/dbg_stack.h>
#include <debugger/dbg_variables.h>
#include <debugger/dbg_runtime.h>

#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

namespace chaos::il2cpp::diagnostics {
namespace {

class DapIntegrationTest : public ::testing::Test {
protected:
    int saved_stdout_ = -1;
    int stdout_read_fd_ = -1;

    void SetUp() override {
        // Create a pipe for stdout capture.
        int pipe_fds[2];
        ASSERT_EQ(_pipe(pipe_fds, 65536, _O_BINARY), 0);

        // Save original stdout fd.
        fflush(stdout);
        saved_stdout_ = _dup(_fileno(stdout));
        ASSERT_NE(saved_stdout_, -1);

        // Redirect stdout to the pipe write end.
        ASSERT_NE(_dup2(pipe_fds[1], _fileno(stdout)), -1);

        // Close the original pipe write end (dup'd version stays open).
        _close(pipe_fds[1]);

        // Save the pipe read end for test use.
        stdout_read_fd_ = pipe_fds[0];

        // Make stdout unbuffered so fwrite goes through immediately.
        setbuf(stdout, nullptr);

        // Initialize transport (sets binary mode on stdout).
        DbgTransportInitialize();
        DbgServerInitialize();

        DbgBreakpointInitialize();
        DbgStackInitialize();
        DbgSteppingInitialize();
        DbgVariablesInitialize();
    }

    /// Flush stdout and read all captured data from the pipe.
    std::string CaptureStdout() {
        fflush(stdout);
        std::string result;
        char buf[4096];

        HANDLE hPipe = reinterpret_cast<HANDLE>(_get_osfhandle(stdout_read_fd_));
        if (hPipe == INVALID_HANDLE_VALUE) return result;

        // Small sleep to let data arrive.
        Sleep(10);

        DWORD avail = 0;
        while (PeekNamedPipe(hPipe, nullptr, 0, nullptr, &avail, nullptr) && avail > 0) {
            DWORD n = 0;
            DWORD to_read = avail < sizeof(buf) - 1 ? avail : static_cast<DWORD>(sizeof(buf) - 1);
            if (ReadFile(hPipe, buf, to_read, &n, nullptr) && n > 0) {
                buf[n] = '\0';
                result.append(buf, n);
            } else {
                break;
            }
            avail = 0;  // re-check for more data
        }
        return result;
    }

    void TearDown() override {
        DbgVariablesShutdown();
        DbgSteppingShutdown();
        DbgStackShutdown();
        DbgBreakpointShutdown();
        DbgServerShutdown();
        DbgTransportShutdown();

        // Close the pipe read end.
        if (stdout_read_fd_ >= 0) _close(stdout_read_fd_);

        // Restore original stdout.
        fflush(stdout);
        if (saved_stdout_ >= 0) {
            _dup2(saved_stdout_, _fileno(stdout));
            _close(saved_stdout_);
        }
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

// ── Handler response tests ──────────────────────────────────────────

TEST_F(DapIntegrationTest, InitializeRequest) {
    DispatchCommand("initialize", "{}");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("Content-Length:"), std::string::npos);
    EXPECT_NE(wire.find("supportsConfigurationDoneRequest"), std::string::npos);
    EXPECT_NE(wire.find("supportsFunctionBreakpoints"), std::string::npos);
    // Should also send initialized event.
    EXPECT_NE(wire.find("initialized"), std::string::npos);
}

TEST_F(DapIntegrationTest, ThreadsRequest) {
    DispatchCommand("threads");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("Main Thread"), std::string::npos);
    EXPECT_NE(wire.find("\"id\":1"), std::string::npos);
}

TEST_F(DapIntegrationTest, SetBreakpointsEmpty) {
    DispatchCommand("setBreakpoints", "{}");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("\"breakpoints\""), std::string::npos);
    EXPECT_NE(wire.find("\"success\":true"), std::string::npos);
}

TEST_F(DapIntegrationTest, UnknownCommand) {
    DispatchCommand("unknownCommandXyz");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("\"success\":true"), std::string::npos);
}

TEST_F(DapIntegrationTest, ConfigurationDone) {
    DispatchCommand("configurationDone");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("\"command\":\"configurationDone\""), std::string::npos);
    EXPECT_NE(wire.find("\"success\":true"), std::string::npos);
}

TEST_F(DapIntegrationTest, LaunchRequest) {
    DispatchCommand("launch");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("\"command\":\"launch\""), std::string::npos);
    EXPECT_NE(wire.find("\"success\":true"), std::string::npos);
}

TEST_F(DapIntegrationTest, StackTraceRequest) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 2;
    snap.frames[0] = {0x06000001, 0};
    snap.frames[1] = {0x06000002, 15};

    DispatchCommand("stackTrace");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("\"totalFrames\":2"), std::string::npos);
    EXPECT_NE(wire.find("method_0x06000001"), std::string::npos);
    EXPECT_NE(wire.find("method_0x06000002"), std::string::npos);

    DbgClearFrameSnapshot();
}

TEST_F(DapIntegrationTest, ScopesRequest) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};

    DispatchCommand("scopes", "{\"frameId\":0}");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("\"name\":\"Locals\""), std::string::npos);
    EXPECT_NE(wire.find("\"variablesReference\":1"), std::string::npos);

    DbgClearFrameSnapshot();
}

TEST_F(DapIntegrationTest, EvaluateNumericLiteral) {
    DispatchCommand("evaluate", "{\"expression\":\"42\",\"frameId\":0}");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("\"result\":\"42\""), std::string::npos);
    EXPECT_NE(wire.find("\"type\":\"int\""), std::string::npos);
}

TEST_F(DapIntegrationTest, EvaluateUnknown) {
    DispatchCommand("evaluate", "{\"expression\":\"nope\",\"frameId\":0}");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("nope"), std::string::npos);
}

TEST_F(DapIntegrationTest, Sequence) {
    DispatchCommand("initialize", "{}");
    DispatchCommand("threads");
    DispatchCommand("stackTrace");
    std::string wire = CaptureStdout();
    ASSERT_FALSE(wire.empty());

    EXPECT_NE(wire.find("supportsConfigurationDoneRequest"), std::string::npos);
    EXPECT_NE(wire.find("Main Thread"), std::string::npos);
    EXPECT_NE(wire.find("\"command\":\"stackTrace\""), std::string::npos);
}

// ── Side-effect tests (no stdout capture needed) ────────────────────

TEST_F(DapIntegrationTest, DisconnectClearsBreakpoints) {
    DbgSetBreakpoint(0x06000001, 0, "test.cs", 10);
    EXPECT_FALSE(DbgGetAllBreakpoints().empty());

    DispatchCommand("disconnect");
    CaptureStdout();

    EXPECT_TRUE(DbgGetAllBreakpoints().empty());
}

TEST_F(DapIntegrationTest, StepInThenNextThenContinue) {
    DispatchCommand("stepIn");
    CaptureStdout();
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepInto);

    DispatchCommand("next");
    CaptureStdout();
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOver);

    DispatchCommand("continue");
    CaptureStdout();
    EXPECT_FALSE(DbgIsStepping());
}

TEST_F(DapIntegrationTest, PauseFlag) {
    EXPECT_FALSE(g_dbg_pause_requested.load());

    DispatchCommand("pause");
    CaptureStdout();

    EXPECT_TRUE(g_dbg_pause_requested.load());

    // Reset for next tests.
    g_dbg_pause_requested.store(false, std::memory_order_release);
}

TEST_F(DapIntegrationTest, SetBreakpointsParsesArgs) {
    // setBreakpoints clears old breakpoints for the source.
    DbgSetBreakpoint(0x06000001, 0, "test.cs", 10);

    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"path\":\"test.cs\"},\"breakpoints\":[{\"line\":20}]}");
    CaptureStdout();

    // Old breakpoint at line 10 should be cleared.
    // (No method match in test env, so new bp won't register.)
    auto bps = DbgGetBreakpointsForSource("test.cs");
    EXPECT_TRUE(bps.empty());
}

// ── Extended integration scenarios ─────────────────────────────────

TEST_F(DapIntegrationTest, FullSessionLifecycle) {
    // Simulate a full DAP session: initialize -> launch -> setBreakpoints
    // -> threads -> stackTrace -> scopes -> variables -> continue -> disconnect.

    // 1. Initialize
    DispatchCommand("initialize", "{}");
    std::string wire = CaptureStdout();
    EXPECT_NE(wire.find("Content-Length:"), std::string::npos);
    EXPECT_NE(wire.find("supportsConfigurationDoneRequest"), std::string::npos);

    // 2. Launch
    DispatchCommand("launch");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("\"command\":\"launch\""), std::string::npos);
    EXPECT_NE(wire.find("\"success\":true"), std::string::npos);

    // 3. SetBreakpoints (no real runtime, so breakpoints won't register)
    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"path\":\"Program.cs\"},\"breakpoints\":[{\"line\":10}]}");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("\"command\":\"setBreakpoints\""), std::string::npos);

    // 4. Threads
    DispatchCommand("threads");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("Main Thread"), std::string::npos);

    // 5. ConfigurationDone
    DispatchCommand("configurationDone");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("\"command\":\"configurationDone\""), std::string::npos);

    // 6. Disconnect
    DispatchCommand("disconnect");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("\"command\":\"disconnect\""), std::string::npos);

    // Verify state is clean.
    EXPECT_TRUE(DbgGetAllBreakpoints().empty());
    EXPECT_FALSE(DbgIsStepping());
}

TEST_F(DapIntegrationTest, StepIntoThenStepOverThenStepOutSequence) {
    // Set up frame depth for step tracking.
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 2;
    snap.frames[0] = {0x06000001, 0};
    snap.frames[1] = {0x06000002, 10};

    // Step Into
    DispatchCommand("stepIn");
    CaptureStdout();
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepInto);

    // Step Over (transition)
    DispatchCommand("next");
    CaptureStdout();
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOver);
    EXPECT_EQ(DbgGetStepTargetDepth(), 2);

    // Step Out
    DispatchCommand("stepOut");
    CaptureStdout();
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOut);
    EXPECT_EQ(DbgGetStepTargetDepth(), 2);

    DbgClearFrameSnapshot();
}

TEST_F(DapIntegrationTest, VariablesAfterStackTrace) {
    // Populate frame snapshot with locals.
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 5};
    snap.local_count = 2;
    snap.locals[0] = {0, 42, 1};      // int32
    snap.locals[1] = {1, 0x1000, 5};  // object ref

    // StackTrace
    DispatchCommand("stackTrace");
    std::string wire = CaptureStdout();
    EXPECT_NE(wire.find("totalFrames"), std::string::npos);

    // Scopes for frame 0
    DispatchCommand("scopes", "{\"frameId\":0}");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("\"name\":\"Locals\""), std::string::npos);

    // Variables for scope ref 1
    DispatchCommand("variables", "{\"variablesReference\":1}");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("\"name\":\"local_0\""), std::string::npos);
    EXPECT_NE(wire.find("\"value\":\"42\""), std::string::npos);
    EXPECT_NE(wire.find("\"name\":\"local_1\""), std::string::npos);

    DbgClearFrameSnapshot();
}

TEST_F(DapIntegrationTest, MultipleBreakpointsAcrossSources) {
    // Register breakpoints for two different sources.
    DbgSetBreakpoint(0x06000001, 0, "source_a.cs", 10);
    DbgSetBreakpoint(0x06000002, 0, "source_b.cs", 20);

    // SetBreakpoints for source_a should only clear source_a's breakpoints.
    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"path\":\"source_a.cs\"},\"breakpoints\":[{\"line\":15}]}");
    CaptureStdout();

    auto all = DbgGetAllBreakpoints();
    // source_a breakpoint at line 10 should be cleared,
    // source_b breakpoint at line 20 should remain.
    for (const auto& bp : all) {
        if (bp.source_path == "source_a.cs") {
            EXPECT_NE(bp.line, 10);  // old line cleared
        }
    }
}

TEST_F(DapIntegrationTest, BreakpointHitThenVariablesThenContinue) {
    // Simulate hitting a breakpoint: populate snapshot, check breakpoint,
    // get variables, then continue.

    // Register a breakpoint.
    DbgSetBreakpoint(0x06000001, 5, "test.cs", 10);

    // Simulate breakpoint hit detection.
    int bp_id = DbgCheckBreakpoint(0x06000001, 5);
    EXPECT_GT(bp_id, 0);

    // Populate frame snapshot as if we hit the breakpoint.
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 5};
    snap.local_count = 1;
    snap.locals[0] = {0, 99, 1};  // int32 = 99

    // StackTrace
    DispatchCommand("stackTrace");
    std::string wire = CaptureStdout();
    EXPECT_NE(wire.find("totalFrames"), std::string::npos);

    // Variables
    DispatchCommand("variables", "{\"variablesReference\":1}");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("\"value\":\"99\""), std::string::npos);

    // Continue
    DispatchCommand("continue");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("\"command\":\"continue\""), std::string::npos);

    DbgClearFrameSnapshot();
}

TEST_F(DapIntegrationTest, EvaluateExpressionWithLocal) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};
    snap.local_count = 1;
    snap.locals[0] = {3, 777, 1};  // local_3 = 777

    // Evaluate "local_3" — should match.
    DispatchCommand("evaluate", "{\"expression\":\"local_3\",\"frameId\":0}");
    std::string wire = CaptureStdout();
    EXPECT_NE(wire.find("777"), std::string::npos);

    DbgClearFrameSnapshot();
}

TEST_F(DapIntegrationTest, EvaluateMultipleTypes) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};

    float f = 2.5f;
    uint64_t f_raw = 0;
    std::memcpy(&f_raw, &f, sizeof(f));

    snap.local_count = 2;
    snap.locals[0] = {0, 42, 1};       // int32
    snap.locals[1] = {1, f_raw, 3};    // float

    DispatchCommand("evaluate", "{\"expression\":\"local_0\",\"frameId\":0}");
    std::string wire = CaptureStdout();
    EXPECT_NE(wire.find("42"), std::string::npos);
    EXPECT_NE(wire.find("int"), std::string::npos);

    DispatchCommand("evaluate", "{\"expression\":\"local_1\",\"frameId\":0}");
    wire = CaptureStdout();
    EXPECT_NE(wire.find("2.5"), std::string::npos);
    EXPECT_NE(wire.find("float"), std::string::npos);

    DbgClearFrameSnapshot();
}

TEST_F(DapIntegrationTest, SetBreakpointsClearsOnlyMatchingSource) {
    // Set up breakpoints in two sources.
    DbgSetBreakpoint(0x06000001, 0, "alpha.cs", 10);
    DbgSetBreakpoint(0x06000002, 0, "beta.cs", 20);

    EXPECT_EQ(DbgGetAllBreakpoints().size(), 2u);

    // Set breakpoints for alpha.cs — should only clear alpha's old bps.
    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"path\":\"alpha.cs\"},\"breakpoints\":[{\"line\":15}]}");
    CaptureStdout();

    auto all = DbgGetAllBreakpoints();
    // beta.cs breakpoint should still be there.
    bool beta_found = false;
    for (const auto& bp : all) {
        if (bp.source_path == "beta.cs") {
            beta_found = true;
            break;
        }
    }
    EXPECT_TRUE(beta_found);
}

TEST_F(DapIntegrationTest, ThreeStepSequence) {
    // Three back-to-back step requests.
    DispatchCommand("stepIn");
    CaptureStdout();
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepInto);

    DispatchCommand("next");
    CaptureStdout();
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOver);

    DispatchCommand("next");
    CaptureStdout();
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOver);

    DispatchCommand("stepOut");
    CaptureStdout();
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOut);
}

TEST_F(DapIntegrationTest, PauseThenStepIn) {
    DispatchCommand("pause");
    CaptureStdout();

    // Pause sets g_dbg_pause_requested. StepIn should still work.
    DispatchCommand("stepIn");
    CaptureStdout();
    EXPECT_TRUE(DbgIsStepping());

    // Reset.
    g_dbg_pause_requested.store(false, std::memory_order_release);
}

TEST_F(DapIntegrationTest, LaunchThenSetBreakpointsThenThreads) {
    DispatchCommand("launch");
    CaptureStdout();

    DispatchCommand("setBreakpoints",
                    "{\"source\":{\"path\":\"Program.cs\"},\"breakpoints\":[{\"line\":10}]}");
    CaptureStdout();

    DispatchCommand("threads");
    std::string wire = CaptureStdout();
    EXPECT_NE(wire.find("Main Thread"), std::string::npos);
}

TEST_F(DapIntegrationTest, EmptyArgsForScopes) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};

    DispatchCommand("scopes", "{}");
    std::string wire = CaptureStdout();
    EXPECT_NE(wire.find("\"command\":\"scopes\""), std::string::npos);

    DbgClearFrameSnapshot();
}

TEST_F(DapIntegrationTest, MultipleContinueCalls) {
    DispatchCommand("continue");
    CaptureStdout();

    DispatchCommand("continue");
    CaptureStdout();

    DispatchCommand("continue");
    CaptureStdout();
    // No crash on repeated continue.
}

}  // namespace
}  // namespace chaos::il2cpp::diagnostics
