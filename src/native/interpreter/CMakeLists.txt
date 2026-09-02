get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

include("${REPO_ROOT}/cmake/config-tier.cmake")

add_library(chaos_interpreter STATIC
    interpreter_vm.cpp
    interpreter_value.cpp
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
    ir_reg_alloc.h
    interp_frame_scanner.cpp
    osr_state.cpp
    osr_state.h
    register_vm_profiler.cpp
    register_vm_profiler.h
    )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_compile_features(chaos_interpreter PUBLIC cxx_std_20)

# Runtime config: same as runtime-core (test-only for dev, desktop for ship).
# chaos_runtime_core sets this via target_compile_definitions(PUBLIC ...)
# but we need it here too since we may be compiled against the prebuilt lib.
if(NOT CHAOS_IL2CPP_EXEC_PROFILE)
  set(CHAOS_IL2CPP_EXEC_PROFILE "test-only")
endif()
target_compile_definitions(chaos_interpreter PUBLIC
    CHAOS_IL2CPP_RUNTIME_CONFIG_FILE="runtime-config/${CHAOS_IL2CPP_EXEC_PROFILE}.h")
target_compile_options(chaos_interpreter PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /D_SILENCE_ALL_CXX17_C99_MATH_MACROS /D_CRT_SECURE_NO_WARNINGS /Zm200>)
target_include_directories(
    chaos_interpreter
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/common"
        "${REPO_ROOT}/src/native/runtime-core"
        "${REPO_ROOT}/src/native/runtime-core/gc"
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/generated")
target_include_directories(
    chaos_interpreter
    PRIVATE
        "${REPO_ROOT}/src/native"
        "${REPO_ROOT}/src/native/jit"
        "${REPO_ROOT}/src/native/hot-update"
        "${REPO_ROOT}/src/native/diagnostics"
        "${REPO_ROOT}/src/native/pal"
        "${REPO_ROOT}/third_party/fmt/include"
        "${REPO_ROOT}/third_party/unordered_dense/include")
target_link_libraries(
    chaos_interpreter
    PUBLIC
        chaos_runtime_core
        chaos_jit)
target_link_libraries(
    chaos_interpreter
    PRIVATE
        chaos_bootstrap
        chaos_hot_update)
