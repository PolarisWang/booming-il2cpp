"""Post-process generated native-aot.generated.cpp to add chaos_valuetype forward declarations.

Some families (e.g. reflection-binding) use extern "C" declarations that reference
chaos_valuetype_* types from external assemblies (Chaos.TestFramework.Sdk attributes).
These types are forward-declared in method signatures but never defined as structs,
causing C2061/C4430/C2146 errors.

Since these functions only appear in vtable entries and are never actually called,
we can use typedef-to-int to make them C-linkage compatible (C2526: C linkage function
cannot return C++ class).

Usage:
    python _gen_valuetype_forward_decls.py <path-to-generated.cpp>
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


def patch_valuetype_forward_decls(filepath: Path) -> bool:
    """Add typedef forward declarations for all referenced chaos_valuetype types.

    Uses typedef int instead of struct forward decl because C2526 forbids
    extern "C" functions from returning C++ class types.
    """
    content = filepath.read_text(encoding="utf-8")

    # Find all chaos_valuetype_* identifiers used in the file
    valuetype_refs: set[str] = set()
    for m in re.finditer(r'\bchaos_valuetype_\w+', content):
        valuetype_refs.add(m.group())

    if not valuetype_refs:
        print(f"  [valuetype_fwd] no chaos_valuetype references found")
        return True

    # Check which ones already have a struct definition or forward declaration
    missing: list[str] = []
    for vt in sorted(valuetype_refs):
        pattern = rf'(struct|typedef)\s+{re.escape(vt)}[\s;{{]'
        if not re.search(pattern, content):
            missing.append(vt)

    if not missing:
        print(f"  [valuetype_fwd] all {len(valuetype_refs)} chaos_valuetype types already defined")
        return True

    # Add typedef forward declarations at namespace-level scope
    # Use typedef int for C-linkage compatibility
    # Find insertion point: after last #include line, before namespace
    namespace_match = re.search(r'\nnamespace\s+chaos::il2cpp::codegen::', content)
    if not namespace_match:
        print(f"  [valuetype_fwd] ERROR: could not find codegen namespace")
        return False

    include_end = content.rfind('\n', 0, namespace_match.start())
    include_end = content.rfind('#include', 0, include_end)
    insert_pos = content.index('\n', include_end) + 1

    fwd_decls = '\n'.join(f'typedef int {vt};' for vt in missing)
    fwd_block = f"\n// Auto-generated chaos_valuetype typedef (C-linkage compatible)\n{fwd_decls}\n"

    new_content = content[:insert_pos] + fwd_block + content[insert_pos:]

    filepath.write_text(new_content, encoding="utf-8")
    print(f"  [valuetype_fwd] added {len(missing)} typedef forward declarations")
    return True


def main():
    if len(sys.argv) < 2:
        print("Usage: python _gen_valuetype_forward_decls.py <file.cpp>")
        sys.exit(1)

    filepath = Path(sys.argv[1])
    if not filepath.exists():
        print(f"File not found: {filepath}")
        sys.exit(1)

    success = patch_valuetype_forward_decls(filepath)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()