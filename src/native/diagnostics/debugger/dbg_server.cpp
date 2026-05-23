// dbg_server.cpp — DAP message server implementation

#include "dbg_server.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_transport.h"
#include "dbg_breakpoint.h"
#include "dbg_runtime.h"
#include "dbg_stepping.h"
#include "dbg_stack.h"
#include "dbg_variables.h"
#include "dbg_protocol.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::diagnostics {

namespace {

// Message sequence numbers.
std::atomic<int> g_seq{1};

// Server state.
std::atomic<bool> g_running{false};
std::atomic<bool> g_client_attached{false};

// Buffer for JSON serialization (one per thread).
thread_local char g_json_buf[kDbgBufferSize];

// ── Response sending helper ────────────────────────────────────────────

void SendResponse(int request_seq, const char* command,
                  const char* body_json, int body_len, bool success)
{
    // Snapshot body to a local buffer before reusing g_json_buf,
    // since body_json often points into the same thread-local buffer.
    char body_copy[2048];
    int copy_len = 0;
    if (body_json && body_len > 0) {
        copy_len = body_len < (int)sizeof(body_copy) - 1 ? body_len : (int)sizeof(body_copy) - 1;
        memcpy(body_copy, body_json, copy_len);
        body_copy[copy_len] = '\0';
    }

    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.GetWriter().BeginObject();
    builder.GetWriter().Key("type");          builder.GetWriter().ValueString("response");
    builder.GetWriter().Key("request_seq");   builder.GetWriter().ValueInt(request_seq);
    builder.GetWriter().Key("success");       builder.GetWriter().ValueBool(success);
    builder.GetWriter().Key("command");       builder.GetWriter().ValueString(command);
    builder.GetWriter().Key("body");
    if (copy_len > 0) {
        builder.GetWriter().AppendRaw(body_copy, copy_len);
    } else {
        builder.GetWriter().ValueNull();
    }
    builder.GetWriter().EndObject();

    DbgTransportWriteMessage(builder.GetString(), builder.GetLength());
}

void SendEvent(const char* event_name, const char* body_json, int body_len)
{
    int seq = g_seq.fetch_add(1, std::memory_order_relaxed);
    // Snapshot body to local buffer before reusing g_json_buf.
    char body_copy[2048];
    int copy_len = 0;
    if (body_json && body_len > 0) {
        copy_len = body_len < (int)sizeof(body_copy) - 1 ? body_len : (int)sizeof(body_copy) - 1;
        memcpy(body_copy, body_json, copy_len);
        body_copy[copy_len] = '\0';
    }

    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.GetWriter().BeginObject();
    builder.GetWriter().Key("type");         builder.GetWriter().ValueString("event");
    builder.GetWriter().Key("seq");          builder.GetWriter().ValueInt(seq);
    builder.GetWriter().Key("event");        builder.GetWriter().ValueString(event_name);
    builder.GetWriter().Key("body");
    if (copy_len > 0) {
        builder.GetWriter().AppendRaw(body_copy, copy_len);
    } else {
        builder.GetWriter().ValueNull();
    }
    builder.GetWriter().EndObject();

    DbgTransportWriteMessage(builder.GetString(), builder.GetLength());
}

// ── JSON helper utilities ─────────────────────────────────────────────

/// Extract a JSON string value by key: "key":"value"
/// Returns true if found and copied; out is always null-terminated.
static bool ExtractJsonStringValue(const char* json, const char* key,
                                    char* out, int out_size) noexcept
{
    if (json == nullptr || key == nullptr || out_size <= 0) return false;
    out[0] = '\0';

    const char* found = strstr(json, key);
    if (!found) return false;
    const char* colon = strchr(found + strlen(key), ':');
    if (!colon) return false;

    const char* start = colon + 1;
    while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') ++start;
    if (*start != '"') return false;
    ++start;

    const char* end = strchr(start, '"');
    if (!end) return false;

    int len = static_cast<int>(end - start);
    if (len >= out_size) len = out_size - 1;
    memcpy(out, start, len);
    out[len] = '\0';
    return true;
}

/// Extract the last integer value matching a key within a bracketed scope.
/// Scans from `scope_start` up to the matching closing bracket.
static int ExtractIntInScope(const char* scope_start, char open_bracket,
                              char close_bracket, const char* key) noexcept
{
    if (scope_start == nullptr || key == nullptr) return 0;

    // Find the matching close bracket, tracking nesting.
    int depth = 1;
    const char* end = scope_start + 1;
    while (*end && depth > 0) {
        if (*end == open_bracket) ++depth;
        else if (*end == close_bracket) --depth;
        ++end;
    }

    // Now scan within [scope_start, end) for key.
    const char* found = strstr(scope_start, key);
    if (!found || found >= end) return 0;
    // Verify this key is a whole key (preceded by '"')
    if (found > scope_start && *(found - 1) != '"') return 0;

    const char* colon = strchr(found + strlen(key), ':');
    if (!colon || colon > end) return 0;

    char* num_end = nullptr;
    long val = strtol(colon + 1, &num_end, 10);
    if (num_end == colon + 1) return 0;
    return static_cast<int>(val);
}

/// Extract all "line" integer values from a JSON array of objects.
/// Handles: "breakpoints":[{"line":10},{"line":20}]
static int ExtractLineNumbers(const char* json, const char* array_key,
                               int* out_lines, int max_lines) noexcept
{
    if (json == nullptr || array_key == nullptr || out_lines == nullptr || max_lines <= 0) return 0;

    // Find the array key.
    const char* found = strstr(json, array_key);
    if (!found) return 0;
    const char* colon = strchr(found + strlen(array_key), ':');
    if (!colon) return 0;

    // Find the opening '[' of the array.
    const char* arr_start = colon + 1;
    while (*arr_start == ' ' || *arr_start == '\t' || *arr_start == '\n' || *arr_start == '\r') ++arr_start;
    if (*arr_start != '[') return 0;

    // Iterate through array elements looking for "line" values.
    int count = 0;
    const char* p = arr_start + 1;
    while (*p && *p != ']' && count < max_lines) {
        // Skip whitespace
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;

        if (*p == '{') {
            int line_val = ExtractIntInScope(p, '{', '}', "\"line\"");
            if (line_val > 0) {
                out_lines[count++] = line_val;
            }
            // Skip past this object.
            int depth = 0;
            while (*p) {
                if (*p == '{') ++depth;
                else if (*p == '}') { --depth; if (depth == 0) { ++p; break; } }
                ++p;
            }
        } else if (*p == ',') {
            ++p;
        } else {
            break;
        }
    }

    return count;
}

// ── Command handlers ───────────────────────────────────────────────────

void HandleInitialize(int request_seq) {
    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.BuildInitializeResponse();
    SendResponse(request_seq, "initialize", builder.GetString(), builder.GetLength(), true);
    g_client_attached.store(true, std::memory_order_release);

    // Send "initialized" event.
    SendEvent("initialized", nullptr, 0);
}

void HandleLaunch(int request_seq) {
    // Launch acknowledgment — nothing extra to configure.
    SendResponse(request_seq, "launch", "{}", 2, true);
}

void HandleSetBreakpoints(int request_seq, const char* args_json) {
    // ── 1. Parse source path from arguments ──────────────────────────
    // Find "source":{"...","path":"..."} block
    const char* source_key = strstr(args_json, "\"source\"");
    if (source_key == nullptr) {
        // No source — return empty breakpoint list.
        DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
        builder.BuildSetBreakpointsResponse({});
        SendResponse(request_seq, "setBreakpoints", builder.GetString(), builder.GetLength(), true);
        return;
    }

    // Extract "path" from within the source block.
    // Find the opening brace of the source object.
    const char* source_brace = strchr(source_key + strlen("\"source\""), '{');
    if (source_brace == nullptr) {
        DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
        builder.BuildSetBreakpointsResponse({});
        SendResponse(request_seq, "setBreakpoints", builder.GetString(), builder.GetLength(), true);
        return;
    }

    char source_path[512] = {};
    ExtractJsonStringValue(source_brace, "\"path\"", source_path, sizeof(source_path));

    // Fallback: if no path, try "name".
    char source_name[256] = {};
    if (source_path[0] == '\0') {
        ExtractJsonStringValue(source_brace, "\"name\"", source_name, sizeof(source_name));
    }

    const char* effective_source = source_path[0] != '\0' ? source_path
                                 : source_name[0] != '\0' ? source_name
                                 : nullptr;

    // ── 2. Parse breakpoint line numbers ─────────────────────────────
    int lines[256];
    // Try "breakpoints" array first, then fallback to "lines" (legacy).
    int line_count = ExtractLineNumbers(args_json, "\"breakpoints\"", lines, 256);
    if (line_count == 0) {
        line_count = ExtractLineNumbers(args_json, "\"lines\"", lines, 256);
    }

    // ── 3. Clear existing breakpoints for this source ────────────────
    if (effective_source != nullptr) {
        auto old_bps = DbgGetBreakpointsForSource(effective_source);
        for (const auto& bp : old_bps) {
            DbgRemoveBreakpoint(bp.id);
        }
    }

    // ── 4. Resolve methods and register breakpoints ──────────────────
    if (effective_source != nullptr && line_count > 0) {
        std::vector<DbgMethodMatch> matches;
        DbgResolveMethodsForSource(effective_source, matches);

        if (!matches.empty()) {
            // Register a breakpoint for each matching method at the
            // first requested line.  Without PDB/sequence-point info we
            // can only set breakpoints at IL offset 0 (method entry).
            int target_line = lines[0];

            // Mark g_dbg_any_breakpoints before registering.
            g_dbg_any_breakpoints.store(true, std::memory_order_release);

            for (const auto& match : matches) {
                DbgSetBreakpoint(match.method_token, 0,
                                 effective_source, target_line);
            }
        }
    }

    // ── 5. Build response with current breakpoints for this source ───
    std::vector<DapBreakpoint> result_bps;
    if (effective_source != nullptr) {
        result_bps = DbgGetBreakpointsForSource(effective_source);
    }

    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.BuildSetBreakpointsResponse(result_bps);
    SendResponse(request_seq, "setBreakpoints", builder.GetString(), builder.GetLength(), true);
}

void HandleSetFunctionBreakpoints(int request_seq, const char* args_json) {
    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.BuildSetBreakpointsResponse({});
    SendResponse(request_seq, "setFunctionBreakpoints", builder.GetString(), builder.GetLength(), true);
}

void HandleStackTrace(int request_seq) {
    auto frames = DbgGetStackTrace(1, 0, kDbgMaxStackFrames);
    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.BuildStackTraceResponse(frames);
    SendResponse(request_seq, "stackTrace", builder.GetString(), builder.GetLength(), true);
}

void HandleScopes(int request_seq, int frame_id) {
    auto scopes = DbgGetScopes(frame_id);
    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.BuildScopesResponse(scopes);
    SendResponse(request_seq, "scopes", builder.GetString(), builder.GetLength(), true);
}

void HandleVariables(int request_seq, int variables_reference) {
    auto vars = DbgGetVariables(variables_reference);
    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.BuildVariablesResponse(vars);
    SendResponse(request_seq, "variables", builder.GetString(), builder.GetLength(), true);
}

void HandleContinue(int request_seq) {
    // Clear stepping state so interpreter resumes.
    DbgEndStepping();
    DbgBreakpointInitialize();  // reset any pause state

    // Signal the interpreter thread to resume execution.
    DbgSignalResume();

    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.BuildContinueResponse();
    SendResponse(request_seq, "continue", builder.GetString(), builder.GetLength(), true);
}

void HandleNext(int request_seq) {
    int depth = DbgGetFrameDepth();
    DbgBeginStepping(1, StepMode::StepOver, depth);
    DbgSignalResume();
    SendResponse(request_seq, "next", "{}", 2, true);
}

void HandleStepIn(int request_seq) {
    DbgBeginStepping(1, StepMode::StepInto, 0);
    DbgSignalResume();
    SendResponse(request_seq, "stepIn", "{}", 2, true);
}

void HandleStepOut(int request_seq) {
    int depth = DbgGetFrameDepth();
    DbgBeginStepping(1, StepMode::StepOut, depth);
    DbgSignalResume();
    SendResponse(request_seq, "stepOut", "{}", 2, true);
}

void HandleThreads(int request_seq) {
    std::vector<DapThread> threads;
    DapThread t;
    t.id = 1;
    t.name = "Main Thread";
    threads.push_back(t);

    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.BuildThreadsResponse(threads);
    SendResponse(request_seq, "threads", builder.GetString(), builder.GetLength(), true);
}

void HandleDisconnect(int request_seq) {
    g_client_attached.store(false, std::memory_order_release);
    DbgEndStepping();
    DbgClearAllBreakpoints();
    SendResponse(request_seq, "disconnect", "{}", 2, true);
}

void HandleConfigurationDone(int request_seq) {
    SendResponse(request_seq, "configurationDone", "{}", 2, true);
}

void HandleEvaluate(int request_seq, const char* args_json) {
    // Parse the "expression" field from args.
    // Arg format: {"expression":"someVar","frameId":0,"context":"watch"}
    if (args_json == nullptr) {
        SendResponse(request_seq, "evaluate", "{\"result\":\"<no args>\",\"type\":\"string\"}", 40, true);
        return;
    }

    char expr[256];
    if (!ExtractJsonStringValue(args_json, "\"expression\"", expr, sizeof(expr))) {
        SendResponse(request_seq, "evaluate", "{\"result\":\"<no expression>\",\"type\":\"string\"}", 46, true);
        return;
    }

    // Look for the expression name in the frame snapshot locals.
    const auto& snap = DbgGetFrameSnapshot();
    char val_buf[128];
    char result_json[512];

    for (int i = 0; i < snap.local_count; ++i) {
        // Build local name: "local_N" or try matching by index-based name.
        char local_name[32];
        snprintf(local_name, sizeof(local_name), "local_%u", snap.locals[i].index);

        if (strcmp(expr, local_name) == 0) {
            // Match found — format the value.
            FormatValue(snap.locals[i].value, snap.locals[i].tag,
                        val_buf, sizeof(val_buf));
            const char* type = TagToTypeName(snap.locals[i].tag);
            snprintf(result_json, sizeof(result_json),
                     "{\"result\":\"%s\",\"type\":\"%s\",\"variablesReference\":0}",
                     val_buf, type);
            SendResponse(request_seq, "evaluate", result_json,
                         static_cast<int>(strlen(result_json)), true);
            return;
        }
    }

    // Also check if the expression is a numeric literal.
    char* end = nullptr;
    long num_val = strtol(expr, &end, 0);
    if (end != expr && *end == '\0') {
        snprintf(result_json, sizeof(result_json),
                 "{\"result\":\"%ld\",\"type\":\"int\",\"variablesReference\":0}",
                 num_val);
        SendResponse(request_seq, "evaluate", result_json,
                     static_cast<int>(strlen(result_json)), true);
        return;
    }

    // Not found — return error-like result.
    snprintf(result_json, sizeof(result_json),
             "{\"result\":\"<unknown: %s>\",\"type\":\"string\",\"variablesReference\":0}",
             expr);
    SendResponse(request_seq, "evaluate", result_json,
                 static_cast<int>(strlen(result_json)), true);
}

void HandlePause(int request_seq) {
    // Set the pause-requested flag.  The interpreter checks this flag
    // before each IL instruction in FastExecute via DbgShouldPause.
    // The next instruction boundary will trigger a stop.
    g_dbg_pause_requested.store(true, std::memory_order_release);

    // Also ensure g_dbg_any_breakpoints is visible so DbgShouldPause
    // doesn't fast-path-skip the pause check.
    g_dbg_any_breakpoints.store(true, std::memory_order_release);

    SendResponse(request_seq, "pause", "{}", 2, true);
}

// ── Message dispatch ───────────────────────────────────────────────────

void DispatchRequest(const char* json_body, int body_len) {
    // Simple dispatch: look for "command":"<name>" in the JSON.
    // Full JSON parsing is intentionally avoided — we only need the
    // command name and the request_seq.
    (void)body_len;

    // ── Helper: safe numeric extraction from JSON value ──────────────
    auto ExtractInt = [](const char* json, const char* key) -> int {
        const char* found = strstr(json, key);
        if (!found) return 0;
        const char* colon = strchr(found + strlen(key), ':');
        if (!colon) return 0;
        char* end = nullptr;
        long val = strtol(colon + 1, &end, 10);
        if (end == colon + 1) return 0;  // no digits
        return static_cast<int>(val);
    };

    // Extract request_seq.
    int request_seq = ExtractInt(json_body, "\"seq\"");

    // Extract command name.
    const char* cmd_key = strstr(json_body, "\"command\"");
    if (!cmd_key) {
        SendResponse(request_seq, "unknown", nullptr, 0, false);
        return;
    }
    const char* cmd_start = strchr(cmd_key + strlen("\"command\""), '"');
    if (!cmd_start) return;
    const char* cmd_end = strchr(cmd_start + 1, '"');
    if (!cmd_end) return;

    // Extract command string via local copy.
    size_t cmd_len = cmd_end - cmd_start - 1;
    char cmd_buf[64];
    if (cmd_len >= sizeof(cmd_buf)) cmd_len = sizeof(cmd_buf) - 1;
    memcpy(cmd_buf, cmd_start + 1, cmd_len);
    cmd_buf[cmd_len] = '\0';

    // Extract arguments JSON (raw, for commands that need it).
    const char* args_key = strstr(json_body, "\"arguments\"");
    const char* args_json = nullptr;
    if (args_key) {
        args_json = strchr(args_key + strlen("\"arguments\""), '{');
    }

    // Dispatch.
    if (strcmp(cmd_buf, "initialize") == 0) {
        HandleInitialize(request_seq);
    } else if (strcmp(cmd_buf, "launch") == 0) {
        HandleLaunch(request_seq);
    } else if (strcmp(cmd_buf, "setBreakpoints") == 0) {
        HandleSetBreakpoints(request_seq, args_json);
    } else if (strcmp(cmd_buf, "setFunctionBreakpoints") == 0) {
        HandleSetFunctionBreakpoints(request_seq, args_json);
    } else if (strcmp(cmd_buf, "stackTrace") == 0) {
        HandleStackTrace(request_seq);
    } else if (strcmp(cmd_buf, "scopes") == 0) {
        HandleScopes(request_seq, ExtractInt(json_body, "\"frameId\""));
    } else if (strcmp(cmd_buf, "variables") == 0) {
        HandleVariables(request_seq, ExtractInt(json_body, "\"variablesReference\""));
    } else if (strcmp(cmd_buf, "continue") == 0) {
        HandleContinue(request_seq);
    } else if (strcmp(cmd_buf, "next") == 0) {
        HandleNext(request_seq);
    } else if (strcmp(cmd_buf, "stepIn") == 0) {
        HandleStepIn(request_seq);
    } else if (strcmp(cmd_buf, "stepOut") == 0) {
        HandleStepOut(request_seq);
    } else if (strcmp(cmd_buf, "threads") == 0) {
        HandleThreads(request_seq);
    } else if (strcmp(cmd_buf, "disconnect") == 0) {
        HandleDisconnect(request_seq);
    } else if (strcmp(cmd_buf, "configurationDone") == 0) {
        HandleConfigurationDone(request_seq);
    } else if (strcmp(cmd_buf, "evaluate") == 0) {
        HandleEvaluate(request_seq, args_json);
    } else if (strcmp(cmd_buf, "pause") == 0) {
        HandlePause(request_seq);
    } else {
        // Unknown command — acknowledge anyway.
        SendResponse(request_seq, cmd_buf, "{}", 2, true);
    }
}

}  // anonymous namespace

// ── Public API ─────────────────────────────────────────────────────────

void DbgServerInitialize() noexcept {
    g_running.store(false, std::memory_order_release);
    g_client_attached.store(false, std::memory_order_release);
    g_seq.store(1, std::memory_order_relaxed);
}

void DbgServerShutdown() noexcept {
    g_running.store(false, std::memory_order_release);
    g_client_attached.store(false, std::memory_order_release);
}

void DbgServerRun() noexcept {
    g_running.store(true, std::memory_order_release);

    char msg_buf[kDbgBufferSize];
    while (g_running.load(std::memory_order_acquire) &&
           DbgTransportIsConnected())
    {
        // Read a DAP message.
        if (!DbgTransportReadMessage(msg_buf, kDbgBufferSize)) {
            break;  // client disconnected
        }

        // Dispatch.
        DispatchRequest(msg_buf, static_cast<int>(strlen(msg_buf)));
    }

    g_running.store(false, std::memory_order_release);
    g_client_attached.store(false, std::memory_order_release);
}

void DbgServerSendStopped(DapStoppedReason reason, int thread_id) noexcept {
    DapMessageBuilder builder(g_json_buf, kDbgBufferSize);
    builder.BuildStoppedEvent(reason, thread_id);
    SendEvent("stopped", builder.GetString(), builder.GetLength());
}

void DbgServerSignalStop() noexcept {
    g_running.store(false, std::memory_order_release);
}

// ── Test-only entry point ─────────────────────────────────────────────
void DbgServerDispatchTestRequest(const char* json_body, int body_len) noexcept {
    DispatchRequest(json_body, body_len);
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER
