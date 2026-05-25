"""Codegen stage runners — entrypoint generation + IL2CPP compile + entry.exe build."""

from __future__ import annotations

import subprocess
import time
from pathlib import Path

from verification.orchestration.context import FamilyContext, StageResult
from verification.orchestration.family_entrypoint import generate_and_build
from verification.orchestration.dispatch_generator import generate_verification_dispatch

_REPO_ROOT = Path(__file__).resolve().parents[4]


def _read_contract(assembly: str, slug: str) -> dict | None:
    """Read contract from testing/ path."""
    testing_path = _REPO_ROOT / "testing" / "foundation-dll" / assembly / slug / "contract.json"
    if testing_path.exists():
        import json
        return json.loads(testing_path.read_text(encoding="utf-8"))
    return None


def _get_testing_base(assembly: str) -> Path:
    return _REPO_ROOT / "testing" / "foundation-dll" / assembly


def run_codegen(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 1: Entrypoint generation + IL2CPP compile (AOT)."""
    start = time.perf_counter()

    from verification.stages.pipeline_native_aot_runner import (
        build_entry_executable,
        build_subjects_dll,
        generate_coverage_json,
        run_convert_to_cpp,
    )

    testing_base = _get_testing_base(ctx.assembly)
    contract = _read_contract(ctx.assembly, ctx.slug)
    if contract is None:
        return StageResult(
            stage="codegen", status="failed",
            summary=f"Contract not found for {ctx.slug}/{ctx.assembly}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", [])]

    # 1. Generate C# entrypoint subjects
    print(f"  [codegen] Generating entrypoint...")
    family_id = contract.get("familyId", f"family/{ctx.assembly}/{ctx.slug.replace('-', '/')}")
    extra_refs = None
    if ctx.slug in ("snapshot-prover",):
        extra_refs = ["../../../../../tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/FixtureAssembly/SnapshotTestFixtures.csproj"]

    ep_result = generate_and_build(
        ctx.managed_dir,
        assembly_name=ctx.assembly,
        family_id=family_id,
        method_subject_ids=mids,
        class_name=None,
        variant="subjects",
        extra_refs=extra_refs,
    )
    if not ep_result.get("success", False):
        return StageResult(
            stage="codegen", status="failed",
            summary=f"Entrypoint generation failed: {ep_result.get('error', 'unknown')}",
            details={"steps": {"entrypoint": ep_result}},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # 2. Run chaos-il2cpp convert-to-cpp
    print(f"  [codegen] Running IL2CPP codegen...")
    build_result = build_subjects_dll(ctx.slug, mids, assembly_name=ctx.assembly, verification=testing_base)
    if not build_result.get("success"):
        return StageResult(
            stage="codegen", status="failed",
            summary=f"subjects DLL build failed: {build_result.get('error', 'unknown')}",
            details={"steps": {"subjects_build": build_result}},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    dll_path = Path(build_result["dll_path"])
    codegen_dir = testing_base / ctx.slug / "codegen"

    # Isolate subjects DLL into clean input directory
    clean_input_dir = codegen_dir / "_subjects_input"
    clean_input_dir.mkdir(parents=True, exist_ok=True)
    import shutil
    clean_dll = clean_input_dir / dll_path.name
    shutil.copy2(str(dll_path), str(clean_dll))
    for ext in (".pdb", ".deps.json"):
        src = dll_path.with_suffix(ext)
        if src.exists():
            shutil.copy2(str(src), clean_input_dir / src.name)

    if not run_convert_to_cpp(
        ctx.slug,
        str(clean_dll),
        verification=testing_base,
        entry_point_subject_id="",
        codegen_mode=ctx.codegen_mode,
    ):
        return StageResult(
            stage="codegen", status="failed",
            summary="convert-to-cpp failed",
            details={"steps": {"convert_to_cpp": {"error": "see logs"}}},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    generate_coverage_json(ctx.slug, ctx.assembly, mids, verification=testing_base)

    # 3. Build entry.exe (full clean build with sentinel dispatch)
    print(f"  [codegen] Building entry.exe...")
    build_ok = build_entry_executable(ctx.slug, verification=testing_base)
    if not build_ok:
        return StageResult(
            stage="codegen", status="failed",
            summary="entry.exe build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # 4. Generate verification dispatch code (overwrites sentinel, incremental rebuild)
    print(f"  [codegen] Generating dispatch code...")
    codegen_dir_path = testing_base / ctx.slug / "codegen"
    manifest_path = None
    for d in codegen_dir_path.iterdir():
        if d.is_dir() and d.name.endswith("Subjects"):
            candidate = d / "native-aot.methods.json"
            if candidate.exists():
                manifest_path = candidate
                break
            candidate = d / "generated" / "native-aot.methods.json"
            if candidate.exists():
                manifest_path = candidate
                break
    if manifest_path is None:
        flat_manifest = codegen_dir_path / "generated" / "native-aot.methods.json"
        if flat_manifest.exists():
            manifest_path = flat_manifest

    if manifest_path:
        dispatch_output = testing_base / ctx.slug / "native" / "verification_dispatch.generated.cpp"
        generate_verification_dispatch(str(manifest_path), str(dispatch_output))
        print(f"  [codegen] Regenerated verification_dispatch.generated.cpp")

        # Incremental rebuild
        native_dir = testing_base / ctx.slug / "native"
        build_dir = native_dir / "build"
        if build_dir.exists():
            for obj in build_dir.rglob("verification_dispatch*"):
                if obj.is_file():
                    obj.unlink()
            rebuild = subprocess.run(
                ["cmake", "--build", str(build_dir), "--config", "RelWithDebInfo", "--target", "entry"],
                capture_output=True, text=True, timeout=300)
            if rebuild.returncode != 0:
                print(f"  [codegen] WARNING: dispatch rebuild failed (continuing)")
    else:
        print(f"  [codegen] WARNING: manifest not found, dispatch generation skipped")

    # 5. Save AOT binary
    native_dir = testing_base / ctx.slug / "native"
    entry_exe = native_dir / "entry.exe"
    aot_exe = native_dir / "entry-aot.exe"
    if entry_exe.exists() and not aot_exe.exists():
        import shutil as _shutil
        _shutil.copy2(str(entry_exe), str(aot_exe))
        print(f"  [codegen] saved entry.exe -> entry-aot.exe")

    return StageResult(
        stage="codegen", status="passed",
        summary="Entrypoint built and IL2CPP compile OK",
        details={
            "methodCount": len(mids),
            "dllPath": str(dll_path),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_jit_codegen(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 2: JIT entrypoint generation + entry-jit.exe build."""
    start = time.perf_counter()

    jit_exe = ctx.entry_jit_exe_path

    if jit_exe.exists():
        return StageResult(
            stage="jit_codegen", status="passed",
            summary="entry-jit.exe already exists, skipping rebuild",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [jit_codegen] Building JIT mode entry-jit.exe...")
    from verification.stages.pipeline_native_aot_runner import run_family as _run_old_family

    testing_base = _get_testing_base(ctx.assembly)
    native_dir = testing_base / ctx.slug / "native"
    entry_exe = native_dir / "entry.exe"
    aot_backup = native_dir / "entry-aot.exe"

    if entry_exe.exists() and not aot_backup.exists():
        import shutil as _shutil
        _shutil.copy2(str(entry_exe), str(aot_backup))
        print(f"  [jit_codegen] backed up entry.exe -> entry-aot.exe")

    jit_result = _run_old_family(ctx.slug, assembly_name=ctx.assembly, codegen_mode="jit")
    if not jit_result.get("success"):
        if aot_backup.exists():
            import shutil as _shutil
            _shutil.copy2(str(aot_backup), str(entry_exe))
        return StageResult(
            stage="jit_codegen", status="failed",
            summary=f"JIT codegen failed: {jit_result.get('message', 'unknown')}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if entry_exe.exists():
        import shutil as _shutil
        _shutil.copy2(str(entry_exe), str(jit_exe))
        print(f"  [jit_codegen] entry.exe -> entry-jit.exe ({jit_exe.stat().st_size} bytes)")

    if aot_backup.exists():
        import shutil as _shutil
        _shutil.copy2(str(aot_backup), str(entry_exe))
        print(f"  [jit_codegen] restored entry.exe from entry-aot.exe")

    return StageResult(
        stage="jit_codegen", status="passed",
        summary="JIT entry-jit.exe built and AOT entry.exe restored",
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
