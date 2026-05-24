# CMakeLists.txt — CRAG GC SOS Debugger Extension (chaos_gc_sos.dll)
#
# Builds a WinDbg extension DLL linking against DbgEng (Debugging Tools for
# Windows).  Requires the Windows SDK (10.0.22621.0 or later) with the
# Debugging Tools for Windows component installed.
#
# Usage: !gc.<command> in WinDbg after .load chaos_gc_sos

cmake_minimum_required(VERSION 3.20)
project(chaos_gc_sos LANGUAGES CXX)

# ── DbgEng SDK paths ──────────────────────────────────────────────
# Debugging Tools for Windows ships a standalone inc/ and lib/ dir
# under the Windows Kits root (NOT the standard SDK include/Lib layout).
set(DBGENG_TOOLS_ROOT "C:/Program Files (x86)/Windows Kits/10/Debuggers")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DBGENG_LIB_PATH "${DBGENG_TOOLS_ROOT}/lib/x64/dbgeng.lib")
else()
    set(DBGENG_LIB_PATH "${DBGENG_TOOLS_ROOT}/lib/x86/dbgeng.lib")
endif()

# ── Include paths ─────────────────────────────────────────────────
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

# ── Shared library ────────────────────────────────────────────────
add_library(chaos_gc_sos SHARED
    gc_sos.cpp
    jit_sos.cpp
)

target_include_directories(chaos_gc_sos PRIVATE
    "${DBGENG_TOOLS_ROOT}/inc"                   # dbgeng.h
    "${CMAKE_CURRENT_SOURCE_DIR}"                 # sos_common.h
    "${REPO_ROOT}/src/native/runtime-core"       # gc/gc_stats.h, gc/gc_debug_contract.h
    "${REPO_ROOT}/src/native/common"             # chaos/native_types.h
    "${REPO_ROOT}/src/native/jit"                # jit_method.h (JitDebugInfoPod)
    "${REPO_ROOT}/contracts/native/v0"            # contracts headers
)

target_compile_options(chaos_gc_sos PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHa /GS->)

target_link_libraries(chaos_gc_sos PRIVATE
    "${DBGENG_LIB_PATH}")

# Mark extension commands as DLL exports.
set_target_properties(chaos_gc_sos PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
    LINKER_LANGUAGE CXX)
