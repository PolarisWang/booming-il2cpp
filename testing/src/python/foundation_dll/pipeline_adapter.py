"""Pipeline Adapter — bridges old foundation_dll pipeline modules into the new framework.

During transition, the new orchestrator delegates to existing Python modules in
build/toolchains/run/testing/foundation_dll/ via this adapter. As each module is
migrated, its adapter function can be swapped to call the new _core implementation.

Phase 2 (current): All codegen stages use testing/ paths directly by calling
  individual step functions with verification=testing_base, skipping run_family()
  which hardcodes verification/ paths.

Migration Phases:
  Phase 1: Adapter routes through old modules with verification/ paths + sync
  Phase 2 (current): Codegen stages call step functions with testing/ paths
  Phase 3: Migrate audit, fact, and remaining stages to _core/
  Phase 4: Adapter removed, old modules archived
"""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any

# ── Old pipeline path ────────────────────────────────────────────────
_OLD_PIPELINE = (
    Path(__file__).resolve().parents[4]  # → agent/booming-il2cpp
    / "build" / "toolchains" / "run" / "testing" / "foundation_dll"
)
if str(_OLD_PIPELINE) not in sys.path:
    sys.path.insert(0, str(_OLD_PIPELINE))

_REPO_ROOT = _OLD_PIPELINE.parents[4]


def _slug_to_subjects_class(slug: str) -> str:
    """Derive subjects class name from family slug (e.g. convert-char → ConvertCharSubjects)."""
    return slug.title().replace("-", "").replace("_", "").replace(",", "") + "Subjects"


def _read_contract(assembly: str, slug: str) -> dict[str, Any] | None:
    """Read contract from testing/ path first, fall back to verification/."""
    # Prefer testing/ path (new framework)
    testing_path = _REPO_ROOT / "testing" / "foundation-dll" / assembly / slug / "contract.json"
    if testing_path.exists():
        return json.loads(testing_path.read_text(encoding="utf-8"))

    # Fallback: verification/ path (legacy)
    verification_path = (
        _REPO_ROOT / "verification" / "foundation-dll" / assembly / slug
        / "capability-family-contract.json"
    )
    if verification_path.exists():
        return json.loads(verification_path.read_text(encoding="utf-8"))

    return None


def generate_entrypoint(slug: str, assembly: str, output_dir: Path) -> dict[str, Any]:
    """Generate C# entrypoint subjects (delegates to family_entrypoint_generator).

    Returns {"success": True/False, "methodCount": N, "dll_path": "...", ...}
    """
    from family_entrypoint_generator import generate_and_build

    contract = _read_contract(assembly, slug)
    if contract is None:
        return {"success": False, "error": f"Contract not found for {slug}/{assembly}"}

    family_id = contract.get("familyId", f"family/{assembly}/{slug.replace('-', '/')}")
    method_subject_ids = contract.get("methodSubjectIds", [])
    if not method_subject_ids:
        method_subject_ids = [m["methodSubjectId"] for m in contract.get("methodContracts", [])]

    result = generate_and_build(
        output_dir,
        assembly_name=assembly,
        family_id=family_id,
        method_subject_ids=method_subject_ids,
        class_name=None,  # auto-derived
        variant="subjects",
    )
    return result


def run_il2cpp_codegen(
    slug: str,
    assembly: str,
    codegen_mode: str | None = None,
) -> dict[str, Any]:
    """Run chaos-il2cpp convert-to-cpp on the pre-built subjects DLL.

    Uses testing/ paths directly instead of calling run_family() which
    hardcodes verification/ paths.

    Returns {"success": True/False, "methodCount": N, "dllPath": "...", ...}
    """
    from pipeline_native_aot_runner import (
        _codegen_patch_undefined_labels,
        _generate_coverage_json,
        _load_method_subject_ids,
        _run_convert_to_cpp,
    )

    contract = _read_contract(assembly, slug)
    if contract is None:
        return {"success": False, "error": f"Contract not found for {slug}/{assembly}"}

    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", [])]
    if not mids:
        return {"success": False, "error": "no method subject IDs"}

    # Derive class name and find the pre-built DLL
    class_name = _slug_to_subjects_class(slug)
    testing_base = _REPO_ROOT / "testing" / "foundation-dll" / assembly
    dll_path = testing_base / slug / "managed" / "build-output" / f"{class_name}.dll"

    if not dll_path.exists():
        return {
            "success": False,
            "error": f"Pre-built subjects DLL not found at {dll_path} (generate_entrypoint may have failed)",
        }

    # Run convert-to-cpp → testing/.../codegen/
    if not _run_convert_to_cpp(
        slug,
        str(dll_path),
        verification=testing_base,
        entry_point_subject_id="",  # subjects variant has no entry point override
        codegen_mode=codegen_mode,
    ):
        return {"success": False, "error": "convert-to-cpp failed", "methodCount": len(mids)}

    # Patch undefined branch target labels
    _codegen_patch_undefined_labels(slug, verification=testing_base)

    # Generate coverage JSON for dashboard/kernel
    _generate_coverage_json(slug, assembly, mids, verification=testing_base)

    return {
        "success": True,
        "methodCount": len(mids),
        "dllPath": str(dll_path),
    }


def generate_dispatch_code(slug: str, assembly: str) -> bool:
    """Generate verification_dispatch.generated.cpp from codegen manifest."""
    from generate_verification_dispatch import generate_verification_dispatch
    from pipeline_native_aot_runner import _build_entry_exe

    testing_base = _REPO_ROOT / "testing" / "foundation-dll" / assembly
    codegen_dir = testing_base / slug / "codegen"

    if not codegen_dir.exists():
        print(f"  [adapter] codegen dir not found: {codegen_dir}")
        return False

    manifest_path = None
    for d in codegen_dir.iterdir():
        if d.is_dir() and d.name.endswith("Subjects"):
            candidate = d / "native-aot.methods.json"
            if candidate.exists():
                manifest_path = candidate
                break

    if manifest_path is None:
        print(f"  [adapter] manifest not found in {codegen_dir}")
        return False

    dispatch_output = testing_base / slug / "native" / "verification_dispatch.generated.cpp"
    generate_verification_dispatch(str(manifest_path), str(dispatch_output))

    # Rebuild entry.exe with dispatch code
    rebuild_ok = _build_entry_exe(slug, verification=testing_base)
    if not rebuild_ok:
        print(f"  [adapter] entry.exe rebuild with dispatch code FAILED")
        return False

    return True


def build_entry_exe(slug: str, assembly: str) -> bool:
    """Build entry.exe via CMake under testing/ paths."""
    from pipeline_native_aot_runner import _build_entry_exe

    testing_base = _REPO_ROOT / "testing" / "foundation-dll" / assembly
    ok = _build_entry_exe(slug, verification=testing_base)
    return ok


def save_aot_binary(slug: str, assembly: str) -> bool:
    """Save entry.exe as entry-aot.exe under testing/ paths."""
    family_dir = _REPO_ROOT / "testing" / "foundation-dll" / assembly / slug
    native_dir = family_dir / "native"
    entry_exe = native_dir / "entry.exe"
    aot_exe = native_dir / "entry-aot.exe"

    if entry_exe.exists() and not aot_exe.exists():
        shutil.copy2(str(entry_exe), str(aot_exe))
        print(f"  [adapter] saved entry.exe -> entry-aot.exe")
        return True
    return False


def restore_aot_binary(slug: str, assembly: str) -> bool:
    """Restore entry.exe from entry-aot.exe backup under testing/ paths."""
    family_dir = _REPO_ROOT / "testing" / "foundation-dll" / assembly / slug
    native_dir = family_dir / "native"
    entry_exe = native_dir / "entry.exe"
    aot_exe = native_dir / "entry-aot.exe"

    if aot_exe.exists():
        shutil.copy2(str(aot_exe), str(entry_exe))
        print(f"  [adapter] restored entry.exe <- entry-aot.exe")
        return True
    return False


def run_fact_verify(slug: str, assembly: str) -> dict[str, Any]:
    """Run native entry.exe and verify fact assertions."""
    from fact_verifier import verify_fact

    result = verify_fact(slug, assembly=assembly)
    return result


def run_audit(slug: str, assembly: str) -> dict[str, Any]:
    """Run mechanism + principle audit.

    TODO(Phase 3): mechanism_audit.py hardcodes _VERIFICATION_BASE. When
    migrated, pass testing/ path directly. Currently reads from verification/
    which may be stale for actively migrated families.
    """
    from mechanism_audit import run_full_audit

    result = run_full_audit(assembly, slug)
    return result


def run_jit_codegen(slug: str, assembly: str) -> dict[str, Any]:
    """Build JIT-mode entry-jit.exe, then restore AOT entry.exe.

    TODO(Phase 3): run_family() still writes JIT codegen to verification/ paths.
    Build from verification/ for now, then copy result to testing/.
    """
    from pipeline_native_aot_runner import run_family, _build_entry_exe

    testing_base = _REPO_ROOT / "testing" / "foundation-dll" / assembly
    native_dir = testing_base / slug / "native"
    jit_exe = native_dir / "entry-jit.exe"

    if jit_exe.exists():
        return {"success": True, "message": "entry-jit.exe already exists"}

    # JIT codegen — run_family writes to verification/ (known limitation, Phase 3)
    jit_result = run_family(slug, assembly_name=assembly, codegen_mode="jit")
    if not jit_result.get("success"):
        return {"success": False, "message": "JIT codegen failed"}

    # Build entry-jit.exe from verification/ path (where codegen was written)
    old_verification = _REPO_ROOT / "verification" / "foundation-dll" / assembly
    build_ok = _build_entry_exe(slug, verification=old_verification, output_name="entry-jit.exe")
    if not build_ok:
        return {"success": False, "message": "entry-jit.exe build failed"}

    # Copy result from verification/ to testing/
    src = old_verification / slug / "native" / "entry-jit.exe"
    if src.exists():
        native_dir.mkdir(parents=True, exist_ok=True)
        shutil.copy2(str(src), str(jit_exe))
        print(f"  [adapter] copied entry-jit.exe from verification/ to testing/")

    # Restore AOT-mode entry.exe (from testing/ backup)
    restore_aot_binary(slug, assembly)

    return {"success": True}


def run_fact_jit(slug: str, assembly: str) -> dict[str, Any]:
    """Run entry-jit.exe for fact verification through interpreter dispatch."""
    from fact_verifier import verify_fact

    testing_base = _REPO_ROOT / "testing" / "foundation-dll" / assembly
    jit_exe = testing_base / slug / "native" / "entry-jit.exe"

    result = verify_fact(slug, assembly=assembly, exe_path=jit_exe)
    return result


def generate_managed_benchmark(slug: str, assembly: str, method_subject_ids: list[str]) -> Path | None:
    """Auto-generate managed benchmark harness for the family."""
    from family_verification_orchestrator import _auto_generate_managed_benchmark

    return _auto_generate_managed_benchmark(slug, assembly, method_subject_ids)
