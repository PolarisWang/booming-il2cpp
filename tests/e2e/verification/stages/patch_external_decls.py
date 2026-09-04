#!/usr/bin/env python3
"""Post-process TPG-generated subjects pages to add missing chaos_external_runtime_* declarations.

The codegen generates a shared header (native-aot.generated.header.h) with extern
declarations for all chaos_external_runtime_* symbols it knows about.  However, TPG
generates wrapper subjects pages that may reference ADDITIONAL symbols (e.g. concretized
generic Lookup<Int32,Int32>::Contains) that the codegen never registered.  These symbols
exist in the page text but have no declaration in the header, causing C3861 on page-split
builds.

This function scans every subjects .cpp file, collects chaos_external_runtime_* call
references, checks the header for existing declarations, and prepends any missing ones
to the header BEFORE the cmake build runs.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path


# Regex patterns mirroring the C# AddExternalRuntimeStubs (NativeAotEmitter.Shared.cs)
_call_rx = re.compile(r'\b(chaos_external_runtime_\w+)\(')
_decl_rx = re.compile(r'(?:extern|static inline|\bvoid)\b.*\bchaos_external_runtime_\w+\s*\(')


def _subjects_sources_dir(native_dir: Path) -> Path:
    """Return the subjects source directory under native_dir."""
    return native_dir / "subjects"


def _header_path(native_dir: Path) -> Path:
    """Return the shared header path under native_dir."""
    return native_dir / "subjects" / "native-aot.generated.header.h"


def _collect_referenced_symbols(subjects_dir: Path) -> set[str]:
    """Scan all .cpp files in subjects_dir for chaos_external_runtime_* calls."""
    referenced = set()
    if not subjects_dir.is_dir():
        return referenced
    for cpp in sorted(subjects_dir.glob("*.cpp")):
        try:
            text = cpp.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        for m in _call_rx.finditer(text):
            referenced.add(m.group(1))
    return referenced


def _collect_declared_symbols(header_path: Path) -> set[str]:
    """Collect all chaos_external_runtime_* symbols that already have declarations/definitions in the header."""
    declared = set()
    if not header_path.is_file():
        return declared
    try:
        text = header_path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return declared
    for m in _decl_rx.finditer(text):
        # Extract the symbol name (the chaos_external_runtime_* part)
        sym = re.search(r'\b(chaos_external_runtime_\w+)\s*\(', m.group())
        if sym:
            declared.add(sym.group(1))
    return declared


def patch_missing_external_declarations(native_dir: Path) -> int:
    """Scan subjects sources, find missing declarations, prepend to header.

    Returns the number of declarations added (0 = nothing to fix).
    """
    subjects_dir = _subjects_sources_dir(native_dir)
    header_path = _header_path(native_dir)

    referenced = _collect_referenced_symbols(subjects_dir)
    if not referenced:
        return 0

    declared = _collect_declared_symbols(header_path)
    missing = referenced - declared
    if not missing:
        return 0

    # Build extern declarations for missing symbols
    lines = [
        "// ── External runtime stubs (post-emission, TPG subjects) ──",
    ]
    for sym in sorted(missing):
        lines.append(f"extern CHAOS_IL2CPP_INTPTR {sym}() noexcept;")
    lines.append("")

    # Prepend to header after the last #include
    try:
        text = header_path.read_text(encoding="utf-8", errors="replace")
    except OSError as e:
        print(f"  [build] WARNING: cannot read header {header_path}: {e}")
        return 0

    anchor = text.rfind("#include")
    if anchor < 0:
        anchor = 0
    else:
        nl = text.find('\n', anchor)
        if nl >= 0:
            anchor = nl + 1

    new_text = text[:anchor] + "\n".join(lines) + "\n" + text[anchor:]
    try:
        header_path.write_text(new_text, encoding="utf-8")
    except OSError as e:
        print(f"  [build] WARNING: cannot write header {header_path}: {e}")
        return 0

    print(f"  [build] Patched {len(missing)} missing chaos_external_runtime_* declarations into {header_path.name}")
    for sym in sorted(missing):
        print(f"    [extern] {sym}")
    return len(missing)


if __name__ == "__main__":
    # Allow running as a standalone script for testing
    if len(sys.argv) < 2:
        print("Usage: python patch_external_decls.py <native_dir>")
        sys.exit(1)
    count = patch_missing_external_declarations(Path(sys.argv[1]))
    print(f"Patched {count} declarations")
    sys.exit(0)