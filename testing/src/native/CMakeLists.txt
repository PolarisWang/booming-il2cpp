cmake_minimum_required(VERSION 3.20)
project(chaos_il2cpp_testing CXX)
set(CMAKE_CXX_STANDARD 20)
enable_testing()

# Options
option(BUILD_TESTS "Build native tests" ON)
option(USE_ASAN "Enable AddressSanitizer" OFF)

# Google Test via FetchContent
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
)
if(USE_ASAN)
    set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
else()
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

# Project root for referencing source headers
get_filename_component(CHAOS_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

# Chaos prebuilt native libraries
set(CHAOS_NATIVE_DIR "${CHAOS_PROJECT_ROOT}/../build/native/src/native")

# Chaos include directories (mirrors project source layout)
set(CHAOS_INCLUDE_DIRS
    "${CHAOS_PROJECT_ROOT}/../src/native"
    "${CHAOS_PROJECT_ROOT}/../src/native/common"
    "${CHAOS_PROJECT_ROOT}/../src/native/interpreter"
    "${CHAOS_PROJECT_ROOT}/../src/native/interpreter/generated"
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core"
    "${CHAOS_PROJECT_ROOT}/../src/native/bootstrap"
    "${CHAOS_PROJECT_ROOT}/../src/native/hot-update"
    "${CHAOS_PROJECT_ROOT}/../src/native/support"
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/threading"
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/tiering"
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc"
    "${CHAOS_PROJECT_ROOT}/../src/native/codegen"
    "${CHAOS_PROJECT_ROOT}/../src/native/engine-bridge"
    "${CHAOS_PROJECT_ROOT}/../contracts/native/v0"
    "${CHAOS_PROJECT_ROOT}/../contracts/engine/v0"
    "${CHAOS_PROJECT_ROOT}/../third_party/unordered_dense/include"
    "${CHAOS_PROJECT_ROOT}/../third_party/fmt/include"
)

# Import chaos prebuilt libraries as static imported targets
macro(add_chaos_imported_lib name subdir)
    if(NOT TARGET ${name})
        add_library(${name} STATIC IMPORTED GLOBAL)
        set_target_properties(${name} PROPERTIES
            IMPORTED_LOCATION_DEBUG          "${CHAOS_NATIVE_DIR}/${subdir}/Debug/${name}.lib"
            IMPORTED_LOCATION_RELEASE        "${CHAOS_NATIVE_DIR}/${subdir}/RelWithDebInfo/${name}.lib"
            IMPORTED_LOCATION_RELWITHDEBINFO "${CHAOS_NATIVE_DIR}/${subdir}/RelWithDebInfo/${name}.lib"
            IMPORTED_LOCATION_MINSIZEREL     "${CHAOS_NATIVE_DIR}/${subdir}/RelWithDebInfo/${name}.lib"
            IMPORTED_LOCATION                "${CHAOS_NATIVE_DIR}/${subdir}/RelWithDebInfo/${name}.lib"
        )
    endif()
endmacro()

add_chaos_imported_lib(chaos_interpreter    interpreter)
add_chaos_imported_lib(chaos_codegen        codegen)
add_chaos_imported_lib(chaos_runtime_core   runtime-core)
add_chaos_imported_lib(chaos_bootstrap      bootstrap)
add_chaos_imported_lib(chaos_common         common)
add_chaos_imported_lib(chaos_support        support)
add_chaos_imported_lib(chaos_hot_update     hot-update)
add_chaos_imported_lib(chaos_engine_bridge  engine-bridge)

# chaos_fmt is built as a standalone project outside the src/native tree
if(NOT TARGET chaos_fmt)
    add_library(chaos_fmt STATIC IMPORTED GLOBAL)
    set(CHAOS_FMT_DIR "${CHAOS_PROJECT_ROOT}/../build/native/fmt_build")
    set_target_properties(chaos_fmt PROPERTIES
        IMPORTED_LOCATION_DEBUG          "${CHAOS_FMT_DIR}/Debug/chaos_fmt.lib"
        IMPORTED_LOCATION_RELEASE        "${CHAOS_FMT_DIR}/RelWithDebInfo/chaos_fmt.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO "${CHAOS_FMT_DIR}/RelWithDebInfo/chaos_fmt.lib"
        IMPORTED_LOCATION_MINSIZEREL     "${CHAOS_FMT_DIR}/RelWithDebInfo/chaos_fmt.lib"
        IMPORTED_LOCATION                "${CHAOS_FMT_DIR}/RelWithDebInfo/chaos_fmt.lib"
    )
endif()

# Compiler settings
add_compile_options(/utf-8 /GS- /W1)

# Provide stubs for debug CRT symbols referenced by chaos prebuilt libraries
set(CHAOS_CRT_STUBS "${CMAKE_CURRENT_SOURCE_DIR}/crt_stubs.cpp")

if(USE_ASAN)
    add_compile_options(/fsanitize=address)
    add_link_options(/fsanitize=address /FORCE:MULTIPLE)
    add_compile_definitions(_DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)

    # Locate the ASan runtime DLL for post-build copy.
    # MSVC places it alongside the toolchain binaries.
    find_file(CHAOS_ASAN_DLL clang_rt.asan_dynamic-x86_64.dll
        PATHS
            "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64"
            "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64"
            "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64"
            "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64"
    )
    if(CHAOS_ASAN_DLL)
        message(STATUS "ASan DLL found: ${CHAOS_ASAN_DLL}")
        # Also copy suppressions file to build root for ASAN_OPTIONS
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/../../asan-suppressions.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/asan-suppressions.txt"
            COPYONLY)
    else()
        message(WARNING "ASan DLL not found — tests may fail with 0xc0000135 at runtime")
    endif()
endif()

# Include sanitizer config modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../config/sanitizers")
include(asan)
include(rtc)

# Convenience: create gtest executable with chaos library support
function(add_chaos_test name)
    cmake_parse_arguments(ARG "" "SANITIZE" "LIBS;INCLUDES;LABELS" ${ARGN})
    add_executable(${name} ${ARG_UNPARSED_ARGUMENTS})
    target_link_libraries(${name} PRIVATE gtest_main ${ARG_LIBS})
    target_include_directories(${name} PRIVATE ${CHAOS_INCLUDE_DIRS} ${ARG_INCLUDES})
    target_link_options(${name} PRIVATE /FORCE:MULTIPLE)
    add_test(NAME ${name} COMMAND ${name})
    if(ARG_LABELS)
        set_tests_properties(${name} PROPERTIES LABELS "${ARG_LABELS}")
    endif()
    # Apply per-target sanitizer config
    if(ARG_SANITIZE STREQUAL "asan")
        chaos_enable_asan(${name})
    elseif(ARG_SANITIZE STREQUAL "rtc")
        chaos_enable_rtc(${name})
    endif()
    # Global USE_ASAN still applies (legacy path)
    if(USE_ASAN AND CHAOS_ASAN_DLL)
        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CHAOS_ASAN_DLL}" "$<TARGET_FILE_DIR:${name}>/"
            COMMENT "Copy ASan DLL to $<TARGET_FILE_DIR:${name}>"
        )
    endif()
    if(USE_ASAN)
        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_BINARY_DIR}/asan-suppressions.txt"
                "$<TARGET_FILE_DIR:${name}>/"
            COMMENT "Copy asan-suppressions.txt to $<TARGET_FILE_DIR:${name}>"
        )
    endif()
endfunction()

# Subdirectories
add_subdirectory(support)
add_subdirectory(abi)
add_subdirectory(common)
add_subdirectory(bootstrap)
add_subdirectory(codegen)
add_subdirectory(engine-bridge)
add_subdirectory(hot-update)
add_subdirectory(runtime-core)
add_subdirectory(fuzz)
