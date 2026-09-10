# Aggregate CMakeLists.txt for System.ComponentModel.TypeConverter
# Generated manually for testing — TPG will produce this in production.

cmake_minimum_required(VERSION 3.20)
project(System.ComponentModel.TypeConverter CXX)
set(CMAKE_CXX_STANDARD 20)

# ── Compiler settings ──
if(MSVC)
  add_compile_options(/utf-8 /GS- /FS /bigobj /wd4716 /MP)
  set(CHAOS_EHA_FLAG "/EHa")
  set(CHAOS_FORCE_MULTIPLE /FORCE:MULTIPLE)
else()
  add_compile_options(-Wall -Wextra -Wno-unused-parameter -fpermissive)
  set(CHAOS_EHA_FLAG -fexceptions)
  set(CHAOS_FORCE_MULTIPLE "-Wl,--allow-multiple-definition")
endif()

set(CHAOS_IL2CPP_CONFIG_TIER "check")
add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=0)
add_compile_definitions(CHAOS_IL2CPP_CONFIG_CHECK)
add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)
add_compile_definitions(CHAOS_IL2CPP_RUNTIME_CONFIG_FILE="runtime-config/test-only.h")
add_compile_definitions(CHAOS_IL2CPP_HAS_TIER_MANAGER=1)
add_compile_definitions(CHAOS_IL2CPP_VERIFICATION_ENABLED=1)

# ── Shared paths ──
set(CHAOS_FOUNDATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CHAOS_SDK_DIR "${CHAOS_FOUNDATION_DIR}/chunks/component-model/native/codegen" CACHE INTERNAL "")
set(CHAOS_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." CACHE INTERNAL "")
set(CHAOS_CODEGEN_DIR "${CHAOS_FOUNDATION_DIR}/chunks/component-model/native/codegen" CACHE INTERNAL "")

find_package(chaos REQUIRED PATHS "${CHAOS_SDK_DIR}")

# ── Shared runtime stubs ──
set(CHAOS_COMMON_STUBS
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/core/gc_alloc_stubs.cpp"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc/gc_api.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/array_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/math_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/simd_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/char_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/vector_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/async_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/misc_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/interop_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/exception_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/object_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/string_stubs.cpp"
    "${CHAOS_SDK_DIR}/runtime_stubs/entry_stubs.cpp"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs/crypto_stubs.cpp"
)

add_library(chaos_common_stubs STATIC ${CHAOS_COMMON_STUBS})
target_include_directories(chaos_common_stubs PUBLIC
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs"
    "${CHAOS_PROJECT_ROOT}/src/native/common"
    "${CHAOS_PROJECT_ROOT}/src/native"
    "${CHAOS_PROJECT_ROOT}/src/native/pal"
    "${CHAOS_SDK_DIR}/include"
)
target_compile_options(chaos_common_stubs PRIVATE ${CHAOS_EHA_FLAG})
target_link_libraries(chaos_common_stubs PUBLIC chaos::runtime)

# ── Chunks ──
add_subdirectory(chunks/component-model/native chunks-component-model)
add_subdirectory(chunks/global-ns/native chunks-global-ns)
add_subdirectory(chunks/system-componentmodel-design/native chunks-system-componentmodel-design)
