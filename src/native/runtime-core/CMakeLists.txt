get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_runtime_core STATIC
    runtime_core.cpp
    runtime_core.h)

target_compile_features(chaos_runtime_core PUBLIC cxx_std_17)
target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_RUNTIME_ABI_EXPORTS)
target_compile_definitions(chaos_runtime_core PUBLIC CHAOS_RUNTIME_ABI_STATIC)
target_include_directories(
    chaos_runtime_core
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${CMAKE_CURRENT_LIST_DIR}")

