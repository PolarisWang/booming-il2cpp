get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_jit STATIC
    code_buffer.cpp
    jit_engine.cpp
    jit_codegen_arith.cpp
    jit_codegen_branch.cpp
    jit_codegen_deopt.cpp
    jit_codegen_gc.cpp
    jit_helpers.cpp
    jit_inline.cpp
    jit_deopt.cpp
    jit_seh.cpp
    jit_unwind.cpp
    jit_binary_reader.cpp
    jit_precode.cpp
    $<$<PLATFORM_ID:Windows>:WinSehHandler.cpp>
    $<$<PLATFORM_ID:Linux>:LinuxSehHandler.cpp>
    tree/jit_intrinsics.cpp
    tree/jit_bounds_check.cpp
    tree/jit_cfg.cpp
    tree/jit_tree_builder.cpp
    tree/jit_tree_mutator.cpp
    tree/jit_linearizer.cpp
    tree/jit_optimizer.cpp
    )

target_compile_features(chaos_jit PUBLIC cxx_std_17)
target_compile_options(chaos_jit PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
# MSVC 14.44 STL bug: _HAS_CXX20 leaks into C++17 mode, triggering
# std::construct_at in <xmemory> which fails for some allocator types.
# Force C++17 code path to use placement new instead.
target_compile_definitions(chaos_jit PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_HAS_CXX20=0>)

# Propagate to consumers: they get this define so they can skip stubs for
# symbols provided by jit_seh.cpp.
target_compile_definitions(chaos_jit INTERFACE
    CHAOS_IL2CPP_HAS_T4_SEH_HANDLER=1)

target_include_directories(
    chaos_jit
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/runtime-core"
        "${CMAKE_CURRENT_LIST_DIR}"
    PRIVATE
        "${REPO_ROOT}/src/native"
        "${REPO_ROOT}/src/native/interpreter"
        "${REPO_ROOT}/third_party/fmt/include")

target_link_libraries(
    chaos_jit
    PUBLIC
        chaos_runtime_core
        chaos_interpreter)
