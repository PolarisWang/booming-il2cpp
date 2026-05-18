get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

add_library(chaos_codegen STATIC
    code_buffer.cpp
    code_generator.cpp
    deopt_runtime.cpp
    )

target_compile_features(chaos_codegen PUBLIC cxx_std_17)
target_compile_options(chaos_codegen PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

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
