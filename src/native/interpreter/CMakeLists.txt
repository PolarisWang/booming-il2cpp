get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_interpreter STATIC
    interpreter_vm.cpp
    interpreter_vm.h)

target_compile_features(chaos_interpreter PUBLIC cxx_std_17)
target_include_directories(
    chaos_interpreter
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/runtime-core"
        "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(
    chaos_interpreter
    PUBLIC
        chaos_runtime_core)
