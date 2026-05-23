// ep_memorydomain_bridge.cpp — EventPipe bridge for MemoryDomain diagnostics events
//
// Registers a MemoryDomainEventCallback during EpInitialize that forwards
// domain lifecycle events (register, unload, usage-limit exceeded) to the
// EventPipe transport layer.

#include "ep_memorydomain_bridge.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include "ep_event.h"
#include <memory_domain_events.h>

#include <cstdint>

namespace chaos::il2cpp::diagnostics {

namespace {

/// Callback registered with MemoryDomainRegisterEventCallback.
/// Translates MemoryDomainEvent (with data) to EventPipe emission calls.
void EpMemoryDomainCallback(memory_domain::MemoryDomainEvent event,
                             const memory_domain::MemoryDomainEventData* data,
                             void* /*user_data*/) noexcept {
    if (data == nullptr) return;

    switch (event) {
    case memory_domain::MemoryDomainEvent::DOMAIN_REGISTERED:
        EpEmitDomainRegistered(data->domain_id, data->module_name,
                                data->module_kind);
        break;
    case memory_domain::MemoryDomainEvent::DOMAIN_UNLOADED:
        EpEmitDomainUnloaded(data->domain_id);
        break;
    case memory_domain::MemoryDomainEvent::DOMAIN_USAGE_LIMIT_EXCEEDED:
        EpEmitDomainUsageLimitExceeded(data->domain_id, data->current_usage,
                                        data->usage_limit);
        break;
    default:
        break;
    }
}

}  // anonymous namespace

void EpMemoryDomainBridgeInitialize() noexcept {
    memory_domain::MemoryDomainRegisterEventCallback(EpMemoryDomainCallback, nullptr);
}

void EpMemoryDomainBridgeShutdown() noexcept {
    // MemoryDomainEventCallbacks cannot be individually unregistered.
    // The memory domain subsystem will stop firing during shutdown naturally.
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
