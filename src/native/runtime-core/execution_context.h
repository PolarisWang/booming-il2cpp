#ifndef CHAOS_IL2CPP_EXECUTION_CONTEXT_H_
#define CHAOS_IL2CPP_EXECUTION_CONTEXT_H_

#include <chaos/native_types.h>

#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::runtime_core::threading {

// ── AsyncLocal value entry ────────────────────────────────────────────
// A single AsyncLocal<T> value captured in an ExecutionContext.
struct AsyncLocalValue {
    uint64_t    key;        // Stable ID of the AsyncLocal<T> instance
    CHAOS_IL2CPP_INTPTR value;  // Boxed value (or 0 if unset)
};

// ── ExecutionContext ──────────────────────────────────────────────────
// Captures AsyncLocal values for flow across thread/ThreadPool boundaries.
// AOT types: small fixed-size array (no heap allocation for common case).
// Dynamic types: heap-allocated list.
struct ExecutionContext {
    /// Number of captured AsyncLocal values.
    uint32_t                value_count{0};
    /// Pointer to AsyncLocalValue array (null if value_count == 0).
    /// For the common case (0-4 values), points to inline_storage.
    /// For larger captures, heap-allocated.
    AsyncLocalValue*        values{nullptr};
    /// Inline storage for small captures (avoids heap allocation).
    AsyncLocalValue         inline_storage[4]{};
    /// True if values points to heap-allocated memory.
    bool                    heap_allocated{false};
};

// ── Public API ────────────────────────────────────────────────────────

/// Capture the current ExecutionContext (saves all current AsyncLocal values).
/// Returns a pointer to a context that must be freed with ExecutionContextFree.
/// Returns nullptr if no AsyncLocal values are set (optimization: no flow needed).
ExecutionContext* ExecutionContextCapture() noexcept;

/// Run a callback under the given ExecutionContext.
/// Temporarily installs the context's AsyncLocal values, invokes the callback,
/// then restores the previous context.
void ExecutionContextRun(ExecutionContext* ctx, void (*callback)(void*), void* state) noexcept;

/// Free an ExecutionContext previously returned by ExecutionContextCapture.
void ExecutionContextFree(ExecutionContext* ctx) noexcept;

/// Set an AsyncLocal value for the current thread.
/// @param key  Stable ID of the AsyncLocal<T> instance.
/// @param value  Boxed value (0 to clear).
void AsyncLocalSetValue(uint64_t key, CHAOS_IL2CPP_INTPTR value) noexcept;

/// Get an AsyncLocal value for the current thread.
/// @param key  Stable ID of the AsyncLocal<T> instance.
/// @return The boxed value, or 0 if not set.
CHAOS_IL2CPP_INTPTR AsyncLocalGetValue(uint64_t key) noexcept;

}  // namespace chaos::il2cpp::runtime_core::threading

// extern "C" bridges for threading_stubs — declared outside the namespace
// so they are visible from chaos::il2cpp::runtime_core.
extern "C" chaos::il2cpp::runtime_core::threading::ExecutionContext* chaos_execution_context_capture() noexcept;
extern "C" void chaos_execution_context_run(
    chaos::il2cpp::runtime_core::threading::ExecutionContext* ctx,
    void (*callback)(void*), void* state) noexcept;
extern "C" void chaos_execution_context_free(
    chaos::il2cpp::runtime_core::threading::ExecutionContext* ctx) noexcept;

#endif  // CHAOS_IL2CPP_EXECUTION_CONTEXT_H_