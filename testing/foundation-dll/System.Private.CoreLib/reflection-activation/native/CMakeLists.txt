cmake_minimum_required(VERSION 3.20)
project(chaos_entry CXX)
set(CMAKE_CXX_STANDARD 20)

# Compiler settings — /EHa needed for catch(...) to intercept C++ exceptions
# thrown by generated code (throw chaos_managed_exception from unresolved calls).
add_compile_options(/utf-8 /GS-)
add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)
add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)

# Find chaos SDK — provides chaos::runtime (prebuilt libs + flags) and
# chaos::codegen (precompiled generated code) via find_package(chaos).
# The SDK root is the codegen output directory from --sdk-out.
set(CHAOS_SDK_DIR "D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/reflection-activation/codegen")
find_package(chaos REQUIRED PATHS "${CHAOS_SDK_DIR}")

# Paths
set(CHAOS_PROJECT_ROOT "D:/agent/booming-il2cpp")
set(CHAOS_CODEGEN_DIR "D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/reflection-activation/codegen")

# Source files — codegen outputs to codegen/<AssemblyName>/generated/
file(GLOB CHAOS_CODEGEN_CPP "${CHAOS_CODEGEN_DIR}/*Subjects/generated/native-aot.generated.cpp")
# Also search native/ (pipeline syncs codegen output there after convert-to-cpp)
file(GLOB CHAOS_CODEGEN_NATIVE_CPP "${CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/native-aot.generated.cpp")
# Use codegen path if available, fall back to native path (avoids compiling both)
if(CHAOS_CODEGEN_CPP)
  set(CHAOS_AOT_GENERATED_CPP ${CHAOS_CODEGEN_CPP})
else()
  set(CHAOS_AOT_GENERATED_CPP ${CHAOS_CODEGEN_NATIVE_CPP})
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
# SDK include/ (via chaos::runtime) provides runtime headers for generated code.
# Native stubs additionally need repo tree paths for internal headers
# (abi_manifest, codegen_bridge, fast_frame_pool, string_table, reflection_api)
# and third-party dependencies (fmt, unordered_dense).
set(CHAOS_ENTRY_INCLUDES
    "${CHAOS_PROJECT_ROOT}/src/native/common"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs"
    "${CHAOS_PROJECT_ROOT}/src/native/bootstrap"
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter"
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter/generated"
    "${CHAOS_PROJECT_ROOT}/src/native/codegen"
    "${CHAOS_PROJECT_ROOT}/src/native/support"
    "${CHAOS_PROJECT_ROOT}/src/native/hot-update"
    "${CHAOS_PROJECT_ROOT}/src/native"
    "${CHAOS_PROJECT_ROOT}/third_party/fmt/include"
    "${CHAOS_PROJECT_ROOT}/third_party/unordered_dense/include"
    "${CHAOS_PROJECT_ROOT}/contracts/native/v0"
    "${CHAOS_CODEGEN_DIR}/NumericAggregationSubjects/generated"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(entry ${CHAOS_ENTRY_SOURCES})
target_include_directories(entry PRIVATE ${CHAOS_ENTRY_INCLUDES})
target_compile_options(entry PRIVATE /EHa)
target_link_libraries(entry PRIVATE

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
    chaos::runtime
)
