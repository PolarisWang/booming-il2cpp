cmake_minimum_required(VERSION 3.20)
project(chaos_entry CXX)
set(CMAKE_CXX_STANDARD 20)

# Compiler settings — /EHa needed for catch(...) to intercept C++ exceptions
# thrown by generated code (throw chaos_managed_exception from unresolved calls).
add_compile_options(/utf-8 /GS-)
add_compile_definitions(CHAOS_IL2CPP_EH_WIN32_SEH)  # chaos-il2cpp SEH (Windows)
add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)
add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)

# Paths
set(CHAOS_PROJECT_ROOT "D:/agent/booming-il2cpp")
set(CHAOS_CODEGEN_DIR "D:/agent/booming-il2cpp/testing/foundation-dll/System.IO.Pipelines/reader-writer/codegen")
set(CHAOS_NATIVE_BUILD "D:/agent/booming-il2cpp/artifacts/presets/windows-x64-reference")

# Source files — codegen outputs to codegen/<AssemblyName>/generated/
file(GLOB CHAOS_CODEGEN_CPP "${CHAOS_CODEGEN_DIR}/*Subjects/generated/native-aot.generated.cpp")
file(GLOB CHAOS_NATIVE_STUBS "*.cpp")
list(REMOVE_ITEM CHAOS_NATIVE_STUBS
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime-entry.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime-patchdata.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/verification_dispatch.generated.cpp"
)
set(CHAOS_ENTRY_SOURCES
    "runtime-entry.cpp"
    "runtime-patchdata.cpp"
    "verification_dispatch.generated.cpp"
    ${CHAOS_NATIVE_STUBS}
    ${CHAOS_CODEGEN_CPP}
)

# Include directories
set(CHAOS_ENTRY_INCLUDES
    "${CHAOS_PROJECT_ROOT}/src/native/common"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc"
    "${CHAOS_PROJECT_ROOT}/src/native/bootstrap"
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter"
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter/generated"
    "${CHAOS_PROJECT_ROOT}/src/native/codegen"
    "${CHAOS_PROJECT_ROOT}/src/native/support"
    "${CHAOS_PROJECT_ROOT}/src/native/hot-update"
    "${CHAOS_PROJECT_ROOT}/src/native"
    "${CHAOS_PROJECT_ROOT}/contracts/native/v0"
    "${CHAOS_PROJECT_ROOT}/third_party/fmt/include"
    "${CHAOS_PROJECT_ROOT}/third_party/fmt/include"
    "${CHAOS_PROJECT_ROOT}/third_party/unordered_dense/include"
    "${CHAOS_CODEGEN_DIR}/NumericAggregationSubjects/generated"
)

# Library link directories
set(CHAOS_LIB_DIRS
    "${CHAOS_NATIVE_BUILD}/src/native/runtime-core"
    "${CHAOS_NATIVE_BUILD}/src/native/bootstrap"
    "${CHAOS_NATIVE_BUILD}/src/native/common"
    "${CHAOS_NATIVE_BUILD}/src/native/interpreter"
    "${CHAOS_NATIVE_BUILD}/src/native/codegen"
    "${CHAOS_NATIVE_BUILD}/src/native/support"
    "${CHAOS_NATIVE_BUILD}/src/native/hot-update"
    "${CHAOS_NATIVE_BUILD}/fmt_build"
)

# Runtime libs to link
set(CHAOS_RUNTIME_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_common
    chaos_interpreter
    chaos_codegen
    chaos_support
    chaos_hot_update
    chaos_fmt
)

add_executable(entry ${CHAOS_ENTRY_SOURCES})
target_include_directories(entry PRIVATE ${CHAOS_ENTRY_INCLUDES})
target_link_directories(entry PRIVATE ${CHAOS_LIB_DIRS})
target_compile_options(entry PRIVATE /EHa)
target_compile_definitions(entry PRIVATE CHAOS_IL2CPP_CONFIG_CHECK)  # chaos-il2cpp config tier
target_link_libraries(entry PRIVATE ${CHAOS_RUNTIME_LIBS})

# Enable link-time optimization (LTO/LTCG) for release/profile builds
if(MSVC)
  set_target_properties(entry PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE
  )
else()
  set_target_properties(entry PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()
