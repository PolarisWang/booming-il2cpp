// Stub implementations for symbols referenced by chaos_runtime_core.lib(misc_stubs.obj)
// but not actually called in the reflection test: ChaosGcCollect (which calls
// RequestGlobalSafepoint/ReleaseGlobalSafepoint) is inside an `extern "C"` block,
// so MSVC generates C-linkage (undecorated) references to these symbols.
// We provide C-linkage stubs so the linker can resolve without pulling in
// thread_state.obj from chaos_runtime_core.lib (which has C++-linkage definitions
// that don't match the C-linkage reference).

#include <cstdint>

extern "C" {

uint32_t RequestGlobalSafepoint() noexcept {
    return 0;  // No-op: safepoint never needed in single-threaded fact mode
}

void ReleaseGlobalSafepoint(uint32_t /*epoch*/) noexcept {
    // No-op: nothing to release
}

}  // extern "C"
