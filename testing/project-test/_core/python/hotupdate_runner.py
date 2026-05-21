"""HotUpdate stage runners — patch lifecycle and behavior-change verification.

Two strategies:
  1. emit-patch-data (default): Build patch DLL, emit-patch-data → .patchdata,
     generate runtime-patchdata.cpp, rebuild entry.exe, run --hotupdate
  2. source-patch (fallback): Modify managed source in-place, rebuild+re-codegen

The emit-patch-data strategy exercises the real runtime hotpatch mechanism
(--hotupdate flag, ApplyPatchFromMemory, HotpatchNameRegistry lookup, etc.),
while source-patch bypasses it by baking patches into generated C++ code.

Flow (emit-patch-data):
  1. Build patch DLL (patch/src/) containing ArithmeticProof with a+b → a*b
  2. chaos-il2cpp emit-patch-data → .patchdata binary
  3. Generate native/runtime-patchdata.cpp from .patchdata (byte array)
  4. Rebuild entry.exe (only runtime-patchdata.cpp changed, codegen is reused)
  5. Run entry.exe --hotupdate
  6. Compare stdout with expected/fact-patched-output.txt
  7. Restore sentinel runtime-patchdata.cpp (kPatchDataSize=0)
"""

from __future__ import annotations

import os
import shutil
import subprocess
import time
from pathlib import Path
from typing import Any

from _pipeline.stage_result import StageResult

# ── Path setup ────────────────────────────────────────────
_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[3]
_OLD_BOOTSTRAP = _REPO_ROOT / "build" / "toolchains" / "run"
_OLD_PIPELINE = _OLD_BOOTSTRAP / "testing" / "foundation_dll"

import sys as _sys
if str(_OLD_BOOTSTRAP) not in _sys.path:
    _sys.path.insert(0, str(_OLD_BOOTSTRAP))
if str(_OLD_PIPELINE) not in _sys.path:
    _sys.path.insert(0, str(_OLD_PIPELINE))

from pipeline_native_aot_runner import _build_entry_exe   # noqa: E402
from _core.python.project_pipeline_adapter import (                    # noqa: E402
    _build_patch_dll,
    _run_emit_patch_data,
    _generate_runtime_patchdata_cpp,
    _write_sentinel_patchdata_for_project,
    _project_base,
)


def _compare_output(actual: str, expected_path: Path) -> tuple[bool, list[str]]:
    """Compare actual stdout lines with expected output."""
    if not expected_path.exists():
        return False, ["expected output file not found"]

    expected_lines = expected_path.read_text(encoding="utf-8").strip().splitlines()
    actual_lines = [l for l in actual.splitlines()
                    if l.startswith("RTS_")]

    mismatches = []
    for i, (exp, act) in enumerate(zip(expected_lines, actual_lines)):
        if exp != act:
            mismatches.append(f"line {i}: expected={exp!r}, actual={act!r}")

    if len(actual_lines) != len(expected_lines):
        mismatches.append(
            f"line count: expected {len(expected_lines)}, got {len(actual_lines)}")

    return len(mismatches) == 0, mismatches


# ── Stage runner ──────────────────────────────────────────

def run_project_hotupdate(ctx, stages) -> StageResult:
    """Stage 6: HotUpdate AOT — emit-patch-data + --hotupdate.

    Uses the real runtime hotpatch mechanism:
      1. Build patch DLL
      2. emit-patch-data → .patchdata binary
      3. Generate runtime-patchdata.cpp with embedded binary
      4. Rebuild entry.exe (links new patchdata)
      5. Run entry.exe --hotupdate
      6. Verify changed output
    """
    start = time.perf_counter()
    project_base = _project_base(ctx)

    # ── Step 1: Build patch DLL ──
    print(f"  [hotupdate] Building patch DLL...")
    patch_dll = _build_patch_dll(ctx)
    if patch_dll is None:
        return StageResult(
            stage="hotupdate", status="failed",
            summary="Patch DLL build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── Step 2: Run emit-patch-data on patch DLL ──
    print(f"  [hotupdate] Running emit-patch-data...")
    patchdata_dir = project_base / ctx.slug / "codegen" / "patchdata"
    patchdata_dir.mkdir(parents=True, exist_ok=True)
    patchdata_path = patchdata_dir / f"{ctx.slug}.patchdata"

    # Try to find aot-core-ir.json for pre-lowered IR embedding
    codegen_dir = project_base / ctx.slug / "codegen"
    aot_core_ir = None
    for d in sorted(codegen_dir.iterdir()):
        candidate = d / "generated" / "aot-core-ir.json"
        if candidate.exists():
            aot_core_ir = str(candidate)
            break

    ok = _run_emit_patch_data(
        str(patch_dll),
        str(patchdata_path),
        aot_core_ir_path=aot_core_ir,
    )
    if not ok or not patchdata_path.exists():
        return StageResult(
            stage="hotupdate", status="failed",
            summary="emit-patch-data failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── Step 3: Generate runtime-patchdata.cpp ──
    print(f"  [hotupdate] Generating runtime-patchdata.cpp...")
    native_dir = project_base / ctx.slug / "native"
    patchdata_cpp = native_dir / "runtime-patchdata.cpp"
    host_class_name = "ArithmeticProof"  # matches AOT type name

    _generate_runtime_patchdata_cpp(patchdata_path, patchdata_cpp, host_class_name)

    try:
        # ── Step 4: Rebuild entry.exe (only runtime-patchdata.cpp changed) ──
        print(f"  [hotupdate] Rebuilding entry.exe with patchdata...")
        build_ok = _build_entry_exe(ctx.slug, verification=project_base)
        if not build_ok:
            return StageResult(
                stage="hotupdate", status="failed",
                summary="entry.exe rebuild with patchdata failed",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        # ── Step 5: Run entry.exe --hotupdate ──
        print(f"  [hotupdate] Running entry.exe --hotupdate...")
        try:
            r = subprocess.run(
                [str(ctx.native_dir / "entry.exe"), "--hotupdate"],
                capture_output=True, text=True, timeout=30,
            )
        except subprocess.TimeoutExpired:
            return StageResult(
                stage="hotupdate", status="failed",
                summary="entry.exe --hotupdate timed out",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        # ── Step 6: Compare output ──
        expected_path = ctx.expected_dir / "fact-patched-output.txt"
        match, mismatches = _compare_output(r.stdout, expected_path)

        if not match:
            details: dict[str, Any] = {
                "exitCode": r.returncode,
                "stdout": r.stdout.strip()[-2000:],
                "mismatches": mismatches,
            }
            return StageResult(
                stage="hotupdate", status="failed",
                summary="patched output mismatch",
                details=details,
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        return StageResult(
            stage="hotupdate", status="passed",
            summary="hotupdate OK: emit-patch-data + --hotupdate",
            details={"exitCode": r.returncode},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    finally:
        # Step 7: Always restore sentinel runtime-patchdata.cpp
        _write_sentinel_patchdata_for_project(project_base, ctx.slug)
        # Clean up patchdata directory
        if patchdata_dir.exists():
            shutil.rmtree(str(patchdata_dir))


def run_project_hotupdate_jit(ctx, stages) -> StageResult:
    """Stage 7: HotUpdate JIT — source-patch + JIT codegen + verify.

    Same source-patch approach as AOT hotupdate, but uses JIT codegen
    (codegen_mode="jit") so all methods execute through the interpreter.
    Validates that JIT execution also produces the patched output.
    """
    start = time.perf_counter()

    jit_exe = ctx.native_dir / "entry-jit.exe"
    if not jit_exe.exists():
        return StageResult(
            stage="hotupdate_jit", status="skipped",
            summary="entry-jit.exe not found (JIT codegen skipped or failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    project_base = _project_base(ctx)

    # ── Step 1: Locate and patch source file ──
    src_file = ctx.managed_dir / "ArithmeticProof.cs"
    if not src_file.exists():
        return StageResult(
            stage="hotupdate_jit", status="skipped",
            summary="No ArithmeticProof.cs — project does not support hotupdate source-patch",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Read original content to restore later
    original_content = src_file.read_text(encoding="utf-8")

    # Apply the patch: change Add from a+b to a*b
    patched_content = original_content.replace("return a + b;", "return a * b;")
    if patched_content == original_content:
        return StageResult(
            stage="hotupdate_jit", status="failed",
            summary="Could not apply patch to ArithmeticProof.cs",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [hotupdate_jit] Applying source patch to ArithmeticProof.cs...")

    try:
        src_file.write_text(patched_content, encoding="utf-8")

        # ── Step 2: Rebuild managed project with patched source ──
        print(f"  [hotupdate_jit] Rebuilding managed project (patched)...")
        build_out = ctx.managed_dir / "bin" / "Release"
        if build_out.exists():
            shutil.rmtree(str(build_out))

        # Rebuild via existing codegen pipeline import
        from _core.python.project_pipeline_adapter import _build_managed_project  # noqa
        dll_path = _build_managed_project(ctx)
        if dll_path is None:
            return StageResult(
                stage="hotupdate_jit", status="failed",
                summary="Patched managed build failed",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        # ── Step 3: Run JIT codegen on patched DLL ──
        print(f"  [hotupdate_jit] Running JIT codegen on patched DLL...")
        from _core.python.project_pipeline_adapter import (
            _patch_console_writeline,
            _codegen_patch_undefined_labels,
            _run_convert_to_cpp,
            _find_entry_method_index,
            _generate_project_dispatch_code,
            _write_project_runtime_entry,
            _write_project_cmakelists,
            _save_jit_binary,
        )

        clean_input_dir = project_base / ctx.slug / "codegen" / "_jit_patch_input"
        clean_input_dir.mkdir(parents=True, exist_ok=True)
        clean_dll = clean_input_dir / dll_path.name
        shutil.copy2(str(dll_path), str(clean_dll))

        cg_ok = _run_convert_to_cpp(
            ctx.slug,
            str(clean_dll),
            verification=project_base,
            entry_point_subject_id="",
            codegen_mode="jit",
        )
        if not cg_ok:
            return StageResult(
                stage="hotupdate_jit", status="failed",
                summary="Patched JIT codegen failed",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        # ── Step 4: Patch Console.WriteLine + undefined labels ──
        _patch_console_writeline(project_base, ctx.slug)
        _codegen_patch_undefined_labels(ctx.slug, verification=project_base)

        # ── Step 5: Generate dispatch code ──
        entry_index = _find_entry_method_index(ctx, project_base)
        _generate_project_dispatch_code(project_base, ctx.slug)

        # ── Step 6: Write runtime entry + CMakeLists ──
        _write_project_runtime_entry(project_base, ctx.slug, entry_index)
        _write_project_cmakelists(project_base, ctx.slug)

        # ── Step 7: Build entry-jit.exe ──
        print(f"  [hotupdate_jit] Building patched entry-jit.exe...")
        build_ok = _build_entry_exe(ctx.slug, verification=project_base)
        if not build_ok:
            return StageResult(
                stage="hotupdate_jit", status="failed",
                summary="Patched entry-jit.exe build failed",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        # ── Step 8: Save as entry-jit.exe ──
        _save_jit_binary(ctx)

        # ── Step 9: Run patched entry-jit.exe ──
        print(f"  [hotupdate_jit] Running patched entry-jit.exe...")
        try:
            r = subprocess.run(
                [str(ctx.native_dir / "entry-jit.exe")],
                capture_output=True, text=True, timeout=30,
            )
        except subprocess.TimeoutExpired:
            return StageResult(
                stage="hotupdate_jit", status="failed",
                summary="Patched entry-jit.exe timed out",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        # ── Step 10: Compare output ──
        expected_path = ctx.expected_dir / "fact-patched-output.txt"
        actual_lines = [l for l in (r.stdout or "").splitlines() if l.startswith("RTS_")]
        actual_output = "\n".join(actual_lines)
        match, mismatches = _compare_output(actual_output, expected_path)

        if not match:
            details: dict[str, Any] = {
                "exitCode": r.returncode,
                "stdout": r.stdout.strip()[-2000:],
                "mismatches": mismatches,
            }
            return StageResult(
                stage="hotupdate_jit", status="failed",
                summary="patched JIT output mismatch",
                details=details,
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        return StageResult(
            stage="hotupdate_jit", status="passed",
            summary="hotupdate JIT OK: RTS_ARITH_ADD 5→6 verified",
            details={"exitCode": r.returncode},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    finally:
        src_file.write_text(original_content, encoding="utf-8")
        jit_patch_input = project_base / ctx.slug / "codegen" / "_jit_patch_input"
        if jit_patch_input.exists():
            shutil.rmtree(str(jit_patch_input))
