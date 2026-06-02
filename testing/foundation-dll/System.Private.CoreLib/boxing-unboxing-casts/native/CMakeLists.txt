cmake_minimum_required(VERSION 3.20)
project(chaos_entry CXX)
set(CMAKE_CXX_STANDARD 20)

# Compiler settings — platform-conditional
# MSVC: /EHa needed for catch(...) to intercept C++ exceptions from generated code.
# GCC/Clang: -fexceptions -finput-charset=utf-8, suppress known warnings
add_compile_options(
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/GS->"
    "$<$<CXX_COMPILER_ID:MSVC>:/FS>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fexceptions>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=utf-8>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wno-error=invalid-offsetof>"
)
# Config tier: controlled by CHAOS_IL2CPP_CONFIG_TIER cmake variable
# (set via -D or CMakePresets.json). Expected values: check / profile / ship.
if(NOT DEFINED CHAOS_IL2CPP_CONFIG_TIER)
  set(CHAOS_IL2CPP_CONFIG_TIER "check")
endif()
if(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "profile")
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_PROFILE)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_PROFILE)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=2)
elseif(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "ship")
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_SHIP)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_SHIP)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=0)
elseif(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "check")
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_CHECK)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)
else()
  message(WARNING "Unknown CHAOS_IL2CPP_CONFIG_TIER: '${CHAOS_IL2CPP_CONFIG_TIER}'. Expected check / profile / ship. Defaulting to CHECK.")
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_CHECK)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)
endif()
if(CHAOS_IL2CPP_JIT_MODE)
    add_compile_definitions(CHAOS_IL2CPP_JIT_MODE)
endif()

# Verification mode: enables if constexpr assertion bodies in generated code
# Set to 0 for AOT-only builds; the fact stage requires assertions to be DISABLED
# because the AOT entry point does not have managed exception handling installed
# during static initialization.
add_compile_definitions(CHAOS_IL2CPP_VERIFICATION_ENABLED=0)

# Find chaos SDK — provides chaos::runtime (prebuilt libs + flags) and
# chaos::codegen (precompiled generated code) via find_package(chaos).
# The SDK root is the codegen output directory from --sdk-out.
set(CHAOS_SDK_DIR "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/boxing-unboxing-casts/codegen/BoxingUnboxingCastsSubjects")
find_package(chaos REQUIRED PATHS "${CHAOS_SDK_DIR}")

# Paths
set(CHAOS_PROJECT_ROOT "/home/debian/agent/booming-il2cpp")
set(CHAOS_CODEGEN_DIR "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/boxing-unboxing-casts/codegen")

# Source files — codegen outputs to codegen/<AssemblyName>/generated/
# Sources at *Subjects/generated/* (single-level, R1 fix).
file(GLOB CHAOS_CODEGEN_CPP
    "${CHAOS_CODEGEN_DIR}/*Subjects/generated/native-aot.generated.cpp"
    "${CHAOS_CODEGEN_DIR}/*Subjects/generated/native-aot.page-*.cpp"
    "${CHAOS_CODEGEN_DIR}/*Subjects/generated/chaos_generated_module.cpp"
)
file(GLOB CHAOS_CODEGEN_NATIVE_CPP
    "${CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/native-aot.generated.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/native-aot.page-*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/chaos_generated_module.cpp"
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
# SDK runtime_stubs sources (resolved directly from the source tree to
# avoid depending on SdkEmitter to stage them into the SDK output dir).
file(GLOB CHAOS_RUNTIME_STUB_SOURCES
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs/*.cpp"
)
list(REMOVE_ITEM CHAOS_RUNTIME_STUB_SOURCES
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs/guid_stubs.cpp"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs/threading_stubs.cpp"
)
# PROFILE globals (compiled from source to avoid stale SDK lib dependency).
# Only generates symbols when CHAOS_IL2CPP_PROFILE_ENABLED=1 (PROFILE tier).
set(CHAOS_PROFILE_GLOBALS
    "${CHAOS_PROJECT_ROOT}/src/native/common/chaos/profile_globals.cpp"
)
set(CHAOS_ENTRY_SOURCES
    "runtime-entry.cpp"
    "runtime-patchdata.cpp"
    "verification_dispatch.generated.cpp"
    ${CHAOS_NATIVE_STUBS}
    ${CHAOS_RUNTIME_STUB_SOURCES}
    ${CHAOS_PROFILE_GLOBALS}
    ${CHAOS_AOT_GENERATED_CPP}
)

# Include directories
# SDK include/ (via chaos::runtime) provides most headers. runtime-core/ is
# retained because runtime_core.h uses quoted includes for ~20 sibling headers
# (exception_jmp.h, marshal_abi.h, etc.) not distributed in the SDK.
set(CHAOS_ENTRY_INCLUDES
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs"  # runtime_stubs headers
    "${CHAOS_PROJECT_ROOT}/src/native/common"
    "${CHAOS_PROJECT_ROOT}/src/native/bootstrap"
    "${CHAOS_PROJECT_ROOT}/src/native"  # parent for bootstrap/bootstrap.h includes
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter"
    "${CHAOS_PROJECT_ROOT}/src/native/interpreter/generated"
    "${CHAOS_PROJECT_ROOT}/src/native/support"
    "${CHAOS_PROJECT_ROOT}/src/native/hot-update"
    "${CHAOS_PROJECT_ROOT}/third_party/unordered_dense/include"
    "${CHAOS_CODEGEN_DIR}/NumericAggregationSubjects/generated"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CHAOS_PROJECT_ROOT}/src/native/jit"
)

add_executable(entry ${CHAOS_ENTRY_SOURCES})
target_include_directories(entry PRIVATE ${CHAOS_ENTRY_INCLUDES})
target_compile_options(entry PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/EHa>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fexceptions>"
)
target_link_libraries(entry PRIVATE chaos::runtime)
target_link_options(entry PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wl,--allow-multiple-definition>"
)
