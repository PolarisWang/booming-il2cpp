#ifndef CHAOS_IL2CPP_FAST_FRAME_POOL_H_
#define CHAOS_IL2CPP_FAST_FRAME_POOL_H_

// ── TLS FastFrame Pool ────────────────────────────────────────────────────
//
// Thread-local pool of pre-allocated FastFrame instances used in the
// InterpreterEntryDirect hot path. Eliminates the ~200ns memset(416 bytes)
// per patched-method call by recycling frames across calls.
//
// Design:
//   - Fixed-size free list (LIFO), O(1) acquire/release
//   - Acquire: pop from free head, no zero-init (caller resets needed fields)
//   - Release: CleanupTracked() then push back to free list
//   - Overflow return nullptr — caller falls back to stack+ memset
//
// Thread safety: NOT thread-safe (thread_local by design).

#include "fast_dispatch.h"

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

class FastFramePool {
public:
    static constexpr uint32_t kPoolSize = 8;

    FastFramePool() noexcept;

    // Acquire a frame from the pool.
    // Returns nullptr if pool is empty (caller should fall back to
    // stack allocation + memset).
    FastFrame* Acquire() noexcept;

    // Return a frame to the pool.
    // Calls CleanupTracked() on the frame before pushing.
    void Release(FastFrame* frame) noexcept;

private:
    FastFrame frames_[kPoolSize];
    uint32_t  free_list_[kPoolSize];
    uint32_t  free_head_;   // index into free_list; 0 = empty
};

// Global thread-local pool instance (defined in fast_frame_pool.cpp).
extern thread_local FastFramePool tls_frame_pool;

// ── SetupFastFrame ────────────────────────────────────────────────────────
// Lightweight FastFrame field setup — replaces ~416-byte memset + field fills.
// Resets only the fields that change between calls (pc, sp, tracked, args,
// dispatch, call_cache).  O(1) ~10 scalar assignments (~20ns) vs ~200ns memset.
void SetupFastFrame(FastFrame* ff,
                    const void* patch_method,
                    const void* args_buf,
                    const void* ir,
                    void* dispatch_fn,
                    void* dispatch_ctx) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_FAST_FRAME_POOL_H_