# ── LTO (Link-Time Optimization) configuration ────────────────────────────
#
# Include after config-tier.cmake in the top-level CMakeLists.txt.
# Enable via -DCHAOS_IL2CPP_LTO=ON or set in CMakePresets.json.
#
# MSVC: /GL (Whole Program Optimization — compile) + /LTCG (Link-Time Code Gen — link).
# The INTERPROCEDURAL_OPTIMIZATION target property is the portable CMake way,
# but on MSVC we set explicit flags for finer control (e.g., /LTCG:incremental
# during development, /LTCG for production).
#
# Usage:
#   include("${CMAKE_SOURCE_DIR}/cmake/lto.cmake")
#   apply_lto(chaos_runtime_core)

option(CHAOS_IL2CPP_LTO "Enable Link-Time Optimization (LTO/LTCG) for ship builds" OFF)

function(apply_lto TARGET)
    if(NOT CHAOS_IL2CPP_LTO)
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # MSVC: /GL enables whole-program analysis during compilation;
        # /LTCG enables link-time code generation (cross-module inlining, dead code elimination).
        target_compile_options(${TARGET} PRIVATE /GL)
        target_link_options(${TARGET} PRIVATE /LTCG)

        # /LTCG:incremental speeds up iterative linking during development;
        # for production /LTCG (non-incremental) gives slightly better optimization.
        # We default to non-incremental for ship-tier builds.
        set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # Clang/GCC: -flto enables link-time optimization.
        target_compile_options(${TARGET} PRIVATE -flto)
        target_link_options(${TARGET} PRIVATE -flto)
        set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endfunction()
