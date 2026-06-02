get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_pal STATIC
    chaos/pal/pal_types.h
    chaos/pal/pal.h
    # ── Phase 1: Dynamic library loading ──────────────────────────
    chaos/pal/pal_dl.h
    $<$<PLATFORM_ID:Windows>:pal_dl_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_dl_posix.cpp>
    # ── Phase 1: Time measurement ─────────────────────────────────
    chaos/pal/pal_time.h
    $<$<PLATFORM_ID:Windows>:pal_time_win32.cpp>
    $<$<PLATFORM_ID:Linux>:pal_time_linux.cpp>
    $<$<PLATFORM_ID:Darwin>:pal_time_macos.cpp>
    # Fallback for platforms without specific time implementation
    $<$<NOT:$<PLATFORM_ID:Windows,Linux,Darwin>>:pal_time_fallback.cpp>
    # ── Phase 2: Virtual memory ────────────────────────────────────
    chaos/pal/pal_mem.h
    $<$<PLATFORM_ID:Windows>:pal_mem_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_mem_posix.cpp>
    # ── Phase 3: CPU feature detection ─────────────────────────────
    chaos/pal/pal_cpu.h
    $<$<PLATFORM_ID:Windows>:pal_cpu_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_cpu_posix.cpp>
    # ── Phase 4: Synchronization ───────────────────────────────────
    chaos/pal/pal_sync.h
    $<$<PLATFORM_ID:Windows>:pal_sync_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_sync_posix.cpp>
    # ── Phase 4: Threading ─────────────────────────────────────────
    chaos/pal/pal_thread.h
    $<$<PLATFORM_ID:Windows>:pal_thread_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_thread_posix.cpp>
    # ── Phase 5: Crash/signal handling ─────────────────────────────
    chaos/pal/pal_crash.h
    $<$<PLATFORM_ID:Windows>:pal_crash_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_crash_posix.cpp>
    # ── Phase 6: Instruction cache coherency ─────────────────────────
    chaos/pal/pal_cache.h
    $<$<PLATFORM_ID:Windows>:pal_cache_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_cache_posix.cpp>
    # ── Phase 3: Preemptive thread suspend (PAL consumer migration) ──
    chaos/pal/pal_preempt.h
    $<$<PLATFORM_ID:Windows>:pal_preempt_win32.cpp>
    $<$<PLATFORM_ID:Linux>:pal_preempt_posix.cpp>
    $<$<PLATFORM_ID:Darwin>:pal_preempt_apple.cpp>
    $<$<PLATFORM_ID:Android>:pal_preempt_android.cpp>
    # ── Track A: Domain heap abstraction ──────────────────────────────
    chaos/pal/pal_heap.h
    $<$<PLATFORM_ID:Windows>:pal_heap_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_heap_posix.cpp>
    # ── Track A: Low-memory monitor ───────────────────────────────────
    chaos/pal/pal_low_mem.h
    $<$<PLATFORM_ID:Windows>:pal_low_mem_win32.cpp>
    $<$<PLATFORM_ID:Linux>:pal_low_mem_linux.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows,Linux>>:pal_low_mem_fallback.cpp>
    # ── Track B: SEH-safe native call ─────────────────────────────────
    chaos/pal/pal_eh.h
    $<$<PLATFORM_ID:Windows>:pal_eh_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_eh_posix.cpp>
    # ── Track C: Wakeable queue (thread pool) ─────────────────────────
    chaos/pal/pal_wakeable.h
    $<$<PLATFORM_ID:Windows>:pal_wakeable_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_wakeable_posix.cpp>
    # ── Track 5: ETW abstraction ───────────────────────────────────────
    chaos/pal/pal_etw.h
    $<$<PLATFORM_ID:Windows>:pal_etw_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_etw_posix.cpp>
    # ── Track 7: Secure random bytes ───────────────────────────────────
    chaos/pal/pal_random.h
    $<$<PLATFORM_ID:Windows>:pal_random_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_random_posix.cpp>
    # ── Track 4: COM abstraction ─────────────────────────────────────────
    chaos/pal/pal_com.h
    $<$<PLATFORM_ID:Windows>:pal_com_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_com_posix.cpp>
    # ── Round 2: OS error/process/thread info ─────────────────────────────
    chaos/pal/pal_error.h
    $<$<PLATFORM_ID:Windows>:pal_error_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_error_posix.cpp>
    # ── Round 2: String encoding ──────────────────────────────────────────
    chaos/pal/pal_encoding.h
    $<$<PLATFORM_ID:Windows>:pal_encoding_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_encoding_posix.cpp>
    )

target_compile_features(chaos_pal PUBLIC cxx_std_17)
target_compile_definitions(chaos_pal PRIVATE NOMINMAX)

target_include_directories(
    chaos_pal
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(chaos_pal PUBLIC chaos_fmt chaos_unordered_dense)
