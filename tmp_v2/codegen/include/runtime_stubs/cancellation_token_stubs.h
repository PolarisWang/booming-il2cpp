// ── CancellationToken / CancellationTokenSource stub declarations ──
//
// codegen-visible declarations for the `chaos_cancellation_token_*` ABI.
// Definitions live in src/native/runtime-core/cancellation_token.cpp (all
// `extern "C"`, built into the runtime static library); the ShapeRegistry
// routes managed CancellationToken / CancellationTokenSource members here.
//
// Why this header exists: the generated TU only sees declarations that reach
// it through runtime_core.h → generated_code_compat.h → runtime_stubs/stubs.h.
// cancellation_token.h is NOT on that path — it declares the internal C++
// handle API (uint32_t CancellationTokenSourceCancel(uint32_t) etc.) and has
// no `extern "C"` names at all.  Without this header every call site in the
// generated C++ fails with C3861.
//
// ABI note: a managed CancellationToken is its CancellationTokenSource id;
// 0 is CancellationToken.None.  See cancellation_token.cpp for the mapping.
// Keep these declarations in exact sync with the definitions there.
//
// Include constraint: this header pulls <chaos/native_types.h>, which drags in
// <chaos/config.h> and the C++ std headers (<thread>, <mutex>, ...).  It must be
// included OUTSIDE any `extern "C"` block (see runtime_stubs/stubs.h) — MSVC
// fails to declare the global ::terminate with C2039 otherwise.
#pragma once

#include <chaos/native_types.h>

extern "C" {

// CancellationToken
CHAOS_IL2CPP_INT32 chaos_cancellation_token_is_cancellation_requested(
    CHAOS_IL2CPP_INT32 source_id) noexcept;
CHAOS_IL2CPP_INT32 chaos_cancellation_token_can_be_canceled(
    CHAOS_IL2CPP_INT32 source_id) noexcept;
void chaos_cancellation_token_throw_if_cancellation_requested(
    CHAOS_IL2CPP_INT32 source_id) noexcept;

// CancellationTokenSource lifecycle
CHAOS_IL2CPP_INT32 chaos_cancellation_token_source_create(void) noexcept;
CHAOS_IL2CPP_INT32 chaos_cancellation_token_source_create_with_timer(
    CHAOS_IL2CPP_INT32 due_time_ms) noexcept;
void chaos_cancellation_token_source_cancel(CHAOS_IL2CPP_INT32 source_id) noexcept;
void chaos_cancellation_token_source_dispose(CHAOS_IL2CPP_INT32 source_id) noexcept;

// In this runtime a CancellationToken IS its source id (0 = None), so
// get_token is the identity — it exists so the managed call site has a symbol.
CHAOS_IL2CPP_INT32 chaos_cancellation_token_source_get_token(
    CHAOS_IL2CPP_INT32 source_id) noexcept;

}  // extern "C"
