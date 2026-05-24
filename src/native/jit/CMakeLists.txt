if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Standalone build: jit/CMakeLists.txt is the root.
    get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
else()
    # Subdirectory build: CMAKE_SOURCE_DIR is the project root.
    set(REPO_ROOT "${CMAKE_SOURCE_DIR}")
endif()

add_library(chaos_jit STATIC
    code_buffer.cpp
    jit_engine.cpp
    jit_helpers.cpp
    jit_inline.cpp
    jit_deopt.cpp
    jit_seh.cpp
    jit_unwind.cpp
    jit_binary_reader.cpp
    jit_precode.cpp
    WinSehHandler.cpp
    $<$<PLATFORM_ID:Linux>:LinuxSehHandler.cpp>
    tree/jit_intrinsics.cpp
    tree/jit_tree_builder.cpp
    tree/jit_tree_mutator.cpp
    tree/jit_linearizer.cpp
    tree/jit_optimizer.cpp
    )

target_compile_features(chaos_jit PUBLIC cxx_std_17)
target_compile_options(chaos_jit PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

# Propagate to consumers: they get this define so they can skip stubs for
# symbols provided by jit_seh.cpp.
target_compile_definitions(chaos_jit INTERFACE CHAOS_IL2CPP_HAS_T4_SEH_HANDLER=1)

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
