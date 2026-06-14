#!/usr/bin/env python3
"""Dynamically generate fact_skip_indices.h from actual entry.exe failures.

Usage:
    python3 _gen_skips_dynamic.py <chunk_native_dir>

Scans the chunk's entry.exe, runs it with --fact-json, collects failed
methodIndex values, and generates or updates fact_skip_indices.h.
Only works for chunks where entry.exe produces parseable JSON output.
"""
import json
import subprocess
import sys
from pathlib import Path


def generate_skip_header(indices, chunk_name=""):
    """Generate fact_skip_indices.h content from a set of method indices."""
    if not indices:
        return None

    sorted_indices = sorted(indices)
    lines = [
        "// Auto-generated fact skip list for AOT-unreachable stub methods.",
        "// Methods marked as AOT-unreachable (CHAOS_IL2CPP_FAIL) during codegen",
        f"// cause fact dispatch to crash. Chunk: {chunk_name}",
        "// GENERATED DYNAMICALLY from entry.exe --fact-json output",
        "//",
        "#pragma once",
        "// NOLINTNEXTLINE",
        "static inline bool IsFactMethodSkipped(int index) noexcept {",
    ]

    # Group indices into efficient contiguous ranges
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
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <chunk_native_dir>")
        sys.exit(1)

    native_dir = Path(sys.argv[1]).resolve()
    entry_exe = native_dir / "entry.exe"
    if not entry_exe.exists():
        print(f"ERROR: {entry_exe} not found")
        sys.exit(1)

    # Derive chunk name from path
    try:
        assembly = native_dir.parent.parent.parent.name
        chunk_name = native_dir.parent.name
        key = f"{assembly}/{chunk_name}"
    except:
        key = native_dir.name

    print(f"[INFO] Running entry.exe --fact-json for {key}...")

    try:
        result = subprocess.run(
            [str(entry_exe), "--fact-json"],
            capture_output=True, text=True, timeout=120
        )
        stdout = result.stdout
        stderr = result.stderr
    except subprocess.TimeoutExpired:
        print(f"[FAIL] {key}: entry.exe timed out (120s)")
        sys.exit(1)
    except Exception as e:
        print(f"[FAIL] {key}: {e}")
        sys.exit(1)

    # Find JSON in stdout (it's the last line before any log output)
    # The JSON starts with {"factResults": and ends with ]}
    json_start = stdout.find('{"factResults"')
    if json_start < 0:
        print(f"[FAIL] {key}: no JSON output found in stdout")
        print(f"  stdout preview: {stdout[:200]}")
        sys.exit(1)

    json_str = stdout[json_start:]

    # Check for truncated JSON
    if not json_str.strip().endswith(']}'):
        print(f"[WARN] {key}: JSON output is truncated (entry.exe crashed)")
        print(f"  Will generate skip list from partial results")
        # Try to find the last complete entry
        last_entry = json_str.rfind('{"si"')
        if last_entry < 0:
            print(f"[FAIL] {key}: no complete entries in truncated JSON")
            sys.exit(1)
        # Truncate to last complete entry
        json_str = json_str[:json_str.rfind('}')+1] + ']}'

    try:
        data = json.loads(json_str)
    except json.JSONDecodeError as e:
        print(f"[FAIL] {key}: JSON parse error: {e}")
        print(f"  Last 200 chars: {json_str[-200:]}")
        sys.exit(1)

    results = data.get('factResults', [])
    total = len(results)
    failed_indices = set()
    for r in results:
        if not r.get('passed', True):
            failed_indices.add(r['methodIndex'])

    # Also include AOT-unreachable closure stubs detected statically.
    # Only include indices that appear in the subject slot map (actual dispatched subjects).
    try:
        subjects_cpp = native_dir / "subjects" / "native-aot.generated.cpp"
        if not subjects_cpp.exists():
            subjects_cpp = native_dir / "codegen" / "generated" / "native-aot.generated.cpp"
        if subjects_cpp.exists():
            content = subjects_cpp.read_text()
            # Find all subject slot map entries — these are the dispatched method indices
            import re
            slot_map_match = re.search(r'int kSubjectSlotMap\[\d+\]\s*=\s*\{([^}]+)\}', content, re.DOTALL)
            if slot_map_match:
                slot_values = [int(x.strip()) for x in slot_map_match.group(1).split(',') if x.strip()]
                subject_indices = set(slot_values)
            else:
                subject_indices = set()

            # Find hotpatch entries with closure patterns
            start_marker = 'extern "C" HotpatchEntryV0 s_hotpatch_entries['
            start_idx = content.find(start_marker)
            if start_idx >= 0:
                brace_idx = content.find('{', start_idx)
                entries_text = content[brace_idx:]
                depth = 0
                end_idx = -1
                for i, c in enumerate(entries_text):
                    if c == '{': depth += 1
                    elif c == '}': depth -= 1
                    if depth == 0: end_idx = brace_idx + i + 1; break

                entry_pattern = r'\{([^}]+)\},\s*//\s*(.+)'
                matches = list(re.finditer(entry_pattern, content[brace_idx:end_idx]))
                for idx, m in enumerate(matches):
                    if idx in subject_indices:
                        comment = m.group(2)
                        if '+<>c' in comment or 'DisplayClass' in comment or 'AnonymousType' in comment:
                            failed_indices.add(idx)

        closure_added = len([i for i in failed_indices if i not in set(r.get('methodIndex', 0) for r in results)])
        if closure_added:
            print(f"  [INFO] Added {closure_added} closure indices from static analysis")
    except Exception as e:
        print(f"  [WARN] Static analysis failed: {e}")

    # Stats: only count failures from JSON results (not static analysis extras)
    json_failed = sum(1 for r in results if not r.get('passed', True))
    print(f"[INFO] {key}: P{total - json_failed}F{json_failed}T{total}")
    if failed_indices:
        print(f"[INFO] Failed methodIndices ({len(failed_indices)} unique): {sorted(failed_indices)}")

    if not failed_indices:
        print(f"[OK] {key}: no failures to skip")
        return

    header = generate_skip_header(failed_indices, key)
    skip_h = native_dir / "fact_skip_indices.h"
    skip_h.write_text(header)
    print(f"[DONE] Wrote {skip_h}")
    print(f"[INFO] Rebuild entry.exe with: cd {native_dir}/build && cmake .. && make -j$(nproc) && cp chaos_entry {native_dir}/entry.exe")


if __name__ == "__main__":
    main()
