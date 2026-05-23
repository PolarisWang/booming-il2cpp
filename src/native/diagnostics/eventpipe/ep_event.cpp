// ep_event.cpp — EventPipe event emission implementation

#include "ep_event.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include "ep_protocol.h"
#include "ep_transport.h"
#include "ep_instance.h"

#include <cstring>

namespace chaos::il2cpp::diagnostics {

namespace {

/// Safe string copy: copies src to dst with guaranteed null termination.
/// If src is longer than dst_size - 1, the result is truncated.
/// Always null-terminates dst even on empty src or zero dst_size.
void SafeStrCpy(char* dst, const char* src, size_t dst_size) noexcept {
    if (dst_size == 0) return;
    if (src == nullptr) { dst[0] = '\0'; return; }
    strncpy_s(dst, dst_size, src, _TRUNCATE);
}

}  // anonymous namespace

// NOTE: The packed payload structs in gc_etw.cpp use #pragma pack(push, 1)
// and direct field layouts.  We reuse the same memory layout here by
// defining local structs that match byte-for-byte.  This avoids a
// cross-module dependency on gc_etw.cpp's private types.

#pragma pack(push, 1)

struct EpPayloadGcStart {
    uint32_t generation;
};

struct EpPayloadGcEnd {
    uint64_t pause_ns;
    uint64_t reclaimed_bytes;
};

struct EpPayloadGcYoungStart {
    uint64_t nursery_used;
};

struct EpPayloadGcYoungEnd {
    uint64_t pause_ns;
    uint64_t objects_promoted;
    uint64_t bytes_promoted;
    uint64_t bytes_reclaimed;
};

struct EpPayloadGcFullStart {
    uint32_t page_count;
};

struct EpPayloadGcFullEnd {
    uint64_t pause_ns;
    uint64_t reclaimed_bytes;
    uint64_t objects_marked;
    uint64_t pages_collected;
};

struct EpPayloadGcGen1Collect {
    uint64_t pause_ns;
    uint64_t objects_promoted;
    uint64_t bytes_reclaimed;
};

struct EpPayloadAllocationTick {
    uint32_t allocation_amount;
    uint32_t allocation_kind;
};

// ThreadPool payloads (simple).
struct EpPayloadWorkerEvent {
    int32_t thread_id;
};

struct EpPayloadWorkItemEvent {
    int64_t work_item_id;
};

struct EpPayloadWorkerAdjust {
    int32_t current_count;
    int32_t new_count;
    int32_t reason;
};

struct EpPayloadIoCompletion {
    uint32_t bytes_transferred;
};

// Exception payloads.
struct EpPayloadException {
    uint64_t exception_ptr;  // serialized pointer value (opaque)
};

// HotUpdate payloads.
struct EpPayloadHuAssemblyEvent {
    uint32_t module_id;
    uint32_t alc_id;
    char     assembly_name[128];  // fixed-size string buffer
};

struct EpPayloadHuPackageEvent {
    char package_id[128];
};

struct EpPayloadHuPackageLoadFailed {
    char package_id[128];
    char error_message[256];
};

struct EpPayloadHuMethodEvent {
    uint32_t method_token;
};

struct EpPayloadHuError {
    char error_message[256];
};

// Reflection payloads.
struct EpPayloadReflectionQuery {
    char     type_name[128];
    char     member_name[128];
    uint32_t result_code;
};

struct EpPayloadReflectionInvoke {
    char     type_name[128];
    char     method_name[128];
    uint32_t arg_count;
    uint32_t result_code;
};

struct EpPayloadTypeResolve {
    char     type_name[128];
    uint32_t result_code;
};

// MemoryDomain payloads.
struct EpPayloadDomainRegistered {
    uint32_t domain_id;
    char     module_name[128];
    uint32_t module_kind;
};

struct EpPayloadDomainUnloaded {
    uint32_t domain_id;
};

struct EpPayloadDomainUsageLimitExceeded {
    uint32_t domain_id;
    int64_t  current_usage;
    int64_t  usage_limit;
};

#pragma pack(pop)

// ── Core emit function ─────────────────────────────────────────────────

void EpEmitEvent(EpEventType event_type, const void* payload, uint32_t payload_size) noexcept {
    // Lazy init on first use (same pattern as gc_etw.cpp).
    if (!EpIsInitialized()) {
        EpInitialize();
        if (!EpIsInitialized()) {
            return;  // init failed — silently drop
        }
    }

    // Build header on stack.
    EpEventHeader header;
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.event_type = event_type;
    header.payload_size = payload_size;

    // Timestamp via QPC.
    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);
    header.timestamp = static_cast<uint64_t>(qpc.QuadPart);

    // Check payload size against max.
    if (payload_size > kEpMaxPayloadSize) {
        payload_size = kEpMaxPayloadSize;
    }

    // Build wire buffer: header + payload + checksum.
    // Use stack buffer for small events.
    uint8_t wire_buf[sizeof(EpEventHeader) + kEpMaxPayloadSize + sizeof(uint32_t)];
    std::memcpy(wire_buf, &header, sizeof(EpEventHeader));
    if (payload != nullptr && payload_size > 0) {
        std::memcpy(wire_buf + sizeof(EpEventHeader), payload, payload_size);
    }

    // Compute and append checksum.
    uint32_t checksum = EpComputeChecksum(header, payload);
    std::memcpy(wire_buf + sizeof(EpEventHeader) + payload_size, &checksum, sizeof(checksum));

    uint32_t total_size = sizeof(EpEventHeader) + payload_size + sizeof(uint32_t);
    EpTransportWrite(wire_buf, total_size);
}

// ── GC event helpers ────────────────────────────────────────────────────

void EpEmitGcStart(uint32_t generation) noexcept {
    EpPayloadGcStart payload{ generation };
    EpEmitEvent(EpEventType::GcStart, &payload, sizeof(payload));
}

void EpEmitGcEnd(uint64_t pause_ns, uint64_t reclaimed_bytes) noexcept {
    EpPayloadGcEnd payload{ pause_ns, reclaimed_bytes };
    EpEmitEvent(EpEventType::GcEnd, &payload, sizeof(payload));
}

void EpEmitGcYoungStart(uint64_t nursery_used) noexcept {
    EpPayloadGcYoungStart payload{ nursery_used };
    EpEmitEvent(EpEventType::GcYoungStart, &payload, sizeof(payload));
}

void EpEmitGcYoungEnd(uint64_t pause_ns, uint64_t objects_promoted,
                       uint64_t bytes_promoted, uint64_t bytes_reclaimed) noexcept
{
    EpPayloadGcYoungEnd payload{ pause_ns, objects_promoted, bytes_promoted, bytes_reclaimed };
    EpEmitEvent(EpEventType::GcYoungEnd, &payload, sizeof(payload));
}

void EpEmitGcFullStart(uint32_t page_count) noexcept {
    EpPayloadGcFullStart payload{ page_count };
    EpEmitEvent(EpEventType::GcFullStart, &payload, sizeof(payload));
}

void EpEmitGcFullEnd(uint64_t pause_ns, uint64_t reclaimed_bytes,
                      uint64_t objects_marked, uint64_t pages_collected) noexcept
{
    EpPayloadGcFullEnd payload{ pause_ns, reclaimed_bytes, objects_marked, pages_collected };
    EpEmitEvent(EpEventType::GcFullEnd, &payload, sizeof(payload));
}

void EpEmitGcOom() noexcept {
    EpEmitEvent(EpEventType::GcOom, nullptr, 0);
}

void EpEmitGcGen1Collect(uint64_t pause_ns, uint64_t objects_promoted,
                          uint64_t bytes_reclaimed) noexcept
{
    EpPayloadGcGen1Collect payload{ pause_ns, objects_promoted, bytes_reclaimed };
    EpEmitEvent(EpEventType::GcGen1Collect, &payload, sizeof(payload));
}

void EpEmitAllocationTick(uint32_t bytes, uint32_t is_large_object) noexcept {
    EpPayloadAllocationTick payload{ bytes, is_large_object };
    EpEmitEvent(EpEventType::GcAllocationTick, &payload, sizeof(payload));
}

// ── ThreadPool event helpers ────────────────────────────────────────────

void EpEmitTpWorkerCreate(int32_t thread_id) noexcept {
    EpPayloadWorkerEvent payload{ thread_id };
    EpEmitEvent(EpEventType::TpWorkerCreate, &payload, sizeof(payload));
}

void EpEmitTpWorkerDestroy(int32_t thread_id) noexcept {
    EpPayloadWorkerEvent payload{ thread_id };
    EpEmitEvent(EpEventType::TpWorkerDestroy, &payload, sizeof(payload));
}

void EpEmitTpWorkItemQueue(int64_t work_item_id) noexcept {
    EpPayloadWorkItemEvent payload{ work_item_id };
    EpEmitEvent(EpEventType::TpWorkItemQueue, &payload, sizeof(payload));
}

void EpEmitTpWorkItemDequeue(int64_t work_item_id) noexcept {
    EpPayloadWorkItemEvent payload{ work_item_id };
    EpEmitEvent(EpEventType::TpWorkItemDequeue, &payload, sizeof(payload));
}

void EpEmitTpWorkerAdjust(int32_t current_count, int32_t new_count, int32_t reason) noexcept {
    EpPayloadWorkerAdjust payload{ current_count, new_count, reason };
    EpEmitEvent(EpEventType::TpWorkerAdjust, &payload, sizeof(payload));
}

void EpEmitTpIoCompletion(uint32_t bytes_transferred) noexcept {
    EpPayloadIoCompletion payload{ bytes_transferred };
    EpEmitEvent(EpEventType::TpIoCompletion, &payload, sizeof(payload));
}

// ── Exception event helpers ─────────────────────────────────────────────

void EpEmitExceptionThrow(const void* exception_obj) noexcept {
    EpPayloadException payload{ reinterpret_cast<uint64_t>(exception_obj) };
    EpEmitEvent(EpEventType::ExceptionThrow, &payload, sizeof(payload));
}

void EpEmitExceptionRethrow(const void* exception_obj) noexcept {
    EpPayloadException payload{ reinterpret_cast<uint64_t>(exception_obj) };
    EpEmitEvent(EpEventType::ExceptionRethrow, &payload, sizeof(payload));
}

void EpEmitExceptionCatch(const void* exception_obj) noexcept {
    EpPayloadException payload{ reinterpret_cast<uint64_t>(exception_obj) };
    EpEmitEvent(EpEventType::ExceptionCatch, &payload, sizeof(payload));
}

// ── HotUpdate event helpers ──────────────────────────────────────────────

void EpEmitHuAssemblyLoaded(uint32_t module_id, uint32_t alc_id,
                             const char* assembly_name) noexcept {
    EpPayloadHuAssemblyEvent payload{};
    payload.module_id = module_id;
    payload.alc_id = alc_id;
    if (assembly_name != nullptr) {
        SafeStrCpy(payload.assembly_name, assembly_name,
                   sizeof(payload.assembly_name));
    }
    EpEmitEvent(EpEventType::HuAssemblyLoaded, &payload, sizeof(payload));
}

void EpEmitHuAssemblyUnloaded(uint32_t module_id, uint32_t alc_id,
                               const char* assembly_name) noexcept {
    EpPayloadHuAssemblyEvent payload{};
    payload.module_id = module_id;
    payload.alc_id = alc_id;
    if (assembly_name != nullptr) {
        SafeStrCpy(payload.assembly_name, assembly_name,
                   sizeof(payload.assembly_name));
    }
    EpEmitEvent(EpEventType::HuAssemblyUnloaded, &payload, sizeof(payload));
}

void EpEmitHuPackageLoaded(const char* package_id) noexcept {
    EpPayloadHuPackageEvent payload{};
    if (package_id != nullptr) {
        SafeStrCpy(payload.package_id, package_id,
                   sizeof(payload.package_id));
    }
    EpEmitEvent(EpEventType::HuPackageLoaded, &payload, sizeof(payload));
}

void EpEmitHuPackageLoadFailed(const char* package_id,
                                const char* error_message) noexcept {
    EpPayloadHuPackageLoadFailed payload{};
    if (package_id != nullptr) {
        SafeStrCpy(payload.package_id, package_id,
                   sizeof(payload.package_id));
    }
    if (error_message != nullptr) {
        SafeStrCpy(payload.error_message, error_message,
                   sizeof(payload.error_message));
    }
    EpEmitEvent(EpEventType::HuPackageLoadFailed, &payload, sizeof(payload));
}

void EpEmitHuPackageUnloaded(const char* package_id) noexcept {
    EpPayloadHuPackageEvent payload{};
    if (package_id != nullptr) {
        SafeStrCpy(payload.package_id, package_id,
                   sizeof(payload.package_id));
    }
    EpEmitEvent(EpEventType::HuPackageUnloaded, &payload, sizeof(payload));
}

void EpEmitHuMethodReplaced(uint32_t method_token) noexcept {
    EpPayloadHuMethodEvent payload{ method_token };
    EpEmitEvent(EpEventType::HuMethodReplaced, &payload, sizeof(payload));
}

void EpEmitHuMethodReverted(uint32_t method_token) noexcept {
    EpPayloadHuMethodEvent payload{ method_token };
    EpEmitEvent(EpEventType::HuMethodReverted, &payload, sizeof(payload));
}

void EpEmitHuError(const char* error_message) noexcept {
    EpPayloadHuError payload{};
    if (error_message != nullptr) {
        SafeStrCpy(payload.error_message, error_message,
                   sizeof(payload.error_message));
    }
    EpEmitEvent(EpEventType::HuError, &payload, sizeof(payload));
}

// ── Reflection event helpers ───────────────────────────────────────────

void EpEmitReflectionQuery(const char* type_name, const char* member_name,
                            uint32_t result_code) noexcept {
    EpPayloadReflectionQuery payload{};
    if (type_name != nullptr) {
        SafeStrCpy(payload.type_name, type_name,
                   sizeof(payload.type_name));
    }
    if (member_name != nullptr) {
        SafeStrCpy(payload.member_name, member_name,
                   sizeof(payload.member_name));
    }
    payload.result_code = result_code;
    EpEmitEvent(EpEventType::ReflectionQuery, &payload, sizeof(payload));
}

void EpEmitReflectionInvoke(const char* type_name, const char* method_name,
                             uint32_t arg_count, uint32_t result_code) noexcept {
    EpPayloadReflectionInvoke payload{};
    if (type_name != nullptr) {
        SafeStrCpy(payload.type_name, type_name,
                   sizeof(payload.type_name));
    }
    if (method_name != nullptr) {
        SafeStrCpy(payload.method_name, method_name,
                   sizeof(payload.method_name));
    }
    payload.arg_count = arg_count;
    payload.result_code = result_code;
    EpEmitEvent(EpEventType::ReflectionInvoke, &payload, sizeof(payload));
}

void EpEmitTypeResolve(const char* type_name, uint32_t result_code) noexcept {
    EpPayloadTypeResolve payload{};
    if (type_name != nullptr) {
        SafeStrCpy(payload.type_name, type_name,
                   sizeof(payload.type_name));
    }
    payload.result_code = result_code;
    EpEmitEvent(EpEventType::TypeResolve, &payload, sizeof(payload));
}

// ── MemoryDomain event helpers ──────────────────────────────────────────

void EpEmitDomainRegistered(uint32_t domain_id, const char* module_name,
                             uint32_t module_kind) noexcept {
    EpPayloadDomainRegistered payload{};
    payload.domain_id = domain_id;
    payload.module_kind = module_kind;
    if (module_name != nullptr) {
        SafeStrCpy(payload.module_name, module_name,
                   sizeof(payload.module_name));
    }
    EpEmitEvent(EpEventType::DomainRegistered, &payload, sizeof(payload));
}

void EpEmitDomainUnloaded(uint32_t domain_id) noexcept {
    EpPayloadDomainUnloaded payload{ domain_id };
    EpEmitEvent(EpEventType::DomainUnloaded, &payload, sizeof(payload));
}

void EpEmitDomainUsageLimitExceeded(uint32_t domain_id, int64_t current_usage,
                                     int64_t usage_limit) noexcept {
    EpPayloadDomainUsageLimitExceeded payload{ domain_id, current_usage, usage_limit };
    EpEmitEvent(EpEventType::DomainUsageLimitExceeded, &payload, sizeof(payload));
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
