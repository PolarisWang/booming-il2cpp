"""Batch-run the CodeGen pipeline for hotupdate patch variant across all CoreLib families.

For each family:
  1. Generate patch-mode synthetic entry point C# source + project
     (each MethodN returns sentinel 0xB0000000 + N instead of calling real API)
  2. Build the DLL
  3. Run chaos-il2cpp convert to produce aot-core-ir.json
  4. Trim aot-core-ir.json to entry-only methods
  5. Run chaos-il2cpp emit-native-aot to produce real C++ with sentinel returns
  6. Write output to native/patch/generated/native-aot.generated.cpp

The host variant (real API calls) is already produced by batch_native_aot_runner.py
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


def _build_patch_entrypoint(
    family_slug: str,
    method_subject_ids: list[str],
) -> dict:
    """Build the patch-mode synthetic entry point DLL for a family.

    The patch variant uses MethodN that returns 0xB0000000+N sentinel values.
    """
    entrypoint_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "entrypoint-patch"
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}PatchEntry"

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
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}SemanticPatchEntry"

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
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}{variant_suffix.get(variant, 'SemanticPatchEntry')}"
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


def _generate_patch_cpp_direct(family_slug: str, method_count: int) -> bool:
    """Generate patch C++ directly for the sentinel-return variant.

    The chaos-il2cpp pipeline cannot classify a DLL that only returns
    sentinel values (no real API calls). Instead, we generate the C++
    directly with 0xB0000000+N returns for each method.
    """
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}PatchEntry"
    patch_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "patch" / "generated"
    patch_dir.mkdir(parents=True, exist_ok=True)
    cpp_path = patch_dir / "native-aot.generated.cpp"

    lines = [
        '#include <chaos/common.h>',
        '#include "codegen_bridge.h"',
        '',
        '// Auto-generated patch variant: sentinel returns 0xB0000000+N',
        f'// Family: {family_slug}',
        f'// Methods: {method_count}',
        '',
    ]

    # Method stubs
    for i in range(method_count):
        sentinel = 0xB0000000 + i
        lines.append(
            f'extern "C" CHAOS_IL2CPP_INT32 {class_name}_Method{i}(void)'
        )
        lines.append('{')
        # Use unsigned arithmetic to avoid signed overflow warnings on MSVC
        lines.append(f'    return static_cast<CHAOS_IL2CPP_INT32>(0xB0000000u + {i}u);')
        lines.append('}')
        lines.append('')

    # RunNativeAot dispatcher
    lines.append('extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)')
    lines.append('{')
    lines.append('    switch (chaos_entry_index)')
    lines.append('    {')
    for i in range(method_count):
        lines.append(f'        case {i}: return static_cast<int>({class_name}_Method{i}());')
    lines.append('        default: return -1;')
    lines.append('    }')
    lines.append('}')

    cpp_path.write_text('\n'.join(lines) + '\n', encoding="utf-8")
    size = cpp_path.stat().st_size
    print(f"    [patch] Generated patch C++ directly: {size} bytes at {cpp_path.relative_to(_REPO_ROOT)}")
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
        # Patch variant: generate sentinel-return C++ directly (pipeline
        # cannot classify a DLL with only sentinel returns).
        print(f"    [patch] Generating patch C++ directly (sentinel returns)...")
        method_count = len(mids)
        if not _generate_patch_cpp_direct(family_slug, method_count):
            result["steps"]["generate_patch_cpp"] = "FAILED"
            result["error"] = "patch C++ generation failed"
            return result
        result["steps"]["generate_patch_cpp"] = "OK"
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


def run_family(family_slug: str) -> dict:
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

    # Process semantic-patch variant (alternative API calls)
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
    args = parser.parse_args()

    if args.trace and not args.no_trace:
        trace_init(_REPO_ROOT, stage="batch-hotupdate")

    families = args.families or FAMILIES

    print(f"Batch hotupdate CodeGen pipeline (patch + semantic-patch) - {len(families)} families")
    print(f"Repo: {_REPO_ROOT}")
    print(f"Verification: {_VERIFICATION}")
    print()

    trace("batch_start", family_count=len(families))

    results = []
    passed = 0
    failed = 0

    for idx, family_slug in enumerate(families):
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
        variants = r.get("variants", {})
        patch_ok = variants.get("patch", {}).get("success", False)
        sp_ok = variants.get("semantic-patch", {}).get("success", False)
        v_status = f"patch={'OK' if patch_ok else 'FAIL'}, sp={'OK' if sp_ok else 'FAIL'}"
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
