"""Post-process generated native-aot.generated.cpp for MSVC 0xC0000409 workaround.

The MSVC compiler crashes (STATUS_STACK_BUFFER_OVERFLOW / 0xC0000409) when a single
function has too many locals (~170 in Program::Main). This script:

1. Replaces CHAOS_IL2CPP_FAIL/ABORT with setjmp/longjmp recovery (Layer 0)
2. Wraps generated functions with #pragma runtime_checks("gs", off)

NOTE: Dispatch table (kAotMethods[], RunNativeAot*) and AotDirectDispatch tables
are now emitted natively by C# codegen (NativeAotLoweringPlanner). This script
only handles the MSVC-specific test-only workarounds.

Usage:
    python _patch_bypass_0xC0000409.py <path-to-native-aot.generated.cpp>
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


def patch_file(filepath: Path) -> bool:
    content = filepath.read_text(encoding="utf-8")

    # ── Cleanup: remove old Layer 0 file-scope injection (if any) ──────────
    layer0_old = re.compile(
        r'\n?// Verification mode: CHAOS_IL2CPP_(?:ABORT|FAIL).*?} while\(0\)\n'
        r'(#pragma runtime_checks\("gs", off\)\n)?',
        re.DOTALL
    )
    layer0_jmp = re.compile(
        r'\n?#include <setjmp\.h>\n'
        r'// Verification mode:.*?longjmp recovery\.\n'
        r'.*?\n.*?\n'
        r'#undef CHAOS_IL2CPP_(?:ABORT|FAIL)\n'
        r'#define CHAOS_IL2CPP_(?:ABORT|FAIL).*?} while\(0\)\n',
        re.DOTALL
    )
    for pat in (layer0_old, layer0_jmp):
        content, n = pat.subn('\n', content)
        if n:
            print(f"  Cleanup: removed old Layer 0 injection ({n} matches)")

    # ── Cleanup: remove old AotDirectDispatch tables (if any) ────────────────
    aotdd_cleanup = re.compile(
        r'\n// ── AotDirectDispatch: pre-resolved AOT function pointers ──────────\n'
        r'.*?'
        r'\}\(\);\n',
        re.DOTALL
    )
    content, n = aotdd_cleanup.subn('\n', content)
    if n:
        print(f"  Cleanup: removed old AotDirectDispatch tables ({n} matches)")

    # Also remove old standalone extern "C" kAotMethodCount if present
    # (codegen now emits it natively).
    method_count_def_old = re.compile(
        r'\n// Definition at global scope with extern "C" linkage '
        r'so runtime-entry\.cpp can reference it\.\n'
        r'extern "C" const int kAotMethodCount = \d+;\n'
    )
    content, n = method_count_def_old.subn('\n', content)
    if n:
        print(f"  Cleanup: removed old extern kAotMethodCount ({n} matches)")

    # ── Cleanup: remove old dispatch table + RunNativeAot replacement (if any) ─
    # The old bypass generated its own kAotMethods[], RunNativeAot, etc.
    # Codegen now emits these natively, so remove old copies if present.
    old_dispatch = re.compile(
        r'\n// Native AOT entry: bypass Program::Main\(\) which causes 0xC0000409 crash\n'
        r'.*?'
        r'return std::chrono::duration<double, std::milli>\(\n'
        r'        end - start\)\.count\(\);\n'
        r'\}',
        re.DOTALL
    )
    content, n = old_dispatch.subn('\n', content)
    if n:
        print(f"  Cleanup: removed old dispatch table replacement ({n} matches)")

    # ── Cleanup: remove old Program::Main() stub (if present) ───────────
    # Codegen no longer emits Program::Main, but old files may still have it.
    main_stub = re.compile(
        r'\n    // STRIPPED - MSVC 0xC0000409 crash with ~170 locals\n    return 0;\n'
    )
    content, n = main_stub.subn('\n', content)
    if n:
        print(f"  Cleanup: removed old Program::Main stub ({n} matches)")

    # ── Always: #pragma runtime_checks("gs", off) wrapping ────────────────
    # Insert #pragma runtime_checks("gs", off) after #pragma warning(disable: ...)
    warning_disable = content.find('#pragma warning(disable:')
    if warning_disable != -1:
        disable_line_end = content.find('\n', warning_disable)
        gs_off = '\n#pragma runtime_checks("gs", off)'
        content = content[:disable_line_end] + gs_off + content[disable_line_end:]

    # ── Always: Layer 0 CHAOS_IL2CPP_FAIL → longjmp recovery ──────────────
    gs_off_pos = content.find('#pragma runtime_checks("gs", off)')
    if gs_off_pos != -1:
        gs_off_line_end = content.find('\n', gs_off_pos)
        abort_override = (
            '\n'
            '#include <chrono>\n'
            '#include <setjmp.h>\n'
            '// Verification mode: CHAOS_IL2CPP_FAIL → longjmp recovery.\n'
            '// Fact runner sees expected failures as bitmask entries instead\n'
            '// of 0xC0000409 process crash. Injected by pipeline Layer 0.\n'
            'thread_local jmp_buf g_chaos_abort_jmp;\n'
            'thread_local int g_chaos_abort_flag = 0;\n'
            '#undef CHAOS_IL2CPP_FAIL\n'
            '#define CHAOS_IL2CPP_FAIL() do { \\\n'
            '    g_chaos_abort_flag = 1; \\\n'
            '    longjmp(g_chaos_abort_jmp, 1); \\\n'
            '} while(0)\n'
            '// Also override CHAOS_IL2CPP_ABORT → FAIL so that older generated code\n'
            '// (e.g. handwritten entrypoints not regenerated by Layer 1 codegen)\n'
            '// doesn\'t call std::abort() → 0xC0000409 segfault.\n'
            '#undef CHAOS_IL2CPP_ABORT\n'
            '#define CHAOS_IL2CPP_ABORT() CHAOS_IL2CPP_FAIL()\n'
        )
        content = content[:gs_off_line_end + 1] + abort_override + content[gs_off_line_end + 1:]

    # ── For handwritten entrypoints: wrap RunNativeAot in setjmp/longjmp ──
    # Check if RunNativeAot exists (handwritten entrypoints may not have the
    # codegen-emitted version).
    run_start = content.find('extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)')
    if run_start != -1:
        # Find the opening brace and wrap inner body with setjmp
        brace_start = content.find('{', run_start)
        if brace_start != -1:
            # Check if it already has setjmp (codegen version doesn't)
            inner_after_brace = content[brace_start + 1:brace_start + 200]
            if 'setjmp' not in inner_after_brace:
                depth = 1
                pos = brace_start + 1
                while depth > 0 and pos < len(content):
                    if content[pos] == '{': depth += 1
                    elif content[pos] == '}': depth -= 1
                    pos += 1
                if depth == 0:
                    closing_brace_pos = pos - 1
                    inner = content[brace_start + 1:closing_brace_pos].strip()
                    # Find return statement
                    return_match = re.search(r'return\s+(\d+|\([^)]+\))\s*;', inner)
                    wrapped = (
                        "\n    // Wrapped in setjmp/longjmp by patch_bypass Layer 0\n"
                        "    CHAOS_IL2CPP_INT32 chaos_result = 0;\n"
                        "    g_chaos_abort_flag = 0;\n"
                        "    if (setjmp(g_chaos_abort_jmp) == 0)\n    {\n"
                        "        " + inner + "\n"
                        "    }\n"
                        "    if (g_chaos_abort_flag) { return 0; }\n"
                    )
                    if return_match:
                        wrapped = (
                            "\n    // Wrapped in setjmp/longjmp by patch_bypass Layer 0\n"
                            "    CHAOS_IL2CPP_INT32 chaos_result = 0;\n"
                            "    g_chaos_abort_flag = 0;\n"
                            "    if (setjmp(g_chaos_abort_jmp) == 0)\n    {\n"
                            "        chaos_result = (" + inner.replace('return ', '').rstrip(';') + ");\n"
                            "    }\n"
                            "    if (g_chaos_abort_flag) { return 0; }\n"
                            "    return chaos_result;\n"
                        )
                    content = content[:brace_start + 1] + wrapped + content[closing_brace_pos:]
                    print("  Wrapped RunNativeAot in setjmp/longjmp")

    # Also wrap RunNativeAotAll in setjmp/longjmp if it exists and not already wrapped
    run_all_start = content.find('extern "C" CHAOS_IL2CPP_INT32 RunNativeAotAll()')
    if run_all_start != -1:
        brace_start = content.find('{', run_all_start)
        if brace_start != -1:
            inner_after_brace = content[brace_start + 1:brace_start + 200]
            if 'setjmp' not in inner_after_brace:
                depth = 1
                pos = brace_start + 1
                while depth > 0 and pos < len(content):
                    if content[pos] == '{': depth += 1
                    elif content[pos] == '}': depth -= 1
                    pos += 1
                if depth == 0:
                    closing_brace_pos = pos - 1
                    inner = content[brace_start + 1:closing_brace_pos].strip()
                    wrapped = (
                        "\n    // Wrapped in setjmp/longjmp by patch_bypass Layer 0\n"
                        "    CHAOS_IL2CPP_INT32 chaos_result = 0;\n"
                        "    g_chaos_abort_flag = 0;\n"
                        "    if (setjmp(g_chaos_abort_jmp) == 0)\n    {\n"
                        "        " + inner + "\n"
                        "    }\n"
                        "    if (g_chaos_abort_flag) { return 0; }\n"
                        "    return chaos_result;\n"
                    )
                    content = content[:brace_start + 1] + wrapped + content[closing_brace_pos:]
                    print("  Wrapped RunNativeAotAll in setjmp/longjmp")

    # ── Always: Inject #pragma restore before warning(pop) ────────────────
    pragma_pop = content.rfind('#pragma warning(pop)')
    gs_restore = '\n#pragma runtime_checks("gs", restore)\n'
    content = content[:pragma_pop] + gs_restore + content[pragma_pop:]

    filepath.write_text(content, encoding="utf-8")
    print(f"  Patched {filepath}")
    return True


def main():
    if len(sys.argv) < 2:
        print("Usage: python _patch_bypass_0xC0000409.py <file.cpp>")
        sys.exit(1)

    filepath = Path(sys.argv[1])
    if not filepath.exists():
        print(f"File not found: {filepath}")
        sys.exit(1)

    success = patch_file(filepath)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()