// pal_preempt_android.cpp — Android preemptive suspend (no-op)
//
// Android's signal handling is managed by the platform (ART/signal
// chaining).  Installing custom SIGUSR2 handlers interferes with
// the platform's crash reporting and GC mechanism.  Preemptive
// suspend requests are no-ops; the safepoint protocol relies on
// cooperative polling.

#include <chaos/pal/pal_preempt.h>

namespace chaos::il2cpp::pal {

void PalPreemptInit(PalPreemptCallback /*callback*/) noexcept {
    // No-op: signal handling is managed by the Android platform.
}

bool PalCaptureReliable() noexcept {
    // Android manages signal handling itself (ART); no reliable register
    // window is captured for its threads.
    return false;
}

bool PalPreemptRequest(void* /*os_handle*/, uint64_t /*os_thread_id*/,
                        uint64_t /*epoch*/) noexcept {
    return false;
}

}  // namespace chaos::il2cpp::pal
