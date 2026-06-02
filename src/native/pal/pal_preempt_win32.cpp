// pal_preempt_win32.cpp — Win32 preemptive suspend (QueueUserAPC)

#include <chaos/pal/pal_preempt.h>

#define NOMINMAX
#include <windows.h>

namespace chaos::il2cpp::pal {

namespace {

PalPreemptCallback s_preempt_callback = nullptr;

/// APC forwarding function: calls the registered consumer callback
/// with the epoch passed via QueueUserAPC's ULONG_PTR parameter.
static void __stdcall PalPreemptApcForwarder(ULONG_PTR param) noexcept {
    if (s_preempt_callback) {
        s_preempt_callback(static_cast<uint64_t>(param));
    }
}

}  // anonymous namespace

void PalPreemptInit(PalPreemptCallback callback) noexcept {
    s_preempt_callback = callback;
}

bool PalPreemptRequest(void* os_handle, uint64_t /*os_thread_id*/,
                        uint64_t epoch) noexcept {
    if (!os_handle) return false;
    return ::QueueUserAPC(PalPreemptApcForwarder,
                          static_cast<HANDLE>(os_handle),
                          static_cast<ULONG_PTR>(epoch)) != 0;
}

}  // namespace chaos::il2cpp::pal
