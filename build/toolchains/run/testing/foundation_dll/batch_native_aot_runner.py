"""Batch-run the CodeGen pipeline across all CoreLib families.

For each family:
  1. Generate synthetic entry point C# source + project (via family_entrypoint_generator)
  2. Build the DLL
  3. Run chaos-il2cpp convert to produce aot-core-ir.json
  4. Trim aot-core-ir.json to entry-only methods
  5. Run chaos-il2cpp emit-native-aot to produce real C++

Results are written to a summary JSON file.
"""

from __future__ import annotations

import json
import os
import subprocess
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]  # D:/agent/booming-il2cpp
_VERIFICATION = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib"
sys.path.insert(0, str(_HERE))

from family_entrypoint_generator import generate_and_build

# Families to process (all 23 non-report CoreLib families)
FAMILIES = [
    "array-indexing-copy",
    "buffer-memory",
    "collections-generic-core",
    "convert-char",
    "enum-parsing",
    "generic-nullable-value",
    "guid-random-hashcode",
    "math-numerics",
    "object-equality-identity",
    "primitive-numeric-conversions",
    "string-char-text-core",
    "time-date-time-timespan",
    "threading-monitor-interlocked",
    "attributes-custom-metadata",
    "boxing-unboxing-casts",
    "delegate-core-invocation",
    "exception-throw-diagnostics",
    "globalization-culture",
    "io-streams-basics",
    "reflection-member-basics",
    "runtime-compiler-services",
    "span-memory-buffers",
    "type-runtime-handles",
    # New reflection families (Phase 1 — 8 families, 135 methods)
    "reflection-type",
    "reflection-assembly",
    "reflection-member-complete",
    "reflection-generics",
    "reflection-parameters",
    "reflection-module",
    "reflection-binding",
    "reflection-activation",
]


def _load_method_subject_ids(family_slug: str) -> list[str]:
    """Load method subject IDs from the capability family contract."""
    contract_path = _VERIFICATION / family_slug / "capability-family-contract.json"
    if not contract_path.exists():
        print(f"  [SKIP] no contract at {contract_path}")
        return []

    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)

    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]

    return mids


def _build_entrypoint(
    family_slug: str,
    method_subject_ids: list[str],
) -> dict:
    """Build the synthetic entry point DLL for a family."""
    entrypoint_dir = _VERIFICATION / family_slug / "entrypoint"
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry"

    result = generate_and_build(
        entrypoint_dir,
        assembly_name="System.Private.CoreLib",
        family_id=f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}",
        method_subject_ids=method_subject_ids,
        class_name=class_name,
    )
    return result


def _run_convert(entrypoint_dir: Path, dll_path: str, entry_point_subject_id: str) -> bool:
    """Run chaos-il2cpp convert on the entrypoint DLL."""
    # Create/overwrite subject.manifest.json with correct format
    # (assemblies must be an array of path strings, not objects)
    manifest_path = entrypoint_dir / "subject.manifest.json"
    manifest = {
        "source": {
            "type": "managed-dlls",
            "assemblies": [
                dll_path,
            ],
            "entry": entry_point_subject_id,
        }
    }
    with open(manifest_path, "w", encoding="utf-8") as f:
        json.dump(manifest, f, indent=2)

    # Delete old closure-sp if it exists (to force clean convert)
    closure_sp = entrypoint_dir / "closure-sp"
    if closure_sp.exists():
        import shutil
        shutil.rmtree(closure_sp)
    closure_sp.mkdir(parents=True, exist_ok=True)

    result = subprocess.run(
        [
            "dotnet", "run", "--no-build",
            "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
            "--", "convert", str(entrypoint_dir),
            "--output", str(closure_sp),
        ],
        capture_output=True, text=True,
        timeout=120,
    )

    if result.returncode != 0:
        print(f"    convert FAILED (rc={result.returncode})")
        for line in result.stderr.splitlines()[-10:]:
            print(f"      {line}")
        return False

    # Verify aot-core-ir.json exists (convert puts it in analysis/ subdir)
    ir_path = closure_sp / "analysis" / "aot-core-ir.json"
    if not ir_path.exists():
        print(f"    convert produced no aot-core-ir.json")
        return False

    with open(ir_path, encoding="utf-8") as f:
        ir = json.load(f)
    method_count = len(ir.get("methods", []))
    print(f"    convert OK: {method_count} methods in IR")
    return True


def _trim_ir(family_slug: str) -> bool:
    """Trim aot-core-ir.json to entry-only methods."""
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry"
    entry_prefix = class_name
    ir_path = _VERIFICATION / family_slug / "entrypoint" / "closure-sp" / "analysis" / "aot-core-ir.json"

    if not ir_path.exists():
        return False

    result = subprocess.run(
        [
            sys.executable,
            str(_HERE / "trim_aot_core_ir.py"),
            "--input", str(ir_path),
            "--entry-prefix", entry_prefix,
            "--output", str(ir_path),
        ],
        capture_output=True, text=True,
        timeout=120,
    )

    if result.returncode != 0:
        print(f"    trim FAILED: {result.stderr}")
        return False

    with open(ir_path, encoding="utf-8") as f:
        ir = json.load(f)
    method_count = len(ir.get("methods", []))
    print(f"    trim OK: {method_count} methods retained")
    return True


def _run_emit_native_aot(family_slug: str) -> bool:
    """Run emit-native-aot to produce real C++."""
    # The convert command puts analysis artifacts in closure-sp/analysis/
    closure_sp_analysis = _VERIFICATION / family_slug / "entrypoint" / "closure-sp" / "analysis"
    genuine_out = _VERIFICATION / family_slug / "native" / "genuine"
    genuine_out.mkdir(parents=True, exist_ok=True)

    if not closure_sp_analysis.exists():
        print(f"    analysis dir not found at {closure_sp_analysis}")
        return False

    result = subprocess.run(
        [
            "dotnet", "run", "--no-build",
            "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
            "--", "emit-native-aot",
            str(closure_sp_analysis),
            str(genuine_out),
        ],
        capture_output=True, text=True,
        timeout=120,
    )

    if result.returncode != 0:
        # Extract the error message
        error_lines = [l for l in result.stderr.splitlines() if l.strip()]
        error_msg = error_lines[-1] if error_lines else "unknown error"
        print(f"    emit-native-aot FAILED: {error_msg[:120]}")
        return False

    # Check output
    cpp_path = genuine_out / "generated" / "native-aot.generated.cpp"
    if cpp_path.exists():
        size = cpp_path.stat().st_size
        print(f"    emit-native-aot OK: {size} bytes")
    else:
        print(f"    emit-native-aot OK (no .cpp output found)")
    return True


def run_family(family_slug: str) -> dict:
    """Run the full pipeline for one family. Returns result dict."""
    result = {
        "family": family_slug,
        "steps": {},
        "success": False,
    }

    print(f"\n{'='*60}")
    print(f"Family: {family_slug}")
    print(f"{'='*60}")

    # Step 0: Load method subject IDs
    mids = _load_method_subject_ids(family_slug)
    if not mids:
        print(f"  [SKIP] no methods in contract")
        result["error"] = "no method subject IDs"
        return result
    print(f"  Methods: {len(mids)}")
    result["methodCount"] = len(mids)

    # Step 1: Build entrypoint
    print(f"  [1/4] Building entrypoint...")
    build_result = _build_entrypoint(family_slug, mids)
    if not build_result.get("success"):
        result["steps"]["build_entrypoint"] = "FAILED"
        result["error"] = build_result.get("error", "build failed")
        print(f"    FAILED: {result['error']}")
        return result
    result["steps"]["build_entrypoint"] = "OK"
    result["entryPointSubjectId"] = build_result["entry_point_subject_id"]
    result["dllPath"] = build_result["dll_path"]

    # Step 2: Convert
    print(f"  [2/4] Convert...")
    entrypoint_dir = _VERIFICATION / family_slug / "entrypoint"
    if not _run_convert(entrypoint_dir, build_result["dll_path"], build_result["entry_point_subject_id"]):
        result["steps"]["convert"] = "FAILED"
        result["error"] = "convert failed"
        return result
    result["steps"]["convert"] = "OK"

    # Step 3: Trim
    print(f"  [3/4] Trimming...")
    if not _trim_ir(family_slug):
        result["steps"]["trim"] = "FAILED"
        result["error"] = "trim failed"
        return result
    result["steps"]["trim"] = "OK"

    # Step 4: Emit native AOT
    print(f"  [4/4] Emitting native AOT...")
    if not _run_emit_native_aot(family_slug):
        result["steps"]["emit_native_aot"] = "FAILED"
        result["error"] = "emit-native-aot failed"
        return result
    result["steps"]["emit_native_aot"] = "OK"

    result["success"] = True
    return result


def main() -> None:
    print(f"Batch native AOT CodeGen pipeline - {len(FAMILIES)} families")
    print(f"Repo: {_REPO_ROOT}")
    print(f"Verification: {_VERIFICATION}")
    print()

    results = []
    passed = 0
    failed = 0

    for idx, family_slug in enumerate(FAMILIES):
        family_result = run_family(family_slug)
        results.append(family_result)

        if family_result["success"]:
            passed += 1
            print(f"  >>> PASSED ({passed}/{idx+1})")
        else:
            failed += 1
            print(f"  >>> FAILED ({failed}/{idx+1}): {family_result.get('error', 'unknown')}")

    # Summary
    print(f"\n{'='*60}")
    print(f"SUMMARY: {passed} passed, {failed} failed, {len(FAMILIES)} total")
    print(f"{'='*60}")
    for r in results:
        status = "PASS" if r["success"] else "FAIL"
        steps = " -> ".join(f"{k}={v}" for k, v in r.get("steps", {}).items())
        print(f"  {status:4s}  {r['family']:35s}  {steps}")

    # Write results
    output_path = _VERIFICATION / "reports" / "batch-native-aot-pipeline-results.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "total": len(FAMILIES),
        "passed": passed,
        "failed": failed,
        "results": results,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
    }
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(summary, f, indent=2, ensure_ascii=False)
    print(f"\nResults written to {output_path}")


if __name__ == "__main__":
    main()
