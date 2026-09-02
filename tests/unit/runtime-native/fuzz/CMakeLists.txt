# Fuzz subsystem tests
# IL fuzzer: links chaos_interpreter for instruction dispatch.
# GC fuzzer: links chaos_runtime_core for GC allocation.
# Directly compiles gc_young_collector.cpp et al. because the prebuilt
# chaos_runtime_core.lib uses GNU ar format that MSVC link.exe cannot
# search for COMDAT resolution (same pattern as gc/CMakeLists.txt Batch D).

set(CHAOS_FUZZ_GC_SRC
    fuzz_gc_test.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime-core/gc/gc_test_base.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime-core/gc/gc_test_stubs.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/gc/gc_young_collector.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/gc/gc_scheduler.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/gc/gc_card_table.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/gc/gc_api.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/gc/gc_region.cpp"
)

set(CHAOS_FUZZ_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_jit
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
)

chaos_native_add_test(test_fuzz_il
    fuzz_il_test.cpp
    LIBS ${CHAOS_FUZZ_LIBS}
    RESOURCE_LOCK bgc
    LABELS "fuzz;interpreter"
)
set_tests_properties(test_fuzz_il PROPERTIES TIMEOUT 600)

chaos_native_add_test(test_fuzz_gc
    ${CHAOS_FUZZ_GC_SRC}
    LIBS ${CHAOS_FUZZ_LIBS}
    RESOURCE_LOCK bgc
    LABELS "fuzz;gc"
)
target_include_directories(test_fuzz_gc PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime-core/gc"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/gc"
)
target_link_options(test_fuzz_gc PRIVATE /FORCE:MULTIPLE)
set_tests_properties(test_fuzz_gc PROPERTIES TIMEOUT 600)

# VTable dispatch fuzzer: random slot lookups + type instantiation.
# Links chaos_runtime_core for VTableRegistry.
chaos_native_add_test(test_fuzz_vtable
    fuzz_vtable_test.cpp
    LIBS ${CHAOS_FUZZ_LIBS}
    LABELS "fuzz;runtime-core"
)
set_tests_properties(test_fuzz_vtable PROPERTIES TIMEOUT 600)
