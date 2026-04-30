set(CHAOS_FAMILY_NATIVE_REFLECTION_BINDING_SOURCES
    RuntimeSkeletonSummary.cpp
    RuntimeSkeletonPage0001.cpp
)

add_library(chaos_family_native_reflection_binding STATIC EXCLUDE_FROM_ALL
    ${CHAOS_FAMILY_NATIVE_REFLECTION_BINDING_SOURCES}
)
target_include_directories(chaos_family_native_reflection_binding PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/runtime-core
    ${CMAKE_SOURCE_DIR}/src/native/bootstrap
    ${CMAKE_SOURCE_DIR}/contracts/native/v0
)
target_link_libraries(chaos_family_native_reflection_binding PRIVATE
    chaos_runtime_core
)

# ---------------------------------------------------------------------------
# Benchmark executable (auto-generated)
# ---------------------------------------------------------------------------
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkNativeEntry.cpp)
add_executable(chaos_benchmark_reflection_binding
    ${CMAKE_SOURCE_DIR}/src/native/benchmark-host/native_aot_main.cpp
    BenchmarkNativeEntry.cpp
)
target_compile_features(chaos_benchmark_reflection_binding PRIVATE cxx_std_17)
target_include_directories(chaos_benchmark_reflection_binding PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/common
    ${CMAKE_SOURCE_DIR}/contracts/native/v0
)
target_link_libraries(chaos_benchmark_reflection_binding PRIVATE chaos_common)
endif()


# Per-family hotupdate test from CodeGen-generated C++
# The patch file (native-aot.patch.generated.cpp) has RunNativeAot
# renamed to RunNativeAot_{ns_slug} to avoid symbol collision with
# the host file's RunNativeAot.

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/hotupdate/HotUpdateTest.cpp)
add_executable(chaos_hotupdate_reflection_binding
    hotupdate/HotUpdateTest.cpp
    hotupdate/genuine-fixed/native-aot.generated.cpp
    patch/native-aot.patch.generated.cpp
    semantic-patch/native-aot.semantic-patch.generated.cpp
)
target_compile_features(chaos_hotupdate_reflection_binding PRIVATE cxx_std_17)
target_compile_definitions(chaos_hotupdate_reflection_binding PRIVATE CHAOS_RUNTIME_ABI_STATIC)
target_include_directories(chaos_hotupdate_reflection_binding PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/common
    ${CMAKE_SOURCE_DIR}/src/native/runtime-core
    ${CMAKE_SOURCE_DIR}/src/native/bootstrap
    ${CMAKE_SOURCE_DIR}/contracts/native/v0
    ${CMAKE_SOURCE_DIR}/verification/foundation-dll/System.Private.CoreLib
)
# Force-include hotupdate config so CodeGen-generated code can find
# chaos_managed_pointer_local_slot_tag, chaos_is_string_id, etc.
target_compile_options(chaos_hotupdate_reflection_binding PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/FI"${CMAKE_SOURCE_DIR}/verification/foundation-dll/System.Private.CoreLib/native_hotupdate_config.h">
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-include${CMAKE_SOURCE_DIR}/verification/foundation-dll/System.Private.CoreLib/native_hotupdate_config.h>
)
# fmt requires /utf-8 on MSVC
target_compile_options(chaos_hotupdate_reflection_binding PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)
target_link_libraries(chaos_hotupdate_reflection_binding PRIVATE
    chaos_hot_update
    chaos_bootstrap
    chaos_runtime_core
    chaos_common
)
endif()
