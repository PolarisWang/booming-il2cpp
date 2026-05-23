// dbg_instance.cpp — Debugger singleton management

#include "dbg_instance.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_transport.h"
#include "dbg_server.h"
#include "dbg_breakpoint.h"
#include "dbg_stepping.h"
#include "dbg_stack.h"
#include "dbg_variables.h"
#include "dbg_runtime.h"

#include <atomic>
#include <mutex>
#include <thread>

namespace chaos::il2cpp::diagnostics {

namespace {

std::atomic<bool> g_initialized{false};
std::once_flag g_init_flag;
std::thread g_server_thread;

void ServerThreadFunc() noexcept {
    DbgServerRun();
}

}  // anonymous namespace

bool DbgInitialize() noexcept {
    bool ok = false;
    std::call_once(g_init_flag, [&ok]() {
        // Initialize transport.
        if (!DbgTransportInitialize()) {
            ok = false;
            return;
        }

        // Initialize sub-modules.
        DbgServerInitialize();
        DbgBreakpointInitialize();
        DbgSteppingInitialize();
        DbgStackInitialize();
        DbgVariablesInitialize();

        // Start DAP server thread.
        g_server_thread = std::thread(ServerThreadFunc);

        g_initialized.store(true, std::memory_order_release);
        ok = true;
    });
    return ok;
}

void DbgShutdown() noexcept {
    if (!g_initialized.load(std::memory_order_acquire)) {
        return;
    }

    // Signal server to stop.
    DbgServerSignalStop();

    if (g_server_thread.joinable()) {
        g_server_thread.join();
    }

    // Shutdown sub-modules.
    DbgVariablesShutdown();
    DbgStackShutdown();
    DbgSteppingShutdown();
    DbgBreakpointShutdown();
    DbgServerShutdown();
    DbgTransportShutdown();

    g_initialized.store(false, std::memory_order_release);
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER
