"""Batch-run the CodeGen pipeline for hotupdate patch variant across all CoreLib families.

For each family:
  1. Generate patch-mode synthetic entry point C# source + project
     (each MethodN returns sentinel 0xB0000000 + N instead of calling real API)
  2. Build the DLL
  3. Run chaos-il2cpp convert to produce aot-core-ir.json
  4. Trim aot-core-ir.json to entry-only methods
  5. Run chaos-il2cpp emit-native-aot to produce real C++ with sentinel returns
  6. Write output to native/patch/generated/native-aot.generated.cpp

The host variant (real API calls) is already produced by pipeline_native_aot_runner.py
at native/genuine/generated/native-aot.generated.cpp. The hotupdate test links both
with symbol renaming to verify the method replacement cycle.

Results are written to a summary JSON file.
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
_REPO_ROOT = _HERE.parents[4]  # D:/agent/booming-il2cpp
_VERIFICATION = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib"
sys.path.insert(0, str(_HERE))
sys.path.insert(0, str(_HERE.parent.parent))  # for testing.trace (parent of run/testing/)

from family_entrypoint_generator import generate_and_build

from testing.trace import trace_init, trace

# Same 23 families as the benchmark pipeline
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
    "threading-thread-basics",
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
    "reflection-type",
    "reflection-assembly",
    "reflection-member-complete",
    "reflection-generics",
    "reflection-parameters",
    "reflection-module",
    "reflection-binding",
    "reflection-activation",
    # COM interop families
    "ccw-basic",
    "rcw-basic",
    "dispatch-basic",
]


def _run_emit_patch_data(dll_path: str, output_path: str, aot_core_ir_path: str | None = None) -> bool:
    """Run emit-patch-data to produce .patchdata from a patch DLL."""
    cmd = [
        "dotnet", "run", "--no-build",
        "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
        "--", "emit-patch-data",
        dll_path,
        output_path,
    ]
    if aot_core_ir_path:
        cmd += ["--aot-core-ir", aot_core_ir_path]

    result = subprocess.run(
        cmd,
        capture_output=True, text=True,
        timeout=120,
    )

    if result.returncode != 0:
        print(f"    emit-patch-data FAILED (rc={result.returncode})")
        for line in result.stderr.splitlines()[-5:]:
            print(f"      {line}")
        return False

    print(f"    emit-patch-data OK: {output_path}")
    return True


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


def _copy_handwritten_cs(entrypoint_dir: Path, class_name: str, family_slug: str) -> None:
    """Copy handwritten partial class .cs files into entrypoint_dir for generate_and_build().

    Checks both managed/patch/ and handwritten/ directories so either location works.
    Only copies files matching {class_name}.Custom.cs (partial class with custom methods).
    """
    custom_name = f"{class_name}.Custom.cs"
    for source_subdir in ("managed/patch", "handwritten"):
        source_dir = _VERIFICATION / family_slug / source_subdir
        src = source_dir / custom_name
        if src.exists():
            dest = entrypoint_dir / custom_name
            entrypoint_dir.mkdir(parents=True, exist_ok=True)
            dest.write_text(src.read_text(encoding="utf-8"), encoding="utf-8")
            print(f"    copied handwritten {custom_name} from {source_subdir}/")
            return


def _build_patch_entrypoint(
    family_slug: str,
    method_subject_ids: list[str],
) -> dict:
    """Build the patch-mode synthetic entry point DLL for a family.

    The patch variant uses MethodN that returns 0xB0000000+N sentinel values.
    """
    entrypoint_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "entrypoint-patch"
    class_name = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}PatchEntry"

    # Copy handwritten Custom.cs before generate_and_build() auto-detects it
    _copy_handwritten_cs(entrypoint_dir, class_name, family_slug)

    result = generate_and_build(
        entrypoint_dir,
        assembly_name="System.Private.CoreLib",
        family_id=f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}",
        method_subject_ids=method_subject_ids,
        class_name=class_name,
        variant="patch",
    )
    return result


def _build_semantic_patch_entrypoint(
    family_slug: str,
    method_subject_ids: list[str],
) -> dict:
    """Build the semantic-patch-mode entry point DLL for a family.

    The semantic-patch variant calls the same real API as benchmark but with
    different parameter values (from TYPE_ALTERNATIVE_MAP), producing different
    but valid results for semantic hotupdate verification.
    """
    entrypoint_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "entrypoint-semantic-patch"
    class_name = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}SemanticPatchEntry"

    # Copy handwritten Custom.cs before generate_and_build() auto-detects it
    _copy_handwritten_cs(entrypoint_dir, class_name, family_slug)

    result = generate_and_build(
        entrypoint_dir,
        assembly_name="System.Private.CoreLib",
        family_id=f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}",
        method_subject_ids=method_subject_ids,
        class_name=class_name,
        variant="semantic-patch",
    )
    return result


def _run_convert(entrypoint_dir: Path, dll_path: str, entry_point_subject_id: str) -> bool:
    """Run chaos-il2cpp convert on the entrypoint DLL."""
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

    # Delete old closure-sp if it exists
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
        capture_output=True, text=True,
        timeout=120,
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
    method_count = len(ir.get("methods", []))
    print(f"    convert OK: {method_count} methods in IR")
    return True


def _trim_ir(family_slug: str, variant: str = "semantic-patch") -> bool:
    """Trim aot-core-ir.json to entry-only methods."""
    variant_suffix = {
        "patch": "PatchEntry",
        "semantic-patch": "SemanticPatchEntry",
    }
    class_name = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}{variant_suffix.get(variant, 'SemanticPatchEntry')}"
    entry_prefix = class_name
    ir_path = _VERIFICATION / family_slug / "il2cpp_dist" / f"entrypoint-{variant}" / "closure-sp" / "analysis" / "aot-core-ir.json"

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


def _run_emit_native_aot(family_slug: str, variant: str = "semantic-patch") -> bool:
    """Run emit-native-aot to produce real C++ for the specified variant.

    Args:
        variant: "patch" for sentinel returns (entrypoint-patch),
                 "semantic-patch" for alternative API calls.
    """
    entrypoint_name = f"entrypoint-{variant}"
    output_name = variant

    closure_sp_analysis = _VERIFICATION / family_slug / "il2cpp_dist" / entrypoint_name / "closure-sp" / "analysis"
    variant_out = _VERIFICATION / family_slug / "il2cpp_dist" / output_name
    variant_out.mkdir(parents=True, exist_ok=True)

    if not closure_sp_analysis.exists():
        print(f"    analysis dir not found at {closure_sp_analysis}")
        return False

    result = subprocess.run(
        [
            "dotnet", "run", "--no-build",
            "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
            "--", "emit-native-aot",
            str(closure_sp_analysis),
            str(variant_out),
        ],
        capture_output=True, text=True,
        timeout=120,
    )

    if result.returncode != 0:
        error_lines = [l for l in result.stderr.splitlines() if l.strip()]
        error_msg = error_lines[-1] if error_lines else "unknown error"
        print(f"    emit-native-aot FAILED: {error_msg[:120]}")
        return False

    cpp_path = variant_out / "generated" / "native-aot.generated.cpp"
    if cpp_path.exists():
        size = cpp_path.stat().st_size
        print(f"    emit-native-aot OK ({variant}): {size} bytes")
    else:
        alt_path = variant_out / "native-aot.generated.cpp"
        if alt_path.exists():
            size = alt_path.stat().st_size
            print(f"    emit-native-aot OK ({variant}): {size} bytes at alt path")
        else:
            print(f"    emit-native-aot OK ({variant}): (no .cpp output found at {cpp_path} or {alt_path})")
    return True


def _run_variant_pipeline(
    family_slug: str,
    mids: list[str],
    variant: str,
    build_fn,
) -> dict:
    """Run the full CodeGen pipeline for one variant (patch or semantic-patch).

    Args:
        variant: "patch" or "semantic-patch".
        build_fn: Function that builds the entrypoint, e.g. _build_patch_entrypoint.

    Returns dict with "success": True/False and steps details.
    """
    result = {"variant": variant, "steps": {}, "success": False}
    variant_label = variant.replace("-", " ")

    # Step 1: Build entrypoint
    print(f"    [{variant}] Building {variant_label} entrypoint...")
    build_result = build_fn(family_slug, mids)
    if not build_result.get("success"):
        result["steps"]["build"] = "FAILED"
        result["error"] = build_result.get("error", "build failed")
        print(f"      FAILED: {result['error']}")
        return result
    result["steps"]["build"] = "OK"
    result["dllPath"] = build_result["dll_path"]
    result["entryPointSubjectId"] = build_result["entry_point_subject_id"]

    if variant == "patch":
        # Patch variant: need to convert+trim first to produce aot-core-ir.json,
        # then embed it into the patchdata so the runtime can deserialize IR for inlining.
        print(f"    [patch] Converting patch entrypoint DLL...")
        entrypoint_dir = _VERIFICATION / family_slug / "il2cpp_dist" / f"entrypoint-{variant}"
        if not _run_convert(entrypoint_dir, build_result["dll_path"], build_result["entry_point_subject_id"]):
            result["steps"]["convert"] = "FAILED"
            result["error"] = "convert failed"
            return result
        result["steps"]["convert"] = "OK"

        print(f"    [patch] Trimming IR to entry-only methods...")
        if not _trim_ir(family_slug, variant):
            result["steps"]["trim"] = "FAILED"
            result["error"] = "trim failed"
            return result
        result["steps"]["trim"] = "OK"

        # Now emit-patch-data with the trimmed aot-core-ir.json.
        print(f"    [patch] Extracting patch data via emit-patch-data...")
        patchdata_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "patch" / "patchdata"
        patchdata_dir.mkdir(parents=True, exist_ok=True)
        patchdata_path = patchdata_dir / f"{family_slug}.patchdata"
        ir_path = _VERIFICATION / family_slug / "il2cpp_dist" / f"entrypoint-{variant}" / "closure-sp" / "analysis" / "aot-core-ir.json"
        ir_arg = str(ir_path) if ir_path.exists() else None
        if not _run_emit_patch_data(build_result["dll_path"], str(patchdata_path), ir_arg):
            result["steps"]["emit_patch_data"] = "FAILED"
            result["error"] = "emit-patch-data failed"
            return result
        result["steps"]["emit_patch_data"] = "OK"
    else:
        # Step 2: Convert
        print(f"    [{variant}] Convert...")
        entrypoint_dir = _VERIFICATION / family_slug / "il2cpp_dist" / f"entrypoint-{variant}"
        if not _run_convert(entrypoint_dir, build_result["dll_path"], build_result["entry_point_subject_id"]):
            result["steps"]["convert"] = "FAILED"
            result["error"] = "convert failed"
            return result
        result["steps"]["convert"] = "OK"

        # Step 3: Trim
        print(f"    [{variant}] Trimming...")
        if not _trim_ir(family_slug, variant):
            result["steps"]["trim"] = "FAILED"
            result["error"] = "trim failed"
            return result
        result["steps"]["trim"] = "OK"

        # Step 4: Emit native AOT
        print(f"    [{variant}] Emitting native AOT...")
        if not _run_emit_native_aot(family_slug, variant):
            result["steps"]["emit"] = "FAILED"
            result["error"] = "emit-native-aot failed"
            return result
        result["steps"]["emit"] = "OK"

    result["success"] = True
    return result


def run_family(family_slug: str, skip_semantic_patch: bool = False) -> dict:
    """Run the full hotupdate pipeline for one family. Returns result dict."""
    result = {
        "family": family_slug,
        "variants": {},
        "success": False,
    }

    print(f"\n{'='*60}")
    print(f"HotUpdate pipeline: {family_slug}")
    print(f"{'='*60}")

    # Step 0: Load method subject IDs
    mids = _load_method_subject_ids(family_slug)
    if not mids:
        print(f"  [SKIP] no methods in contract")
        result["error"] = "no method subject IDs"
        trace("family_skip", family=family_slug, reason="no methods")
        return result
    print(f"  Methods: {len(mids)}")
    result["methodCount"] = len(mids)

    # Process patch variant (sentinel returns: 0xB0000000 + N)
    print(f"\n  --- Patch variant ---")
    patch_result = _run_variant_pipeline(family_slug, mids, "patch", _build_patch_entrypoint)
    result["variants"]["patch"] = {
        "success": patch_result["success"],
        "error": patch_result.get("error"),
    }
    if not patch_result["success"]:
        result["error"] = f"patch variant failed: {patch_result.get('error')}"
        trace("family_patch_failed", family=family_slug, error=patch_result.get("error"))
        return result

    # Semantic-patch variant (optional — D3 dispatch replaces the need for SP C++)
    if skip_semantic_patch:
        print(f"\n  --- Semantic-patch variant (skipped) ---")
        result["variants"]["semantic-patch"] = {"success": True, "skipped": True}
    else:
        print(f"\n  --- Semantic-patch variant ---")
        sp_result = _run_variant_pipeline(family_slug, mids, "semantic-patch", _build_semantic_patch_entrypoint)
        result["variants"]["semantic-patch"] = {
            "success": sp_result["success"],
            "error": sp_result.get("error"),
        }
        if not sp_result["success"]:
            result["error"] = f"semantic-patch variant failed: {sp_result.get('error')}"
            trace("family_sp_failed", family=family_slug, error=sp_result.get("error"))
            return result

    result["success"] = True
    trace("family_passed", family=family_slug, method_count=len(mids))
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description="Batch hotupdate CodeGen pipeline")
    parser.add_argument("--trace", action="store_true", default=True, help="Enable JSONL trace logging (default: on)")
    parser.add_argument("--no-trace", action="store_true", help="Disable JSONL trace logging")
    parser.add_argument("--families", nargs="*", help="Space-separated subset of family slugs to process")
    parser.add_argument("--skip-semantic-patch", action="store_true", default=True, help="Skip semantic-patch variant (Hotpatch dispatch replaces it)")
    parser.add_argument("--no-skip-semantic-patch", action="store_false", dest="skip_semantic_patch", help="Include semantic-patch variant")
    args = parser.parse_args()

    if args.trace and not args.no_trace:
        trace_init(_REPO_ROOT, stage="batch-hotupdate")

    families = args.families or FAMILIES

    print(f"Batch hotupdate CodeGen pipeline (patch + {'(skipped) ' if args.skip_semantic_patch else ''}semantic-patch) - {len(families)} families")
    print(f"Repo: {_REPO_ROOT}")
    print(f"Verification: {_VERIFICATION}")
    print()

    trace("batch_start", family_count=len(families))

    results = []
    passed = 0
    failed = 0

    for idx, family_slug in enumerate(families):
        family_result = run_family(family_slug, skip_semantic_patch=args.skip_semantic_patch)
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
        variants = r.get("variants", {})
        patch_ok = variants.get("patch", {}).get("success", False)
        sp = variants.get("semantic-patch", {})
        sp_status = "SKIP" if sp.get("skipped") else ("OK" if sp.get("success") else "FAIL")
        v_status = f"patch={'OK' if patch_ok else 'FAIL'}, sp={sp_status}"
        print(f"  {status:4s}  {r['family']:35s}  {v_status}")

    # Write results
    output_path = _VERIFICATION / "reports" / "batch-hotupdate-pipeline-results.json"
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
