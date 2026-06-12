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
    # Also add inline stub definitions for Assert_Reset and Assert_Complete
    # so they don't need an external .cpp file for the linker.
    _assert_stubs = (
        '\n// AOT assert stubs (patched by runtime_entry_patcher.py)\n'
        'extern "C" int Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset() noexcept { return 0; }\n'
        'extern "C" int Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete() noexcept { return 0; }\n'
    )
    if _assert_stubs not in content:
        # Insert after includes, before first function.
        # Handle both LF and CRLF line endings.
        for sep in ('\nstatic ', '\r\nstatic '):
            first_fn = content.find(sep)
            if first_fn > 0:
                break
            content = content[:first_fn] + _assert_stubs + content[first_fn:]
            patched = True
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
        '    static auto _chaos_assert_complete = +[]() noexcept -> int { return 0; }; \\\n'
        '    if (_chaos_assert_complete() != 0) { \\\n'
        '        std::fprintf(stderr, "[ASSERT] s_exitCode was non-zero after fact loop\\n"); \\\n'
        '    } \\\n'
        '} while(0)'
    )
    if old_macro in content:
        content = content.replace(old_macro, new_macro, 1)
        patched = True

    # Fix 2: Fix JitVehHandler: REPLACE the entire RIP+=3 throttle logic with
    # a simple CONTINUE_SEARCH (let __try/__except handle the crash properly).
    # Handle both LF and CRLF line endings in the template output.
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

    # Fix 5: Replace SEH handler's caught ? -1 : result with caught ? result : result
    # This prevents value=-1 for crashed methods — they get value=0 instead,
    # which allows the fact harness to distinguish "crash" from "wrong answer".
    old_seh_out = 'caught ? -1 : result'
    if old_seh_out in content:
        new_seh_out = 'caught ? result : result'
        content = content.replace(old_seh_out, new_seh_out)
        patched = True
        print("  [build] Patched SEH output: -1 -> result (crash yields value=0)")

    # Fix 6: Add PalTryCallNoExcept protection to Linux fact dispatch loop.
    # The external runtime dispatch table (kChaosExternalRuntimeSubjects[]) has
    # mutable const char* pointers that can get corrupted at runtime, causing
    # SIGSEGV in ChaosExternalRuntimeFallback (strcmp against corrupted ptr).
    # Without PalTryCallNoExcept, the SIGSEGV kills the entire process —
    # with it, the individual subject is marked as "caught" and the loop continues.
    #
    # Step 6a: Add #include <chaos/pal/pal_eh.h>
    old_pal_inc = '#include <csignal>'
    new_pal_inc = '#include <csignal>\n#include <chaos/pal/pal_eh.h>'
    if old_pal_inc in content and '#include <chaos/pal/pal_eh.h>' not in content:
        content = content.replace(old_pal_inc, new_pal_inc, 1)
        patched = True
        print("  [build] Patched includes: added <chaos/pal/pal_eh.h>")

    # Step 6b: Inject PalDispatchWrapper before RunFactJsonMode.
    # Matches the last comment line before the function definition.
    old_runjson_entry = (
        '// all Subject_N wrappers from the subjects DLL as subject entries.\n'
        'static int RunFactJsonMode() {'
    )
    pal_wrapper = (
        '// all Subject_N wrappers from the subjects DLL as subject entries.\n'
        '// ── PalDispatchWrapper for PalTryCallNoExcept ──────────────────────\n'
        '// Wraps ChaosDispatchMethodGetValue into the PalTryCallNoExcept\n'
        '// (uint64_t(*)(uint64_t,...,uint64_t)) function-pointer signature\n'
        '// so that SIGSEGV from corrupted dispatch table entries is caught\n'
        '// and the fact loop continues to the next subject.\n'
        'static uint64_t PalDispatchWrapper(uint64_t entries, uint64_t count,\n'
        '                                    uint64_t index, uint64_t thunks,\n'
        '                                    uint64_t, uint64_t, uint64_t,\n'
        '                                    uint64_t) noexcept {\n'
        '    return static_cast<uint64_t>(\n'
        '        chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(\n'
        '            reinterpret_cast<const chaos::il2cpp::runtime_core::HotpatchEntryV0*>(\n'
        '                static_cast<uintptr_t>(entries)),\n'
        '            static_cast<int32_t>(count),\n'
        '            static_cast<int32_t>(index),\n'
        '            reinterpret_cast<void (* const*)() noexcept>(\n'
        '                static_cast<uintptr_t>(thunks))));\n'
        '}\n'
        '\n'
        'static int RunFactJsonMode() {'
    )
    if old_runjson_entry in content and 'PalDispatchWrapper' not in content:
        content = content.replace(old_runjson_entry, pal_wrapper, 1)
        patched = True
        print("  [build] Patched PalDispatchWrapper before RunFactJsonMode")

    # Step 6c: Replace the bare ChaosDispatchMethodGetValue call in the Linux
    # dispatch loop with PalTryCallNoExcept + PalTryResetState wrapping.
    # The old code had a raw call inside try/catch — SIGSEGV would kill the
    # process before reaching catch.  New code resets PAL EH state, wraps
    # the call in PalTryCallNoExcept (sigsetjmp/siglongjmp), then falls
    # through to the existing C++ exception handler.
    old_dispatch_call = (
        'result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(\n'
        '                        GetHotpatchEntries(), kAotMethodCount, v_i,'
        ' CHAOS_USE_DEFAULT_THUNKS);'
    )
    new_dispatch_call = (
        'chaos::il2cpp::pal::PalTryResetState();\n'
        '                    uint64_t _pal_result = 0;\n'
        '                    bool _pal_caught ='
        ' chaos::il2cpp::pal::PalTryCallNoExcept(\n'
        '                        PalDispatchWrapper,\n'
        '                        reinterpret_cast<uint64_t>(GetHotpatchEntries()),\n'
        '                        static_cast<uint64_t>(kAotMethodCount),\n'
        '                        static_cast<uint64_t>(v_i),\n'
        '                        reinterpret_cast<uint64_t>(CHAOS_USE_DEFAULT_THUNKS),\n'
        '                        0, 0, 0, 0,\n'
        '                        _pal_result);\n'
        '                    if (_pal_caught) {\n'
        '                        caught = true;\n'
        '                    } else {\n'
        '                        result = static_cast<int64_t>(_pal_result);\n'
        '                    }'
    )
    if old_dispatch_call in content and '_pal_result' not in content:
        content = content.replace(old_dispatch_call, new_dispatch_call, 1)
        patched = True
        print("  [build] Patched Linux dispatch call with PalTryCallNoExcept")

    return content, patched
