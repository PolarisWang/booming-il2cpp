get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_bootstrap STATIC
    bootstrap.cpp
    bootstrap.h)

target_compile_features(chaos_bootstrap PUBLIC cxx_std_17)
target_compile_definitions(chaos_bootstrap PRIVATE CHAOS_RUNTIME_ABI_EXPORTS)
target_compile_definitions(chaos_bootstrap PUBLIC CHAOS_RUNTIME_ABI_STATIC)
target_include_directories(
    chaos_bootstrap
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${CMAKE_CURRENT_LIST_DIR}"
    PRIVATE
        "${REPO_ROOT}/src/native/runtime-core"
        "${REPO_ROOT}/src/native/support")
target_link_libraries(
    chaos_bootstrap
    PUBLIC
        chaos_runtime_core
    PRIVATE
        chaos_support)
