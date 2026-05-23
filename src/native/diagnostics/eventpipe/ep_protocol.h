// ep_protocol.h — EventPipe wire protocol definitions
//
// Binary format (sent over named pipe):
//
//   ┌──────────────────────────────────────────────┐
//   │ Magic:       [0xCE, 0x12, 0xDB, 0xAC] (4B)  │
//   │ Version:     uint32_t                        │
//   │ EventType:   uint32_t                        │
//   │ PayloadSize: uint32_t                        │
//   │ Timestamp:   uint64_t (QPC ticks)            │
//   │ Payload:     [PayloadSize bytes]             │
//   │ Checksum:    uint32_t (XOR of header+payload)│
//   └──────────────────────────────────────────────┘
//
// Checksum is a simple XOR of all bytes in the header (excluding checksum
// field itself) + payload.  Used for basic corruption detection, not
// security.

#ifndef CHAOS_IL2CPP_EP_PROTOCOL_H_
#define CHAOS_IL2CPP_EP_PROTOCOL_H_

#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::diagnostics {

// ── Protocol constants ──────────────────────────────────────────────────

static constexpr uint32_t kEpMagic = 0xACDB12CEu;
static constexpr uint32_t kEpProtocolVersion = 1;

// ── Event types ─────────────────────────────────────────────────────────

enum class EpEventType : uint32_t {
    // GC events (range 0x0100-0x01FF)
    GcStart          = 0x0101,
    GcEnd            = 0x0102,
    GcYoungStart     = 0x0103,
    GcYoungEnd       = 0x0104,
    GcFullStart      = 0x0105,
    GcFullEnd        = 0x0106,
    GcOom            = 0x0107,
    GcGen1Collect    = 0x0108,
    GcAllocationTick = 0x0109,

    // ThreadPool events (range 0x0200-0x02FF)
    TpWorkerCreate   = 0x0201,
    TpWorkerDestroy  = 0x0202,
    TpWorkItemQueue  = 0x0203,
    TpWorkItemDequeue= 0x0204,
    TpWorkerAdjust   = 0x0205,
    TpIoCompletion   = 0x0206,

    // Exception events (range 0x0300-0x03FF)
    ExceptionThrow   = 0x0301,
    ExceptionRethrow = 0x0302,
    ExceptionCatch   = 0x0303,

    // HotUpdate events (range 0x0400-0x04FF)
    HuAssemblyLoaded    = 0x0401,
    HuAssemblyUnloaded  = 0x0402,
    HuPackageLoaded     = 0x0403,
    HuPackageLoadFailed = 0x0404,
    HuPackageUnloaded   = 0x0405,
    HuMethodReplaced    = 0x0406,
    HuMethodReverted    = 0x0407,
    HuError             = 0x0408,

    // Reflection events (range 0x0500-0x05FF)
    ReflectionQuery   = 0x0501,
    ReflectionInvoke  = 0x0502,
    TypeResolve       = 0x0503,

    // MemoryDomain events (range 0x0600-0x06FF)
    DomainRegistered          = 0x0601,
    DomainUnloaded            = 0x0602,
    DomainUsageLimitExceeded  = 0x0603,

    // COM events (range 0x0700-0x07FF)
    ComRcwCreated           = 0x0701,
    ComRcwReleased          = 0x0702,
    ComCcwCreated           = 0x0703,
    ComCcwReleased          = 0x0704,
    ComConnectionPointAdvise  = 0x0705,
    ComConnectionPointUnadvise= 0x0706,
    ComDispatchInvoke        = 0x0707,
};

// ── Wire header ─────────────────────────────────────────────────────────
//
// Total header size = 24 bytes (fixed).
// Followed by PayloadSize bytes of payload.

struct EpEventHeader {
    uint32_t magic;          // kEpMagic
    uint32_t version;        // kEpProtocolVersion
    EpEventType event_type;  // event type discriminator
    uint32_t payload_size;   // size of payload that follows
    uint64_t timestamp;      // QPC ticks at event creation
};
// Header is exactly 24 bytes.
static_assert(sizeof(EpEventHeader) == 24, "EpEventHeader must be 24 bytes");

// ── Checksum helper ─────────────────────────────────────────────────────
//
// Computes a simple XOR checksum over the header (excluding the checksum
// field itself) and payload.  The caller writes the result into the
// checksum field in the wire format (sent immediately after payload).

inline uint32_t EpComputeChecksum(const EpEventHeader& header, const void* payload) noexcept {
    const auto* hdr_bytes = reinterpret_cast<const uint8_t*>(&header);
    uint32_t xor_sum = 0;

    // XOR over header bytes (all 24 bytes — checksum field is at the end
    // of the wire format but not part of the header struct itself).
    for (size_t i = 0; i < sizeof(EpEventHeader); ++i) {
        xor_sum ^= static_cast<uint32_t>(hdr_bytes[i]);
    }

    // XOR over payload bytes.
    if (payload != nullptr && header.payload_size > 0) {
        const auto* payload_bytes = static_cast<const uint8_t*>(payload);
        for (uint32_t i = 0; i < header.payload_size; ++i) {
            xor_sum ^= static_cast<uint32_t>(payload_bytes[i]);
        }
    }

    return xor_sum;
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EP_PROTOCOL_H_
