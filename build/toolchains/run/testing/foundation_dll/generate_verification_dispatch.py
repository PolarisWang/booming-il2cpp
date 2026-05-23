"""Generate verification_dispatch.generated.cpp from methods-manifest.json.

Reads the methods-manifest.json emitted by codegen (NativeAotLoweringPlanner)
and produces verification_dispatch.generated.cpp containing:

  - kSubjectEntryIndices[]   — subject→kAotMethod index mapping
  - RunFactAll()             — all-methods fact loop (uses RunNativeAot)
  - RunBenchmark(idx, iters) — timing loop via RunNativeAot
  - RunHotpatchAll()         — all-methods loop via RunNativeAot
  - RunHotpatchBenchmark()   — timing loop via RunNativeAot

All functions delegate to RunNativeAot (defined in native-aot.generated.cpp as
extern "C"), avoiding direct dependency on static symbols (kAotMethods[],
s_hotpatch_entries) that are file-scoped in the codegen output.

The generated file is compiled into entry.exe for use by the family
verification orchestrator stages.
"""

from __future__ import annotations

import json
import os
import re
from pathlib import Path
from typing import Any


def _cpp_string_literal(s: str) -> str:
    """Escape a string for use as a C++ string literal."""
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') + '"'


def generate_verification_dispatch(
    manifest_path: str | Path,
    output_path: str | Path,
) -> None:
    """Generate verification_dispatch.generated.cpp from manifest JSON.

    Args:
        manifest_path: Path to methods-manifest.json emitted by codegen.
        output_path: Output path for verification_dispatch.generated.cpp.
    """
    manifest_path = Path(manifest_path)
    output_path = Path(output_path)

    if not manifest_path.exists():
        print(f"  [generate_dispatch] manifest not found: {manifest_path}")
        return

    with open(manifest_path, encoding="utf-8") as f:
        manifest = json.load(f)

    methods: list[dict[str, Any]] = manifest.get("methods", [])
    if not methods:
        print(f"  [generate_dispatch] empty manifest, nothing to generate")
        return

    assembly_name: str = manifest.get("assemblyName", "Unknown")
    method_count: int = manifest.get("methodCount", len(methods))

    lines: list[str] = []

    def write(line: str = "") -> None:
        lines.append(line)

    # ── File header ─────────────────────────────────────────────────
    write("// verification_dispatch.generated.cpp — auto-generated")
    write(f"// Assembly: {assembly_name}, Methods: {method_count}")
    write('#include <cstdint>')
    write('#include <chrono>')
    write('#include <chaos/native_types.h>')
    write('')

    # Extern declarations (provided by native-aot.generated.cpp)
    write('extern "C" const int kAotMethodCount;')
    write('extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32);')
    write('')

    # ── Build subject entry indices ─────────────────────────────────
    # Extract subjectIndex from each method and build kSubjectEntryIndices[]
    subject_indices: list[int] = []
    for m in methods:
        si = m.get("subjectIndex", -1)
        if si >= 0:
            method_idx = m["index"]
            while len(subject_indices) <= si:
                subject_indices.append(-1)
            subject_indices[si] = method_idx

    # Fill trailing -1 entries with identity mapping
    for i in range(len(subject_indices)):
        if subject_indices[i] < 0:
            subject_indices[i] = i

    if subject_indices:
        # NOTE: kSubjectEntryCount / kSubjectEntryIndices are defined in
        # native-aot.generated.cpp (with actual values).  We declare them
        # extern here to avoid multiply-defined-symbol linker errors when
        # both TUs are compiled into entry.exe.
        write(f'extern "C" const int kSubjectEntryCount;')
        write(f'extern "C" const int kSubjectEntryIndices[];')
        write('// (defined in native-aot.generated.cpp)')
        write('')
        write('')

    # ── RunFactAll ──────────────────────────────────────────────────
    # Uses RunNativeAot for each subject entry, catching exceptions.
    # Iterates kSubjectEntryIndices (subject methods only) rather than
    # all kAotMethodCount methods, to avoid false failures from
    # interpreter-unsupported EH patterns (fault/filter/nested-catch).
    write("// ── RunFactAll: run every subject entry via RunNativeAot, return failure count ──")
    write('extern "C" CHAOS_IL2CPP_INT32 RunFactAll() {')
    write('    int failed_count = 0;')
    write('    for (int si = 0; si < kSubjectEntryCount; si++) {')
    write('        int i = kSubjectEntryIndices[si];')
    write('        try {')
    write('            RunNativeAot(i);')
    write('        } catch (...) {')
    write('            ++failed_count;')
    write('        }')
    write('    }')
    write('    return failed_count;')
    write('}')
    write('')

    # ── RunBenchmark ────────────────────────────────────────────────
    # Uses RunNativeAot in the timing loop.  This measures the native path
    # when no hotpatch is active, or the interpreter path after a hotpatch.
    # RunNativeAot handles all hotpatch routing internally.
    write("// ── RunBenchmark: timing loop via RunNativeAot ───────────────────")
    write('extern "C" double RunBenchmark(int entry_index, int iterations) {')
    write('    if (entry_index < 0 || entry_index >= kAotMethodCount)')
    write('        return -1.0;')
    write('    auto start = std::chrono::steady_clock::now();')
    write('    for (int i = 0; i < iterations; i++) {')
    write('        RunNativeAot(entry_index);')
    write('    }')
    write('    auto end = std::chrono::steady_clock::now();')
    write('    return std::chrono::duration<double, std::milli>(end - start).count();')
    write('}')
    write('')

    # ── RunHotpatchAll ──────────────────────────────────────────────
    # Same as RunFactAll but semantically "after hotpatch".  RunNativeAot
    # checks the hotpatch dispatch table internally.  Iterates subject
    # entries only (same reason as RunFactAll).
    write("// ── RunHotpatchAll: all-subject-entries loop via RunNativeAot (post-patch) ──")
    write('extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() {')
    write('    int failed_count = 0;')
    write('    for (int si = 0; si < kSubjectEntryCount; si++) {')
    write('        int i = kSubjectEntryIndices[si];')
    write('        try {')
    write('            RunNativeAot(i);')
    write('        } catch (...) {')
    write('            ++failed_count;')
    write('        }')
    write('    }')
    write('    return failed_count;')
    write('}')
    write('')

    # ── RunHotpatchBenchmark ────────────────────────────────────────
    # Timing loop via RunNativeAot, semantically for post-hotpatch measurement.
    write("// ── RunHotpatchBenchmark: timing loop via RunNativeAot (post-patch) ──")
    write('extern "C" double RunHotpatchBenchmark(int entry_index, int iterations) {')
    write('    if (entry_index < 0 || entry_index >= kAotMethodCount)')
    write('        return -1.0;')
    write('    auto start = std::chrono::steady_clock::now();')
    write('    for (int i = 0; i < iterations; i++) {')
    write('        RunNativeAot(entry_index);')
    write('    }')
    write('    auto end = std::chrono::steady_clock::now();')
    write('    return std::chrono::duration<double, std::milli>(end - start).count();')
    write('}')

    # ── Write output ────────────────────────────────────────────────
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"  [generate_dispatch] wrote {len(methods)} methods -> {output_path}")


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Generate verification dispatch C++")
    parser.add_argument("manifest_path", help="Path to methods-manifest.json")
    parser.add_argument("output_path", help="Output path for verification_dispatch.generated.cpp")
    args = parser.parse_args()
    generate_verification_dispatch(args.manifest_path, args.output_path)


if __name__ == "__main__":
    main()
