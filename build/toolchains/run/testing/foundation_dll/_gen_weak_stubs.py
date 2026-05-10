"""Generate stub definitions for unresolved externals in generated C++.

The full-closure translation pulls in vtable arrays that reference extern "C"
functions from external assemblies (e.g. Chaos.TestFramework.Sdk attributes).
These functions are never actually called — they just appear in vtable entries
via reinterpret_cast<void*>(fn).

This script generates actual C function stub definitions.

Usage:
    python _gen_weak_stubs.py <path-to-generated.cpp> <stub-output-dir> [family-slug]
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


def find_vtable_fn_names(content: str) -> set[str]:
    """Find extern C functions referenced in vtable arrays.

    Scans the generated C++ for:
    1. All extern "C" function declarations
    2. All vtable array entries using reinterpret_cast<void*>(FnName)
    3. Filters to those in vtable refs but NOT defined in the file
    """
    # All extern "C" declarations
    extern_fns: set[str] = set()
    for m in re.finditer(
        r'extern\s+"C"\s+(?:CHAOS_IL2CPP_\w+|chaos_valuetype_\w+|void|CHAOS_IL2CPP_INTPTR|int)\s+'
        r'(\w+)\s*\(',
        content
    ):
        extern_fns.add(m.group(1))

    # vtable array entries use reinterpret_cast<void*>(FnName)
    vtable_refs: set[str] = set()
    for m in re.finditer(
        r'reinterpret_cast<void\*>\((\w+)\)',
        content
    ):
        vtable_refs.add(m.group(1))

    # Function definitions in this file (after namespace opening, has a body)
    defined_fns: set[str] = set()
    for m in re.finditer(
        r'\n\w+\s+(\w+)\s*\([^)]*\)\s*\n?\{',
        content
    ):
        defined_fns.add(m.group(1))
    for m in re.finditer(
        r'extern\s+"C"\s+\w+\s+(\w+)\s*\([^)]*\)\s*\{',
        content
    ):
        defined_fns.add(m.group(1))

    # Filter: must be in both extern declarations and vtable refs, NOT defined
    candidates = vtable_refs & extern_fns - defined_fns

    # Filter out known codegen-internal symbols
    known_internal = {
        'InterpreterEntryDirect',
        'RunNativeAot',
    }
    candidates = candidates - known_internal

    return candidates


# Known mapping from function names to their return+param types
# Most TestFramework.Sdk functions return/get/set chaos_valuetype_* types
# or take CHAOS_IL2CPP_INTPTR. We match by prefix patterns.
def _stub_for_fn(fn_name: str) -> str:
    """Generate a stub definition matching the function name patterns.

    All stubs use 'int' as return/param type since chaos_valuetype_* was
    typedef'd to int. These stubs are never actually called at runtime —
    they only exist in vtable arrays as reinterpret_cast<void*>(fn).
    """
    if fn_name.endswith('__ctor'):
        return f'extern "C" void {fn_name}(int chaos_fn_arg_0)\n' \
               f'{{\n' \
               f'    (void)chaos_fn_arg_0;\n' \
               f'}}\n'
    elif '_get_' in fn_name or fn_name.endswith('_get_'):
        return f'extern "C" int {fn_name}(int chaos_fn_arg_0)\n' \
               f'{{\n' \
               f'    (void)chaos_fn_arg_0;\n' \
               f'    return 0;\n' \
               f'}}\n'
    elif '_set_' in fn_name or fn_name.endswith('_set_'):
        return f'extern "C" void {fn_name}(int chaos_fn_arg_0, int chaos_fn_arg_1)\n' \
               f'{{\n' \
               f'    (void)chaos_fn_arg_0; (void)chaos_fn_arg_1;\n' \
               f'}}\n'
    else:
        return f'extern "C" int {fn_name}(int chaos_fn_arg_0)\n' \
               f'{{\n' \
               f'    (void)chaos_fn_arg_0;\n' \
               f'    return 0;\n' \
               f'}}\n'


def generate_stub_cpp(unresolved: set[str], output_dir: Path, family_slug: str) -> Path:
    """Generate actual C function stub definitions."""
    if not unresolved:
        return None

    lines = [
        "// Auto-generated stub definitions for unresolved externals",
        f"// Family: {family_slug}",
        "// These functions appear in vtable arrays but are never actually called.",
        "// They are referenced because the full-closure translation pulled in",
        "// attribute type metadata from external assemblies.",
        '#include <cstdint>',
        '',
        'using CHAOS_IL2CPP_INTPTR = intptr_t;',
        'using CHAOS_IL2CPP_INT32 = int32_t;',
        '',
    ]

    for fn_name in sorted(unresolved):
        lines.append(_stub_for_fn(fn_name))
        lines.append('')

    stub_path = output_dir / f"stubs-{family_slug}.cpp"
    stub_path.write_text("\n".join(lines), encoding="utf-8")
    print(f"  [gen_stubs] wrote {len(unresolved)} stub definitions to {stub_path}")
    return stub_path


def main():
    if len(sys.argv) < 3:
        print("Usage: python _gen_weak_stubs.py <path-to-generated.cpp> <stub-output-dir> [family-slug]")
        sys.exit(1)

    filepath = Path(sys.argv[1])
    output_dir = Path(sys.argv[2])
    family_slug = sys.argv[3] if len(sys.argv) > 3 else "unknown"

    if not filepath.exists():
        print(f"File not found: {filepath}")
        sys.exit(1)

    content = filepath.read_text(encoding="utf-8")
    unresolved = find_vtable_fn_names(content)

    if not unresolved:
        print(f"  [gen_stubs] no unresolved externals found in vtable refs")
        sys.exit(0)

    stub_path = generate_stub_cpp(unresolved, output_dir, family_slug)
    if stub_path:
        print(f"  [gen_stubs] generated {len(unresolved)} stubs")
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()