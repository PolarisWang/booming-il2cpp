# -*- coding: utf-8 -*-
"""Post-patch for runtime-entry.cpp: fix C2713 SEH conflict + JitVehHandler crash loop.

Applied by build.py after TPG generates runtime-entry.cpp.
"""

import re


def patch_runtime_entry(content: str) -> tuple[str, bool]:
    """Apply patches to runtime-entry.cpp content. Returns (patched_content, was_modified)."""
    patched = False

    # Fix 1: Remove try/catch from CHAOS_FACT_CHECK macro (causes C2713)
    old_macro = (
        '#define CHAOS_FACT_CHECK()   do { \\\n'
        '    try { \\\n'
        '        if (Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete() != 0) { \\\n'
        '            std::fprintf(stderr, "[ASSERT] s_exitCode was non-zero after fact loop\\n"); \\\n'
        '        } \\\n'
        '    } catch (...) { \\\n'
        '        /* JIT mode: managed AssertionException may propagate; swallow gracefully */ \\\n'
        '        std::fprintf(stderr, "[ASSERT] JIT assertion check threw\\n"); \\\n'
        '    } \\\n'
        '} while(0)'
    )
    new_macro = (
        '#define CHAOS_FACT_CHECK()   do { \\\n'
        '    if (Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete() != 0) { \\\n'
        '        std::fprintf(stderr, "[ASSERT] s_exitCode was non-zero after fact loop\\n"); \\\n'
        '    } \\\n'
        '} while(0)'
    )
    if old_macro in content:
        content = content.replace(old_macro, new_macro, 1)
        patched = True
        print("  [build] Patched CHAOS_FACT_CHECK: removed try/catch")

    # Fix 2: Disable JitVehHandler (crash loop in benchmark mode)
    old_veh = '    AddVectoredExceptionHandler(1, JitVehHandler);'
    new_veh = '    // AddVectoredExceptionHandler(1, JitVehHandler);  // disabled - __except handles JIT crashes'
    if old_veh in content:
        content = content.replace(old_veh, new_veh, 1)
        patched = True
        print("  [build] Disabled JitVehHandler: __except handles JIT crashes")

    return content, patched
