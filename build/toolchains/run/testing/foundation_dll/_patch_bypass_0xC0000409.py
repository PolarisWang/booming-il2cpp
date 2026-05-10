"""Post-process generated native-aot.generated.cpp to bypass MSVC 0xC0000409 crash.

The MSVC compiler crashes (STATUS_STACK_BUFFER_OVERRUN / 0xC0000409) when a single
function has too many locals (~170 in Program::Main) or too many inline call blocks
(~8+ in RunNativeAot). This script:

1. Strips Program::Main() body to a minimal `return 0` stub
2. Replaces RunNativeAot() with a function-pointer dispatch table + loop

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
    # The old approach injected a #define at file scope which breaks non-void
    # helper functions. Remove it cleanly so the new longjmp approach works.
    # Handle both old format ("sets exitCode and returns") and new format
    # ("→ longjmp recovery"), including both CHAOS_IL2CPP_ABORT (pre-Layer 1)
    # and CHAOS_IL2CPP_FAIL (post-Layer 1) variants.
    layer0_old = re.compile(
        r'\n?// Verification mode: CHAOS_IL2CPP_(?:ABORT|FAIL).*?} while\(0\)\n'
        r'(#pragma runtime_checks\("gs", off\)\n)?',
        re.DOTALL
    )
    # Also remove the #include <setjmp.h> / thread_local variables from old runs
    layer0_jmp = re.compile(
        r'\n?#include <setjmp\.h>\n'
        r'// Verification mode:.*?longjmp recovery\.\n'
        r'.*?\n.*?\n'  # thread_local x 2
        r'#undef CHAOS_IL2CPP_(?:ABORT|FAIL)\n'
        r'#define CHAOS_IL2CPP_(?:ABORT|FAIL).*?} while\(0\)\n',
        re.DOTALL
    )
    for pat in (layer0_old, layer0_jmp):
        content, n = pat.subn('\n', content)
        if n:
            print(f"  Cleanup: removed old Layer 0 injection ({n} matches)")

    # ── Determine if this is a MethodN-based or handwritten entrypoint ──
    # Find method forward declarations ONLY (extern "C" void ... MethodN(void);)
    # These appear at the top of the file, before the actual function definitions.
    # Use forward declarations to avoid counting both decl and def.
    method_decls: list[tuple[int, str]] = []
    fwd_pattern = re.compile(
        r'extern "C" void\s+(\w+_(?:CustomEntry)?Method(\d+))\(void\)\s*;'
    )
    seen = set()
    for m in fwd_pattern.finditer(content):
        name, idx_str = m.group(1), m.group(2)
        idx = int(idx_str)
        if name not in seen:
            seen.add(name)
            method_decls.append((idx, name))

    is_handwritten = len(method_decls) == 0
    if is_handwritten:
        print("  Handwritten entrypoint (no MethodN forward declarations)")
    else:
        method_decls.sort(key=lambda x: x[0])
        method_count = len(method_decls)
        print(f"  Found {method_count} methods via forward declarations")

    # ── Find the static variable prefix for ___exitCode ──
    # Only needed for MethodN dispatch table; handwritten entrypoints don't
    # have this variable since they use custom RunNativeAot.
    exit_code_var = None
    if not is_handwritten:
        prefix_match = re.search(r'(chaos_static_\w+)___exitCode', content)
        if not prefix_match:
            print("  ERROR: no ___exitCode static variable found")
            return False
        exit_code_var = prefix_match.group(1) + "___exitCode"
        print(f"  Using exitCode var: {exit_code_var}")

    # ── 1. Strip Program::Main() body (MethodN families only) ────────────
    if not is_handwritten:
        # Find Program::Main opening brace
        main_start = re.search(
            r'extern "C" CHAOS_IL2CPP_INT32 \w+_Program_Main\(void\)\s*\{',
            content
        )
        if not main_start:
            print("  ERROR: could not find Program::Main()")
            return False

        # Find the matching closing brace by counting braces
        brace_start = main_start.end() - 1  # position of the {
        depth = 1
        pos = brace_start + 1
        while depth > 0 and pos < len(content):
            if content[pos] == '{':
                depth += 1
            elif content[pos] == '}':
                depth -= 1
            pos += 1

        if depth != 0:
            print("  ERROR: unbalanced braces in Program::Main()")
            return False

        header_end = brace_start + 1
        closing_brace_pos = pos - 1

        comment_start = content.rfind('// Managed method:', 0, main_start.start())
        if comment_start == -1:
            comment_start = content.rfind('\n\n', 0, main_start.start()) + 2

        new_content = (
            content[comment_start:header_end] +
            "\n    // STRIPPED - MSVC 0xC0000409 crash with ~170 locals\n    return 0;\n" +
            content[closing_brace_pos:]
        )

        new_content = content[:comment_start] + new_content
        content = new_content
        print("  Stripped Program::Main() body to return 0")

    # ── 2. Replace RunNativeAot() body (MethodN families only) ───────────
    if not is_handwritten:
        func_names = [name for _, name in method_decls]
        dispatch_table = "\n    ".join(f"{name}," for name in func_names)

        run_aot_replacement = f"""// Native AOT entry: bypass Program::Main() which causes 0xC0000409 crash
// Provides both single-method dispatch (for benchmark) and all-methods loop (for fact verify).
// Routes through HotpatchLookupBySlot for patch-aware dispatch.
using MethodFn = void (*)();
static constexpr MethodFn kAotMethods[] = {{
    {dispatch_table}
}};

// Forward declaration inside namespace for use below.
// Actual definition with 'extern \"C\"' is placed after the namespace closes.
const int kAotMethodCount = {method_count};

// --- Dispatch via Hotpatch dispatch table --------------------------------
// Routes through s_hotpatch_entries[] so that ApplyPatchFromMemory()
// (which sets kHotpatchActive) takes effect. When not patched, falls
// back to the direct kAotMethods[] array.
static inline auto* DispatchSlotGet(uint32_t slot) noexcept {{
    return chaos::il2cpp::runtime_core::HotpatchLookupBySlot(0u, slot);
}}

// Single-method dispatch: used by --benchmark N and --hotupdate-and-benchmark N
// Uses setjmp/longjmp recovery so CHAOS_IL2CPP_ABORT (from null checks on
// external runtime stubs) doesn't kill the process.
#pragma runtime_checks("gs", off)
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{{
    // Sentinel -1: return immediately for benchmark overhead calibration
    if (chaos_entry_index < 0) return 0;
    if (chaos_entry_index >= kAotMethodCount) return -1;
    {exit_code_var} = 0;
    g_chaos_abort_flag = 0;
    if (setjmp(g_chaos_abort_jmp) == 0) {{
        auto* hot_entry = DispatchSlotGet(static_cast<uint32_t>(chaos_entry_index));
        if (hot_entry && (reinterpret_cast<HotpatchEntryV0*>(hot_entry)->flags & kHotpatchActive)) {{
            chaos::il2cpp::runtime_core::InterpreterEntryDirect(
                reinterpret_cast<HotpatchEntryV0*>(hot_entry)->method_key, nullptr, nullptr);
        }} else if (hot_entry) {{
            auto* direct_fn = reinterpret_cast<HotpatchEntryV0*>(hot_entry)->direct_ptr;
            reinterpret_cast<void(*)()>(direct_fn)();
        }} else {{
            kAotMethods[chaos_entry_index]();
        }}
    }}
    if (g_chaos_abort_flag) {{ {exit_code_var} = 1; }}
    return ({exit_code_var} != 0) ? 0 : 1;
}}
#pragma runtime_checks("gs", restore)

// All-methods loop: used by fact verify and hotupdate (entry.exe with no args)
// Uses setjmp/longjmp recovery so CHAOS_IL2CPP_ABORT sets exitCode bitmask
// instead of crashing with STATUS_STACK_BUFFER_OVERRUN (0xC0000409).
#pragma runtime_checks("gs", off)
extern "C" CHAOS_IL2CPP_INT32 RunNativeAotAll()
{{
    CHAOS_IL2CPP_INT32 result = 0;
    for (int i = 0; i < kAotMethodCount; i++)
    {{
        {exit_code_var} = 0;
        g_chaos_abort_flag = 0;
        if (setjmp(g_chaos_abort_jmp) == 0) {{
            auto* hot_entry = DispatchSlotGet(static_cast<uint32_t>(i));
            if (hot_entry && (reinterpret_cast<HotpatchEntryV0*>(hot_entry)->flags & kHotpatchActive)) {{
                chaos::il2cpp::runtime_core::InterpreterEntryDirect(
                    reinterpret_cast<HotpatchEntryV0*>(hot_entry)->method_key, nullptr, nullptr);
            }} else if (hot_entry) {{
                auto* direct_fn = reinterpret_cast<HotpatchEntryV0*>(hot_entry)->direct_ptr;
                reinterpret_cast<void(*)()>(direct_fn)();
            }} else {{
                kAotMethods[i]();
            }}
        }}
        if (g_chaos_abort_flag) {{ {exit_code_var} = 1; }}
        if ({exit_code_var}) result |= (1 << i);
    }}
    return result;
}}
#pragma runtime_checks("gs", restore)

// Benchmark: measure pure generated code, no dispatch/hotpatch overhead.
// Directly calls kAotMethods[index]() to avoid HotpatchLookupBySlot overhead.
extern \"C\" double BenchmarkMethod(int chaos_entry_index, int iterations) {{
    if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount) return -1.0;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {{
        kAotMethods[chaos_entry_index]();
    }}
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}}"""

        # Find RunNativeAot
        run_start = content.find('RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)')
        if run_start == -1:
            print("  ERROR: could not find RunNativeAot()")
            return False

        # Find the opening brace of RunNativeAot
        brace_start = content.find('{', run_start)
        if brace_start == -1:
            print("  ERROR: could not find RunNativeAot() opening brace")
            return False

        # Find matching closing brace
        depth = 1
        pos = brace_start + 1
        while depth > 0 and pos < len(content):
            if content[pos] == '{':
                depth += 1
            elif content[pos] == '}':
                depth -= 1
            pos += 1

        if depth != 0:
            print("  ERROR: unbalanced braces in RunNativeAot()")
            return False

        closing_brace_pos = pos - 1

        # Find start of the RunNativeAot block
        aot_comment = content.rfind('// Native AOT entry', 0, run_start)
        block_start = aot_comment if aot_comment != -1 else 0

        new_content = content[:block_start] + run_aot_replacement + content[closing_brace_pos + 1:]
        content = new_content

        count = content.count('kAotMethods')
        print(f"  Replaced RunNativeAot() with dispatch table + loop ({count} refs)")

        # Append extern "C" definition of kAotMethodCount AFTER the namespace closes.
        pragma_pos = content.rfind('#pragma warning(pop)')
        if pragma_pos == -1:
            print("  ERROR: could not find #pragma warning(pop) insertion point")
            return False

        method_count_def = (
            '\n'
            '// Definition at global scope with extern "C" linkage '
            'so runtime-entry.cpp can reference it.\n'
            f'extern "C" const int kAotMethodCount = {method_count};\n'
        )
        content = content[:pragma_pos] + method_count_def + content[pragma_pos:]

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
            f'#define CHAOS_IL2CPP_FAIL() do {{ \\\n'
            f'    g_chaos_abort_flag = 1; \\\n'
            f'    longjmp(g_chaos_abort_jmp, 1); \\\n'
            f'}} while(0)\n'
            '// Also override CHAOS_IL2CPP_ABORT → FAIL so that older generated code\n'
            '// (e.g. handwritten entrypoints not regenerated by Layer 1 codegen)\n'
            '// doesn\'t call std::abort() → 0xC0000409 segfault.\n'
            '#undef CHAOS_IL2CPP_ABORT\n'
            '#define CHAOS_IL2CPP_ABORT() CHAOS_IL2CPP_FAIL()\n'
        )
        content = content[:gs_off_line_end + 1] + abort_override + content[gs_off_line_end + 1:]

    # ── For handwritten entrypoints: wrap RunNativeAot in setjmp/longjmp ──
    if is_handwritten:
        # Find the RunNativeAot function body and wrap it with setjmp/longjmp
        # so that CHAOS_IL2CPP_ABORT/FAIL (which is overridden to longjmp)
        # doesn't crash the process — it returns a failure code instead.
        run_start = content.find('extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)')
        if run_start != -1:
            brace_start = content.find('{', run_start)
            if brace_start != -1:
                depth = 1
                pos = brace_start + 1
                while depth > 0 and pos < len(content):
                    if content[pos] == '{': depth += 1
                    elif content[pos] == '}': depth -= 1
                    pos += 1
                if depth == 0:
                    closing_brace_pos = pos - 1
                    inner = content[brace_start + 1:closing_brace_pos].strip()
                    # Remove trailing 'return' keyword so we can assign to chaos_result
                    # The body is typically "return SomeFunction(args);"
                    inner_no_return = re.sub(r'^return\s+', '', inner).rstrip(';')
                    wrapped = (
                        "\n    // Wrapped in setjmp/longjmp by patch_bypass Layer 0\n"
                        "    CHAOS_IL2CPP_INT32 chaos_result = 0;\n"
                        "    g_chaos_abort_flag = 0;\n"
                        "    if (setjmp(g_chaos_abort_jmp) == 0)\n    {\n"
                        "        chaos_result = " + inner_no_return + ";\n"
                        "    }\n"
                        "    if (g_chaos_abort_flag) { return 0; }\n"
                        "    return chaos_result;\n"
                    )
                    content = content[:brace_start + 1] + wrapped + content[closing_brace_pos:]
                    print("  Wrapped handwritten RunNativeAot in setjmp/longjmp")
                else:
                    print("  WARNING: unbalanced braces in handwritten RunNativeAot")

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