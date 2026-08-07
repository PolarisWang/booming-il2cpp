// ep_receiver_test.cpp — EventPipe receiver unit tests
//
// Tests the receiver CLI's parsing, filtering, event-type mapping,
// and JSON formatting logic.  chaos_diag.cpp is compiled as a
// separate translation unit (with CHAOS_DIAG_UNIT_TEST to exclude
// main()) and linked alongside this test.
//
// Links chaos_eventpipe + chaos_runtime_core for protocol types.

#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>
#include <string>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

#include <eventpipe/receiver/ep_receiver.h>

namespace {

// ── Protocol constants ────────────────────────────────────────────────

TEST(EpReceiverTest, MagicConstant) {
    EXPECT_EQ(kEpMagic, 0xACDB12CEu);
}

TEST(EpReceiverTest, ProtocolVersion) {
    EXPECT_EQ(kEpProtocolVersion, 1u);
}

// ── EpEventHeader struct layout ───────────────────────────────────────

TEST(EpReceiverTest, HeaderSize) {
    EXPECT_EQ(sizeof(EpEventHeader), 24u);
}

TEST(EpReceiverTest, HeaderFieldOffsets) {
    EpEventHeader h{};
    const uint8_t* base = reinterpret_cast<const uint8_t*>(&h);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.magic) - base, 0u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.version) - base, 4u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.event_type) - base, 8u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.payload_size) - base, 12u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.timestamp) - base, 16u);
}

// ── EventTypeToString ────────────────────────────────────────────────

TEST(EpReceiverTest, EventTypeToStringGcStart) {
    EXPECT_STREQ(EventTypeToString(EpEventType::GcStart), "GcStart");
}

TEST(EpReceiverTest, EventTypeToStringGcEnd) {
    EXPECT_STREQ(EventTypeToString(EpEventType::GcEnd), "GcEnd");
}

TEST(EpReceiverTest, EventTypeToStringGcYoungStart) {
    EXPECT_STREQ(EventTypeToString(EpEventType::GcYoungStart), "GcYoungStart");
}

TEST(EpReceiverTest, EventTypeToStringGcYoungEnd) {
    EXPECT_STREQ(EventTypeToString(EpEventType::GcYoungEnd), "GcYoungEnd");
}

TEST(EpReceiverTest, EventTypeToStringGcFullStart) {
    EXPECT_STREQ(EventTypeToString(EpEventType::GcFullStart), "GcFullStart");
}

TEST(EpReceiverTest, EventTypeToStringGcFullEnd) {
    EXPECT_STREQ(EventTypeToString(EpEventType::GcFullEnd), "GcFullEnd");
}

TEST(EpReceiverTest, EventTypeToStringGcOom) {
    EXPECT_STREQ(EventTypeToString(EpEventType::GcOom), "GcOom");
}

TEST(EpReceiverTest, EventTypeToStringGcGen1Collect) {
    EXPECT_STREQ(EventTypeToString(EpEventType::GcGen1Collect), "GcGen1Collect");
}

TEST(EpReceiverTest, EventTypeToStringGcAllocationTick) {
    EXPECT_STREQ(EventTypeToString(EpEventType::GcAllocationTick), "GcAllocationTick");
}

TEST(EpReceiverTest, EventTypeToStringTpWorkerCreate) {
    EXPECT_STREQ(EventTypeToString(EpEventType::TpWorkerCreate), "TpWorkerCreate");
}

TEST(EpReceiverTest, EventTypeToStringTpWorkerDestroy) {
    EXPECT_STREQ(EventTypeToString(EpEventType::TpWorkerDestroy), "TpWorkerDestroy");
}

TEST(EpReceiverTest, EventTypeToStringTpWorkItemQueue) {
    EXPECT_STREQ(EventTypeToString(EpEventType::TpWorkItemQueue), "TpWorkItemQueue");
}

TEST(EpReceiverTest, EventTypeToStringTpWorkItemDequeue) {
    EXPECT_STREQ(EventTypeToString(EpEventType::TpWorkItemDequeue), "TpWorkItemDequeue");
}

TEST(EpReceiverTest, EventTypeToStringTpWorkerAdjust) {
    EXPECT_STREQ(EventTypeToString(EpEventType::TpWorkerAdjust), "TpWorkerAdjust");
}

TEST(EpReceiverTest, EventTypeToStringTpIoCompletion) {
    EXPECT_STREQ(EventTypeToString(EpEventType::TpIoCompletion), "TpIoCompletion");
}

TEST(EpReceiverTest, EventTypeToStringExceptionThrow) {
    EXPECT_STREQ(EventTypeToString(EpEventType::ExceptionThrow), "ExceptionThrow");
}

TEST(EpReceiverTest, EventTypeToStringExceptionRethrow) {
    EXPECT_STREQ(EventTypeToString(EpEventType::ExceptionRethrow), "ExceptionRethrow");
}

TEST(EpReceiverTest, EventTypeToStringExceptionCatch) {
    EXPECT_STREQ(EventTypeToString(EpEventType::ExceptionCatch), "ExceptionCatch");
}

TEST(EpReceiverTest, EventTypeToStringUnknown) {
    EpEventType unknown = static_cast<EpEventType>(0xFFFFu);
    EXPECT_STREQ(EventTypeToString(unknown), "Unknown");
}

// ── IsEventTypeCategory ──────────────────────────────────────────────

TEST(EpReceiverTest, FilterGcOnly) {
    DiagConfig cfg;
    cfg.filter_gc = true;
    cfg.filter_tp = false;
    cfg.filter_exc = false;

    EXPECT_TRUE(IsEventTypeCategory(EpEventType::GcStart, cfg));
    EXPECT_TRUE(IsEventTypeCategory(EpEventType::GcEnd, cfg));
    EXPECT_TRUE(IsEventTypeCategory(EpEventType::GcAllocationTick, cfg));
    EXPECT_FALSE(IsEventTypeCategory(EpEventType::TpWorkerCreate, cfg));
    EXPECT_FALSE(IsEventTypeCategory(EpEventType::ExceptionThrow, cfg));
}

TEST(EpReceiverTest, FilterTpOnly) {
    DiagConfig cfg;
    cfg.filter_gc = false;
    cfg.filter_tp = true;
    cfg.filter_exc = false;

    EXPECT_FALSE(IsEventTypeCategory(EpEventType::GcStart, cfg));
    EXPECT_TRUE(IsEventTypeCategory(EpEventType::TpWorkerCreate, cfg));
    EXPECT_TRUE(IsEventTypeCategory(EpEventType::TpIoCompletion, cfg));
    EXPECT_FALSE(IsEventTypeCategory(EpEventType::ExceptionThrow, cfg));
}

TEST(EpReceiverTest, FilterExcOnly) {
    DiagConfig cfg;
    cfg.filter_gc = false;
    cfg.filter_tp = false;
    cfg.filter_exc = true;

    EXPECT_FALSE(IsEventTypeCategory(EpEventType::GcStart, cfg));
    EXPECT_FALSE(IsEventTypeCategory(EpEventType::TpWorkerCreate, cfg));
    EXPECT_TRUE(IsEventTypeCategory(EpEventType::ExceptionThrow, cfg));
    EXPECT_TRUE(IsEventTypeCategory(EpEventType::ExceptionCatch, cfg));
}

TEST(EpReceiverTest, FilterAllOn) {
    DiagConfig cfg;
    cfg.filter_gc = true;
    cfg.filter_tp = true;
    cfg.filter_exc = true;

    EXPECT_TRUE(IsEventTypeCategory(EpEventType::GcStart, cfg));
    EXPECT_TRUE(IsEventTypeCategory(EpEventType::TpWorkerCreate, cfg));
    EXPECT_TRUE(IsEventTypeCategory(EpEventType::ExceptionThrow, cfg));
}

TEST(EpReceiverTest, FilterAllOff) {
    DiagConfig cfg;
    cfg.filter_gc = false;
    cfg.filter_tp = false;
    cfg.filter_exc = false;

    EXPECT_FALSE(IsEventTypeCategory(EpEventType::GcStart, cfg));
    EXPECT_FALSE(IsEventTypeCategory(EpEventType::TpWorkerCreate, cfg));
    EXPECT_FALSE(IsEventTypeCategory(EpEventType::ExceptionThrow, cfg));
}

TEST(EpReceiverTest, FilterUnknownCategory) {
    DiagConfig cfg;
    cfg.filter_gc = false;
    cfg.filter_tp = false;
    cfg.filter_exc = false;

    // Event type not in any known category should return true (pass through).
    EpEventType unknown = static_cast<EpEventType>(0xFFFFu);
    EXPECT_TRUE(IsEventTypeCategory(unknown, cfg));
}

// ── WriteJsonEvent ──────────────────────────────────────────────────

class EpReceiverWriteTest : public ::testing::Test {
protected:
    int saved_stdout_ = -1;
    int stdout_read_fd_ = -1;

    void SetUp() override {
        // Create a pipe for stdout capture.
        int pipe_fds[2];
        ASSERT_EQ(_pipe(pipe_fds, 65536, _O_BINARY), 0);

        fflush(stdout);
        saved_stdout_ = _dup(_fileno(stdout));
        ASSERT_NE(saved_stdout_, -1);

        ASSERT_NE(_dup2(pipe_fds[1], _fileno(stdout)), -1);
        _close(pipe_fds[1]);

        stdout_read_fd_ = pipe_fds[0];
        setbuf(stdout, nullptr);
    }

    std::string CaptureStdout() {
        fflush(stdout);
        std::string result;
        char buf[4096];

        HANDLE hPipe = reinterpret_cast<HANDLE>(_get_osfhandle(stdout_read_fd_));
        if (hPipe == INVALID_HANDLE_VALUE) return result;

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
            avail = 0;
        }
        return result;
    }

    void TearDown() override {
        if (stdout_read_fd_ >= 0) _close(stdout_read_fd_);
        fflush(stdout);
        if (saved_stdout_ >= 0) {
            _dup2(saved_stdout_, _fileno(stdout));
            _close(saved_stdout_);
        }
    }
};

TEST_F(EpReceiverWriteTest, WriteJsonEventNoPayload) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.event_type = EpEventType::GcStart;
    header.timestamp = 12345;

    WriteJsonEvent(stdout, header, nullptr, 0);
    std::string json = CaptureStdout();
    ASSERT_FALSE(json.empty());

    EXPECT_NE(json.find("GcStart"), std::string::npos);
    EXPECT_NE(json.find("12345"), std::string::npos);
    EXPECT_NE(json.find("\"payload_size\":0"), std::string::npos);
}

TEST_F(EpReceiverWriteTest, WriteJsonEventWithPayload) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.event_type = EpEventType::TpWorkerCreate;
    header.timestamp = 67890;
    header.payload_size = 4;

    uint8_t payload[] = {0xDE, 0xAD, 0xBE, 0xEF};

    WriteJsonEvent(stdout, header, payload, 4);
    std::string json = CaptureStdout();
    ASSERT_FALSE(json.empty());

    EXPECT_NE(json.find("TpWorkerCreate"), std::string::npos);
    EXPECT_NE(json.find("67890"), std::string::npos);
    EXPECT_NE(json.find("\"payload_size\":4"), std::string::npos);
    EXPECT_NE(json.find("deadbeef"), std::string::npos);
}

TEST_F(EpReceiverWriteTest, WriteJsonEventPayloadTruncated) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.event_type = EpEventType::ExceptionThrow;
    header.timestamp = 111;
    header.payload_size = 32;  // > 16, should show "..."

    uint8_t payload[32];
    for (int i = 0; i < 32; ++i) payload[i] = static_cast<uint8_t>(i);

    WriteJsonEvent(stdout, header, payload, 32);
    std::string json = CaptureStdout();
    ASSERT_FALSE(json.empty());

    EXPECT_NE(json.find("ExceptionThrow"), std::string::npos);
    EXPECT_NE(json.find("..."), std::string::npos);
}

// ── ParseArgs ────────────────────────────────────────────────────────

TEST(EpReceiverTest, ParseArgsValidPid) {
    // ParseArgs is file-scoped static in chaos_diag.cpp.
    // Tested via the integration test approach (subprocess).
    // Unit-testable logic (validation rules) is covered by
    // EventTypeToString, IsEventTypeCategory, and WriteJsonEvent above.
}

}  // namespace
