cmake_minimum_required(VERSION 3.20)
project(chaos_entry CXX)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(/utf-8 /GS-)
add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)
add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)
set(CHAOS_PROJECT_ROOT "D:/agent/booming-il2cpp")
set(CHAOS_NATIVE_DIR "D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/native")
set(CHAOS_CODEGEN_DIR "D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/codegen")
set(CHAOS_NATIVE_BUILD "D:/agent/booming-il2cpp/artifacts/presets/windows-x64-reference")
file(GLOB CHAOS_CODEGEN_CPP "${CHAOS_CODEGEN_DIR}/*Subjects/generated/native-aot.generated.cpp")
file(GLOB CHAOS_CODEGEN_NATIVE_CPP "${CHAOS_NATIVE_DIR}/*Subjects/generated/native-aot.generated.cpp")
if(CHAOS_CODEGEN_CPP)
  set(CHAOS_AOT_GENERATED_CPP ${CHAOS_CODEGEN_CPP})
else()
  set(CHAOS_AOT_GENERATED_CPP ${CHAOS_CODEGEN_NATIVE_CPP})
endif()
file(GLOB CHAOS_NATIVE_STUBS "${CHAOS_NATIVE_DIR}/*.cpp")
list(REMOVE_ITEM CHAOS_NATIVE_STUBS
    "${CHAOS_NATIVE_DIR}/runtime-entry.cpp"
    "${CHAOS_NATIVE_DIR}/runtime-patchdata.cpp"
    "${CHAOS_NATIVE_DIR}/verification_dispatch.generated.cpp"
    "${CHAOS_NATIVE_DIR}/jit_stubs.cpp"
)
set(CHAOS_ENTRY_SOURCES
    "${CHAOS_NATIVE_DIR}/runtime-entry.cpp"
    "${CHAOS_NATIVE_DIR}/runtime-patchdata.cpp"
    "${CHAOS_NATIVE_DIR}/verification_dispatch.generated.cpp"
    ${CHAOS_NATIVE_STUBS}
    ${CHAOS_AOT_GENERATED_CPP}
)
set(CHAOS_ENTRY_INCLUDES
    "${CHAOS_PROJECT_ROOT}/src/native/common"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs"
    "${CHAOS_PROJECT_ROOT}/src/native/bootstrap"
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter"
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter/generated"
    "${CHAOS_PROJECT_ROOT}/src/native/codegen"
    "${CHAOS_PROJECT_ROOT}/src/native/jit"
    "${CHAOS_PROJECT_ROOT}/src/native/support"
    "${CHAOS_PROJECT_ROOT}/src/native/hot-update"
    "${CHAOS_PROJECT_ROOT}/src/native"
    "${CHAOS_PROJECT_ROOT}/contracts/native/v0"
    "${CHAOS_PROJECT_ROOT}/third_party/fmt/include"
    "${CHAOS_PROJECT_ROOT}/third_party/fmt/include"
    "${CHAOS_PROJECT_ROOT}/third_party/unordered_dense/include"
    "${CHAOS_CODEGEN_DIR}/NumericAggregationSubjects/generated"
    "${CHAOS_NATIVE_DIR}"
)
set(CHAOS_LIB_DIRS
    "${CHAOS_NATIVE_BUILD}/src/native/runtime-core"
    "${CHAOS_NATIVE_BUILD}/src/native/bootstrap"
    "${CHAOS_NATIVE_BUILD}/src/native/common"
    "${CHAOS_NATIVE_BUILD}/src/native/interpreter"
    "${CHAOS_NATIVE_BUILD}/src/native/jit"
    "${CHAOS_NATIVE_BUILD}/src/native/support"
    "${CHAOS_NATIVE_BUILD}/src/native/hot-update"
    "${CHAOS_NATIVE_BUILD}/src/native/diagnostics/eventpipe"
    "${CHAOS_NATIVE_BUILD}/src/native/diagnostics/debugger"
    "${CHAOS_NATIVE_BUILD}/fmt_build"
)
set(CHAOS_RUNTIME_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_common
    chaos_interpreter
    chaos_jit
    chaos_support
    chaos_hot_update
    chaos_eventpipe
    chaos_fmt
    chaos_debugger
)
add_executable(entry ${CHAOS_ENTRY_SOURCES})
target_include_directories(entry PRIVATE ${CHAOS_ENTRY_INCLUDES})
target_link_directories(entry PRIVATE ${CHAOS_LIB_DIRS})
target_compile_options(entry PRIVATE /EHa)
target_link_libraries(entry PRIVATE ${CHAOS_RUNTIME_LIBS})
target_link_options(entry PRIVATE /FORCE:MULTIPLE)
