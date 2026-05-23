// ep_instance.cpp — EventPipe singleton management implementation

#include "ep_instance.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include "ep_transport.h"
#include "ep_gc_bridge.h"
#include "ep_threadpool_bridge.h"
#include "ep_hotupdate_bridge.h"
#include "ep_reflection_bridge.h"
#include "ep_memorydomain_bridge.h"
#include "ep_com_bridge.h"

#include <atomic>
#include <mutex>

namespace chaos::il2cpp::diagnostics {

namespace {

std::atomic<bool> g_initialized{false};
std::once_flag g_init_flag;

}  // anonymous namespace

bool EpInitialize() noexcept {
    bool already = false;
    std::call_once(g_init_flag, [&already]() {
        // Get current process ID for pipe name.
        uint32_t pid = static_cast<uint32_t>(GetCurrentProcessId());

        // Initialize transport (named pipe server).
        if (!EpTransportInitialize(pid)) {
            already = false;
            return;
        }

        // Initialize bridges.
        EpGcBridgeInitialize();
        EpTpBridgeInitialize();
        EpHotupdateBridgeInitialize();
        EpReflectionBridgeInitialize();
        EpMemoryDomainBridgeInitialize();
        EpComBridgeInitialize();

        g_initialized.store(true, std::memory_order_release);
        already = true;
    });
    return already;
}

void EpShutdown() noexcept {
    if (!g_initialized.load(std::memory_order_acquire)) {
        return;
    }

    // Shutdown in reverse order of initialization.
    EpHotupdateBridgeShutdown();
    EpReflectionBridgeShutdown();
    EpMemoryDomainBridgeShutdown();
    EpComBridgeShutdown();
    EpTpBridgeShutdown();
    EpGcBridgeShutdown();
    EpTransportShutdown();

    g_initialized.store(false, std::memory_order_release);
}

bool EpIsInitialized() noexcept {
    return g_initialized.load(std::memory_order_acquire);
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
