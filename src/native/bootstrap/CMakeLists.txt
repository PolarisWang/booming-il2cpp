get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_bootstrap STATIC
    bootstrap.cpp
    bootstrap_delegates.cpp
    bootstrap_icall.cpp
    bootstrap.h
    bootstrap_internal.h)

target_compile_features(chaos_bootstrap PUBLIC cxx_std_17)
target_compile_definitions(chaos_bootstrap PRIVATE CHAOS_RUNTIME_ABI_EXPORTS _CRT_SECURE_NO_WARNINGS)
target_compile_options(chaos_bootstrap PRIVATE /utf-8)
target_include_directories(
    chaos_bootstrap
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${CMAKE_CURRENT_LIST_DIR}"
    PRIVATE
        "${REPO_ROOT}/src/native/common"
        "${REPO_ROOT}/src/native/hot-update"
        "${REPO_ROOT}/src/native/runtime-core"
        "${REPO_ROOT}/src/native/support"
        "${REPO_ROOT}/src/native/interpreter"
        "${REPO_ROOT}/src/native/interpreter/generated"
        "${REPO_ROOT}/third_party/fmt/include")
target_link_libraries(
    chaos_bootstrap
    PUBLIC
        chaos_runtime_core
    PRIVATE
        chaos_codegen
        chaos_hot_update
        chaos_support)
