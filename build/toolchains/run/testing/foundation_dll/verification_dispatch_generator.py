"""Generate verification_dispatch.generated.cpp from methods-manifest.json.

Reads the methods-manifest.json emitted by codegen (NativeAotLoweringPlanner)
and produces verification_dispatch.generated.cpp containing:

  - kSubjectSlotMap[]       — subject→kMethodTable index mapping (extern decl)
  - RunFactAll()            — all-methods fact loop via ChaosDispatchMethod
  - RunBenchmark(idx, iters)— timing loop via ChaosDispatchMethod
  - RunHotpatchAll()        — post-patch all-methods loop via ChaosDispatchMethod
  - RunHotpatchBenchmark()  — post-patch timing loop via ChaosDispatchMethod

All functions delegate to ChaosDispatchMethod (from <chaos/hotpatch_dispatch.h>)
and GetHotpatchEntries() (from the hotpatch-table generated TU), avoiding any
dependency on static symbols in the codegen output.

The generated file is compiled into entry.exe for use by the family
verification orchestrator stages.
"""

from __future__ import annotations

import json
import os
from pathlib import Path
from typing import Any


def _is_jit_mode_from_manifest(manifest: dict[str, Any]) -> bool:
    """Check manifest for JIT-related flags to determine mode."""
    mode = manifest.get("codegenMode", "")
    return mode == "jit"


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
    is_jit = _is_jit_mode_from_manifest(manifest)

    lines: list[str] = []

    def write(line: str = "") -> None:
        lines.append(line)

    # ── File header ─────────────────────────────────────────────────
    write("// verification_dispatch.generated.cpp — auto-generated")
    write(f"// Assembly: {assembly_name}, Methods: {method_count}, Mode: {'JIT' if is_jit else 'AOT'}")
    write('')
    write('#include <cstdint>')
    write('#include <chrono>')
    write('#include <chaos/eh.h>')
    write('#include <chaos/hotpatch_dispatch.h>')
    write('')

    # Extern declarations (defined in native-aot.generated.cpp and hotpatch-table.generated.cpp)
    write('extern "C" const int kAotMethodCount;')
    write('extern "C" const int kSubjectEntryCount;')
    write('extern "C" const int kSubjectSlotMap[];')
    write('// kSubjectEntryCount/kSubjectSlotMap defined in native-aot.generated.cpp (DispatchEntryCode template)')
    write('')
    if not is_jit:
        write('// kDefaultArgThunks defined in native-aot.generated.cpp (AOT-only)')
        write('extern "C" void (*kDefaultArgThunks[])();')
        write('')
    # GetHotpatchEntries/GetHotpatchEntryCount: extern "C" defined in hotpatch-table.generated.cpp
    write('extern "C" const chaos::il2cpp::runtime_core::HotpatchEntryV0* GetHotpatchEntries() noexcept;')
    write('extern "C" int32_t GetHotpatchEntryCount() noexcept;')
    write('')

    # ── Build subject slot map ──────────────────────────────────────
    # Extract subjectIndex from each method — this is a local build used
    # for debug output only. The actual kSubjectSlotMap is in native-aot.generated.cpp.
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
        write(f'// Subject entry count: {len(subject_indices)} (from manifest)')
        write('')
        write('')

    # ── RunFactAll ──────────────────────────────────────────────────
    # Uses ChaosDispatchMethod for each subject entry, catching exceptions.
    # Iterates kSubjectSlotMap (subject methods only) rather than all
    # kAotMethodCount methods, to avoid false failures from
    # interpreter-unsupported EH patterns (fault/filter/nested-catch).
    if not is_jit:
        fact_thunks = 'kDefaultArgThunks'
    else:
        fact_thunks = 'nullptr'

    write("// ── RunFactAll: run every subject entry via ChaosDispatchMethod, return failure count ──")
    write('extern "C" CHAOS_IL2CPP_INT32 RunFactAll() {')
    write('    int failed_count = 0;')
    write('    for (int si = 0; si < kSubjectEntryCount; si++) {')
    write('        int i = kSubjectSlotMap[si];')
    write('        CHAOS_EH_TRY')
    write(f'            ChaosDispatchMethod(GetHotpatchEntries(), kAotMethodCount, i, {fact_thunks});')
    write('        CHAOS_EH_CATCH_BEGIN')
    write('            ++failed_count;')
    write('        CHAOS_EH_END')
    write('    }')
    write('    return failed_count;')
    write('}')
    write('')

    # ── RunBenchmark ────────────────────────────────────────────────
    # Uses ChaosDispatchMethod in the timing loop.  This measures the native path
    # when no hotpatch is active, or the interpreter path after a hotpatch.
    # ChaosDispatchMethod handles all hotpatch routing internally.
    write("// ── RunBenchmark: timing loop via ChaosDispatchMethod ───────────────────")
    write('extern "C" double RunBenchmark(int entry_index, int iterations) {')
    write('    if (entry_index < 0 || entry_index >= kAotMethodCount)')
    write('        return -1.0;')
    write('    auto* entries = GetHotpatchEntries();')
    write('    auto start = std::chrono::steady_clock::now();')
    write('    for (int i = 0; i < iterations; i++) {')
    write(f'        ChaosDispatchMethod(entries, kAotMethodCount, entry_index, {fact_thunks});')
    write('    }')
    write('    auto end = std::chrono::steady_clock::now();')
    write('    return std::chrono::duration<double, std::milli>(end - start).count();')
    write('}')
    write('')

    # ── RunHotpatchAll ──────────────────────────────────────────────
    # Same as RunFactAll but semantically "after hotpatch".  ChaosDispatchMethod
    # checks the hotpatch dispatch table internally.  Iterates subject
    # entries only (same reason as RunFactAll).
    write("// ── RunHotpatchAll: all-subject-entries loop via ChaosDispatchMethod (post-patch) ──")
    write('extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() {')
    write('    int failed_count = 0;')
    write('    for (int si = 0; si < kSubjectEntryCount; si++) {')
    write('        int i = kSubjectSlotMap[si];')
    write('        CHAOS_EH_TRY')
    write(f'            ChaosDispatchMethod(GetHotpatchEntries(), kAotMethodCount, i, {fact_thunks});')
    write('        CHAOS_EH_CATCH_BEGIN')
    write('            ++failed_count;')
    write('        CHAOS_EH_END')
    write('    }')
    write('    return failed_count;')
    write('}')
    write('')

    # ── RunHotpatchBenchmark ────────────────────────────────────────
    # Timing loop via ChaosDispatchMethod, semantically for post-hotpatch measurement.
    write("// ── RunHotpatchBenchmark: timing loop via ChaosDispatchMethod (post-patch) ──")
    write('extern "C" double RunHotpatchBenchmark(int entry_index, int iterations) {')
    write('    if (entry_index < 0 || entry_index >= kAotMethodCount)')
    write('        return -1.0;')
    write('    auto* entries = GetHotpatchEntries();')
    write('    auto start = std::chrono::steady_clock::now();')
    write('    for (int i = 0; i < iterations; i++) {')
    write(f'        ChaosDispatchMethod(entries, kAotMethodCount, entry_index, {fact_thunks});')
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
