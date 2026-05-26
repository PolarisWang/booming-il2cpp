"""Generate verification_dispatch.generated.cpp from methods-manifest.json.

Reads the methods-manifest.json emitted by codegen (NativeAotLoweringPlanner)
and produces verification_dispatch.generated.cpp containing:

  - RunFactAll()            — all-methods fact loop via ChaosDispatchMethod
  - RunBenchmark(idx, iters)— timing loop via ChaosDispatchMethod
  - RunHotpatchAll()        — post-patch all-methods loop via ChaosDispatchMethod
  - RunHotpatchBenchmark()  — post-patch timing loop via ChaosDispatchMethod

All functions delegate to ChaosDispatchMethod (from <chaos/hotpatch_dispatch.h>)
and GetHotpatchEntries() (from the hotpatch-table generated TU).

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
    if is_jit:
        write('// SEH support for JIT-mode dispatch (catches access violations from JIT code)')
        write('#include <excpt.h>')
        write('')
    write('')
    write('using chaos::il2cpp::runtime_core::ChaosDispatchMethod;')
    write('using chaos::il2cpp::runtime_core::ChaosDispatchMethodAllModules;')
    write('using chaos::il2cpp::runtime_core::chaos_gc_get_allocated_bytes_for_current_thread;')
    write('')

    # Extern declarations (defined in native-aot.generated.cpp and hotpatch-table.generated.cpp)
    write('extern "C" const int kAotMethodCount;')
    write('extern "C" const int kSubjectEntryCount;')
    write('extern "C" const int kSubjectSlotMap[];')
    write('// kSubjectEntryCount/kSubjectSlotMap defined in native-aot.generated.cpp (DispatchEntryCode template)')
    write('')

    # kDefaultArgThunks defined in native-aot.generated.cpp (AOT mode only)
    if not is_jit:
        write('// kDefaultArgThunks defined in native-aot.generated.cpp')
        write('extern "C" void (*kDefaultArgThunks[])() noexcept;')
        write('')

    write('// In JIT mode, benchmark functions use nullptr instead of kDefaultArgThunks')
    write('// because the JIT DispatchEntryCode template skips kDefaultArgThunks emission.')
    write('// ChaosDispatchMethod accepts nullptr thunks and calls direct_ptr from')
    write('// hotpatch entries (SetDirectDispatch has already been called by RegisterJitEntryMethods).')
    write('')
    write('extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept;')
    write('extern "C" int32_t GetHotpatchEntryCount() noexcept;')
    write('')

    # ── Build subject slot map ──────────────────────────────────────
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
    thunks = 'nullptr' if is_jit else 'kDefaultArgThunks'
    if is_jit:
        write("// ── RunFactAll: dispatch subject entries only (JIT mode) ──")
        write("// Uses kSubjectSlotMap + kSubjectEntryCount to skip runtime")
        write("// intrinsic stubs that may crash via JIT direct_ptr.")
        write(f'extern "C" CHAOS_IL2CPP_INT32 RunFactAll() {{')
        write(f'    auto* entries = GetHotpatchEntries();')
        write(f'    int32_t failures = 0;')
        write(f'    for (int32_t si = 0; si < kSubjectEntryCount; si++) {{')
        write(f'        int32_t i = kSubjectSlotMap[si];')
        write(f'        try {{')
        write(f'            ChaosDispatchMethod(entries, kAotMethodCount, i, {thunks});')
        write(f'        }} catch(...) {{')
        write(f'            ++failures;')
        write(f'        }}')
        write(f'    }}')
        write(f'    return failures;')
        write('}')
    else:
        write("// ── RunFactAll: dispatch ALL methods across ALL registered modules ────")
        write("// Uses ChaosDispatchMethodAllModules from hotpatch_dispatch.h which iterates")
        write("// every module in HotpatchNameRegistry.  Returns total failure count.")
        write(f'extern "C" CHAOS_IL2CPP_INT32 RunFactAll() {{')
        write(f'    return ChaosDispatchMethodAllModules({thunks});')
        write('}')
    write('')

    # ── RunBenchmark ────────────────────────────────────────────────
    write("// ── RunBenchmark: timing loop via ChaosDispatchMethod ───────────────────")
    write('struct BenchmarkResult {')
    write('    double elapsed_ms;')
    write('    int64_t allocated_bytes;')
    write('};')
    write('')
    write('extern "C" BenchmarkResult RunBenchmark(int entry_index, int iterations) {')
    write('    if (entry_index < 0 || entry_index >= kAotMethodCount)')
    write('        return {-1.0, 0};')
    write('    auto* entries = GetHotpatchEntries();')
    write('    auto alloc_before = chaos_gc_get_allocated_bytes_for_current_thread();')
    write('    auto start = std::chrono::steady_clock::now();')
    write('    for (int i = 0; i < iterations; i++) {')
    write(f'        ChaosDispatchMethod(entries, kAotMethodCount, entry_index, {thunks});')
    write('    }')
    write('    auto end = std::chrono::steady_clock::now();')
    write('    auto alloc_after = chaos_gc_get_allocated_bytes_for_current_thread();')
    write('    BenchmarkResult result;')
    write('    result.elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();')
    write('    result.allocated_bytes = alloc_after - alloc_before;')
    write('    return result;')
    write('}')
    write('')

    # ── RunHotpatchAll and RunHotpatchBenchmark ───────────────────
    if is_jit:
        write("// ── RunHotpatchAll: dispatch subject entries only (JIT mode) ──")
        write("// Uses kSubjectSlotMap + kSubjectEntryCount matching hotupdate")
        write("// pattern. Avoids intrinsic stubs that crash via JIT direct_ptr.")
        write(f'extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() {{')
        write(f'    auto* entries = GetHotpatchEntries();')
        write(f'    int32_t failures = 0;')
        write(f'    for (int32_t si = 0; si < kSubjectEntryCount; si++) {{')
        write(f'        int32_t i = kSubjectSlotMap[si];')
        write(f'        try {{')
        write(f'            ChaosDispatchMethod(entries, kAotMethodCount, i, {thunks});')
        write(f'        }} catch(...) {{')
        write(f'            ++failures;')
        write(f'        }}')
        write(f'    }}')
        write(f'    return failures;')
        write('}')
    else:
        write("// ── RunHotpatchAll: dispatch ALL methods across ALL modules (post-patch) ──")
        write("// Uses ChaosDispatchMethodAllModules, same thunk semantics as RunFactAll.")
        write(f'extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() {{')
        write(f'    return ChaosDispatchMethodAllModules({thunks});')
        write('}')
    write('')

    # ── RunHotpatchBenchmark ──────────────────────────────────────
    write("// ── RunHotpatchBenchmark: timing loop via ChaosDispatchMethod (post-patch) ──")
    write('extern "C" BenchmarkResult RunHotpatchBenchmark(int entry_index, int iterations) {')
    write('    if (entry_index < 0 || entry_index >= kAotMethodCount)')
    write('        return {-1.0, 0};')
    write('    auto* entries = GetHotpatchEntries();')
    write('    auto alloc_before = chaos_gc_get_allocated_bytes_for_current_thread();')
    write('    auto start = std::chrono::steady_clock::now();')
    write('    for (int i = 0; i < iterations; i++) {')
    write(f'        ChaosDispatchMethod(entries, kAotMethodCount, entry_index, {thunks});')
    write('    }')
    write('    auto end = std::chrono::steady_clock::now();')
    write('    auto alloc_after = chaos_gc_get_allocated_bytes_for_current_thread();')
    write('    BenchmarkResult result;')
    write('    result.elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();')
    write('    result.allocated_bytes = alloc_after - alloc_before;')
    write('    return result;')
    write('}')

    # ── Write output ────────────────────────────────────────────────
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"  [generate_dispatch] wrote {len(methods)} methods -> {output_path}")
