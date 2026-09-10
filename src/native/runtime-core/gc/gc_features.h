#ifndef CHAOS_IL2CPP_GC_FEATURES_H_
#define CHAOS_IL2CPP_GC_FEATURES_H_

/// @file gc_features.h
/// GC compile-time feature flag reference (G-29).
///
/// Each flag is a CMake option (see src/native/runtime-core/CMakeLists.txt)
/// propagated as a preprocessor define.  Disable features to reduce binary
/// size and complexity on memory-constrained or embedded platforms.
///
/// CoreCLR equivalent: FEATURE_* macros in gcpriv.h.
///
/// == Flag reference ==
///
/// CHAOS_IL2CPP_GC_BGC (=1)
///   Background GC concurrent mark-sweep.  When disabled, all collections
///   are stop-the-world.  Saves ~3 KLOC of concurrent GC code.
///   Default: ON (enabled).
///
/// CHAOS_IL2CPP_GC_PARALLEL_MARK (=1)
///   Parallel marking using worker threads during GC mark phase.  When
///   disabled, marking is single-threaded.  Saves worker pool overhead.
///   Default: ON (enabled) — Windows x64 reference.
///
/// CHAOS_IL2CPP_GC_EVENTS (=1)
///   GC event tracing and callbacks (GcFireEvent, GcRegisterCallback).
///   When disabled, GcFireEvent becomes a no-op and callback storage is
///   eliminated.  Saves ~256 bytes of static data + dispatch overhead.
///   Default: ON (enabled).
///
/// CHAOS_IL2CPP_GC_LARGE_PAGES (=1)
///   Large page (2MB) support for GC heap allocations.  Requires
///   SeLockMemoryPrivilege on Windows.  Reduces TLB miss rates for
///   large heaps.  Falls back to 4KB pages when large pages are
///   unavailable or privilege is lacking.
///   Default: ON (enabled) with automatic fallback to 4KB.
///
/// CHAOS_IL2CPP_GC_SERVER (=0)
///   Server GC mode with per-NUMA-node heaps.  When enabled, each NUMA
///   node gets its own heap context (old gen, LOH, scheduler).  Threads
///   bind to a heap based on NUMA affinity.  GC runs across all heaps in
///   parallel via GcCoordinator.
///   When disabled (default): single-heap WKS mode with global state.
///   Default: OFF (disabled) — WKS workstation GC compatible with Unity.
///
/// CHAOS_IL2CPP_GC_HEAP_HARD_LIMIT_MB (=0)
///   Hard memory limit in MB.  When the estimated heap size (old-gen page
///   usage + external memory pressure) exceeds this limit, allocation
///   fails and returns nullptr, triggering the OOM recovery chain.
///   After a full GC frees memory, allocation may succeed if the freed
///   space brings the heap back below the limit.
///   0 = no hard limit (default).
///
/// CHAOS_IL2CPP_GC_HEAP_SOFT_LIMIT_MB (=0)
///   Soft memory limit in MB.  When the estimated heap size exceeds this
///   limit, a full GC is requested at the next safepoint (via
///   RequestFullGc).  Allocation is not blocked — the limit is advisory.
///   Useful for proactive GC before the hard limit is reached.
///   0 = no soft limit (default).

#endif  // CHAOS_IL2CPP_GC_FEATURES_H_
