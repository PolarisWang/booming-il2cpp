get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_interpreter STATIC
    interpreter_vm.cpp
    interpreter_vm.h
    interpreter_entry.cpp
    interpreter_entry.h
    fast_dispatch.cpp
    fast_dispatch.h
    fast_frame_pool.cpp
    fast_frame_pool.h
    token_resolver.cpp
    token_resolver.h
    ir_reg_alloc.cpp
    ir_reg_alloc.h)

target_compile_features(chaos_interpreter PUBLIC cxx_std_17)
target_compile_options(chaos_interpreter PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(
    chaos_interpreter
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/runtime-core"
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/generated")
target_include_directories(
    chaos_interpreter
    PRIVATE
        "${REPO_ROOT}/src/native"
        "${REPO_ROOT}/third_party/fmt/include")
target_link_libraries(
    chaos_interpreter
    PUBLIC
        chaos_runtime_core)
target_link_libraries(
    chaos_interpreter
    PRIVATE
        chaos_bootstrap)
