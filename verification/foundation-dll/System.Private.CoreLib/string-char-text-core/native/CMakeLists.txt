# ── Benchmark executable ──────────────────────────────────────────
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkNativeEntry.cpp)
add_executable(chaos_benchmark_string_char_text_core
    ${CMAKE_SOURCE_DIR}/src/native/benchmark-host/native_aot_main.cpp
    BenchmarkNativeEntry.cpp
)
target_compile_features(chaos_benchmark_string_char_text_core PRIVATE cxx_std_17)
target_include_directories(chaos_benchmark_string_char_text_core PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/common
    ${CMAKE_SOURCE_DIR}/contracts/native/v0
)
target_link_libraries(chaos_benchmark_string_char_text_core PRIVATE chaos_common)
endif()
