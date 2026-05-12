get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_hot_update STATIC
    hot_update.cpp
    hot_update.h
    method_replacement.cpp
    method_replacement.h)

target_compile_features(chaos_hot_update PUBLIC cxx_std_17)
target_compile_definitions(chaos_hot_update PUBLIC CHAOS_RUNTIME_ABI_STATIC)
target_compile_options(chaos_hot_update PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(
    chaos_hot_update
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/runtime-core"
        "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(
    chaos_hot_update
    PRIVATE
        "${REPO_ROOT}/third_party/fmt/include")
target_link_libraries(
    chaos_hot_update
    PUBLIC
        chaos_runtime_core)
