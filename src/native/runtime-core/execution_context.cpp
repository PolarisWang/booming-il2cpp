// execution_context.cpp — ExecutionContext capture/restore + AsyncLocal value storage
//
// Design:
//   - AsyncLocal values are stored in a thread_local map (small array for common case).
//   - ExecutionContextCapture() snapshots all current AsyncLocal values into a context.
//   - ExecutionContextRun() temporarily installs the context's values, invokes the callback,
//     then restores the previous values.
//   - Thread.Start and ThreadPool work items call ExecutionContextRun() to flow context.
//
// Performance:
//   - No AsyncLocal values set → ExecutionContextCapture() returns nullptr (zero cost).
//   - 1-4 values → inline_storage used (no heap allocation).
//   - 5+ values → heap-allocated array (rare in practice).

#include "execution_context.h"

#include <chaos/log.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <new>

namespace chaos::il2cpp::runtime_core::threading {

// ── Thread-local AsyncLocal value storage ─────────────────────────────
// Small-fallback pattern: most threads have 0-4 AsyncLocal values.

namespace {

constexpr uint32_t kInlineValueCount = 4;
constexpr uint32_t kMaxValues = 64;  // Sanity cap

struct AsyncLocalMap {
    AsyncLocalValue inline_values[kInlineValueCount]{};
    AsyncLocalValue* heap_values{nullptr};
    uint32_t heap_capacity{0};
    uint32_t count{0};

    ~AsyncLocalMap() noexcept {
        delete[] heap_values;
        heap_values = nullptr;
    }

    AsyncLocalValue& ValueAt(uint32_t idx) noexcept {
        return idx < kInlineValueCount ? inline_values[idx] : heap_values[idx - kInlineValueCount];
    }

    bool EnsureHeapCapacity(uint32_t needed) noexcept {
        if (needed <= kInlineValueCount) return true;
        uint32_t cap = needed + 4;  // small slop
        if (cap > kMaxValues) cap = kMaxValues;
        if (heap_values != nullptr && heap_capacity >= cap) return true;
        auto* new_heap = new (std::nothrow) AsyncLocalValue[cap];
        if (new_heap == nullptr) return false;
        // Copy existing heap values.
        uint32_t existing_heap = count > kInlineValueCount ? count - kInlineValueCount : 0;
        for (uint32_t i = 0; i < existing_heap && i < cap; i++) {
            new_heap[i] = heap_values[i];
        }
        delete[] heap_values;
        heap_values = new_heap;
        heap_capacity = cap;
        return true;
    }
};

thread_local AsyncLocalMap tls_async_locals;
thread_local int32_t tls_suppress_flow_depth = 0;

uint32_t FindValueIndex(uint64_t key) noexcept {
    for (uint32_t i = 0; i < tls_async_locals.count; i++) {
        if (tls_async_locals.ValueAt(i).key == key) {
            return i;
        }
    }
    return UINT32_MAX;
}

AsyncLocalValue* FindValue(uint64_t key) noexcept {
    uint32_t idx = FindValueIndex(key);
    return (idx != UINT32_MAX) ? &tls_async_locals.ValueAt(idx) : nullptr;
}

}  // anonymous namespace

// ── AsyncLocalSetValue / AsyncLocalGetValue ───────────────────────────

void AsyncLocalSetValue(uint64_t key, CHAOS_IL2CPP_INTPTR value) noexcept {
    uint32_t existing_idx = FindValueIndex(key);
    if (existing_idx != UINT32_MAX) {
        if (value == 0) {
            // Remove the entry by shifting remaining entries left.
            uint32_t count = tls_async_locals.count;
            for (uint32_t i = existing_idx + 1; i < count; i++) {
                tls_async_locals.ValueAt(i - 1) = tls_async_locals.ValueAt(i);
            }
            tls_async_locals.count--;
        } else {
            tls_async_locals.ValueAt(existing_idx).value = value;
        }
        return;
    }

    if (value == 0) return;  // Don't add a zero-valued entry.

    if (tls_async_locals.count >= kMaxValues) {
        CHAOS_IL2CPP_LOG_WARN("AsyncLocal", "exceeded max values");
        return;
    }

    uint32_t idx = tls_async_locals.count;
    if (!tls_async_locals.EnsureHeapCapacity(idx + 1)) {
        CHAOS_IL2CPP_LOG_WARN("AsyncLocal", "failed to allocate heap storage");
        return;
    }

    tls_async_locals.ValueAt(idx).key = key;
    tls_async_locals.ValueAt(idx).value = value;
    tls_async_locals.count++;
}

CHAOS_IL2CPP_INTPTR AsyncLocalGetValue(uint64_t key) noexcept {
    auto* existing = FindValue(key);
    if (existing != nullptr) {
        return existing->value;
    }
    return 0;
}

// ── ExecutionContextCapture ───────────────────────────────────────────

ExecutionContext* ExecutionContextCapture() noexcept {
    if (tls_suppress_flow_depth > 0) {
        return nullptr;  // Flow suppressed — don't capture.
    }

    if (tls_async_locals.count == 0) {
        return nullptr;  // No values to capture — optimization
    }

    auto* ctx = new (std::nothrow) ExecutionContext();
    if (ctx == nullptr) return nullptr;

    ctx->value_count = tls_async_locals.count;

    if (tls_async_locals.count <= kInlineValueCount) {
        // Use inline storage (no heap allocation).
        ctx->values = ctx->inline_storage;
        ctx->heap_allocated = false;
    } else {
        // Heap-allocated array (rare).
        auto* heap_values = new (std::nothrow) AsyncLocalValue[tls_async_locals.count];
        if (heap_values == nullptr) {
            delete ctx;
            return nullptr;
        }
        ctx->values = heap_values;
        ctx->heap_allocated = true;
    }

    // Copy values.
    for (uint32_t i = 0; i < tls_async_locals.count; i++) {
        ctx->values[i] = tls_async_locals.ValueAt(i);
    }

    return ctx;
}

// ── ExecutionContextRun ───────────────────────────────────────────────

void ExecutionContextRun(ExecutionContext* ctx, void (*callback)(void*), void* state) noexcept {
    if (ctx == nullptr) {
        // No context to flow — run directly.
        if (callback) callback(state);
        return;
    }

    // Save current AsyncLocal values (heap-allocated if needed).
    uint32_t saved_count = tls_async_locals.count;
    AsyncLocalValue* saved_values = nullptr;
    bool saved_on_heap = false;

    if (saved_count > 0) {
        if (saved_count <= kInlineValueCount) {
            saved_values = new (std::nothrow) AsyncLocalValue[kInlineValueCount];
            if (saved_values != nullptr) {
                for (uint32_t i = 0; i < saved_count; i++) {
                    saved_values[i] = tls_async_locals.ValueAt(i);
                }
            }
            saved_on_heap = false;
        } else {
            saved_values = new (std::nothrow) AsyncLocalValue[saved_count];
            if (saved_values != nullptr) {
                for (uint32_t i = 0; i < saved_count; i++) {
                    saved_values[i] = tls_async_locals.ValueAt(i);
                }
            }
            saved_on_heap = true;
        }
    }

    // Ensure TLS has enough capacity for context values.
    if (ctx->value_count > 0) {
        tls_async_locals.EnsureHeapCapacity(ctx->value_count);
    }

    // Install context values.
    tls_async_locals.count = ctx->value_count;
    for (uint32_t i = 0; i < ctx->value_count; i++) {
        tls_async_locals.ValueAt(i) = ctx->values[i];
    }

    // Run the callback.
    if (callback) callback(state);

    // Restore saved values.
    tls_async_locals.count = saved_count;
    if (saved_values != nullptr) {
        for (uint32_t i = 0; i < saved_count; i++) {
            tls_async_locals.ValueAt(i) = saved_values[i];
        }
        if (saved_on_heap) {
            delete[] saved_values;
        } else {
            delete[] saved_values;
        }
    }
}

// ── ExecutionContextFree ──────────────────────────────────────────────

void ExecutionContextFree(ExecutionContext* ctx) noexcept {
    if (ctx == nullptr) return;

    if (ctx->heap_allocated && ctx->values != nullptr) {
        delete[] ctx->values;
    }

    delete ctx;
}

// ── SuppressFlow / RestoreFlow ────────────────────────────────────────

int32_t ExecutionContextSuppressFlow() noexcept {
    int32_t prev = tls_suppress_flow_depth;
    tls_suppress_flow_depth++;
    return prev;
}

void ExecutionContextRestoreFlow(int32_t cookie) noexcept {
    tls_suppress_flow_depth = cookie;
}

bool ExecutionContextIsFlowSuppressed() noexcept {
    return tls_suppress_flow_depth > 0;
}

}  // namespace chaos::il2cpp::runtime_core::threading

// extern "C" bridges for threading_stubs
extern "C" chaos::il2cpp::runtime_core::threading::ExecutionContext* chaos_execution_context_capture() noexcept {
    return chaos::il2cpp::runtime_core::threading::ExecutionContextCapture();
}

extern "C" void chaos_execution_context_run(
    chaos::il2cpp::runtime_core::threading::ExecutionContext* ctx,
    void (*callback)(void*), void* state) noexcept
{
    chaos::il2cpp::runtime_core::threading::ExecutionContextRun(ctx, callback, state);
}

extern "C" void chaos_execution_context_free(
    chaos::il2cpp::runtime_core::threading::ExecutionContext* ctx) noexcept
{
    chaos::il2cpp::runtime_core::threading::ExecutionContextFree(ctx);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_execution_context_suppress_flow() noexcept {
    return chaos::il2cpp::runtime_core::threading::ExecutionContextSuppressFlow();
}

extern "C" void chaos_execution_context_restore_flow(CHAOS_IL2CPP_INT32 cookie) noexcept {
    chaos::il2cpp::runtime_core::threading::ExecutionContextRestoreFlow(cookie);
}

extern "C" bool chaos_execution_context_is_flow_suppressed() noexcept {
    return chaos::il2cpp::runtime_core::threading::ExecutionContextIsFlowSuppressed() ? 1 : 0;
}