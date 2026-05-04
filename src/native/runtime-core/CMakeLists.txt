get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_runtime_core STATIC
    runtime_core.cpp
    runtime_core.h
    memory_domain.cpp
    memory_domain.h
    vtable_registry.cpp
    vtable_registry.h
    generic_context.cpp
    generic_context.h
    string_table.h
    string_table.cpp
    type_registry.cpp
    runtime_stubs.cpp
    module_registry.cpp
    module_registry.h
    layout_engine.cpp
    layout_engine.h
    generic_method_instantiation.cpp
    generic_method_instantiation.h
    method_table.cpp
    method_table.h
    reflection_api.cpp
    reflection_api.h
    il_to_ir_lowerer.cpp
    il_to_ir_lowerer.h
    token_resolver.cpp
    token_resolver.h
    abi_manifest.cpp)

target_compile_features(chaos_runtime_core PUBLIC cxx_std_17)
target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_RUNTIME_ABI_EXPORTS)
target_compile_definitions(chaos_runtime_core PUBLIC CHAOS_RUNTIME_ABI_STATIC)
target_include_directories(
    chaos_runtime_core
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/common"
        "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(chaos_runtime_core PUBLIC chaos_bdwgc)
target_include_directories(chaos_runtime_core PRIVATE
    "${REPO_ROOT}/third_party/bdwgc/include"
    "${REPO_ROOT}/src/native/interpreter")

