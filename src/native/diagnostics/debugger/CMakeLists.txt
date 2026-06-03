# CMakeLists.txt — IL-level Debugger (chaos_debugger)
#
# Builds a static library providing DAP-based IL-level debugging support
# for the interpreter subsystem.  Uses stdio transport for DAP protocol.

cmake_minimum_required(VERSION 3.20)
project(chaos_debugger LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)

# ── Static library ───────────────────────────────────────────────────────
add_library(chaos_debugger STATIC
    dbg_config.h
    dbg_protocol.h
    dbg_transport.h
    dbg_transport.cpp
    dbg_breakpoint.h
    dbg_breakpoint.cpp
    dbg_stepping.h
    dbg_stepping.cpp
    dbg_stack.h
    dbg_stack.cpp
    dbg_variables.h
    dbg_variables.cpp
    dbg_runtime.h
    dbg_runtime.cpp
    dbg_server.h
    dbg_server.cpp
    dbg_instance.h
    dbg_instance.cpp
)

target_include_directories(chaos_debugger PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.."          # debugger/... from diagnostics root
    "${REPO_ROOT}/src/native/common"          # chaos/native_types.h
    "${REPO_ROOT}/src/native/runtime-core"    # module_registry.h, reflection_query_model.h
)

# Transitive includes for runtime-core headers (contract ABI headers).
target_include_directories(chaos_debugger PRIVATE
    "${REPO_ROOT}/contracts/native/v0"        # runtime_abi.h, codegen_bridge.h
    "${REPO_ROOT}/src/native/pal"             # chaos/pal/pal_io.h
    "${REPO_ROOT}/third_party/unordered_dense/include"  # ankerl/unordered_dense.h
    "${REPO_ROOT}/third_party/fmt/include"              # fmt/format.h
)

target_compile_options(chaos_debugger PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHa- /GS- /W3 /D_SILENCE_ALL_CXX17_C99_MATH_MACROS>)

target_compile_features(chaos_debugger PUBLIC cxx_std_17)
