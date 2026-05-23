cmake_minimum_required(VERSION 3.20)
project(chaos_il2cpp_tools LANGUAGES CXX)

# ── MSVC debug info: Embedded (/Z7) avoids shared .pdb contention ─────
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "Embedded")
endif()

# ── Paths ─────────────────────────────────────────────────────────────
set(CHAOS_NATIVE_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(CHAOS_THIRD_PARTY_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../third_party")
set(CHAOS_BUILD_TREE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../build/native" CACHE PATH
    "Path to the pre-configured native build directory (Debug)")

# Library search paths (must be before add_subdirectory)
link_directories(
    "${CHAOS_BUILD_TREE_DIR}/src/native/common/Debug"
    "${CHAOS_BUILD_TREE_DIR}/src/native/runtime-core/Debug"
    "${CHAOS_BUILD_TREE_DIR}/src/native/interpreter/Debug"
    "${CHAOS_BUILD_TREE_DIR}/src/native/bootstrap/Debug"
    "${CHAOS_BUILD_TREE_DIR}/src/native/support/Debug"
    "${CHAOS_BUILD_TREE_DIR}/src/native/jit/Debug"
    "${CHAOS_BUILD_TREE_DIR}/src/native/hot-update/Debug"
    "${CHAOS_BUILD_TREE_DIR}/fmt_build/Debug"
)

add_subdirectory(ir_precompile)

# Attach include directories to the target after it is created
target_include_directories(ir_precompile
    PRIVATE
        "${CHAOS_NATIVE_ROOT}/common"
        "${CHAOS_NATIVE_ROOT}/runtime-core"
        "${CHAOS_NATIVE_ROOT}/interpreter"
        "${CHAOS_NATIVE_ROOT}/interpreter/generated"
        "${CHAOS_NATIVE_ROOT}/bootstrap"
        "${CHAOS_NATIVE_ROOT}/support"
        "${CHAOS_NATIVE_ROOT}/jit"
        "${CHAOS_NATIVE_ROOT}/hot-update"
        "${CMAKE_CURRENT_LIST_DIR}/ir_precompile"
        "${CMAKE_CURRENT_LIST_DIR}/../../../contracts/native/v0"
        "${CHAOS_THIRD_PARTY_ROOT}/fmt/include"
        "${CHAOS_THIRD_PARTY_ROOT}/unordered_dense/include"
)
