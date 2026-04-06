get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_support STATIC
    support.cpp
    support.h)

target_compile_features(chaos_support PUBLIC cxx_std_17)
target_compile_definitions(chaos_support PUBLIC CHAOS_RUNTIME_ABI_STATIC)
target_include_directories(
    chaos_support
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${CMAKE_CURRENT_LIST_DIR}"
    PRIVATE
        "${REPO_ROOT}/src/native/runtime-core")
target_link_libraries(
    chaos_support
    PUBLIC
        chaos_runtime_core)
