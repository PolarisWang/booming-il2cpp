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
    ff->exception_obj_val = 0;
    ff->tracked_cnt = 0;  // CleanupTracked already freed all objects
    ff->tracked_overflow = nullptr;

    // OSR state: reset backedge counter, wire patch_method for tier access.
    ff->patch_method = const_cast<void*>(patch_method);
    ff->loop_counter = 0;
    ff->osr_reenable = false;
    ff->last_pc = 0;
    ff->pgo_branch_taken = 0;
    ff->pgo_branch_not_taken = 0;

    // Set arg count, args buffer, and arg type tags.
    auto* pm = static_cast<const PatchMethod*>(patch_method);
    if (pm->cached_sig_valid) {
        ff->arg_count = pm->cached_arg_count;
        ff->arg_type_tags = pm->cached_arg_types;
    } else {
        ff->arg_count = 0;
        ff->arg_type_tags = nullptr;
        if (pm->signature_blob != nullptr && pm->signature_len > 1) {
            // ECMA-335 II.23.2.12: compressed unsigned integer encoding.
            constexpr uint8_t kSigMaxOneByte  = 0x7F;
            constexpr uint8_t kSigMaxTwoByte  = 0xBF;
            constexpr uint8_t kSigTwoByteMask = 0x3F;
            constexpr uint8_t kSigHasThisFlag = 0x20;
            const uint8_t* sig = pm->signature_blob;
            const uint8_t* sig_data = sig + 1;
            uint8_t count_byte = sig_data[1];
            if (count_byte <= kSigMaxOneByte) {
                ff->arg_count = count_byte;
            } else if (count_byte <= kSigMaxTwoByte) {
                ff->arg_count = static_cast<uint32_t>(
                    ((count_byte & kSigTwoByteMask) << 8) | sig_data[2]);
            }
            if ((sig_data[0] & kSigHasThisFlag) == kSigHasThisFlag) {
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

    // Wire SEH clauses (nullptr/0 when method has none).
    ff->seh_clauses = method_ir->seh_clauses.data();
    ff->seh_clause_count = static_cast<uint32_t>(method_ir->seh_clauses.size());

    // Reset SEH state.
    ff->exception_in_flight = false;
    ff->unwind_catch_clause = -1;
    ff->unwind_finally_count = 0;
    ff->unwind_finally_current = 0;
    ff->pending_leave = false;
    ff->pending_leave_target = 0;
}

}  // namespace chaos::il2cpp::runtime_core