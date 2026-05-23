// dbg_protocol.h — DAP (Debug Adapter Protocol) message definitions
//
// Minimal subset of the DAP protocol needed for IL-level debugging:
//   initialize / initialized / launch / setBreakpoints / setFunctionBreakpoints
//   stackTrace / scopes / variables / threads / continue / next / stepIn
//   stepOut / stopped / exited / terminate / disconnect
//
// Uses a lightweight JSON writer (no external dependency).
// Full DAP spec: https://microsoft.github.io/debug-adapter-protocol/

#ifndef CHAOS_IL2CPP_DBG_PROTOCOL_H_
#define CHAOS_IL2CPP_DBG_PROTOCOL_H_

#include "dbg_config.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace chaos::il2cpp::diagnostics {

// ── DAP content types ─────────────────────────────────────────────────

struct DapSource {
    std::string name;
    std::string path;
    int source_reference = 0;
};

struct DapBreakpoint {
    int id = 0;
    bool verified = false;
    std::string message;
    std::string source_path;
    int line = 0;
    int column = 0;
};

struct DapStackFrame {
    int id = 0;
    std::string name;
    std::string source_path;
    int line = 0;
    int column = 0;
};

struct DapScope {
    std::string name;
    int variables_reference = 0;
    bool expensive = false;
};

struct DapVariable {
    std::string name;
    std::string value;
    std::string type;
    int variables_reference = 0;
};

struct DapThread {
    int id = 0;
    std::string name;
};

// ── Stopped reason enum ────────────────────────────────────────────────

enum class DapStoppedReason {
    Step,
    Breakpoint,
    Exception,
    Pause,
    Entry,
};

// ── Lightweight JSON writer ────────────────────────────────────────────
//
// Builds JSON strings without external dependencies.
// Allocates on a pre-sized buffer.

class JsonWriter {
public:
    explicit JsonWriter(char* buffer, int capacity)
        : buf_(buffer), cap_(capacity), pos_(0) {
        if (cap_ > 0) buf_[0] = '\0';
    }

    void BeginObject() { WriteSep(); Append("{"); }
    void EndObject() { Append("}"); needs_sep_ = true; }
    void BeginArray() { WriteSep(); Append("["); }
    void EndArray() { Append("]"); needs_sep_ = true; }

    void Key(const char* key) {
        if (needs_sep_) { Append(","); needs_sep_ = false; }
        Append("\"");
        Append(key);
        Append("\":");
    }

    void ValueNull() { WriteSep(); Append("null"); needs_sep_ = true; }
    void ValueBool(bool v) { WriteSep(); Append(v ? "true" : "false"); needs_sep_ = true; }
    void ValueInt(int64_t v) {
        WriteSep();
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "%lld", (long long)v);
        Append(tmp);
        needs_sep_ = true;
    }
    void ValueUint(uint64_t v) {
        WriteSep();
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "%llu", (unsigned long long)v);
        Append(tmp);
        needs_sep_ = true;
    }
    void ValueString(const char* s) {
        WriteSep();
        Append("\"");
        // Escape JSON special characters.
        for (const char* p = s; *p; ++p) {
            char c = *p;
            switch (c) {
            case '"': Append("\\\""); break;
            case '\\': Append("\\\\"); break;
            case '\n': Append("\\n"); break;
            case '\r': Append("\\r"); break;
            case '\t': Append("\\t"); break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char hex[8];
                    snprintf(hex, sizeof(hex), "\\u%04x", c);
                    Append(hex);
                } else {
                    AppendChar(c);
                }
                break;
            }
        }
        Append("\"");
        needs_sep_ = true;
    }
    void ValueString(const std::string& s) { ValueString(s.c_str()); }

    /// Append raw JSON content (already valid JSON, no escaping).
    /// Uses memmove to handle overlapping source/destination (common when
    /// the source is from the same buffer, e.g. SendResponse wrapping
    /// a body that was built on the same thread-local buffer).
    void AppendRaw(const char* raw, int len) {
        WriteSep();
        if (pos_ + len < cap_) {
            memmove(buf_ + pos_, raw, len);
            pos_ += len;
            buf_[pos_] = '\0';
        }
        needs_sep_ = true;
    }

    /// Get the built JSON string.
    const char* GetString() const { return buf_; }
    int GetLength() const { return pos_; }

    /// Reset for reuse.
    void Reset() { pos_ = 0; needs_sep_ = false; if (cap_ > 0) buf_[0] = '\0'; }

private:
    void WriteSep() {
        if (needs_sep_) Append(",");
        needs_sep_ = false;
    }

    void Append(const char* s) {
        int len = static_cast<int>(strlen(s));
        if (pos_ + len < cap_) {
            memcpy(buf_ + pos_, s, len);
            pos_ += len;
            buf_[pos_] = '\0';
        }
    }
    void AppendChar(char c) {
        if (pos_ + 1 < cap_) {
            buf_[pos_++] = c;
            buf_[pos_] = '\0';
        }
    }

    char* buf_;
    int cap_;
    int pos_;
    bool needs_sep_ = false;
};

// ── DAP Message ────────────────────────────────────────────────────────
//
// Represents a single DAP request/response/event.

struct DapMessage {
    int seq = 0;
    std::string type;       // "request", "response", "event"
    std::string command;    // for requests/responses
    std::string event_name; // for events

    // For requests: raw arguments JSON (parsed on demand).
    std::string arguments_json;

    // For responses.
    bool success = true;
    std::string body_json;

    // Protocol header fields (Content-Length).
    int content_length = 0;
};

// ── DAP message builder ────────────────────────────────────────────────

class DapMessageBuilder {
public:
    explicit DapMessageBuilder(char* buffer, int capacity)
        : writer_(buffer, capacity) {}

    /// Build an "initialize" response body.
    void BuildInitializeResponse(bool supports_breakpoints = true) {
        writer_.BeginObject();
        writer_.Key("supportsConfigurationDoneRequest"); writer_.ValueBool(true);
        writer_.Key("supportsSetVariable"); writer_.ValueBool(false);
        writer_.Key("supportsConditionalBreakpoints"); writer_.ValueBool(false);
        writer_.Key("supportsFunctionBreakpoints"); writer_.ValueBool(true);
        writer_.Key("supportsStepBack"); writer_.ValueBool(false);
        writer_.EndObject();
    }

    /// Build a "stopped" event body.
    void BuildStoppedEvent(DapStoppedReason reason, int thread_id, const char* description = nullptr) {
        writer_.BeginObject();
        writer_.Key("reason");
        switch (reason) {
        case DapStoppedReason::Step:        writer_.ValueString("step"); break;
        case DapStoppedReason::Breakpoint:  writer_.ValueString("breakpoint"); break;
        case DapStoppedReason::Exception:   writer_.ValueString("exception"); break;
        case DapStoppedReason::Pause:       writer_.ValueString("pause"); break;
        case DapStoppedReason::Entry:       writer_.ValueString("entry"); break;
        }
        writer_.Key("threadId"); writer_.ValueInt(thread_id);
        if (description) {
            writer_.Key("description"); writer_.ValueString(description);
        }
        writer_.Key("allThreadsStopped"); writer_.ValueBool(true);
        writer_.EndObject();
    }

    /// Build a "threads" response body.
    void BuildThreadsResponse(const std::vector<DapThread>& threads) {
        writer_.BeginObject();
        writer_.Key("threads");
        writer_.BeginArray();
        for (const auto& t : threads) {
            writer_.BeginObject();
            writer_.Key("id"); writer_.ValueInt(t.id);
            writer_.Key("name"); writer_.ValueString(t.name);
            writer_.EndObject();
        }
        writer_.EndArray();
        writer_.EndObject();
    }

    /// Build a "stackTrace" response body.
    void BuildStackTraceResponse(const std::vector<DapStackFrame>& frames) {
        writer_.BeginObject();
        writer_.Key("stackFrames");
        writer_.BeginArray();
        for (const auto& f : frames) {
            writer_.BeginObject();
            writer_.Key("id"); writer_.ValueInt(f.id);
            writer_.Key("name"); writer_.ValueString(f.name);
            writer_.Key("line"); writer_.ValueInt(f.line);
            writer_.Key("column"); writer_.ValueInt(f.column);
            if (!f.source_path.empty()) {
                writer_.Key("source");
                writer_.BeginObject();
                writer_.Key("name"); writer_.ValueString(f.source_path);
                writer_.Key("path"); writer_.ValueString(f.source_path);
                writer_.EndObject();
            }
            writer_.EndObject();
        }
        writer_.EndArray();
        writer_.Key("totalFrames"); writer_.ValueInt(static_cast<int>(frames.size()));
        writer_.EndObject();
    }

    /// Build a "scopes" response body.
    void BuildScopesResponse(const std::vector<DapScope>& scopes) {
        writer_.BeginObject();
        writer_.Key("scopes");
        writer_.BeginArray();
        for (const auto& s : scopes) {
            writer_.BeginObject();
            writer_.Key("name"); writer_.ValueString(s.name);
            writer_.Key("variablesReference"); writer_.ValueInt(s.variables_reference);
            writer_.Key("expensive"); writer_.ValueBool(s.expensive);
            writer_.EndObject();
        }
        writer_.EndArray();
        writer_.EndObject();
    }

    /// Build a "variables" response body.
    void BuildVariablesResponse(const std::vector<DapVariable>& variables) {
        writer_.BeginObject();
        writer_.Key("variables");
        writer_.BeginArray();
        for (const auto& v : variables) {
            writer_.BeginObject();
            writer_.Key("name"); writer_.ValueString(v.name);
            writer_.Key("value"); writer_.ValueString(v.value);
            writer_.Key("type"); writer_.ValueString(v.type);
            if (v.variables_reference > 0) {
                writer_.Key("variablesReference"); writer_.ValueInt(v.variables_reference);
            }
            writer_.EndObject();
        }
        writer_.EndArray();
        writer_.EndObject();
    }

    /// Build a "setBreakpoints" response body.
    void BuildSetBreakpointsResponse(const std::vector<DapBreakpoint>& breakpoints) {
        writer_.BeginObject();
        writer_.Key("breakpoints");
        writer_.BeginArray();
        for (const auto& b : breakpoints) {
            writer_.BeginObject();
            writer_.Key("id"); writer_.ValueInt(b.id);
            writer_.Key("verified"); writer_.ValueBool(b.verified);
            writer_.Key("line"); writer_.ValueInt(b.line);
            writer_.Key("column"); writer_.ValueInt(b.column);
            if (!b.message.empty()) {
                writer_.Key("message"); writer_.ValueString(b.message);
            }
            writer_.EndObject();
        }
        writer_.EndArray();
        writer_.EndObject();
    }

    /// Build a "continue" response body (no-op body).
    void BuildContinueResponse() {
        writer_.BeginObject();
        writer_.Key("allThreadsContinued"); writer_.ValueBool(true);
        writer_.EndObject();
    }

    /// Get the built JSON string.
    const char* GetString() const { return writer_.GetString(); }
    int GetLength() const { return writer_.GetLength(); }

    /// Get the underlying writer for custom JSON composition.
    JsonWriter& GetWriter() { return writer_; }

private:
    JsonWriter writer_;
};

// ── DAP Wire Format ────────────────────────────────────────────────────
//
// DAP uses a HTTP-like header with Content-Length followed by JSON:
//   Content-Length: <N>\r\n\r\n<JSON body of N bytes>

inline std::string DapEncodeMessage(const char* json_body, int body_len) {
    char header[64];
    int header_len = snprintf(header, sizeof(header),
                              "Content-Length: %d\r\n\r\n", body_len);
    return std::string(header, header_len) + std::string(json_body, body_len);
}

inline int DapParseHeader(const char* data, int data_len) {
    // Look for "Content-Length: <N>\r\n\r\n" and extract N.
    const char* content_key = "Content-Length: ";
    const char* found = strstr(data, content_key);
    if (!found) return -1;

    const char* num_start = found + strlen(content_key);
    int len = 0;
    while (num_start < data + data_len && *num_start >= '0' && *num_start <= '9') {
        len = len * 10 + (*num_start - '0');
        ++num_start;
    }

    // Verify we have "\r\n\r\n" after the length.
    const char* delim = strstr(found, "\r\n\r\n");
    if (!delim) return -1;

    return len;
}

inline const char* DapFindJsonStart(const char* data, int data_len) {
    const char* delim = strstr(data, "\r\n\r\n");
    if (!delim) return nullptr;
    return delim + 4;
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER

#endif  // CHAOS_IL2CPP_DBG_PROTOCOL_H_
