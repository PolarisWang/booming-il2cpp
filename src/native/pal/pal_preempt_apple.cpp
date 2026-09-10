// pal_preempt_apple.cpp — Apple (macOS/iOS) preemptive suspend (no-op)
//
// Apple platforms do not support signal-based thread preemption for
// safepoint suspension.  pthread_kill with SIGUSR2 is not reliable
// on Darwin (the signal may be delivered to a different thread or
// blocked by the system).  Preemptive suspend requests are no-ops;
// the safepoint protocol relies on cooperative polling.

#include <chaos/pal/pal_preempt.h>

namespace chaos::il2cpp::pal {

void PalPreemptInit(PalPreemptCallback /*callback*/) noexcept {
    // No-op: no signal handler installation on Apple.
}

bool PalPreemptRequest(void* /*os_handle*/, uint64_t /*os_thread_id*/,
                        uint64_t /*epoch*/) noexcept {
    return false;
}

}  // namespace chaos::il2cpp::pal
