get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

add_library(chaos_codegen STATIC
    code_buffer.cpp
    code_generator.cpp
    codegen_helpers.cpp
    deopt_runtime.cpp
    t4_seh_handler.cpp
    )

target_compile_features(chaos_codegen PUBLIC cxx_std_17)
target_compile_options(chaos_codegen PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

# Propagate to consumers: they get this define so they can skip stubs for
# symbols provided by t4_seh_handler.cpp.
target_compile_definitions(chaos_codegen INTERFACE CHAOS_IL2CPP_HAS_T4_SEH_HANDLER=1)

target_include_directories(
    chaos_codegen
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/runtime-core"
        "${CMAKE_CURRENT_LIST_DIR}"
    PRIVATE
        "${REPO_ROOT}/src/native"
        "${REPO_ROOT}/src/native/interpreter"
        "${REPO_ROOT}/third_party/fmt/include")

target_link_libraries(
    chaos_codegen
    PUBLIC
        chaos_runtime_core
        chaos_interpreter)
