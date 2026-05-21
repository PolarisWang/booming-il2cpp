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
    "${CHAOS_PROJECT_ROOT}/../contracts/native/v0"
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
if(USE_ASAN)
    add_compile_options(/fsanitize=address)
    add_link_options(/fsanitize=address /FORCE:MULTIPLE)
    add_compile_definitions(_DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)
endif()

# Convenience: create gtest executable with chaos library support
function(add_chaos_test name)
    cmake_parse_arguments(ARG "" "" "LIBS;INCLUDES" ${ARGN})
    add_executable(${name} ${ARG_UNPARSED_ARGUMENTS})
    target_link_libraries(${name} PRIVATE gtest_main ${ARG_LIBS})
    target_include_directories(${name} PRIVATE ${CHAOS_INCLUDE_DIRS} ${ARG_INCLUDES})
    if(USE_ASAN)
        target_link_options(${name} PRIVATE /FORCE:MULTIPLE)
    endif()
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# Subdirectories
add_subdirectory(support)
add_subdirectory(abi)
add_subdirectory(runtime-core)
add_subdirectory(hot-update)
