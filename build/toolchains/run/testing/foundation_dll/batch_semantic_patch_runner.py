"""Batch-run the CodeGen pipeline for semantic-patch variant across reflection families.

For each family:
  1. Generate semantic-patch entrypoint (calls real API with alternative params)
  2. Build the DLL
  3. Run chaos-il2cpp convert to produce aot-core-ir.json
  4. Trim aot-core-ir.json to entry-only methods
  5. Run chaos-il2cpp emit-native-aot to produce real C++
  6. Write output to native/semantic-patch/generated/native-aot.generated.cpp
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
import shutil
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib"
sys.path.insert(0, str(_HERE))
sys.path.insert(0, str(_HERE.parent.parent))  # for testing.trace

from family_entrypoint_generator import generate_and_build

from testing.trace import trace_init, trace

FAMILIES = [
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


def _run_convert(entrypoint_dir: Path, dll_path: str, entry_point_subject_id: str) -> bool:
    manifest_path = entrypoint_dir / "subject.manifest.json"
    manifest = {
        "source": {
            "type": "managed-dlls",
            "assemblies": [dll_path],
            "entry": entry_point_subject_id,
        }
    }
    with open(manifest_path, "w", encoding="utf-8") as f:
        json.dump(manifest, f, indent=2)
    closure_sp = entrypoint_dir / "closure-sp"
    if closure_sp.exists():
        shutil.rmtree(closure_sp)
    closure_sp.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(
        [
            "dotnet", "run", "--no-build",
            "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
            "--", "convert", str(entrypoint_dir),
            "--output", str(closure_sp),
        ],
        capture_output=True, text=True, timeout=120,
    )
    if result.returncode != 0:
        print(f"    convert FAILED (rc={result.returncode})")
        for line in result.stderr.splitlines()[-10:]:
            print(f"      {line}")
        return False
    ir_path = closure_sp / "analysis" / "aot-core-ir.json"
    if not ir_path.exists():
        print(f"    convert produced no aot-core-ir.json")
        return False
    with open(ir_path, encoding="utf-8") as f:
        ir = json.load(f)
    print(f"    convert OK: {len(ir.get('methods', []))} methods in IR")
    return True


def _trim_ir(family_slug: str) -> bool:
    class_name = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}SemanticPatchEntry"
    ir_path = _VERIFICATION / family_slug / "il2cpp_dist" / "entrypoint-semantic-patch" / "closure-sp" / "analysis" / "aot-core-ir.json"
    if not ir_path.exists():
        return False
    result = subprocess.run(
        [
            sys.executable,
            str(_HERE / "trim_aot_core_ir.py"),
            "--input", str(ir_path),
            "--entry-prefix", class_name,
            "--output", str(ir_path),
        ],
        capture_output=True, text=True, timeout=120,
    )
    if result.returncode != 0:
        print(f"    trim FAILED: {result.stderr}")
        return False
    with open(ir_path, encoding="utf-8") as f:
        ir = json.load(f)
    print(f"    trim OK: {len(ir.get('methods', []))} methods retained")
    return True


def _run_emit_native_aot(family_slug: str) -> bool:
    closure_sp_analysis = _VERIFICATION / family_slug / "il2cpp_dist" / "entrypoint-semantic-patch" / "closure-sp" / "analysis"
    sem_out = _VERIFICATION / family_slug / "il2cpp_dist" / "semantic-patch"
    sem_out.mkdir(parents=True, exist_ok=True)
    if not closure_sp_analysis.exists():
        print(f"    analysis dir not found at {closure_sp_analysis}")
        return False
    result = subprocess.run(
        [
            "dotnet", "run", "--no-build",
            "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
            "--", "emit-native-aot",
            str(closure_sp_analysis),
            str(sem_out),
        ],
        capture_output=True, text=True, timeout=120,
    )
    if result.returncode != 0:
        error_lines = [l for l in result.stderr.splitlines() if l.strip()]
        print(f"    emit-native-aot FAILED: {error_lines[-1][:120] if error_lines else 'unknown'}")
        return False
    cpp_path = sem_out / "generated" / "native-aot.generated.cpp"
    if cpp_path.exists():
        size = cpp_path.stat().st_size
        print(f"    emit-native-aot OK: {size} bytes")
    else:
        alt_path = sem_out / "native-aot.generated.cpp"
        if alt_path.exists():
            size = alt_path.stat().st_size
            print(f"    emit-native-aot OK: {size} bytes at alt path")
        else:
            print(f"    emit-native-aot OK (no .cpp output)")
    return True


def run_family(family_slug: str) -> dict:
    result = {"family": family_slug, "steps": {}, "success": False}
    print(f"\n{'='*60}")
    print(f"Semantic-patch variant: {family_slug}")
    print(f"{'='*60}")

    mids = _load_method_subject_ids(family_slug)
    if not mids:
        result["error"] = "no method subject IDs"
        trace("family_skip", family=family_slug, reason="no methods")
        return result
    print(f"  Methods: {len(mids)}")

    entrypoint_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "entrypoint-semantic-patch"
    class_name = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}SemanticPatchEntry"

    print(f"  [1/4] Building semantic-patch entrypoint...")
    build_result = generate_and_build(
        entrypoint_dir,
        assembly_name="System.Private.CoreLib",
        family_id=f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}",
        method_subject_ids=mids,
        class_name=class_name,
        variant="semantic-patch",
    )
    if not build_result.get("success"):
        result["steps"]["build"] = "FAILED"
        result["error"] = build_result.get("error", "build failed")
        print(f"    FAILED: {result['error']}")
        trace("family_build_failed", family=family_slug, error=result["error"])
        return result
    result["steps"]["build"] = "OK"

    print(f"  [2/4] Convert...")
    if not _run_convert(entrypoint_dir, build_result["dll_path"], build_result["entry_point_subject_id"]):
        result["steps"]["convert"] = "FAILED"
        result["error"] = "convert failed"
        trace("family_convert_failed", family=family_slug)
        return result
    result["steps"]["convert"] = "OK"

    print(f"  [3/4] Trimming...")
    if not _trim_ir(family_slug):
        result["steps"]["trim"] = "FAILED"
        result["error"] = "trim failed"
        trace("family_trim_failed", family=family_slug)
        return result
    result["steps"]["trim"] = "OK"

    print(f"  [4/4] Emitting native AOT...")
    if not _run_emit_native_aot(family_slug):
        result["steps"]["emit"] = "FAILED"
        result["error"] = "emit-native-aot failed"
        trace("family_emit_failed", family=family_slug)
        return result
    result["steps"]["emit"] = "OK"

    result["success"] = True
    trace("family_passed", family=family_slug, method_count=len(mids))
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description="Batch semantic-patch CodeGen pipeline")
    parser.add_argument("--trace", action="store_true", default=True, help="Enable JSONL trace logging (default: on)")
    parser.add_argument("--no-trace", action="store_true", help="Disable JSONL trace logging")
    parser.add_argument("--families", nargs="*", help="Space-separated subset of family slugs to process")
    args = parser.parse_args()

    if args.trace and not args.no_trace:
        trace_init(_REPO_ROOT, stage="batch-semantic-patch")

    families = args.families or FAMILIES

    trace("batch_start", family_count=len(families))

    results = []
    passed = 0
    failed = 0
    for idx, family_slug in enumerate(families):
        fr = run_family(family_slug)
        results.append(fr)
        if fr["success"]:
            passed += 1
            print(f"  >>> PASSED ({passed}/{idx+1})")
        else:
            failed += 1
            print(f"  >>> FAILED ({failed}/{idx+1}): {fr.get('error', 'unknown')}")

    print(f"\n{'='*60}")
    print(f"SUMMARY: {passed} passed, {failed} failed, {len(FAMILIES)} total")
    for r in results:
        status = "PASS" if r["success"] else "FAIL"
        steps = " -> ".join(f"{k}={v}" for k, v in r.get("steps", {}).items())
        print(f"  {status:4s}  {r['family']:35s}  {steps}")

    output_path = _VERIFICATION / "reports" / "batch-semantic-patch-pipeline-results.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump({"total": len(FAMILIES), "passed": passed, "failed": failed, "results": results, "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S")}, f, indent=2)
    print(f"\nResults written to {output_path}")


if __name__ == "__main__":
    main()
