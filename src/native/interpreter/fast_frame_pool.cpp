#include "fast_frame_pool.h"
#include "patch_loader.h"
#include <interpreter_vm.h>

namespace chaos::il2cpp::runtime_core {

// Thread-local FastFrame pool for interpreter hot path.
// Pre-allocates frames to eliminate ~200ns memset per patched-method call.
thread_local FastFramePool tls_frame_pool;

// ── FastFramePool ─────────────────────────────────────────────────────────

FastFramePool::FastFramePool() noexcept {
    for (uint32_t i = 0; i < kPoolSize; ++i) {
        free_list_[i] = i;
    }
    free_head_ = kPoolSize;
}

FastFrame* FastFramePool::Acquire() noexcept {
    if (free_head_ == 0) return nullptr;
    --free_head_;
    return &frames_[free_list_[free_head_]];
}

void FastFramePool::Release(FastFrame* frame) noexcept {
    frame->CleanupTracked();
    uint32_t idx = static_cast<uint32_t>(frame - frames_);
    free_list_[free_head_] = idx;
    ++free_head_;
}

// ── SetupFastFrame ────────────────────────────────────────────────────────

void SetupFastFrame(FastFrame* ff,
                    const void* patch_method,
                    const void* args_buf,
                    const void* ir,
                    void* dispatch_fn,
                    void* dispatch_ctx) noexcept {
    // Reset runtime state (cheaper than memset of 416 bytes).
    ff->sp = 0;
    ff->pc = 0;
    ff->has_ret = false;
    ff->ret_val = 0;
    ff->ret_tag = 0;
    ff->threw_exception = false;
    ff->exception_obj_val = nullptr;
    ff->tracked_cnt = 0;  // CleanupTracked already freed all objects

    // Set arg count and args buffer.
    auto* pm = static_cast<const PatchMethod*>(patch_method);
    if (pm->cached_sig_valid) {
        ff->arg_count = pm->cached_arg_count;
    } else {
        ff->arg_count = 0;
        if (pm->signature_blob != nullptr && pm->signature_len > 1) {
            const uint8_t* sig = pm->signature_blob;
            const uint8_t* sig_data = sig + 1;
            uint8_t count_byte = sig_data[1];
            if (count_byte <= 0x7F) {
                ff->arg_count = count_byte;
            } else if (count_byte <= 0xBF) {
                ff->arg_count = static_cast<uint32_t>(
                    ((count_byte & 0x3F) << 8) | sig_data[2]);
            }
            if ((sig_data[0] & 0x20) == 0x20) {
                ff->arg_count += 1;
            }
        }
    }
    ff->args = args_buf;

    // Wire dispatch callback and call-site cache.
    ff->dispatch_fn = dispatch_fn;
    ff->dispatch_ctx = dispatch_ctx;
    ff->call_cache = pm->call_cache;
    auto* method_ir = static_cast<const interpreter::IRMethod*>(ir);
    ff->call_count = static_cast<uint32_t>(method_ir->instructions.size());
}

}  // namespace chaos::il2cpp::runtime_core