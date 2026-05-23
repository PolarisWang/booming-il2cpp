// ep_receiver.h — EventPipe receiver shared types and declarations
//
// Shared between chaos_diag.cpp (receiver CLI) and unit tests.
// Contains protocol constants, event types, and utility functions
// that are tested independently of named pipe I/O.

#ifndef CHAOS_IL2CPP_EP_RECEIVER_H_
#define CHAOS_IL2CPP_EP_RECEIVER_H_

#include <cstdint>
#include <cstdio>
#include <string>

// ── Wire protocol constants (standalone, no dependency on runtime ep_protocol.h) ──

static constexpr uint32_t kEpMagic = 0xACDB12CEu;
static constexpr uint32_t kEpProtocolVersion = 1;

enum class EpEventType : uint32_t {
    GcStart          = 0x0101,
    GcEnd            = 0x0102,
    GcYoungStart     = 0x0103,
    GcYoungEnd       = 0x0104,
    GcFullStart      = 0x0105,
    GcFullEnd        = 0x0106,
    GcOom            = 0x0107,
    GcGen1Collect    = 0x0108,
    GcAllocationTick = 0x0109,
    TpWorkerCreate   = 0x0201,
    TpWorkerDestroy  = 0x0202,
    TpWorkItemQueue  = 0x0203,
    TpWorkItemDequeue= 0x0204,
    TpWorkerAdjust   = 0x0205,
    TpIoCompletion   = 0x0206,
    ExceptionThrow   = 0x0301,
    ExceptionRethrow = 0x0302,
    ExceptionCatch   = 0x0303,
};

struct EpEventHeader {
    uint32_t magic;
    uint32_t version;
    EpEventType event_type;
    uint32_t payload_size;
    uint64_t timestamp;
};

struct DiagConfig {
    uint32_t pid = 0;
    bool filter_gc = true;
    bool filter_tp = true;
    bool filter_exc = true;
    bool output_to_file = false;
    std::string output_path;
};

// ── Utility function declarations ─────────────────────────────────────────

const char* EventTypeToString(EpEventType type);

bool IsEventTypeCategory(EpEventType type, const DiagConfig& config);

void WriteJsonEvent(FILE* out, const EpEventHeader& header,
                    const uint8_t* payload, uint32_t payload_size);

#endif  // CHAOS_IL2CPP_EP_RECEIVER_H_
