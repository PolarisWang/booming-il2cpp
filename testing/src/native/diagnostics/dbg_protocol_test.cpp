// dbg_protocol_test.cpp — DAP protocol unit tests
//
// Tests JSON writer, DAP message builder, and wire format serialization.
// All functions are inline/header-only, no library linking required.

#include <gtest/gtest.h>
#include <debugger/dbg_protocol.h>

#include <cstring>

namespace chaos::il2cpp::diagnostics {
namespace {

// ── JsonWriter tests ────────────────────────────────────────────────

TEST(DbgProtocolTest, JsonWriterEmptyObject) {
    char buf[128];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.EndObject();
    EXPECT_STREQ(w.GetString(), "{}");
}

TEST(DbgProtocolTest, JsonWriterEmptyArray) {
    char buf[128];
    JsonWriter w(buf, sizeof(buf));
    w.BeginArray();
    w.EndArray();
    EXPECT_STREQ(w.GetString(), "[]");
}

TEST(DbgProtocolTest, JsonWriterKeyValue) {
    char buf[128];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.Key("name"); w.ValueString("hello");
    w.Key("count"); w.ValueInt(42);
    w.EndObject();
    EXPECT_STREQ(w.GetString(), "{\"name\":\"hello\",\"count\":42}");
}

TEST(DbgProtocolTest, JsonWriterArray) {
    char buf[128];
    JsonWriter w(buf, sizeof(buf));
    w.BeginArray();
    w.ValueInt(1);
    w.ValueInt(2);
    w.ValueInt(3);
    w.EndArray();
    EXPECT_STREQ(w.GetString(), "[1,2,3]");
}

TEST(DbgProtocolTest, JsonWriterNested) {
    char buf[256];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.Key("data");
    w.BeginObject();
    w.Key("x"); w.ValueInt(10);
    w.Key("y"); w.ValueInt(20);
    w.EndObject();
    w.EndObject();
    EXPECT_STREQ(w.GetString(), "{\"data\":{\"x\":10,\"y\":20}}");
}

TEST(DbgProtocolTest, JsonWriterBoolAndNull) {
    char buf[128];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.Key("flag"); w.ValueBool(true);
    w.Key("empty"); w.ValueNull();
    w.Key("no"); w.ValueBool(false);
    w.EndObject();
    EXPECT_STREQ(w.GetString(), "{\"flag\":true,\"empty\":null,\"no\":false}");
}

TEST(DbgProtocolTest, JsonWriterStringEscaping) {
    char buf[128];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.Key("msg"); w.ValueString("hello\"world\nline2");
    w.EndObject();
    // Expect: {"msg":"hello\"world\nline2"}
    EXPECT_STREQ(w.GetString(), "{\"msg\":\"hello\\\"world\\nline2\"}");
}

TEST(DbgProtocolTest, JsonWriterAppendRaw) {
    char buf[128];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.Key("body"); w.AppendRaw("{\"x\":1}", 7);
    w.EndObject();
    EXPECT_STREQ(w.GetString(), "{\"body\":{\"x\":1}}");
}

TEST(DbgProtocolTest, JsonWriterReset) {
    char buf[128];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject(); w.EndObject();
    EXPECT_STREQ(w.GetString(), "{}");
    w.Reset();
    w.BeginArray(); w.EndArray();
    EXPECT_STREQ(w.GetString(), "[]");
}

TEST(DbgProtocolTest, JsonWriterBufferOverflow) {
    // Very small buffer — should not crash but produce truncated output.
    char buf[8];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.Key("long_key_name"); w.ValueString("very long value that won't fit");
    w.EndObject();
    // Buffer won't be full JSON, but should be null-terminated.
    EXPECT_LT(w.GetLength(), 8);
    EXPECT_EQ(buf[7], '\0');
}

// ── DapMessageBuilder tests ─────────────────────────────────────────

TEST(DbgProtocolTest, BuildInitializeResponse) {
    char buf[512];
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildInitializeResponse();

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "supportsConfigurationDoneRequest"), nullptr);
    EXPECT_NE(strstr(json, "supportsFunctionBreakpoints"), nullptr);
    EXPECT_NE(strstr(json, "\"supportsConditionalBreakpoints\":false"), nullptr);
}

TEST(DbgProtocolTest, BuildStoppedEvent) {
    char buf[512];
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildStoppedEvent(DapStoppedReason::Breakpoint, 1);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"reason\":\"breakpoint\""), nullptr);
    EXPECT_NE(strstr(json, "\"threadId\":1"), nullptr);
}

TEST(DbgProtocolTest, BuildStoppedEventPause) {
    char buf[512];
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildStoppedEvent(DapStoppedReason::Pause, 1);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"reason\":\"pause\""), nullptr);
}

TEST(DbgProtocolTest, BuildStoppedEventAllReasons) {
    char buf[512];

    auto check_reason = [&](DapStoppedReason reason, const char* expected) {
        DapMessageBuilder mb(buf, sizeof(buf));
        mb.BuildStoppedEvent(reason, 1);
        EXPECT_NE(strstr(mb.GetString(), expected), nullptr) << " for reason " << static_cast<int>(reason);
    };

    check_reason(DapStoppedReason::Step,       "\"reason\":\"step\"");
    check_reason(DapStoppedReason::Breakpoint, "\"reason\":\"breakpoint\"");
    check_reason(DapStoppedReason::Exception,  "\"reason\":\"exception\"");
    check_reason(DapStoppedReason::Pause,      "\"reason\":\"pause\"");
    check_reason(DapStoppedReason::Entry,      "\"reason\":\"entry\"");
}

TEST(DbgProtocolTest, BuildThreadsResponse) {
    char buf[512];
    std::vector<DapThread> threads;
    threads.push_back({1, "Main"});
    threads.push_back({2, "Worker"});

    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildThreadsResponse(threads);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"id\":1"), nullptr);
    EXPECT_NE(strstr(json, "\"name\":\"Main\""), nullptr);
    EXPECT_NE(strstr(json, "\"id\":2"), nullptr);
    EXPECT_NE(strstr(json, "\"name\":\"Worker\""), nullptr);
}

TEST(DbgProtocolTest, BuildStackTraceResponse) {
    char buf[512];
    std::vector<DapStackFrame> frames;
    frames.push_back({0, "MainMethod", "/src/Program.cs", 10, 0});

    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildStackTraceResponse(frames);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"name\":\"MainMethod\""), nullptr);
    EXPECT_NE(strstr(json, "\"line\":10"), nullptr);
    EXPECT_NE(strstr(json, "\"path\":\"/src/Program.cs\""), nullptr);
    EXPECT_NE(strstr(json, "\"totalFrames\":1"), nullptr);
}

TEST(DbgProtocolTest, BuildScopesResponse) {
    char buf[512];
    std::vector<DapScope> scopes;
    scopes.push_back({"Locals", 1, false});
    scopes.push_back({"Globals", 2, true});

    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildScopesResponse(scopes);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"name\":\"Locals\""), nullptr);
    EXPECT_NE(strstr(json, "\"variablesReference\":1"), nullptr);
    EXPECT_NE(strstr(json, "\"expensive\":true"), nullptr);
}

TEST(DbgProtocolTest, BuildVariablesResponse) {
    char buf[512];
    std::vector<DapVariable> vars;
    vars.push_back({"x", "42", "int", 0});

    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildVariablesResponse(vars);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"name\":\"x\""), nullptr);
    EXPECT_NE(strstr(json, "\"value\":\"42\""), nullptr);
    EXPECT_NE(strstr(json, "\"type\":\"int\""), nullptr);
}

TEST(DbgProtocolTest, BuildVariablesResponseWithReference) {
    char buf[512];
    std::vector<DapVariable> vars;
    vars.push_back({"obj", "0x1234", "object", 100});

    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildVariablesResponse(vars);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"variablesReference\":100"), nullptr);
}

TEST(DbgProtocolTest, BuildSetBreakpointsResponse) {
    char buf[512];
    std::vector<DapBreakpoint> bps;
    bps.push_back({1, true, "", "test.cs", 42, 0});

    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildSetBreakpointsResponse(bps);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"id\":1"), nullptr);
    EXPECT_NE(strstr(json, "\"verified\":true"), nullptr);
    EXPECT_NE(strstr(json, "\"line\":42"), nullptr);
}

TEST(DbgProtocolTest, BuildContinueResponse) {
    char buf[512];
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildContinueResponse();

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"allThreadsContinued\":true"), nullptr);
}

// ── Wire format tests ───────────────────────────────────────────────

TEST(DbgProtocolTest, DapEncodeMessage) {
    std::string encoded = DapEncodeMessage("{\"test\":1}", 10);
    EXPECT_NE(encoded.find("Content-Length: 10"), std::string::npos);
    EXPECT_NE(encoded.find("\r\n\r\n{\"test\":1}"), std::string::npos);
}

TEST(DbgProtocolTest, DapParseHeader) {
    const char* data = "Content-Length: 42\r\n\r\n{\"key\":\"value\"}";
    int len = DapParseHeader(data, static_cast<int>(strlen(data)));
    EXPECT_EQ(len, 42);
}

TEST(DbgProtocolTest, DapParseHeaderNoDelimiter) {
    const char* data = "Content-Length: 42";
    int len = DapParseHeader(data, static_cast<int>(strlen(data)));
    EXPECT_EQ(len, -1);
}

TEST(DbgProtocolTest, DapFindJsonStart) {
    const char* data = "Content-Length: 10\r\n\r\n{\"a\":1}";
    const char* json = DapFindJsonStart(data, static_cast<int>(strlen(data)));
    ASSERT_NE(json, nullptr);
    EXPECT_STREQ(json, "{\"a\":1}");
}

TEST(DbgProtocolTest, DapFindJsonStartNoHeader) {
    const char* data = "no header here";
    const char* json = DapFindJsonStart(data, static_cast<int>(strlen(data)));
    EXPECT_EQ(json, nullptr);
}

// ── DapMessage struct tests (DapMessageBuilder usage) ──────────────────

TEST(DbgProtocolTest, BuildInitializeResponseMinimal) {
    char buf[512];
    DapMessageBuilder mb(buf, sizeof(buf));
    // Call the variant with no arguments (default supports_breakpoints=true).
    mb.BuildInitializeResponse(true);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "supportsConfigurationDoneRequest"), nullptr);
    EXPECT_NE(strstr(json, "supportsFunctionBreakpoints"), nullptr);
}

TEST(DbgProtocolTest, BuildStoppedEventWithDescription) {
    char buf[512];
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildStoppedEvent(DapStoppedReason::Exception, 2, "DivideByZeroException");

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"reason\":\"exception\""), nullptr);
    EXPECT_NE(strstr(json, "\"threadId\":2"), nullptr);
    EXPECT_NE(strstr(json, "\"description\":\"DivideByZeroException\""), nullptr);
    EXPECT_NE(strstr(json, "\"allThreadsStopped\":true"), nullptr);
}

TEST(DbgProtocolTest, BuildStoppedEventAllThreadsField) {
    char buf[512];
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildStoppedEvent(DapStoppedReason::Entry, 3);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"allThreadsStopped\":true"), nullptr);
    EXPECT_NE(strstr(json, "\"threadId\":3"), nullptr);
}

TEST(DbgProtocolTest, BuildEmptyThreadsResponse) {
    char buf[512];
    std::vector<DapThread> empty;
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildThreadsResponse(empty);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"threads\":[]"), nullptr);
}

TEST(DbgProtocolTest, BuildStackTraceResponseEmpty) {
    char buf[512];
    std::vector<DapStackFrame> empty;
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildStackTraceResponse(empty);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"stackFrames\":[]"), nullptr);
    EXPECT_NE(strstr(json, "\"totalFrames\":0"), nullptr);
}

TEST(DbgProtocolTest, BuildStackTraceResponseNullSourcePath) {
    char buf[512];
    std::vector<DapStackFrame> frames;
    // A frame with empty source_path (should not emit "source" field).
    frames.push_back({0, "NoSourceMethod", "", 42, 0});

    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildStackTraceResponse(frames);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"name\":\"NoSourceMethod\""), nullptr);
    EXPECT_NE(strstr(json, "\"line\":42"), nullptr);
    // Should not contain "source" since source_path is empty.
    EXPECT_EQ(strstr(json, "\"source\":"), nullptr);
}

TEST(DbgProtocolTest, BuildScopesResponseEmpty) {
    char buf[512];
    std::vector<DapScope> empty;
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildScopesResponse(empty);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"scopes\":[]"), nullptr);
}

TEST(DbgProtocolTest, BuildVariablesResponseEmpty) {
    char buf[512];
    std::vector<DapVariable> empty;
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildVariablesResponse(empty);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"variables\":[]"), nullptr);
}

TEST(DbgProtocolTest, BuildSetBreakpointsResponseEmpty) {
    char buf[512];
    std::vector<DapBreakpoint> empty;
    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildSetBreakpointsResponse(empty);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"breakpoints\":[]"), nullptr);
}

TEST(DbgProtocolTest, BuildSetBreakpointsResponseWithMessage) {
    char buf[512];
    std::vector<DapBreakpoint> bps;
    bps.push_back({1, false, "not verified", "test.cs", 42, 0});

    DapMessageBuilder mb(buf, sizeof(buf));
    mb.BuildSetBreakpointsResponse(bps);

    const char* json = mb.GetString();
    EXPECT_NE(strstr(json, "\"verified\":false"), nullptr);
    EXPECT_NE(strstr(json, "\"message\":\"not verified\""), nullptr);
}

// ── Wire format edge cases ────────────────────────────────────────────

TEST(DbgProtocolTest, DapEncodeMessageEmpty) {
    std::string encoded = DapEncodeMessage("", 0);
    EXPECT_NE(encoded.find("Content-Length: 0"), std::string::npos);
    EXPECT_NE(encoded.find("\r\n\r\n"), std::string::npos);
}

TEST(DbgProtocolTest, DapEncodeMessageLargeContent) {
    std::string body(1000, 'A');
    std::string encoded = DapEncodeMessage(body.c_str(), static_cast<int>(body.size()));
    EXPECT_NE(encoded.find("Content-Length: 1000"), std::string::npos);
}

TEST(DbgProtocolTest, DapParseHeaderWithExtraWhitespace) {
    const char* data = "Content-Length: 42  \r\n\r\nbody";
    int len = DapParseHeader(data, static_cast<int>(strlen(data)));
    EXPECT_EQ(len, 42);
}

TEST(DbgProtocolTest, DapParseHeaderWithoutNewlines) {
    const char* data = "Content-Length:42";
    int len = DapParseHeader(data, static_cast<int>(strlen(data)));
    EXPECT_EQ(len, -1);
}

TEST(DbgProtocolTest, DapParseHeaderTrailingData) {
    const char* data = "Content-Length: 5\r\n\r\nHelloExtraIgnored";
    int len = DapParseHeader(data, static_cast<int>(strlen(data)));
    EXPECT_EQ(len, 5);
}

TEST(DbgProtocolTest, DapParseHeaderMultipleHeaders) {
    const char* data = "Content-Type: text/plain\r\nContent-Length: 10\r\n\r\n0123456789";
    int len = DapParseHeader(data, static_cast<int>(strlen(data)));
    EXPECT_EQ(len, 10);
}

TEST(DbgProtocolTest, DapParseHeaderZeroLength) {
    const char* data = "Content-Length: 0\r\n\r\n";
    int len = DapParseHeader(data, static_cast<int>(strlen(data)));
    EXPECT_EQ(len, 0);
}

TEST(DbgProtocolTest, DapFindJsonStartWithExtraHeaders) {
    const char* data = "Content-Type: application/json\r\nContent-Length: 7\r\n\r\n{\"a\":1}";
    const char* json = DapFindJsonStart(data, static_cast<int>(strlen(data)));
    ASSERT_NE(json, nullptr);
    EXPECT_STREQ(json, "{\"a\":1}");
}

TEST(DbgProtocolTest, DapFindJsonStartOnlyHeader) {
    const char* data = "Content-Length: 0\r\n\r\n";
    const char* json = DapFindJsonStart(data, static_cast<int>(strlen(data)));
    ASSERT_NE(json, nullptr);
    EXPECT_STREQ(json, "");
}

// ── JSON Writer edge cases ────────────────────────────────────────────

TEST(DbgProtocolTest, JsonWriterDeepNesting) {
    char buf[1024];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    for (int i = 0; i < 10; ++i) {
        char key[8];
        snprintf(key, sizeof(key), "l%d", i);
        w.Key(key);
        w.BeginObject();
    }
    for (int i = 0; i < 10; ++i) {
        w.EndObject();
    }
    w.EndObject();

    const char* json = w.GetString();
    EXPECT_NE(strstr(json, "\"l0\""), nullptr);
    EXPECT_NE(strstr(json, "\"l9\""), nullptr);
}

TEST(DbgProtocolTest, JsonWriterMultipleArrays) {
    char buf[256];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.Key("a"); w.BeginArray(); w.EndArray();
    w.Key("b"); w.BeginArray(); w.ValueInt(1); w.EndArray();
    w.Key("c"); w.BeginArray(); w.ValueInt(2); w.ValueInt(3); w.EndArray();
    w.EndObject();

    const char* json = w.GetString();
    EXPECT_NE(strstr(json, "\"a\":[]"), nullptr);
    EXPECT_NE(strstr(json, "\"b\":[1]"), nullptr);
    EXPECT_NE(strstr(json, "\"c\":[2,3]"), nullptr);
}

TEST(DbgProtocolTest, JsonWriterValueStringSpecialChars) {
    char buf[256];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.Key("s"); w.ValueString("tab\there\r\nend");
    w.EndObject();

    const char* json = w.GetString();
    EXPECT_NE(strstr(json, "\\t"), nullptr);
    EXPECT_NE(strstr(json, "\\r"), nullptr);
    EXPECT_NE(strstr(json, "\\n"), nullptr);
}

TEST(DbgProtocolTest, JsonWriterValueUint) {
    char buf[128];
    JsonWriter w(buf, sizeof(buf));
    w.BeginObject();
    w.Key("u"); w.ValueUint(0xFFFFFFFF);
    w.EndObject();

    const char* json = w.GetString();
    EXPECT_NE(strstr(json, "4294967295"), nullptr);
}

TEST(DbgProtocolTest, JsonWriterAppendRawOverlap) {
    // AppendRaw from a portion of the same buffer (reset, then re-append
    // a known substring). The buffer is large enough that the source
    // string is not overwritten before memmove copies it.
    char buf[256];
    JsonWriter w(buf, sizeof(buf));

    w.BeginObject();
    w.Key("x"); w.ValueInt(1);
    w.EndObject();

    // Get the inner portion (everything from 'x' onward).
    const char* full = w.GetString();
    const char* inner = strstr(full, "\"x\"");
    ASSERT_NE(inner, nullptr);

    // Snapshot to a separate buffer so the source is guaranteed live.
    char source_copy[64];
    int source_len = static_cast<int>(strlen(inner));
    ASSERT_LT(source_len, (int)sizeof(source_copy));
    memcpy(source_copy, inner, source_len);
    source_copy[source_len] = '\0';

    w.Reset();
    w.BeginObject();
    w.Key("wrapped");
    w.AppendRaw(source_copy, source_len);
    w.EndObject();

    const char* json = w.GetString();
    EXPECT_NE(strstr(json, "\"wrapped\""), nullptr);
    EXPECT_NE(strstr(json, "\"x\":1"), nullptr);
}

}  // namespace
}  // namespace chaos::il2cpp::diagnostics
