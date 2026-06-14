#!/usr/bin/env python3
"""Generate fact_skip_indices.h for all chunks with AOT-unreachable stubs.

Scans each chunk's native-aot.generated.cpp (in subjects/) for functions
containing CHAOS_IL2CPP_FAIL() which indicates AOT-unreachable stubs, then
maps them to hotpatch table indices and writes fact_skip_indices.h.
"""
import re, os, sys
from pathlib import Path

FD = Path("/home/debian/agent/booming-il2cpp/testing/foundation-dll")

def find_aot_unreachable_hotpatch_indices(subjects_cpp):
    """Return set of hotpatch entry indices that are AOT-unreachable stubs."""
    content = subjects_cpp.read_text()

    # Find the s_hotpatch_entries array
    start_marker = 'extern "C" HotpatchEntryV0 s_hotpatch_entries['
    start_idx = content.find(start_marker)
    if start_idx < 0:
        print(f"  WARNING: no hotpatch entries in {subjects_cpp}")
        return set()

    brace_idx = content.find('{', start_idx)
    entries_text = content[brace_idx:]

    # Find closing brace
    depth = 0; end_idx = -1
    for i, c in enumerate(entries_text):
        if c == '{': depth += 1
        elif c == '}': depth -= 1
        if depth == 0: end_idx = brace_idx + i + 1; break

    entries_section = content[brace_idx:end_idx]

    # Parse entries with their comments
    entry_pattern = r'\{([^}]+)\},\s*//\s*(.+)'
    matches = list(re.finditer(entry_pattern, entries_section))

    unreachable_indices = set()
    for idx, m in enumerate(matches):
        comment = m.group(2)
        # Check if this is an AOT-unreachable closure stub
        # Stubs are entries with function names containing "__c_" or "DisplayClass"
        # AND the flag is not kHotpatchKeepNative (so they actually crash when dispatched)
        entry_body = m.group(1)

        # The comment contains the subject name
        if '+<>c' in comment or 'DisplayClass' in comment or 'AnonymousType' in comment:
            unreachable_indices.add(idx)

    return unreachable_indices


def generate_skip_header(indices, chunk_name=""):
    """Generate fact_skip_indices.h content."""
    if not indices:
        return None

    sorted_indices = sorted(indices)
    lines = [
        "// Auto-generated fact skip list for AOT-unreachable stub methods.",
        "// Methods marked as AOT-unreachable (CHAOS_IL2CPP_FAIL) during codegen",
        f"// cause fact dispatch to crash. Chunk: {chunk_name}",
        "//",
        "#pragma once",
        "// NOLINTNEXTLINE",
        f"static inline bool IsFactMethodSkipped(int index) noexcept {{",
    ]

    # Group indices into efficient range checks
    # Find contiguous ranges
    ranges = []
    start = sorted_indices[0]
    end = start
    for i in sorted_indices[1:]:
        if i == end + 1:
            end = i
        else:
            ranges.append((start, end))
            start = i
            end = i
    ranges.append((start, end))

    for r_start, r_end in ranges:
        if r_start == r_end:
            lines.append(f"    if (index == {r_start}) return true;")
        else:
            lines.append(f"    if (index >= {r_start} && index <= {r_end}) return true;")

    lines.append("    return false;")
    lines.append("}")
    lines.append("")
    return "\n".join(lines)


def main():
    chunks_with_issues = []

    for entry_exe in sorted(FD.glob("*/chunks/*/native/entry.exe")):
        native_dir = entry_exe.parent
        assembly = native_dir.parent.parent.parent.name
        chunk_name = native_dir.parent.name
        key = f"{assembly}/{chunk_name}"

        # Check if fact_skip_indices.h already exists
        skip_h = native_dir / "fact_skip_indices.h"
        if skip_h.exists():
            print(f"[SKIP] {key} — fact_skip_indices.h already exists")
            continue

        subjects_cpp = native_dir / "subjects" / "native-aot.generated.cpp"
        if not subjects_cpp.exists():
            subjects_cpp = native_dir / "codegen" / "generated" / "native-aot.generated.cpp"
        if not subjects_cpp.exists():
            continue

        indices = find_aot_unreachable_hotpatch_indices(subjects_cpp)
        if indices:
            print(f"[FOUND] {key}: {len(indices)} AOT-unreachable stubs")
            header = generate_skip_header(indices, key)
            skip_h.write_text(header)
            print(f"  Wrote {skip_h}")
            chunks_with_issues.append(key)
        else:
            print(f"[OK] {key} — no AOT-unreachable stubs")

    print(f"\n=== Summary: {len(chunks_with_issues)} chunks got fact_skip_indices.h ===")
    for key in chunks_with_issues:
        print(f"  {key}")


if __name__ == "__main__":
    main()
