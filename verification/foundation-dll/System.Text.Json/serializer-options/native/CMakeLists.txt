set(chaos_family_native_serializer_options_SOURCES
    RuntimeSkeletonSummary.cpp
    RuntimeSkeletonPage0001.cpp
)

add_library(chaos_family_native_serializer_options STATIC EXCLUDE_FROM_ALL
    ${chaos_family_native_serializer_options_SOURCES}
)
target_include_directories(chaos_family_native_serializer_options PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/runtime-core
    ${CMAKE_SOURCE_DIR}/src/native/bootstrap
    ${CMAKE_SOURCE_DIR}/contracts/native/v0
)
target_link_libraries(chaos_family_native_serializer_options PRIVATE
    chaos_runtime_core
)

# ---------------------------------------------------------------------------
# Benchmark executable (auto-generated)
# ---------------------------------------------------------------------------
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkNativeEntry.cpp)
add_executable(chaos_benchmark_serializer_options
    ${CMAKE_SOURCE_DIR}/src/native/benchmark-host/native_aot_main.cpp
    BenchmarkNativeEntry.cpp
)
target_compile_features(chaos_benchmark_serializer_options PRIVATE cxx_std_17)
target_include_directories(chaos_benchmark_serializer_options PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/common
    ${CMAKE_SOURCE_DIR}/contracts/native/v0
)
target_link_libraries(chaos_benchmark_serializer_options PRIVATE chaos_common)
endif()
