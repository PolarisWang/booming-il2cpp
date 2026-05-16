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
    runtime_stubs/random_stubs.cpp
    runtime_stubs/stream_stubs.cpp
    runtime_stubs/hashcode_stubs.cpp
    runtime_stubs/interlocked_stubs.cpp
    runtime_stubs/string_stubs.cpp
    runtime_stubs/exception_stubs.cpp
    runtime_stubs/object_stubs.cpp
    runtime_stubs/threading_stubs.cpp
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
    method_table.cpp
    method_table.h
    reflection_api.cpp
    reflection_api.h
    abi_manifest.cpp
    hotpatch_table.h
    hotpatch_table.cpp
    patch_loader.h
    patch_loader.cpp
    metadata_interface.h
    metadata_interface.cpp
    assembly_manager.h
    assembly_manager.cpp
    gc/gc_bump_cache.h
    gc/gc_bit_utils.h
    gc/gc_helpers.h
    gc/gc_transition.h
    gc/gc_scheduler.h
    gc/gc_scheduler.cpp
    gc/gc_region.h
    gc/gc_region.cpp
    gc/gc_card_table.h
    gc/gc_card_table.cpp
    gc/gc_root_scanner.h
    gc/gc_root_scanner.cpp
    gc/gc_young_collector.h
    gc/gc_young_collector.cpp
    gc/gc_old_gen.h
    gc/gc_old_gen.cpp
    gc/gc_layout.h
    gc/gc_layout.cpp
    gc/gc_loh.h
    gc/gc_loh.cpp
    gc/gc_parallel_mark.h
    gc/gc_parallel_mark.cpp
    gc/gc_events.h
    gc/gc_events.cpp
    gc/gc_stats.h
    gc/gc_stats.cpp
    gc/gc_worker_pool.h
    gc/gc_worker_pool.cpp
    gc/gc_bgc.h
    gc/gc_bgc.cpp
    gc/gc_bgc_inline.h
    domain_unloader.h
    domain_unloader.cpp
    native_library.h
    native_library.cpp
    exception_api.cpp
    exception_helpers.h
    exception_helpers.cpp
    )
target_compile_features(chaos_runtime_core PUBLIC cxx_std_17)
target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_RUNTIME_ABI_EXPORTS)
target_compile_definitions(chaos_runtime_core PUBLIC GC_NOT_DLL)
target_compile_definitions(chaos_runtime_core PUBLIC NOMINMAX)
target_compile_options(chaos_runtime_core PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /FS /EHa>)
target_include_directories(
    chaos_runtime_core
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/common"
        "${REPO_ROOT}/src/native/interpreter"
        "${REPO_ROOT}/src/native/interpreter/generated"
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/gc")

target_link_libraries(chaos_runtime_core PUBLIC chaos_bdwgc chaos_fmt chaos_common)
target_include_directories(chaos_runtime_core PRIVATE
    "${REPO_ROOT}/third_party/bdwgc/include"
    "${REPO_ROOT}/src/native"
    "${REPO_ROOT}/third_party/fmt/include")
