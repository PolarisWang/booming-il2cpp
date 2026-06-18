"""Post-process generated C++ code to add missing chaos_valuetype_* typedefs.

Scans all generated .cpp page files for chaos_valuetype_* references that lack
a corresponding typedef in the generated header, and appends the missing typedefs.
This is a robust fallback for types referenced in cross-assembly stub declarations
whose TypeSubjectId metadata is missing from the AOT IR JSON.
"""
import re
import sys
from pathlib import Path


def find_missing_value_type_typedefs(native_dir: Path) -> int:
    """Find and fix missing chaos_valuetype_* typedefs in the generated header."""
    header = native_dir / "subjects" / "native-aot.generated.header.h"
    if not header.exists():
        print(f"  [fix-vt] Header not found: {header}")
        return 0

    # Read header content
    hdr_text = header.read_text(encoding="utf-8")

    # Collect existing typedefs
    existing_typedefs = set()
    for m in re.finditer(r'typedef\s+CHAOS_IL2CPP_INT32\s+(chaos_valuetype_\w+)', hdr_text):
        existing_typedefs.add(m.group(1))

    # Scan all generated .cpp files for chaos_valuetype_* references
    needed = set()
    pattern = re.compile(r'\b(chaos_valuetype_\w+)\b')
    for cpp_file in native_dir.glob("subjects/native-aot.generated*.cpp"):
        text = cpp_file.read_text(encoding="utf-8")
        for m in pattern.finditer(text):
            sym = m.group(1)
            if sym not in existing_typedefs:
                needed.add(sym)

    if not needed:
        return 0

    # Append missing typedefs to header
    with header.open("a", encoding="utf-8") as f:
        f.write("\n// Auto-fixed: missing chaos_valuetype_* typedefs\n")
        for sym in sorted(needed):
            f.write(f"typedef CHAOS_IL2CPP_INT32 {sym};\n")

    print(f"  [fix-vt] Added {len(needed)} missing chaos_valuetype_* typedefs:")
    for sym in sorted(needed):
        print(f"    - {sym}")
    return len(needed)


if __name__ == "__main__":
    native_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else Path.cwd()
    count = find_missing_value_type_typedefs(native_dir)
    sys.exit(0 if count >= 0 else 1)
