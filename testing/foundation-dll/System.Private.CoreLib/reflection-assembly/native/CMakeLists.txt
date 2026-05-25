cmake_minimum_required(VERSION 3.20)
project(chaos_entry CXX)
set(CMAKE_CXX_STANDARD 20)

# Compiler settings — /EHa needed for catch(...) to intercept C++ exceptions
# thrown by generated code (throw chaos_managed_exception from unresolved calls).
add_compile_options(/utf-8 /GS- /FS)
add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)
add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)

# Find chaos SDK — provides chaos::runtime (prebuilt libs + flags) and
# chaos::codegen (precompiled generated code) via find_package(chaos).
# The SDK root is the codegen output directory from --sdk-out.
set(CHAOS_SDK_DIR "D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/reflection-assembly/codegen")
find_package(chaos REQUIRED PATHS "${CHAOS_SDK_DIR}")

# Paths
set(CHAOS_PROJECT_ROOT "D:/agent/booming-il2cpp")
set(CHAOS_CODEGEN_DIR "D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/reflection-assembly/codegen")

# Source files — codegen outputs to codegen/<AssemblyName>/generated/
# Flat layout (single file): *Subjects/generated/native-aot.generated.cpp
# Paged layout (large families): *Subjects/generated/generated/ (page files + module)
file(GLOB CHAOS_CODEGEN_CPP
    "${CHAOS_CODEGEN_DIR}/*Subjects/generated/native-aot.generated.cpp"
    "${CHAOS_CODEGEN_DIR}/*Subjects/generated/generated/native-aot.generated.cpp"
    "${CHAOS_CODEGEN_DIR}/*Subjects/generated/generated/native-aot.page-*.cpp"
    "${CHAOS_CODEGEN_DIR}/*Subjects/generated/generated/chaos_generated_module.cpp"
)
file(GLOB CHAOS_CODEGEN_NATIVE_CPP
    "${CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/native-aot.generated.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/generated/native-aot.generated.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/generated/native-aot.page-*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/generated/chaos_generated_module.cpp"
)
# Prefer native (pipeline post-processed) copies over codegen originals
if(CHAOS_CODEGEN_NATIVE_CPP)
  set(CHAOS_AOT_GENERATED_CPP ${CHAOS_CODEGEN_NATIVE_CPP})
else()
  set(CHAOS_AOT_GENERATED_CPP ${CHAOS_CODEGEN_CPP})
endif()
file(GLOB CHAOS_NATIVE_STUBS "*.cpp")
list(REMOVE_ITEM CHAOS_NATIVE_STUBS
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime-entry.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime-patchdata.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/verification_dispatch.generated.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jit_stubs.cpp"
)
set(CHAOS_ENTRY_SOURCES
    "runtime-entry.cpp"
    "runtime-patchdata.cpp"
    "verification_dispatch.generated.cpp"
    ${CHAOS_NATIVE_STUBS}
    ${CHAOS_AOT_GENERATED_CPP}
)

# Include directories
# SDK include/ (via chaos::runtime) provides most headers. runtime-core/ is
# retained because runtime_core.h uses quoted includes for ~20 sibling headers
# (exception_jmp.h, marshal_abi.h, etc.) not distributed in the SDK.
set(CHAOS_ENTRY_INCLUDES
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs"  # runtime_stubs for enum_stubs.h
    "${CHAOS_PROJECT_ROOT}/src/native/bootstrap"
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter"
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter/generated"
    "${CHAOS_PROJECT_ROOT}/src/native/support"
    "${CHAOS_PROJECT_ROOT}/src/native/hot-update"
    "${CHAOS_PROJECT_ROOT}/third_party/unordered_dense/include"
    "${CHAOS_CODEGEN_DIR}/NumericAggregationSubjects/generated"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(entry ${CHAOS_ENTRY_SOURCES})
target_include_directories(entry PRIVATE ${CHAOS_ENTRY_INCLUDES})
target_compile_options(entry PRIVATE /EHa)
target_link_libraries(entry PRIVATE
    chaos::runtime
)
target_link_options(entry PRIVATE /FORCE:MULTIPLE)
