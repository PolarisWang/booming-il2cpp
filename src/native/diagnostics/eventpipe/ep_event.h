// ep_event.h — EventPipe event emission API
//
// High-level event emission functions.  Each function serializes the
// event payload, constructs the wire header, computes the checksum,
// and writes to the transport layer.
//
// All functions are safe to call from any thread (including GC safepoints)
// and are no-ops when EventPipe is not initialized.

#ifndef CHAOS_IL2CPP_EP_EVENT_H_
#define CHAOS_IL2CPP_EP_EVENT_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include "ep_protocol.h"

#include <cstdint>

namespace chaos::il2cpp::diagnostics {

/// Emit a raw event with arbitrary payload.
/// @param event_type  Event type discriminator.
/// @param payload     Pointer to payload data (may be nullptr if size == 0).
/// @param payload_size Size of payload in bytes.
void EpEmitEvent(EpEventType event_type, const void* payload, uint32_t payload_size) noexcept;

// ── GC event convenience helpers ────────────────────────────────────────
// These mirror the gc_etw.h API for parity.

void EpEmitGcStart(uint32_t generation) noexcept;
void EpEmitGcEnd(uint64_t pause_ns, uint64_t reclaimed_bytes) noexcept;
void EpEmitGcYoungStart(uint64_t nursery_used) noexcept;
void EpEmitGcYoungEnd(uint64_t pause_ns, uint64_t objects_promoted,
                       uint64_t bytes_promoted, uint64_t bytes_reclaimed) noexcept;
void EpEmitGcFullStart(uint32_t page_count) noexcept;
void EpEmitGcFullEnd(uint64_t pause_ns, uint64_t reclaimed_bytes,
                      uint64_t objects_marked, uint64_t pages_collected) noexcept;
void EpEmitGcOom() noexcept;
void EpEmitGcGen1Collect(uint64_t pause_ns, uint64_t objects_promoted,
                          uint64_t bytes_reclaimed) noexcept;
void EpEmitAllocationTick(uint32_t bytes, uint32_t is_large_object) noexcept;

// ── ThreadPool event convenience helpers ────────────────────────────────

void EpEmitTpWorkerCreate(int32_t thread_id) noexcept;
void EpEmitTpWorkerDestroy(int32_t thread_id) noexcept;
void EpEmitTpWorkItemQueue(int64_t work_item_id) noexcept;
void EpEmitTpWorkItemDequeue(int64_t work_item_id) noexcept;
void EpEmitTpWorkerAdjust(int32_t current_count, int32_t new_count, int32_t reason) noexcept;
void EpEmitTpIoCompletion(uint32_t bytes_transferred) noexcept;

// ── Exception event convenience helpers ─────────────────────────────────

void EpEmitExceptionThrow(const void* exception_obj) noexcept;
void EpEmitExceptionRethrow(const void* exception_obj) noexcept;
void EpEmitExceptionCatch(const void* exception_obj) noexcept;

// ── HotUpdate event convenience helpers ─────────────────────────────────

void EpEmitHuAssemblyLoaded(uint32_t module_id, uint32_t alc_id,
                             const char* assembly_name) noexcept;
void EpEmitHuAssemblyUnloaded(uint32_t module_id, uint32_t alc_id,
                               const char* assembly_name) noexcept;
void EpEmitHuPackageLoaded(const char* package_id) noexcept;
void EpEmitHuPackageLoadFailed(const char* package_id,
                                const char* error_message) noexcept;
void EpEmitHuPackageUnloaded(const char* package_id) noexcept;
void EpEmitHuMethodReplaced(uint32_t method_token) noexcept;
void EpEmitHuMethodReverted(uint32_t method_token) noexcept;
void EpEmitHuError(const char* error_message) noexcept;

// ── Reflection event convenience helpers ────────────────────────────

void EpEmitReflectionQuery(const char* type_name, const char* member_name,
                            uint32_t result_code) noexcept;
void EpEmitReflectionInvoke(const char* type_name, const char* method_name,
                             uint32_t arg_count, uint32_t result_code) noexcept;
void EpEmitTypeResolve(const char* type_name, uint32_t result_code) noexcept;

// ── MemoryDomain event convenience helpers ──────────────────────────

void EpEmitDomainRegistered(uint32_t domain_id, const char* module_name,
                             uint32_t module_kind) noexcept;
void EpEmitDomainUnloaded(uint32_t domain_id) noexcept;
void EpEmitDomainUsageLimitExceeded(uint32_t domain_id, int64_t current_usage,
                                     int64_t usage_limit) noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE

#endif  // CHAOS_IL2CPP_EP_EVENT_H_
