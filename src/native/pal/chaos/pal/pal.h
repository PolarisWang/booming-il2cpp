#ifndef CHAOS_IL2CPP_PAL_H_
#define CHAOS_IL2CPP_PAL_H_

// ── PAL (Platform Abstraction Layer) ──────────────────────────────────
//
// The PAL provides a uniform API over OS services: dynamic library loading,
// virtual memory, synchronization, threading, crash/signal handling, CPU
// feature detection, and time measurement.
//
// Design principles:
//   1. Pure C functions (namespaced in chaos::il2cpp::pal) — no virtual
//      dispatch, no templates.  Zero runtime overhead.
//   2. Compile-time file selection — the linker sees only the correct
//      platform implementation.  No indirect calls.
//   3. Opaque handles — platform types are never exposed to consumers.
//   4. noexcept — PAL functions never throw.
//
// Adding a new platform:
//   1. Add a new _<platform>.cpp implementation file for each subsystem.
//   2. Update CMakeLists.txt to select the right files per target.
//   3. Implement every function declared in the headers.
// ============================================================================

#include <chaos/pal/pal_types.h>

// Subsystem headers (included unconditionally; unused functions are
// eliminated by the linker).
#include <chaos/pal/pal_dl.h>
#include <chaos/pal/pal_time.h>
#include <chaos/pal/pal_mem.h>
#include <chaos/pal/pal_cpu.h>
#include <chaos/pal/pal_sync.h>
#include <chaos/pal/pal_thread.h>
#include <chaos/pal/pal_crash.h>
#include <chaos/pal/pal_cache.h>

#endif  // CHAOS_IL2CPP_PAL_H_
