"""Codegen stage runners — entrypoint generation + IL2CPP compile + entry.exe build."""

from __future__ import annotations

import json
import shutil
import subprocess
import time
from pathlib import Path

from verification.orchestration.context import FamilyContext, StageResult
from verification.orchestration.family_entrypoint import generate_and_build

_REPO_ROOT = Path(__file__).resolve().parents[4]


def _find_contract(assembly: str, slug: str) -> Path | None:
    """Find the contract file for a family."""
    for fname in ("contract.json", "capability-family-contract.json"):
        path = _REPO_ROOT / "testing" / "foundation-dll" / assembly / slug / fname
        if path.exists():
            return path
    return None


def _run_test_project_generator_emit(contract_path: Path | None, output_dir: Path, extra_flags: list[str] | None = None) -> bool:
    """Run TestProjectGenerator emit to generate verification_dispatch.generated.cpp.

    Args:
        extra_flags: Additional flags to pass to TPG (e.g. ["--jit"], ["--config-tier", "profile"]).

    Returns True if successful, False otherwise (caller should fall back to sentinel).
    """
    if contract_path is None or not contract_path.exists():
        print(f"  [codegen] TestProjectGenerator: no contract at {contract_path}")
        return False

    generator_proj = _REPO_ROOT / "src" / "tools" / "Chaos.IL2CPP.Tools.TestProjectGenerator"
    generator_dll = generator_proj / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Tools.TestProjectGenerator.dll"

    # Build the generator tool if DLL doesn't exist
    if not generator_dll.exists():
        print(f"  [codegen] Building TestProjectGenerator...")
        build_result = subprocess.run(
            ["dotnet", "build", str(generator_proj), "-c", "Release", "--nologo", "-v", "quiet"],
            capture_output=True, text=True, timeout=120)
        if build_result.returncode != 0:
            print(f"  [codegen] TestProjectGenerator build FAILED: {build_result.stderr[:200]}")
            return False
        if not generator_dll.exists():
            print(f"  [codegen] TestProjectGenerator DLL not found after build: {generator_dll}")
            return False

    cmd = ["dotnet", "exec", str(generator_dll), "emit",
           "--contract", str(contract_path),
           "--output", str(output_dir)]
    if extra_flags:
        cmd.extend(extra_flags)
    result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)

    if result.returncode != 0:
        print(f"  [codegen] TestProjectGenerator emit FAILED (rc={result.returncode})")
        for line in (result.stderr.splitlines() + result.stdout.splitlines())[-10:]:
            print(f"      {line}")
        return False

    # Print generator output
    for line in result.stdout.splitlines():
        print(f"      {line}")
    for line in result.stderr.splitlines():
        if "TIMING" in line:
            print(f"      {line}")

    return True


def _incremental_dispatch_rebuild(native_dir: Path) -> None:
    """Incremental cmake rebuild after dispatch regeneration.

    Deletes stale object files for dispatch and runtime-entry so that
    Ninja recompiles them from the TPG-emitted sources (which include
    --fact-json support that the old committed runtime-entry.cpp lacks).
    """
    build_dir = native_dir / "build"
    if not build_dir.exists():
        return
    for pattern in ("verification_dispatch*", "runtime-entry*"):
        for obj in build_dir.rglob(pattern):
            if obj.is_file():
                obj.unlink()
    rebuild = subprocess.run(
        ["cmake", "--build", str(build_dir), "--config", "RelWithDebInfo", "--target", "entry"],
        capture_output=True, text=True, timeout=300)
    if rebuild.returncode != 0:
        print(f"  [codegen] WARNING: dispatch rebuild failed (continuing)")


def _read_contract(assembly: str, slug: str) -> dict | None:
    """Read contract from testing/ path (contract.json or capability-family-contract.json)."""
    for fname in ("contract.json", "capability-family-contract.json"):
        testing_path = _REPO_ROOT / "testing" / "foundation-dll" / assembly / slug / fname
        if testing_path.exists():
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

    if not mids:
        return StageResult(
            stage="codegen", status="passed",
            summary="0 methods — no codegen needed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # 1. Generate C# entrypoint subjects
    print(f"  [codegen] Generating entrypoint...")
    family_id = contract.get("familyId", f"family/{ctx.assembly}/{ctx.slug.replace('-', '/')}")
    extra_refs = None
    if ctx.slug in ("snapshot-prover",):
        extra_refs = ["../../../../../tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/FixtureAssembly/SnapshotTestFixtures.csproj"]
    elif ctx.slug in ("generic-supplement",):
        extra_refs = ["../../../../../src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj"]

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

    # 4. Generate verification dispatch code via TestProjectGenerator (overwrites sentinel, incremental rebuild)
    print(f"  [codegen] Generating dispatch code via TestProjectGenerator...")
    native_dir = testing_base / ctx.slug / "native"
    contract_path = _find_contract(ctx.assembly, ctx.slug)

    tpg_flags = ["--config-tier", ctx.native_config]
    if not _run_test_project_generator_emit(contract_path, native_dir, extra_flags=tpg_flags):
        print(f"  [codegen] WARNING: TestProjectGenerator emit failed (continuing with sentinel dispatch)")
    else:
        print(f"  [codegen] Regenerated verification_dispatch.generated.cpp via TestProjectGenerator")

    # Incremental rebuild after dispatch regeneration
    _incremental_dispatch_rebuild(native_dir)

    # 5. Save AOT binary
    native_dir = testing_base / ctx.slug / "native"
    entry_exe = native_dir / "entry.exe"
    aot_exe = native_dir / "entry-aot.exe"
    if entry_exe.exists():
        if not aot_exe.exists() or entry_exe.stat().st_mtime > aot_exe.stat().st_mtime:
            shutil.copy2(str(entry_exe), str(aot_exe))
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
        # Check if TPG templates have been updated since binary was built
        # (the dispatch is generated by TestProjectGenerator, not dispatch_generator.py)
        tpg_templates = Path(__file__).resolve().parent.parent.parent.parent / "src" / "tools" / "Chaos.IL2CPP.Tools.TestProjectGenerator" / "Templates"
        tpg_mtime = max(
            (f.stat().st_mtime for f in tpg_templates.rglob("*.scriban") if f.is_file()),
            default=0,
        )
        if tpg_mtime > 0 and jit_exe.stat().st_mtime >= tpg_mtime:
            return StageResult(
                stage="jit_codegen", status="passed",
                summary="entry-jit.exe already exists, skipping rebuild",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )
        print(f"  [jit_codegen] TPG templates updated, rebuilding stale entry-jit.exe...")

    print(f"  [jit_codegen] Building JIT mode entry-jit.exe...")

    # Check if there are methods to codegen
    from verification.orchestration.context import FamilyContext
    contract = _read_contract(ctx.assembly, ctx.slug)
    if contract is not None:
        mids = contract.get("methodSubjectIds", [])
        if not mids:
            mids = [m["methodSubjectId"] for m in contract.get("methodContracts", [])]
        if not mids:
            return StageResult(
                stage="jit_codegen", status="passed",
                summary="0 methods — no JIT codegen needed",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

    from verification.stages.pipeline_native_aot_runner import run_family as _run_old_family

    testing_base = _get_testing_base(ctx.assembly)
    native_dir = testing_base / ctx.slug / "native"
    entry_exe = native_dir / "entry.exe"
    aot_backup = native_dir / "entry-aot.exe"

    if entry_exe.exists():
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

    # Regenerate dispatch via TPG with JIT mode, then incremental rebuild
    contract_path = _find_contract(ctx.assembly, ctx.slug)
    tpg_flags = ["--jit", "--config-tier", ctx.native_config]
    if _run_test_project_generator_emit(contract_path, native_dir, extra_flags=tpg_flags):
        print(f"  [jit_codegen] Regenerated dispatch via TPG (JIT mode)")
        _incremental_dispatch_rebuild(native_dir)
    else:
        print(f"  [jit_codegen] WARNING: TPG emit failed (continuing with existing dispatch)")

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
