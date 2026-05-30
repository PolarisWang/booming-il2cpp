"""Codegen stage runners — entrypoint generation + IL2CPP compile + entry.exe build."""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import time
from pathlib import Path

from verification.orchestration.context import FamilyContext, StageResult, resolve_contract_path, load_contract
from verification.orchestration.family_entrypoint import generate_and_build

_REPO_ROOT = Path(__file__).resolve().parents[4]


def _find_contract(assembly: str, slug: str) -> Path | None:
    """Find the contract file for a family using canonical path."""
    from verification.orchestration.context import resolve_contract_path
    from verification._path import _HERE as _VERIFICATION_ROOT
    path = resolve_contract_path(_VERIFICATION_ROOT.parent / assembly / slug)
    return path if path.exists() else None


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


def _incremental_dispatch_rebuild(native_dir: Path, build_subdir: str = "build") -> None:
    """Incremental cmake rebuild after dispatch regeneration.

    Deletes stale object files for dispatch and runtime-entry so that
    Ninja recompiles them from the TPG-emitted sources (which include
    --fact-json support that the old committed runtime-entry.cpp lacks).

    Retries with exponential backoff on Windows file-lock races
    (the parallel AOT build may still hold handles on .obj files).

    Args:
        build_subdir: cmake build subdirectory name ("build" for AOT, "build_jit" for JIT).
    """
    import time as _time
    build_dir = native_dir / build_subdir
    if not build_dir.exists():
        return
    for pattern in ("verification_dispatch*", "runtime-entry*"):
        for obj in build_dir.rglob(pattern):
            if not obj.is_file():
                continue
            for _attempt in range(5):
                try:
                    obj.unlink()
                    break
                except (PermissionError, OSError):
                    if _attempt < 4:
                        _time.sleep(1 << _attempt)
                    else:
                        print(f"  [codegen] WARNING: could not remove stale {obj.name}: file locked")


# Sentinel dispatch detection — patterns characteristic of the
# placeholder stub (vs. real TPG-generated dispatch code).
_SENTINEL_MARKER_RE = re.compile(
    r'sentinel\s*\(pre-(codegen|TPG)\)',
    re.IGNORECASE,
)


def _is_sentinel_dispatch(dispatch_cpp: Path) -> bool:
    """Check whether verification_dispatch.generated.cpp is a sentinel stub.

    Returns True if the file:
      - Does not exist, OR
      - Contains the sentinel marker comment, OR
      - Lacks a ChaosDispatchMethodAllModules / ChaosDispatchMethod call
        (which every real TPG-generated dispatch includes).
    """
    if not dispatch_cpp.is_file():
        return True
    try:
        text = dispatch_cpp.read_text(encoding="utf-8")
    except OSError:
        return True

    # Explicit sentinel marker
    if _SENTINEL_MARKER_RE.search(text):
        return True

    # Real dispatch always calls through the AOT dispatch API
    if "ChaosDispatchMethodAllModules" in text or "ChaosDispatchMethod" in text:
        return False

    # Falls through — no sentinel marker and no dispatch call.
    # Conservative: treat as sentinel so the pipeline fails closed.
    return True


def _find_built_exe(build_dir: Path) -> Path | None:
    """Locate ``entry.exe`` (or ``entry`` on Linux) in a cmake build output dir.

    MSVC places the binary under a ``RelWithDebInfo`` / ``Release`` / ``Debug``
    sub-directory; Ninja places it directly in ``build_dir``.
    """
    for candidate in (
        build_dir / "RelWithDebInfo" / "entry.exe",
        build_dir / "Release" / "entry.exe",
        build_dir / "Debug" / "entry.exe",
        build_dir / "entry.exe",
        build_dir / "entry",
    ):
        if candidate.exists():
            return candidate
    return None


def _read_contract(assembly: str, slug: str) -> dict | None:
    """Read contract using canonical loader from context."""
    from verification._path import _HERE as _VERIFICATION_ROOT
    family_dir = _VERIFICATION_ROOT.parent / assembly / slug
    return load_contract(family_dir)


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

    # ── Step timing tracking ──
    step_times: dict[str, float] = {}
    _step = time.perf_counter()

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

    step_times["entrypoint"] = time.perf_counter() - _step
    _step = time.perf_counter()
    print(f"    [codegen timing] entrypoint + subjects build: {step_times['entrypoint']:.2f}s")

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

    step_times["il2cpp_codegen"] = time.perf_counter() - _step
    _step = time.perf_counter()
    print(f"    [codegen timing] IL2CPP codegen: {step_times['il2cpp_codegen']:.2f}s")

    # 3. Generate verification dispatch code via TestProjectGenerator (before native builds
    #    so both AOT and JIT compile the real dispatch — no sentinel + incremental rebuild)
    print(f"  [codegen] Generating dispatch code via TestProjectGenerator...")
    native_dir = testing_base / ctx.slug / "native"
    contract_path = _find_contract(ctx.assembly, ctx.slug)
    dispatch_cpp = native_dir / "verification_dispatch.generated.cpp"

    tpg_flags = ["--config-tier", ctx.native_config]
    tpg_start = time.perf_counter()
    tpg_ok = _run_test_project_generator_emit(contract_path, native_dir, extra_flags=tpg_flags)
    tpg_elapsed = time.perf_counter() - tpg_start
    print(f"    [TPG timing] emit took {tpg_elapsed:.2f}s")

    # Validate: must be real dispatch, not sentinel
    if not tpg_ok or _is_sentinel_dispatch(dispatch_cpp):
        return StageResult(
            stage="codegen", status="failed",
            summary=(
                "TestProjectGenerator emit FAILED — verification_dispatch.generated.cpp "
                "is still a sentinel stub. All fact/benchmark stages would silently pass "
                "without running any test. Fix TPG configuration or contract before retrying."
            ),
            details={"steps": {"tpg_emit": {"success": tpg_ok}}},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [codegen] Regenerated verification_dispatch.generated.cpp via TestProjectGenerator")
    step_times["tpg"] = time.perf_counter() - _step
    _step = time.perf_counter()
    print(f"    [codegen timing] TPG emit: {step_times['tpg']:.2f}s")

    # 4. Prepare native directory (clean, sync codegen→native, sentinels, libs)
    print(f"  [codegen] Preparing native directory...")
    build_entry_executable(
        ctx.slug, verification=testing_base, config_tier=ctx.native_config,
        prep_only=True,
    )

    # 5. Build AOT entry.exe and JIT entry-jit.exe in parallel.
    # AOT uses Visual Studio generator; JIT uses Ninja generator (avoids
    # MSBuild file-lock contention between two concurrent cmake builds).
    # Build directories and output binaries are separate:
    #   AOT: native/build/ → entry.exe
    #   JIT: native/build_jit/ → entry-jit.exe
    jit_exe = native_dir / "entry-jit.exe"
    jit_needs_build = not jit_exe.exists()

    if jit_needs_build:
        print(f"  [codegen] Building AOT entry.exe + JIT entry-jit.exe (parallel)...")
        import os as _os
        from concurrent.futures import ThreadPoolExecutor
        half_cores = max(1, _os.cpu_count() // 2) if _os.cpu_count() else 4
        aot_future = None
        jit_future = None
        with ThreadPoolExecutor(max_workers=2) as pool:
            aot_future = pool.submit(
                build_entry_executable, ctx.slug,
                verification=testing_base, config_tier=ctx.native_config,
                is_jit=False, skip_prep=True, parallel_jobs=half_cores,
            )
            jit_future = pool.submit(
                build_entry_executable, ctx.slug,
                verification=testing_base, config_tier=ctx.native_config,
                is_jit=True, output_name="entry-jit.exe", skip_prep=True,
                parallel_jobs=half_cores,
            )
            aot_ok = aot_future.result()
            jit_ok = jit_future.result()
    else:
        print(f"  [codegen] Building AOT entry.exe (JIT cached)...")
        aot_ok = build_entry_executable(
            ctx.slug,
            verification=testing_base, config_tier=ctx.native_config,
            is_jit=False, skip_prep=True,
        )
        jit_ok = True

    if not aot_ok:
        return StageResult(
            stage="codegen", status="failed",
            summary="entry.exe build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    step_times["native_builds"] = time.perf_counter() - _step
    _step = time.perf_counter()
    print(f"    [codegen timing] native builds: {step_times['native_builds']:.2f}s")

    if jit_needs_build:
        if jit_ok:
            print(f"  [codegen] JIT entry-jit.exe built successfully")
        else:
            print(f"  [codegen] WARNING: JIT entry-jit.exe build failed (jit_codegen stage will rebuild)")
    else:
        print(f"  [codegen] entry-jit.exe already exists, skipping JIT pre-build")

    # 6. Save AOT binary for jit_codegen stage restore
    entry_exe = native_dir / "entry.exe"
    aot_exe = native_dir / "entry-aot.exe"
    if entry_exe.exists():
        if not aot_exe.exists() or entry_exe.stat().st_mtime > aot_exe.stat().st_mtime:
            shutil.copy2(str(entry_exe), str(aot_exe))
            print(f"  [codegen] saved entry.exe -> entry-aot.exe")

    total_codegen = time.perf_counter() - start
    print(f"    [codegen timing] TOTAL: {total_codegen:.2f}s"
          f"  (entrypoint={step_times['entrypoint']:.2f}s"
          f"  il2cpp={step_times['il2cpp_codegen']:.2f}s"
          f"  tpg={step_times['tpg']:.2f}s"
          f"  native_builds={step_times['native_builds']:.2f}s)")

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

    testing_base = _get_testing_base(ctx.assembly)
    native_dir = testing_base / ctx.slug / "native"
    entry_exe = native_dir / "entry.exe"
    aot_backup = native_dir / "entry-aot.exe"

    if entry_exe.exists():
        import shutil as _shutil
        _shutil.copy2(str(entry_exe), str(aot_backup))
        print(f"  [jit_codegen] backed up entry.exe -> entry-aot.exe")

    # If codegen stage already pre-built entry-jit.exe, skip the full cmake configure
    # and go straight to TPG emit + incremental rebuild.  Otherwise build from existing
    # codegen output via build_entry_executable (no subjects DLL or convert-to-cpp).
    if jit_exe.exists():
        print(f"  [jit_codegen] using pre-built JIT binary from codegen stage, incremental update only...")
        jit_build_ok = True
    else:
        # Build entry-jit.exe directly from existing codegen output (no subjects DLL or
        # convert-to-cpp).  This is the same build_entry_executable() that run_codegen()
        # uses for the pre-build — avoid the old run_family() pipeline entirely since it
        # can re-compile entry.exe with stale dispatch files from git history.
        from verification.stages.pipeline_native_aot_runner import build_entry_executable
        jit_build_ok = build_entry_executable(
            ctx.slug, verification=testing_base, config_tier=ctx.native_config,
            is_jit=True, output_name="entry-jit.exe",
        )
        if not jit_build_ok:
            if aot_backup.exists():
                import shutil as _shutil
                _shutil.copy2(str(aot_backup), str(entry_exe))
            return StageResult(
                stage="jit_codegen", status="failed",
                summary=f"JIT entry-jit.exe build failed (old pipeline fallback removed)",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

    # Regenerate dispatch via TPG with JIT mode, then incremental rebuild
    contract_path = _find_contract(ctx.assembly, ctx.slug)
    tpg_flags = ["--jit", "--config-tier", ctx.native_config]
    dispatch_cpp = native_dir / "verification_dispatch.generated.cpp"
    tpg_start = time.perf_counter()
    tpg_ok = _run_test_project_generator_emit(contract_path, native_dir, extra_flags=tpg_flags)
    tpg_elapsed = time.perf_counter() - tpg_start
    print(f"    [TPG timing] JIT emit took {tpg_elapsed:.2f}s")
    if tpg_ok:
        print(f"  [jit_codegen] Regenerated dispatch via TPG (JIT mode)")
        _incremental_dispatch_rebuild(native_dir, build_subdir="build_jit")
        # Rebuild JIT binary with the new dispatch file
        build_jit_dir = native_dir / "build_jit"
        print(f"  [jit_codegen] Rebuilding JIT binary after TPG emit...")
        rebuild_r = subprocess.run(
            ["cmake", "--build", str(build_jit_dir), "--target", "entry", "--parallel", "--config", "RelWithDebInfo"],
            capture_output=True, text=True, timeout=300,
        )
        if rebuild_r.returncode == 0:
            # Locate rebuilt binary and copy to entry-jit.exe
            jit_built = _find_built_exe(build_jit_dir)
            if jit_built and jit_built.exists():
                import shutil as _shutil
                import time as _time
                for _copy_attempt in range(5):
                    try:
                        if jit_exe.exists():
                            jit_exe.unlink()
                        _shutil.copy2(str(jit_built), str(jit_exe))
                        print(f"  [jit_codegen] rebuilt entry-jit.exe ({jit_exe.stat().st_size} bytes)")
                        break
                    except (PermissionError, OSError) as _e:
                        if _copy_attempt < 4:
                            _time.sleep(1 << _copy_attempt)
                        else:
                            print(f"  [jit_codegen] WARNING: could not save entry-jit.exe: {_e}")
        else:
            print(f"  [jit_codegen] WARNING: JIT incremental rebuild failed, keeping existing entry-jit.exe")
            for line in (rebuild_r.stderr.splitlines() + rebuild_r.stdout.splitlines())[-10:]:
                print(f"      {line}")
    else:
        # AOT codegen already validated the dispatch — safe to reuse
        if _is_sentinel_dispatch(dispatch_cpp):
            return StageResult(
                stage="jit_codegen", status="failed",
                summary="No valid dispatch file available for JIT (AOT codegen did not produce one either)",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )
        print(f"  [jit_codegen] TPG emit failed, reusing AOT-generated dispatch")

    # Restore AOT entry.exe from backup
    if aot_backup.exists():
        import shutil as _shutil
        import time as _time
        for _copy_attempt in range(5):
            try:
                if entry_exe.exists():
                    entry_exe.unlink()
                _shutil.copy2(str(aot_backup), str(entry_exe))
                print(f"  [jit_codegen] restored entry.exe from entry-aot.exe")
                break
            except (PermissionError, OSError) as _e:
                if _copy_attempt < 4:
                    _time.sleep(1 << _copy_attempt)
                else:
                    print(f"  [jit_codegen] WARNING: could not restore entry.exe: {_e}")

    return StageResult(
        stage="jit_codegen", status="passed",
        summary="JIT entry-jit.exe built and AOT entry.exe restored",
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
