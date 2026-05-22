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
/// CHAOS_IL2CPP_GC_STRESS (=1)
///   GC stress testing mode.  Forces frequent GCs and randomizes heap
///   state for correctness validation.  Not intended for production use.
///   Default: OFF (disabled).
///
/// CHAOS_IL2CPP_GC_LARGE_PAGES (=1)
///   Large page (2MB) support for GC heap allocations.  Requires
///   SeLockMemoryPrivilege on Windows.  Reduces TLB miss rates for
///   large heaps.  See gc_loh.cpp for usage.
///   Default: OFF (disabled).

#endif  // CHAOS_IL2CPP_GC_FEATURES_H_
