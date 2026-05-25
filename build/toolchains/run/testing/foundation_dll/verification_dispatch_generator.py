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
    jit_mode: bool | None = None,
) -> None:
    """Generate verification_dispatch.generated.cpp from manifest JSON.

    Args:
        manifest_path: Path to methods-manifest.json emitted by codegen.
        output_path: Output path for verification_dispatch.generated.cpp.
        jit_mode: If set, overrides the JIT mode detection from manifest.
                  True=JIT dispatch (nullptr thunks), False=AOT dispatch.
                  None=auto-detect from manifest's codegenMode field.
    """
    manifest_path = Path(manifest_path)
    output_path = Path(output_path)

    if not manifest_path.exists():
        print(f"  [generate_dispatch] manifest not found: {manifest_path}")
        return

    with open(manifest_path, encoding="utf-8-sig") as f:
        manifest = json.load(f)

    methods: list[dict[str, Any]] = manifest.get("methods", [])
    if not methods:
        print(f"  [generate_dispatch] empty manifest, nothing to generate")
        return

    assembly_name: str = manifest.get("assemblyName", "Unknown")
    method_count: int = manifest.get("methodCount", len(methods))
    if jit_mode is not None:
        is_jit = jit_mode
    else:
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
    write('#include <chaos/native_types.h>')
    write('#include <runtime_core.h>')
    write('#include <chaos/eh.h>')
    write('#include <chaos/hotpatch_dispatch.h>')
    write('')
    write('using chaos::il2cpp::runtime_core::ChaosDispatchMethod;')
    write('using chaos::il2cpp::runtime_core::ChaosDispatchMethodAllModules;')
    write('')

    # Extern declarations (defined in native-aot.generated.cpp and hotpatch-table.generated.cpp)
    write('extern "C" const int kAotMethodCount;')
    write('extern "C" const int kSubjectEntryCount;')
    write('extern "C" const int kSubjectSlotMap[];')
    write('// kSubjectEntryCount/kSubjectSlotMap defined in native-aot.generated.cpp (DispatchEntryCode template)')
    write('')
    write('// kDefaultArgThunks defined in native-aot.generated.cpp')
    write('extern "C" void (*kDefaultArgThunks[])() noexcept;')
    write('')
    # GetHotpatchEntries/GetHotpatchEntryCount: extern "C" defined in hotpatch-table.generated.cpp
    write('extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept;')
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
    # Uses ChaosDispatchMethodAllModules to dispatch ALL methods across
    # ALL registered modules (not just the entry module's subjects).
    # Each module's entries are dispatched individually with EH try/catch
    # inside the runtime function.  This provides full multi-assembly
    # coverage for the verification pipeline.
    # Both AOT and JIT modes: AOT uses kDefaultArgThunks for native dispatch;
    # JIT mode uses nullptr (direct_ptr via hotpatch entry) which works because
    # FillExternalRuntimeStubs() fills all kChaosExternalRuntimeFnTable entries
    # with safe stubs during bootstrap.
    if is_jit:
        fact_thunks = 'nullptr'
    else:
        fact_thunks = 'kDefaultArgThunks'

    write("// ── RunFactAll: dispatch ALL methods across ALL registered modules ────")
    write("// Uses ChaosDispatchMethodAllModules from hotpatch_dispatch.h which iterates")
    write("// every module in HotpatchNameRegistry.  Returns total failure count.")
    write(f'extern "C" CHAOS_IL2CPP_INT32 RunFactAll() {{')
    write(f'    return ChaosDispatchMethodAllModules({fact_thunks});')
    write('}')
    write('')

    # ── RunBenchmark ────────────────────────────────────────────────
    # Uses ChaosDispatchMethod in the timing loop with kDefaultArgThunks
    # so the actual function body is executed regardless of JIT/AOT mode.
    # (JIT nullptr mode is only valid via direct_ptr, which doesn't work
    # when the file is compiled in AOT mode.)
    write("// ── RunBenchmark: timing loop via ChaosDispatchMethod ───────────────────")
    write('extern "C" double RunBenchmark(int entry_index, int iterations) {')
    write('    if (entry_index < 0 || entry_index >= kAotMethodCount)')
    write('        return -1.0;')
    write('    auto* entries = GetHotpatchEntries();')
    write('    auto start = std::chrono::steady_clock::now();')
    write('    for (int i = 0; i < iterations; i++) {')
    write('        ChaosDispatchMethod(entries, kAotMethodCount, entry_index, kDefaultArgThunks);')
    write('    }')
    write('    auto end = std::chrono::steady_clock::now();')
    write('    return std::chrono::duration<double, std::milli>(end - start).count();')
    write('}')
    write('')

    # ── RunHotpatchAll ──────────────────────────────────────────────
    # Same as RunFactAll but semantically "after hotpatch".
    # Uses ChaosDispatchMethodAllModules to cover ALL registered modules.
    # ChaosDispatchMethod handles all hotpatch routing internally.
    write("// ── RunHotpatchAll: dispatch ALL methods across ALL modules (post-patch) ──")
    write(f'extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() {{')
    write(f'    return ChaosDispatchMethodAllModules({fact_thunks});')
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
    write(f'        ChaosDispatchMethod(entries, kAotMethodCount, entry_index, kDefaultArgThunks);')
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
    parser.add_argument("--jit-mode", action="store_true", default=None,
                        help="Force JIT dispatch mode (nullptr thunks)")
    parser.add_argument("--aot-mode", action="store_true", default=None,
                        help="Force AOT dispatch mode (kDefaultArgThunks thunks)")
    args = parser.parse_args()
    jit_mode = None
    if args.jit_mode:
        jit_mode = True
    elif args.aot_mode:
        jit_mode = False
    generate_verification_dispatch(args.manifest_path, args.output_path, jit_mode=jit_mode)


if __name__ == "__main__":
    main()
