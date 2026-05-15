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
    AsyncLocalValue values[kInlineValueCount]{};
    uint32_t count{0};
};

thread_local AsyncLocalMap tls_async_locals;

/// Find an AsyncLocal value by key in the thread-local map.
AsyncLocalValue* FindValue(uint64_t key) noexcept {
    for (uint32_t i = 0; i < tls_async_locals.count; i++) {
        if (tls_async_locals.values[i].key == key) {
            return &tls_async_locals.values[i];
        }
    }
    return nullptr;
}

}  // anonymous namespace

// ── AsyncLocalSetValue / AsyncLocalGetValue ───────────────────────────

void AsyncLocalSetValue(uint64_t key, CHAOS_IL2CPP_INTPTR value) noexcept {
    auto* existing = FindValue(key);
    if (existing != nullptr) {
        existing->value = value;
        return;
    }

    // Add new entry.
    if (tls_async_locals.count >= kMaxValues) {
        CHAOS_IL2CPP_LOG_WARN("AsyncLocal", "exceeded max values");
        return;
    }

    if (tls_async_locals.count < kInlineValueCount) {
        tls_async_locals.values[tls_async_locals.count].key = key;
        tls_async_locals.values[tls_async_locals.count].value = value;
        tls_async_locals.count++;
    } else {
        CHAOS_IL2CPP_LOG_WARN("AsyncLocal", "inline storage full, value not stored");
    }
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
        ctx->values[i] = tls_async_locals.values[i];
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

    // Save current AsyncLocal values.
    AsyncLocalValue saved_values[kInlineValueCount];
    uint32_t saved_count = tls_async_locals.count;
    for (uint32_t i = 0; i < saved_count && i < kInlineValueCount; i++) {
        saved_values[i] = tls_async_locals.values[i];
    }

    // Install context values.
    tls_async_locals.count = ctx->value_count;
    for (uint32_t i = 0; i < ctx->value_count && i < kInlineValueCount; i++) {
        tls_async_locals.values[i] = ctx->values[i];
    }

    // Run the callback.
    if (callback) callback(state);

    // Restore saved values.
    tls_async_locals.count = saved_count;
    for (uint32_t i = 0; i < saved_count && i < kInlineValueCount; i++) {
        tls_async_locals.values[i] = saved_values[i];
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

}  // namespace chaos::il2cpp::runtime_core::threading