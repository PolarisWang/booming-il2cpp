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
    )

target_compile_features(chaos_pal PUBLIC cxx_std_17)
target_compile_definitions(chaos_pal PRIVATE NOMINMAX)

target_include_directories(
    chaos_pal
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(chaos_pal PUBLIC chaos_fmt chaos_unordered_dense)
