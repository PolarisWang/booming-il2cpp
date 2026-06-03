cmake_minimum_required(VERSION 3.15)
project(chaos_runtime_core LANGUAGES CXX)

# Note: CMAKE_MSVC_RUNTIME_LIBRARY intentionally not set.
# MSVC defaults to /MD on this toolchain, which matches the main build's convention.
# Do NOT add explicit MultiThreadedDLL here — it's the MSVC default when unset.

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

include("${REPO_ROOT}/cmake/config-tier.cmake")

add_library(chaos_runtime_core STATIC
    runtime_core.cpp
    runtime_core.h
    memory_domain.cpp
    memory_domain.h
    memory_domain_events.h
    vtable_registry.cpp
    vtable_registry.h
    convert.h
    convert.cpp
    parse_convert.h
    parse_convert.cpp
    runtime_vtable.h
    generic_context.cpp
    generic_context.h
    string_table.h
    string_table.cpp
    type_registry.cpp
    eeclass.h
    eeclass.cpp
    runtime_stubs/math_stubs.cpp
    runtime_stubs/array_stubs.cpp
    runtime_stubs/datetime_stubs.cpp
    runtime_stubs/char_stubs.cpp
    runtime_stubs/guid_stubs.cpp
    runtime_stubs/enum_stubs.cpp
    runtime_stubs/random_stubs.cpp
    runtime_stubs/stream_stubs.cpp
    runtime_stubs/hashcode_stubs.cpp
    runtime_stubs/interlocked_stubs.cpp
    runtime_stubs/string_stubs.cpp
    runtime_stubs/exception_stubs.cpp
    runtime_stubs/object_stubs.cpp
    runtime_stubs/threading_stubs.cpp
    runtime_stubs/monitor_stubs.cpp
    runtime_stubs/mutex_stubs.cpp
    runtime_stubs/threadpool_events.cpp
    runtime_stubs/convert_stubs.cpp
    runtime_stubs/collection_stubs.cpp
    runtime_stubs/misc_stubs.cpp
    runtime_stubs/interpreter_entry_direct.cpp
    runtime_stubs/registration_globals.cpp
    module_registry.cpp
    module_registry.h
    layout_engine.cpp
    layout_engine.h
    instantiation_engine.cpp
    instantiation_engine.h
    thread_state.cpp
    thread_state.h
    jit_demotion.cpp
    jit_demotion.h
    jit/jit_debug_contract.cpp
    thread_pool.cpp
    thread_pool.h
    timer_queue.cpp
    timer_queue.h
    wait_handle.cpp
    wait_handle.h
    synchronization.cpp
    synchronization.h
    execution_context.cpp
    execution_context.h
    cancellation_token.cpp
    cancellation_token.h
    task_runner.cpp
    generic_method_instantiation.cpp
    generic_method_instantiation.h
    aot_core_ir_reader.cpp
    aot_core_ir_reader.h
    core/delegate_helpers.cpp
    core/delegate_helpers.h
    method_table.cpp
    method_table.h
    reflection_api.cpp
    reflection_api.h
    abi_manifest.cpp
    hotpatch_table.h
    hotpatch_table.cpp
    patch_loader.h
    patch_loader.cpp
    jit_registration.h
    jit_registration.cpp
    metadata_interface.h
    metadata_interface.cpp
    assembly_manager.h
    assembly_manager.cpp
    static_var_store.h
    static_var_store.cpp
    gc/gc_bump_cache.h
    gc/gc_bit_utils.h
    gc/gc_features.h
    gc/gc_helpers.h
    gc/gc_transition.h
    gc/gc_scheduler.h
    gc/gc_scheduler.cpp
    gc/gc_region.h
    gc/gc_region.cpp
    gc/gc_root_change.h
    gc/gc_root_change.cpp
    gc/gc_card_table.h
    gc/gc_card_table.cpp
    gc/gc_root_scanner.h
    gc/gc_root_scanner.cpp
    gc/gc_static_roots.h
    gc/gc_static_roots.cpp
    gc/gc_young_collector.h
    gc/gc_young_collector.cpp
    gc/gc_old_gen.h
    gc/gc_old_gen.cpp
    gc/gc_demotion.h
    gc/gc_demotion.cpp
    gc/gc_layout.h
    gc/gc_layout.cpp
    gc/gc_loh.h
    gc/gc_loh.cpp
    gc/gc_numa.h
    gc/gc_parallel_mark.h
    gc/gc_parallel_mark.cpp
    gc/gc_events.h
    gc/gc_events.cpp
    gc/gc_etw.h
    gc/gc_etw.cpp
    gc/gc_gen1.h
    gc/gc_gen1.cpp
    gc/gc_stats.h
    gc/gc_stats.cpp
    gc/gc_stress.h
    gc/gc_stress.cpp
    gc/gc_worker_pool.h
    gc/gc_worker_pool.cpp
    gc/gc_bgc.h
    gc/gc_bgc.cpp
    gc/gc_bgc_inline.h
    gc/gc_api.h
    gc/gc_api.cpp
    gc/gc_coordinator.h
    gc/gc_coordinator.cpp
    gc/gc_heap.h
    gc/gc_heap_manager.h
    gc/gc_heap_manager.cpp
    gc/gc_low_mem.h
    gc/gc_low_mem.cpp
    com_rcw.h
    com_rcw.cpp
    com_ccw.h
    com_ccw.cpp
    com_connection_point.h
    com_connection_point.cpp
    com_platform.h
    com_factory.h
    com_factory.cpp
    com_typelib.h
    com_typelib.cpp
    domain_unloader.h
    domain_unloader.cpp
    native_library.h
    native_library.cpp
    exception_api.cpp
    exception_helpers.h
    exception_helpers.cpp
    tier_manager.h
    tier_manager.cpp
    )
# ── 执行引擎配置（编译时常量）─────────────────────────────────────
# 选择 ExecutionConfig 平台配置，控制 JIT/AOT 引擎的激活状态。
# 通过 -DCHAOS_IL2CPP_EXEC_PROFILE=desktop|mobile-release|test-only 设置。
# 默认 desktop（开发调试全模式）。
option(CHAOS_IL2CPP_EXEC_PROFILE "Execution engine profile: desktop|mobile-release|test-only" "desktop")
if(CHAOS_IL2CPP_EXEC_PROFILE STREQUAL "mobile-release")
    target_compile_definitions(chaos_runtime_core PUBLIC
        CHAOS_IL2CPP_RUNTIME_CONFIG_FILE="runtime-config/mobile-release.h")
elseif(CHAOS_IL2CPP_EXEC_PROFILE STREQUAL "test-only")
    target_compile_definitions(chaos_runtime_core PUBLIC
        CHAOS_IL2CPP_RUNTIME_CONFIG_FILE="runtime-config/test-only.h")
else()
    target_compile_definitions(chaos_runtime_core PUBLIC
        CHAOS_IL2CPP_RUNTIME_CONFIG_FILE="runtime-config/desktop.h")
endif()

# ── GC feature selection (G-29: compile-time feature toggles) ──────
#
# Each option maps to a CHAOS_IL2CPP_GC_* compile definition.
# Disable features to reduce binary size and complexity on constrained
# platforms.  CoreCLR equivalent: FEATURE_* macros in gcpriv.h.
#
# See gc/gc_features.h for the full feature flag reference.

option(CHAOS_IL2CPP_GC_BGC          "Enable Background GC (BGC) concurrent mark-sweep"     ON)
option(CHAOS_IL2CPP_GC_PARALLEL_MARK "Enable parallel marking in GC"                        ON)
option(CHAOS_IL2CPP_GC_EVENTS       "Enable GC event tracing and callbacks"                 ON)
option(CHAOS_IL2CPP_GC_STRESS       "Enable GC stress testing mode (debug/validation only)" OFF)
option(CHAOS_IL2CPP_GC_LARGE_PAGES  "Enable large page (2MB) support (requires SeLockMemoryPrivilege)" ON)
option(CHAOS_IL2CPP_GC_SERVER      "Enable Server GC mode with per-NUMA-node heaps"              OFF)

option(CHAOS_IL2CPP_GC_HEAP_HARD_LIMIT_MB
    "Hard memory limit in MB (0=disabled). Allocation exceeding this fails."
    0)
option(CHAOS_IL2CPP_GC_HEAP_SOFT_LIMIT_MB
    "Soft memory limit in MB (0=disabled). Triggers GC when exceeded, does not fail."
    0)

if(CHAOS_IL2CPP_GC_BGC)
    target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_IL2CPP_GC_BGC=1)
endif()
if(CHAOS_IL2CPP_GC_PARALLEL_MARK)
    target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_IL2CPP_GC_PARALLEL_MARK=1)
endif()
if(CHAOS_IL2CPP_GC_EVENTS)
    target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_IL2CPP_GC_EVENTS=1)
endif()
if(CHAOS_IL2CPP_GC_STRESS)
    target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_IL2CPP_GC_STRESS=1)
endif()
if(CHAOS_IL2CPP_GC_LARGE_PAGES)
    target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_IL2CPP_GC_LARGE_PAGES=1)
endif()
if(CHAOS_IL2CPP_GC_SERVER)
    target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_IL2CPP_GC_SERVER=1)
endif()
if(CHAOS_IL2CPP_GC_HEAP_HARD_LIMIT_MB)
    target_compile_definitions(chaos_runtime_core PRIVATE
        CHAOS_IL2CPP_GC_HEAP_HARD_LIMIT_MB=${CHAOS_IL2CPP_GC_HEAP_HARD_LIMIT_MB})
endif()
if(CHAOS_IL2CPP_GC_HEAP_SOFT_LIMIT_MB)
    target_compile_definitions(chaos_runtime_core PRIVATE
        CHAOS_IL2CPP_GC_HEAP_SOFT_LIMIT_MB=${CHAOS_IL2CPP_GC_HEAP_SOFT_LIMIT_MB})
endif()
target_compile_options(chaos_runtime_core PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHa /GS- /FS>)

# guid_stubs uses SSE4.1 intrinsics (_mm_blendv_epi8) on x86; ARM64 uses NEON.
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
  set_source_files_properties(runtime_stubs/guid_stubs.cpp PROPERTIES COMPILE_OPTIONS "-msse4.1")
endif()
target_include_directories(
    chaos_runtime_core
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/common"
        "${REPO_ROOT}/src/native/interpreter"
        "${REPO_ROOT}/src/native/interpreter/generated"
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/gc"
        "${CMAKE_CURRENT_LIST_DIR}/runtime_stubs"
        "${REPO_ROOT}/third_party/unordered_dense/include"
        "${REPO_ROOT}/cmake")

target_link_libraries(chaos_runtime_core PUBLIC chaos_fmt chaos_common)
if(WIN32)
    target_link_libraries(chaos_runtime_core PUBLIC chaos_eventpipe)
    target_link_libraries(chaos_runtime_core PUBLIC chaos_debugger)
    target_link_libraries(chaos_runtime_core PRIVATE bcrypt)
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
    target_link_libraries(chaos_runtime_core PUBLIC chaos_interpreter)
    target_link_libraries(chaos_runtime_core PUBLIC chaos_eventpipe)
    target_link_libraries(chaos_runtime_core PRIVATE numa)
endif()

# Platform-specific sources
if(WIN32)
    target_sources(chaos_runtime_core PRIVATE
        gc/gc_numa_win.cpp
        runtime_stubs/mutex_stubs_win32.cpp)
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
    target_sources(chaos_runtime_core PRIVATE
        gc/gc_numa_linux.cpp
        runtime_stubs/linux_platform_stubs.cpp
        runtime_stubs/mutex_stubs_posix.cpp)
else()
    target_sources(chaos_runtime_core PRIVATE
        gc/gc_numa_other.cpp
        runtime_stubs/mutex_stubs_posix.cpp)
endif()
target_include_directories(chaos_runtime_core PRIVATE
    "${REPO_ROOT}/src/native"
    "${REPO_ROOT}/src/native/diagnostics"
    "${REPO_ROOT}/third_party/fmt/include")
