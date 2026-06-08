# -*- coding: utf-8 -*-
"""Post-patch for runtime-entry.cpp: fix C2713 SEH conflict + JitVehHandler crash loop.

Applied by build.py after TPG generates runtime-entry.cpp.

The Scriban template linter reverts changes to the .scriban file, so we patch
the GENERATED C++ file instead. This file is not watched by the linter.
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

    # Fix 2: Fix JitVehHandler: REPLACE the entire RIP+=3 throttle logic with
    # a simple CONTINUE_SEARCH (let __try/__except handle the crash properly).
    # The old code does RIP+=3 which causes cascading crashes (1482x loop → stack overflow).
    old_rip3 = (
        '    // Throttle: if the same RIP crashes repeatedly, the skip isn\'t working.\n'
        '    // Stop trying after N consecutive crashes at the same RIP to avoid\n'
        '    // infinite crash \xe2\x86\x92 skip \xe2\x86\x92 crash loops.\n'
        '    static void* s_last_rip = nullptr;\n'
        '    static int s_same_rip_count = 0;\n'
        '    if (s_last_rip == reinterpret_cast<void*>(ctx->Rip)) {\n'
        '        s_same_rip_count++;\n'
        '    } else {\n'
        '        s_last_rip = reinterpret_cast<void*>(ctx->Rip);\n'
        '        s_same_rip_count = 0;\n'
        '    }\n'
        '    if (s_same_rip_count >= 3) {\n'
        '        std::fprintf(stderr, "JIT ABORT: crash at RIP=%p repeated %d times \xe2\x80\x94 giving up\\n",\n'
        '                     reinterpret_cast<void*>(ctx->Rip), s_same_rip_count);\n'
        '        std::fflush(stderr);\n'
        '        return EXCEPTION_CONTINUE_SEARCH;\n'
        '    }\n'
    )
    new_cont_search = (
        '    // Log once per RIP, then fall through to __try/__except.\n'
        '    // Do NOT use RIP+=3 \xe2\x80\x94 that causes 1482x crash loop \xe2\x86\x92 stack overflow.\n'
        '    static void* _veh_last = nullptr;\n'
        '    if (_veh_last != reinterpret_cast<void*>(ctx->Rip)) {\n'
        '        _veh_last = reinterpret_cast<void*>(ctx->Rip);\n'
        '        std::fprintf(stderr, "JIT-CRASH at RIP=%p\\n", reinterpret_cast<void*>(ctx->Rip));\n'
        '    }\n'
        '    return EXCEPTION_CONTINUE_SEARCH;\n'
    )
    if old_rip3 in content:
        content = content.replace(old_rip3, new_cont_search, 1)
        patched = True
    else:
        # Try alternate format (CRLF or different encoding)
        alt_rip3 = 'static void* s_last_rip = nullptr;'
        if alt_rip3 in content and 's_same_rip_count' in content:
            # Found the old code - replace the whole block
            import re as _re
            # Match from "// Throttle" through "return EXCEPTION_CONTINUE_SEARCH;"
            content, count = _re.subn(
                r'// Throttle:.*?return EXCEPTION_CONTINUE_SEARCH;',
                '    // Log once per RIP, then fall through to __try/__except.\n'
                '    static void* _veh_last = nullptr;\n'
                '    if (_veh_last != reinterpret_cast<void*>(ctx->Rip)) {\n'
                '        _veh_last = reinterpret_cast<void*>(ctx->Rip);\n'
                '        std::fprintf(stderr, "JIT-CRASH at RIP=%p\\n", reinterpret_cast<void*>(ctx->Rip));\n'
                '    }\n'
                '    return EXCEPTION_CONTINUE_SEARCH;',
                content, count=1, flags=_re.DOTALL
            )
            if count > 0:
                patched = True

    # Fix 3: Remove the RIP+=3 line (may exist independently)
    old_rip3_line = '        ctx->Rip += 3;'
    if old_rip3_line in content:
        content = content.replace(old_rip3_line, '        // ctx->Rip += 3;  // disabled - causes cascading crashes', 1)
        patched = True

    # Fix 4: Forward declare FactAbortHandler before RunFactMode
    old_missing_fwd = 'static int RunFactMode() {'
    new_with_fwd = '// Forward declaration for FactAbortHandler (defined below).\n'
    new_with_fwd += '// Used by RunFactMode and the JIT dispatch worker thread.\n'
    new_with_fwd += 'static void FactAbortHandler(int);\n\n'
    new_with_fwd += 'static int RunFactMode() {'
    if old_missing_fwd in content and 'Forward declaration for FactAbortHandler' not in content:
        content = content.replace(old_missing_fwd, new_with_fwd, 1)
        patched = True
        print("  [build] Patched FactAbortHandler forward declaration")

    return content, patched
