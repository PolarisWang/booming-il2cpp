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

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]  # D:/agent/booming-il2cpp
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"
_VERIFICATION: Path | None = None  # set per-assembly in main()
sys.path.insert(0, str(_HERE))

from family_entrypoint_generator import generate_and_build

try:
    from testing.trace import trace_init, trace
except ImportError:
    def trace_init(*args, **kwargs):
        pass
    def trace(*args, **kwargs):
        pass

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


def _discover_families(assembly_name: str) -> list[str]:
    """Discover families for an assembly by scanning for capability-family-contract.json files."""
    asm_dir = _VERIFICATION_BASE / assembly_name
    if not asm_dir.exists():
        return []
    families = []
    for d in sorted(asm_dir.iterdir()):
        if d.is_dir() and (d / "capability-family-contract.json").exists():
            families.append(d.name)
    return families


def _load_method_subject_ids(family_slug: str, *, verification: Path | None = None) -> list[str]:
    """Load method subject IDs from the capability family contract."""
    v = verification or _VERIFICATION
    contract_path = v / family_slug / "capability-family-contract.json"
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
    *,
    assembly_name: str = "System.Private.CoreLib",
    verification: Path | None = None,
    variant: str = "benchmark",
) -> dict:
    """Build the synthetic entry point DLL for a family.

    If a hand-written directory exists at verification/<family_slug>/handwritten/,
    use it directly instead of auto-generating.
    """
    v = verification or _VERIFICATION
    entrypoint_dir = v / family_slug / "il2cpp_dist" / "entrypoint"
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry"

    # Check for hand-written entrypoint
    handwritten_dir = v / family_slug / "handwritten"
    if handwritten_dir.exists():
        print(f"    using hand-written entrypoint from {handwritten_dir}")
        entrypoint_dir.mkdir(parents=True, exist_ok=True)
        for f in handwritten_dir.iterdir():
            if f.is_file():
                dest = entrypoint_dir / f.name
                dest.write_text(f.read_text(encoding="utf-8"), encoding="utf-8")
        # Build
        csproj = next(entrypoint_dir.glob("*.csproj"), None)
        if not csproj:
            return {"success": False, "error": "no .csproj in handwritten entrypoint"}
        build_out = entrypoint_dir / "build-output"
        result = subprocess.run(
            ["dotnet", "build", str(csproj), "-o", str(build_out), "--nologo", "-v", "quiet"],
            capture_output=True, text=True,
        )
        if result.returncode != 0:
            return {"success": False, "error": result.stderr or result.stdout}
        dll = next(build_out.glob("*.dll"), None)
        if not dll:
            return {"success": False, "error": "no DLL produced"}
        return {
            "success": True,
            "dll_path": str(dll),
            "csproj_path": str(csproj),
            "source_path": str(next(entrypoint_dir.glob("*.cs"), None)),
            "entry_point_subject_id": f"{class_name}/{class_name}::Run:System.Int32(System.Int32)",
        }

    result = generate_and_build(
        entrypoint_dir,
        assembly_name=assembly_name,
        family_id=f"family/{assembly_name}/{family_slug.replace('-', '/')}",
        method_subject_ids=method_subject_ids,
        class_name=class_name,
        variant=variant,
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


def _trim_ir(family_slug: str, *, verification: Path | None = None, class_name: str | None = None) -> bool:
    """Trim aot-core-ir.json to entry-only methods."""
    v = verification or _VERIFICATION
    entry_prefix = class_name or f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry"
    ir_path = v / family_slug / "il2cpp_dist" / "entrypoint" / "closure-sp" / "analysis" / "aot-core-ir.json"

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


def _run_emit_native_aot(family_slug: str, *, verification: Path | None = None) -> bool:
    """Run emit-native-aot to produce real C++."""
    v = verification or _VERIFICATION
    closure_sp_analysis = v / family_slug / "il2cpp_dist" / "entrypoint" / "closure-sp" / "analysis"
    genuine_out = v / family_slug / "il2cpp_dist" / "genuine"
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


def _has_synthetic_method_ids(method_subject_ids: list[str]) -> bool:
    """Check if method subject IDs are synthetic (not real callable APIs).

    Synthetic contracts use placeholder IDs like:
      TypeName::TypeName::Void(System.Object)    — constructor
      TypeName::get_Property:System.Void()         — mock property
      TypeName::MethodN:System.Void(P0, P1)        — generic methods
    """
    if not method_subject_ids:
        return True
    for mid in method_subject_ids[:5]:  # check first few
        if "::" not in mid:
            continue
        path_sig = mid.split("::", 1)
        type_path = path_sig[0]
        sig = path_sig[1]

        # Extract type name from path
        raw_type = type_path.split("/", 1)[1] if "/" in type_path else type_path
        type_name = raw_type.rsplit(".", 1)[-1]

        paren = sig.find("(")
        if paren <= 0:
            continue
        method_part = sig[:paren]
        colon = method_part.rfind(":")
        raw_method = method_part[:colon] if colon > 0 else method_part

        # Constructor pattern: method_name == type_name (e.g. AsnDecoder::Void)
        if raw_method == type_name:
            return True
        # Genuinely synthetic method names (property-like names are real API methods
        # e.g. Nullable<T>.get_Value, MemberInfo.get_Name, Delegate.get_Method)
        if raw_method in ("Method1", "Method2", "Method3"):
            return True
    return False


def run_family(family_slug: str, *, assembly_name: str = "System.Private.CoreLib", variant: str | None = None) -> dict:
    """Run the full pipeline for one family. Returns result dict."""
    verification = _VERIFICATION_BASE / assembly_name
    result = {
        "family": family_slug,
        "steps": {},
        "success": False,
    }

    print(f"\n{'='*60}")
    print(f"Family: {family_slug}")
    print(f"{'='*60}")

    # Step 0: Load method subject IDs
    mids = _load_method_subject_ids(family_slug, verification=verification)
    if not mids:
        print(f"  [SKIP] no methods in contract")
        result["error"] = "no method subject IDs"
        trace("family_skip", family=family_slug, reason="no methods")
        return result
    print(f"  Methods: {len(mids)}")
    result["methodCount"] = len(mids)

    # Step 1: Build entrypoint (auto-detect synthetic contracts → use patch variant)
    auto_variant = variant or ("patch" if _has_synthetic_method_ids(mids) else "benchmark")
    print(f"  [1/4] Building entrypoint (variant={auto_variant})...")
    build_result = _build_entrypoint(family_slug, mids, assembly_name=assembly_name, verification=verification, variant=auto_variant)
    # If benchmark build failed and variant was auto-detected, retry with patch
    if not build_result.get("success") and auto_variant == "benchmark" and not variant:
        print(f"    benchmark build failed, retrying with patch variant...")
        auto_variant = "patch"
        build_result = _build_entrypoint(family_slug, mids, assembly_name=assembly_name, verification=verification, variant="patch")
    if not build_result.get("success"):
        result["steps"]["build_entrypoint"] = "FAILED"
        result["error"] = build_result.get("error", "build failed")
        print(f"    FAILED: {result['error']}")
        trace("family_entrypoint_build_failed", family=family_slug, error=result["error"])
        return result
    result["steps"]["build_entrypoint"] = "OK"
    result["entryPointSubjectId"] = build_result["entry_point_subject_id"]
    result["dllPath"] = build_result["dll_path"]

    # Step 2: Convert
    print(f"  [2/4] Convert...")
    entrypoint_dir = verification / family_slug / "il2cpp_dist" / "entrypoint"
    if not _run_convert(entrypoint_dir, build_result["dll_path"], build_result["entry_point_subject_id"]):
        result["steps"]["convert"] = "FAILED"
        result["error"] = "convert failed"
        trace("family_convert_failed", family=family_slug)
        return result
    result["steps"]["convert"] = "OK"

    # Step 3: Trim (use correct class name prefix matching the variant)
    # Step 3: Trim (use correct class name prefix matching the variant)
    trim_class_name = build_result.get("entry_point_subject_id", "").split("/")[0] if build_result.get("entry_point_subject_id") else ""
    if not trim_class_name:
        trim_class_name = f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry"
    print(f"  [3/4] Trimming (prefix={trim_class_name})...")
    if not _trim_ir(family_slug, verification=verification, class_name=trim_class_name):
        result["steps"]["trim"] = "FAILED"
        result["error"] = "trim failed"
        trace("family_trim_failed", family=family_slug)
        return result
    result["steps"]["trim"] = "OK"

    # Step 4: Emit native AOT
    print(f"  [4/4] Emitting native AOT...")
    if not _run_emit_native_aot(family_slug, verification=verification):
        result["steps"]["emit_native_aot"] = "FAILED"
        result["error"] = "emit-native-aot failed"
        trace("family_emit_failed", family=family_slug)
        return result
    result["steps"]["emit_native_aot"] = "OK"

    result["success"] = True
    trace("family_passed", family=family_slug, method_count=len(mids))
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description="Batch native AOT CodeGen pipeline")
    parser.add_argument("--assembly-name", default="System.Private.CoreLib", help="Assembly name to process")
    parser.add_argument("--trace", action="store_true", help="Enable JSONL trace logging")
    parser.add_argument("--families", nargs="*", help="Space-separated subset of family slugs to process. Auto-discovers from contracts if not specified.")
    args = parser.parse_args()

    global _VERIFICATION
    _VERIFICATION = _VERIFICATION_BASE / args.assembly_name

    if not _VERIFICATION.exists():
        print(f"FATAL: verification directory not found: {_VERIFICATION}", file=sys.stderr)
        sys.exit(1)

    if args.trace:
        trace_init(_REPO_ROOT, stage="batch-native-aot")
        print("[trace] JSONL trace enabled")

    families = args.families
    if not families:
        families = _discover_families(args.assembly_name)
        if not families:
            print(f"No families found for {args.assembly_name} (no contracts)")
            # Fall back to FAMILIES constant for backward compat
            families = FAMILIES

    print(f"Batch native AOT CodeGen pipeline - {len(families)} families")
    print(f"Assembly: {args.assembly_name}")
    print(f"Verification: {_VERIFICATION}")
    print()

    trace("batch_start", assembly=args.assembly_name, family_count=len(families))

    results = []
    passed = 0
    failed = 0

    for idx, family_slug in enumerate(families):
        family_result = run_family(family_slug, assembly_name=args.assembly_name)
        results.append(family_result)

        if family_result["success"]:
            passed += 1
            print(f"  >>> PASSED ({passed}/{idx+1})")
        else:
            failed += 1
            print(f"  >>> FAILED ({failed}/{idx+1}): {family_result.get('error', 'unknown')}")

    # Summary
    print(f"\n{'='*60}")
    print(f"{args.assembly_name}: {passed} passed, {failed} failed, {len(families)} total")
    print(f"{'='*60}")
    for r in results:
        status = "PASS" if r["success"] else "FAIL"
        steps = " -> ".join(f"{k}={v}" for k, v in r.get("steps", {}).items())
        print(f"  {status:4s}  {r['family']:35s}  {steps}")

    # Write results per-assembly
    output_path = _VERIFICATION / "reports" / "batch-native-aot-pipeline-results.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "assembly": args.assembly_name,
        "total": len(families),
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
