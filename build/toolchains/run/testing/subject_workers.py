from __future__ import annotations

import json
from pathlib import Path
from typing import Any
import statistics
import sys
import time

try:
    from ..common import combine_process_output, read_json, run_process, write_json
    from .. import tooling as tooling_module
    from . import contracts as contracts_module
    from . import perf as perf_module
    from . import subjects as subjects_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import combine_process_output, read_json, run_process, write_json
    import tooling as tooling_module
    from testing import contracts as contracts_module
    from testing import perf as perf_module
    from testing import subjects as subjects_module


DRIVER_PROJECT_PATH = Path("src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj")
DRIVER_DLL_PATH = Path("src/managed/Chaos.IL2CPP.Driver/bin/Release/net8.0/Chaos.IL2CPP.Driver.dll")
HOST_EMBEDDING_PROJECT_PATH = Path("subjects/HostEmbeddingLite/source/HostEmbeddingLite.csproj")
HOST_EMBEDDING_DLL_PATH = Path("artifacts/smoke/bin/HostEmbeddingLite/Release/net8.0/HostEmbeddingLite.dll")
WINDOWS_TRACE_SNAPSHOT_PATH = Path("tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json")
WINDOWS_TRACE_SCHEMA_PATH = Path("tests/contracts/trace/schema/warmup-trace.schema.json")
WINDOWS_REFERENCE_BUILD_TARGET = "chaos_subject_reference_proof"
WINDOWS_REFERENCE_RUN_TARGET = "chaos_subject_reference_proof_run"
VARIANT_MACROS = {
    "CHECK": {
        "codegen": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
        "native": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
    },
    "PROFILE": {
        "codegen": ["CHAOS_VARIANT_PROFILE", "CHAOS_VARIANT_NAME=PROFILE"],
        "native": ["CHAOS_VARIANT_PROFILE", "CHAOS_VARIANT_NAME=PROFILE"],
    },
    "SHIP": {
        "codegen": ["CHAOS_VARIANT_SHIP", "CHAOS_VARIANT_NAME=SHIP"],
        "native": ["CHAOS_VARIANT_SHIP", "CHAOS_VARIANT_NAME=SHIP"],
    },
}


def _resolve(repo_root: Path, relative_path: str) -> Path:
    return repo_root / Path(relative_path)


def _relative(repo_root: Path, path: Path) -> str:
    return path.relative_to(repo_root).as_posix()


def _selection_variant(selection: dict[str, Any]) -> str:
    return str(selection.get("variant") or "CHECK")


def _variant_macros(variant: str) -> dict[str, list[str]]:
    if variant not in VARIANT_MACROS:
        raise RuntimeError(f"unsupported subject variant: {variant}")
    return dict(VARIANT_MACROS[variant])


def _run_checked(
    arguments: list[str],
    *,
    repo_root: Path,
    failure_message: str,
    env: dict[str, str] | None = None,
) -> str:
    completed = run_process(arguments, cwd=repo_root, env=env)
    output = combine_process_output(completed)
    if completed.returncode != 0:
        raise RuntimeError(f"{failure_message}\n{output}".strip())
    return output


def _windows_visual_studio_generator(repo_root: Path) -> str:
    cmake_path = tooling_module.find_cmake_executable(repo_root) or "cmake"
    return tooling_module.detect_visual_studio_generator(cmake_path) or "Visual Studio 17 2022"


def _windows_native_cmake_context(repo_root: Path) -> tuple[str, dict[str, str] | None, str | None]:
    cmake_path, _cmake_env = tooling_module.cmake_environment(repo_root)
    developer_env = tooling_module.windows_developer_environment()
    ninja_path = tooling_module.find_ninja_executable()
    return cmake_path or "cmake", developer_env or None, ninja_path


def _normalize_host_platform(host_platform: str) -> str:
    return "windows" if host_platform.startswith("windows") else host_platform


def _dotnet_intermediate_args(project_name: str, host_platform: str) -> list[str]:
    normalized_host = _normalize_host_platform(host_platform)
    intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(project_name, host_platform=normalized_host)
    if intermediate_root is None:
        return []

    intermediate_text = intermediate_root.as_posix() + "/$(MSBuildProjectName)/"
    return [
        f"-p:BaseIntermediateOutputPath={intermediate_text}",
        f"-p:MSBuildProjectExtensionsPath={intermediate_text}",
    ]


def _success_result(
    *,
    bucket_manifest_path: str,
    report_paths: list[str],
    primary_evidence_paths: list[str],
    stdout_path: str | None = None,
    stderr_path: str | None = None,
    duration_ms: int = 0,
    details: dict[str, Any] | None = None,
) -> dict[str, Any]:
    return {
        "status": "ok",
        "bucketManifestPath": bucket_manifest_path,
        "reportPaths": report_paths,
        "primaryEvidencePaths": primary_evidence_paths,
        "metrics": {"durationMs": duration_ms},
        "diagnostics": {"stdoutPath": stdout_path, "stderrPath": stderr_path},
        "details": dict(details or {}),
        "failure": None,
    }


def run_source_resolve(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    source = dict(request["selection"]["source"])
    selection = dict(request["selection"])
    source_path = _resolve(repo_root, str(source["path"]))
    source_root = source_path.parent
    inputs = sorted(
        _relative(repo_root, candidate)
        for candidate in source_root.rglob("*")
        if candidate.is_file()
    )
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "bucket": "source",
        "sourceType": str(source["type"]),
        "sourcePath": str(source["path"]),
        "entry": str(source["entry"]),
        "validationProfileId": selection.get("validationProfileId"),
        "validationKind": selection.get("validationKind"),
        "variant": _selection_variant(selection),
        "inputs": inputs,
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[str(source["path"])],
    )


def run_dotnet_host_input_builder(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    source = dict(request["selection"]["source"])
    selection = dict(request["selection"])
    output_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    output_root.mkdir(parents=True, exist_ok=True)
    host_platform = str(request["selection"]["executionContext"]["hostPlatform"])

    _run_checked(
        [
            "dotnet",
            "build",
            str(_resolve(repo_root, str(source["path"]))),
            "-c",
            "Release",
            "-o",
            str(output_root),
            *_dotnet_intermediate_args(str(request["selection"]["subjectId"]), host_platform),
        ],
        repo_root=repo_root,
        failure_message=f"dotnet build failed: {source['path']}",
    )

    subject_id = str(request["selection"]["subjectId"])
    primary_assembly_path = output_root / f"{subject_id}.dll"
    files = [
        _relative(repo_root, candidate)
        for candidate in sorted(output_root.iterdir())
        if candidate.is_file()
        and candidate.suffix.lower() in {".dll", ".deps.json", ".runtimeconfig.json", ".pdb", ".exe"}
    ]
    manifest = {
        "subjectId": subject_id,
        "bucket": "host-input",
        "sourceManifestPath": str(request["upstream"]["source"]["manifestPath"]),
        "primaryAssemblyPath": _relative(repo_root, primary_assembly_path),
        "variant": _selection_variant(selection),
        "files": files,
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, primary_assembly_path)],
    )


def _ensure_driver_built(repo_root: Path) -> Path:
    host_platform = "windows" if sys.platform.startswith("win") else ""
    arguments = ["dotnet", "build", str(repo_root / DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"]
    intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(DRIVER_PROJECT_PATH.stem, host_platform=host_platform)
    if intermediate_root is not None:
        arguments.append(f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/")

    _run_checked(
        arguments,
        repo_root=repo_root,
        failure_message=f"dotnet build failed: {DRIVER_PROJECT_PATH.as_posix()}",
    )
    return repo_root / DRIVER_DLL_PATH


def run_frontend_pipeline_worker(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    driver_dll_path = _ensure_driver_built(repo_root)
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

    selection = dict(request["selection"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    assembly_path = _resolve(repo_root, str(host_input_manifest["primaryAssemblyPath"]))
    output_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    output_root.mkdir(parents=True, exist_ok=True)

    _run_checked(
        ["dotnet", str(driver_dll_path), str(assembly_path), str(output_root)],
        repo_root=repo_root,
        failure_message=f"managed closure materialization failed: {assembly_path}",
    )

    contracts_module.validate_analysis_contracts(repo_root)
    report_path = _resolve(repo_root, request["paths"]["reportPaths"][0])
    write_json(
        report_path,
        {
            "subjectId": str(request["selection"]["subjectId"]),
            "status": "ok",
            "variant": variant,
            "validationProfileId": selection.get("validationProfileId"),
            "validationKind": selection.get("validationKind"),
            "codegenMacros": list(variant_macros["codegen"]),
            "schemaPath": "contracts/artifacts/v0/schemas",
            "snapshotPath": "contracts/artifacts/v0/snapshots",
            "errors": [],
        },
    )

    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "bucket": "analysis",
        "hostInputManifestPath": str(request["upstream"]["host-input"]["manifestPath"]),
        "bundleKind": "proof-input-bundle",
        "validationProfileId": selection.get("validationProfileId"),
        "validationKind": selection.get("validationKind"),
        "variant": variant,
        "codegenMacros": list(variant_macros["codegen"]),
        "artifacts": {
            "typedIlIrPath": _relative(repo_root, output_root / "typed-il-ir.json"),
            "aotManifestPath": _relative(repo_root, output_root / "aot-manifest.json"),
            "metadataRegistrationPath": _relative(repo_root, output_root / "metadata-registration.json"),
            "codeRegistrationPath": _relative(repo_root, output_root / "code-registration.json"),
            "closureManifestPath": _relative(repo_root, output_root / "closure.manifest.json"),
        },
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[
            manifest["artifacts"]["typedIlIrPath"],
            manifest["artifacts"]["closureManifestPath"],
        ],
    )


def run_native_proof_emitter(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    driver_dll_path = _ensure_driver_built(repo_root)
    selection = dict(request["selection"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    analysis_root = _resolve(repo_root, request["upstream"]["analysis"]["manifestPath"]).parent
    output_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    output_root.mkdir(parents=True, exist_ok=True)

    _run_checked(
        ["dotnet", str(driver_dll_path), "emit-native-reference", str(analysis_root), str(output_root)],
        repo_root=repo_root,
        failure_message=f"native proof emission failed: {analysis_root}",
    )

    generated_manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "bucket": "generated",
        "analysisManifestPath": str(request["upstream"]["analysis"]["manifestPath"]),
        "validationProfileId": selection.get("validationProfileId"),
        "validationKind": selection.get("validationKind"),
        "variant": variant,
        "codegenMacros": list(variant_macros["codegen"]),
        "generatedSourcePath": _relative(repo_root, output_root / "generated" / "native-reference.generated.cpp"),
        "nativeProofManifestPath": _relative(repo_root, output_root / "native-proof.manifest.json"),
        "nativeProofPlanPath": _relative(repo_root, output_root / "native-proof.plan.json"),
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), generated_manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[
            generated_manifest["generatedSourcePath"],
            generated_manifest["nativeProofManifestPath"],
        ],
    )


def _windows_subject_build(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    selection = dict(request["selection"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    cmake_path, developer_env, ninja_path = _windows_native_cmake_context(repo_root)
    generator = "Ninja Multi-Config"
    cmake_binary_dir = tooling_module.allocate_cmake_binary_dir(
        build_root / "cmake",
        host_platform="windows",
        generator=generator,
    )
    out_root = build_root / "out"
    runtime_root = build_root.parent / "runtime"
    generated_root = _resolve(repo_root, request["upstream"]["generated"]["manifestPath"]).parent

    _run_checked(
        [
            cmake_path,
            "-S",
            str(repo_root),
            "-B",
            str(cmake_binary_dir),
            "-G",
            generator,
            "-DROADMAP0_PRESET_TARGET=windows-x64-reference",
            f"-DCMAKE_TOOLCHAIN_FILE={repo_root / 'build' / 'toolchains' / 'windows-x64-reference.cmake'}",
            *([f"-DCMAKE_MAKE_PROGRAM={ninja_path}"] if ninja_path else []),
            f"-DCHAOS_SUBJECT_VARIANT={variant}",
            f"-DCHAOS_SUBJECT_GENERATED_ROOT={generated_root}",
            f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={out_root}",
            f"-DCHAOS_SUBJECT_RUNTIME_ROOT={runtime_root}",
        ],
        repo_root=repo_root,
        failure_message="cmake preset configure failed: windows-x64-reference",
        env=developer_env,
    )
    _run_checked(
        [
            cmake_path,
            "--build",
            str(cmake_binary_dir),
            "--config",
            "Release",
            "--target",
            WINDOWS_REFERENCE_BUILD_TARGET,
        ],
        repo_root=repo_root,
        failure_message="subject proof build failed: windows-x64-reference",
        env=developer_env,
    )

    proof_exe = out_root / f"{WINDOWS_REFERENCE_BUILD_TARGET}.exe"
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "build",
        "targetPlatform": str(request["selection"]["executionContext"]["targetPlatform"]),
        "toolchainProfile": str(request["selection"]["executionContext"]["toolchainProfile"]),
        "variant": variant,
        "variantMacros": {
            "codegen": list(variant_macros["codegen"]),
            "native": list(variant_macros["native"]),
        },
        "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
        "binaryRoot": _relative(repo_root, out_root),
        "outputs": [_relative(repo_root, proof_exe)],
        "cmakeBinaryDir": tooling_module.path_text(repo_root, cmake_binary_dir),
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, proof_exe)],
    )


def _validate_only_build(
    *,
    repo_root: Path,
    request: dict[str, Any],
    preset_target: str,
    toolchain_file: Path,
) -> dict[str, Any]:
    build_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    selection = dict(request["selection"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    execution_context = dict(request["selection"].get("executionContext") or {})
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    generator = "Ninja"
    instance_spec: str | None = None
    if host_platform == "windows":
        generator = _windows_visual_studio_generator(repo_root)
        instance_spec = tooling_module.detect_visual_studio_instance_spec(generator)
        cmake_binary_dir = tooling_module.allocate_cmake_binary_dir(
            build_root / "cmake",
            host_platform=host_platform,
            generator=generator,
        )
    else:
        cmake_binary_dir = build_root / "cmake"
    success_marker = build_root / "validate-only.success.txt"

    _run_checked(
        [
            "cmake",
            "-S",
            str(repo_root),
            "-B",
            str(cmake_binary_dir),
            "-G",
            generator,
            f"-DROADMAP0_PRESET_TARGET={preset_target}",
            "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
            f"-DCHAOS_SUBJECT_VARIANT={variant}",
            f"-DCMAKE_TOOLCHAIN_FILE={repo_root / toolchain_file}",
            *([f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}"] if instance_spec else []),
        ],
        repo_root=repo_root,
        failure_message=f"cmake routing configure failed: {preset_target}",
    )
    _run_checked(
        ["cmake", "--build", str(cmake_binary_dir)],
        repo_root=repo_root,
        failure_message=f"cmake routing build failed: {preset_target}",
    )

    success_marker.parent.mkdir(parents=True, exist_ok=True)
    success_marker.write_text("ok\n", encoding="utf-8")
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "build",
        "targetPlatform": str(request["selection"]["executionContext"]["targetPlatform"]),
        "toolchainProfile": str(request["selection"]["executionContext"]["toolchainProfile"]),
        "variant": variant,
        "variantMacros": {
            "codegen": list(variant_macros["codegen"]),
            "native": list(variant_macros["native"]),
        },
        "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
        "binaryRoot": _relative(repo_root, build_root),
        "outputs": [_relative(repo_root, success_marker)],
        "cmakeBinaryDir": tooling_module.path_text(repo_root, cmake_binary_dir),
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, success_marker)],
    )


def run_build_target(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    target_platform = str(request["selection"]["executionContext"]["targetPlatform"])
    if target_platform == "windows-x64":
        return _windows_subject_build(repo_root=repo_root, request=request)
    if target_platform == "android-arm64":
        return _validate_only_build(
            repo_root=repo_root,
            request=request,
            preset_target="android-arm64-smoke",
            toolchain_file=Path("build/toolchains/android-arm64.cmake"),
        )
    if target_platform == "linux-x64":
        return _validate_only_build(
            repo_root=repo_root,
            request=request,
            preset_target="linux-x64-packaging",
            toolchain_file=Path("build/toolchains/linux-x64.cmake"),
        )
    raise RuntimeError(f"unsupported build target platform: {target_platform}")


def run_runtime_observe(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_manifest = read_json(_resolve(repo_root, request["upstream"]["build"]["manifestPath"]))
    if not isinstance(build_manifest, dict):
        raise RuntimeError("build manifest must be an object")

    cmake_path, developer_env, _ninja_path = _windows_native_cmake_context(repo_root)
    cmake_binary_dir = _resolve(repo_root, str(build_manifest["cmakeBinaryDir"]))
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    _run_checked(
        [
            cmake_path,
            "--build",
            str(cmake_binary_dir),
            "--config",
            "Release",
            "--target",
            WINDOWS_REFERENCE_RUN_TARGET,
        ],
        repo_root=repo_root,
        failure_message="subject proof run failed: windows-x64-reference",
        env=developer_env,
    )

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "runtime",
        "variant": _selection_variant(dict(request["selection"])),
        "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "tracePaths": [],
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[manifest["stdoutPath"], manifest["exitCodePath"]],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
    )


def run_managed_runtime_output(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

    assembly_path = _resolve(repo_root, str(host_input_manifest["primaryAssemblyPath"]))
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    completed = run_process(["dotnet", str(assembly_path)], cwd=repo_root)
    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    stdout_path.write_text(completed.stdout or "", encoding="utf-8")
    stderr_path.write_text(completed.stderr or "", encoding="utf-8")
    exit_code_path.write_text(f"{completed.returncode}\n", encoding="utf-8")

    output_lines = [line for line in (completed.stdout or "").splitlines() if line.strip()]
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "runtime",
        "variant": _selection_variant(dict(request["selection"])),
        "hostInputManifestPath": str(request["upstream"]["host-input"]["manifestPath"]),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "outputLines": output_lines,
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

    if completed.returncode != 0:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": [manifest["stdoutPath"]],
            "metrics": {"durationMs": 0},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "failure": f"managed runtime execution failed: {assembly_path}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[manifest["stdoutPath"]],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
    )


def _perf_sample_count(runtime_profile: str) -> int:
    return 10 if "release" in runtime_profile else 5


def _perf_harness_iterations(runtime_profile: str) -> int:
    return 10000 if "release" in runtime_profile else 1000


def _perf_summary_metrics(samples: list[dict[str, Any]]) -> dict[str, float | int]:
    durations = [float(sample["durationMs"]) for sample in samples]
    return {
        "sampleCount": len(samples),
        "meanDurationMs": round(statistics.fmean(durations), 3) if durations else 0.0,
        "minDurationMs": round(min(durations), 3) if durations else 0.0,
        "maxDurationMs": round(max(durations), 3) if durations else 0.0,
    }


def run_runtime_perf_collect(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

    selection = dict(request["selection"])
    execution_context = dict(selection.get("executionContext") or {})
    subject_id = str(selection["subjectId"])
    matrix_id = str(selection["matrixId"])
    variant = _selection_variant(selection)
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    runtime_profile = str(execution_context.get("runtimeProfile") or "")
    sample_count = _perf_sample_count(runtime_profile)
    iterations = _perf_harness_iterations(runtime_profile)
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    validation_spec = subjects_module.find_validation(manifest, "perf")
    perf_project_path = str(validation_spec.get("project") or "")
    if not perf_project_path:
        raise RuntimeError(f"perf validation project missing for subject: {subject_id}")

    project_path = _resolve(repo_root, perf_project_path)
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)
    harness_root = runtime_root / "harness"
    harness_dll_path = harness_root / f"{project_path.stem}.dll"

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"

    _run_checked(
        [
            "dotnet",
            "build",
            str(project_path),
            "-c",
            "Release",
            "-o",
            str(harness_root),
            *_dotnet_intermediate_args(project_path.stem, host_platform),
        ],
        repo_root=repo_root,
        failure_message=f"dotnet build failed: {perf_project_path}",
    )

    samples: list[dict[str, Any]] = []
    stdout_chunks: list[str] = []
    stderr_chunks: list[str] = []
    output_lines: list[str] = []
    last_exit_code = 0

    for sample_index in range(sample_count):
        started = time.perf_counter()
        completed = run_process(["dotnet", str(harness_dll_path), str(iterations)], cwd=repo_root)
        duration_ms = round((time.perf_counter() - started) * 1000, 3)
        stdout_text = completed.stdout or ""
        stderr_text = completed.stderr or ""
        output_lines = [line for line in stdout_text.splitlines() if line.strip()]
        last_exit_code = int(completed.returncode)
        if output_lines:
            try:
                payload = json.loads(output_lines[-1])
            except ValueError:
                payload = {}
            if isinstance(payload, dict) and isinstance(payload.get("elapsedMilliseconds"), (int, float)):
                duration_ms = round(float(payload["elapsedMilliseconds"]), 3)
        samples.append(
            {
                "sampleIndex": sample_index + 1,
                "durationMs": duration_ms,
                "exitCode": last_exit_code,
            }
        )
        stdout_chunks.append(f"=== sample {sample_index + 1} ({duration_ms:.3f} ms) ===\n{stdout_text}".rstrip() + "\n")
        if stderr_text:
            stderr_chunks.append(f"=== sample {sample_index + 1} ({duration_ms:.3f} ms) ===\n{stderr_text}".rstrip() + "\n")
        if last_exit_code != 0:
            break

    stdout_path.write_text("".join(stdout_chunks), encoding="utf-8")
    stderr_path.write_text("".join(stderr_chunks), encoding="utf-8")
    exit_code_path.write_text(f"{last_exit_code}\n", encoding="utf-8")

    summary_metrics = _perf_summary_metrics(samples)
    perf_result = perf_module.evaluate_perf_subject(
        repo_root=repo_root,
        subject_id=subject_id,
        matrix_id=matrix_id,
        host_platform=host_platform,
        metrics=summary_metrics,
        update_baseline=False,
    )
    performance = {
        "samples": samples,
        "metrics": dict(perf_result["metrics"]),
        "baselinePath": str(perf_result["baselinePath"]),
        "baseline": dict(perf_result["baseline"]),
        "baselineUpdated": bool(perf_result["baselineUpdated"]),
        "regressionStatus": str(perf_result["regressionStatus"]),
        "regressions": list(perf_result.get("regressions") or []),
    }
    manifest = {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "runtime",
        "variant": variant,
        "hostInputManifestPath": str(request["upstream"]["host-input"]["manifestPath"]),
        "perfHarnessProjectPath": perf_project_path,
        "perfHarnessDllPath": _relative(repo_root, harness_dll_path),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "outputLines": output_lines,
        "samples": samples,
        "summaryMetrics": dict(performance["metrics"]),
        "baselinePath": str(performance["baselinePath"]),
        "baseline": dict(performance["baseline"]),
        "baselineUpdated": bool(performance["baselineUpdated"]),
        "regressionStatus": str(performance["regressionStatus"]),
        "regressions": list(performance["regressions"]),
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

    if last_exit_code != 0:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": [manifest["stdoutPath"]],
            "metrics": {"durationMs": int(round(sum(float(sample["durationMs"]) for sample in samples)))},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "details": {"performance": performance},
            "failure": f"managed perf execution failed: {harness_dll_path}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[manifest["stdoutPath"]],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
        duration_ms=int(round(sum(float(sample["durationMs"]) for sample in samples))),
        details={"performance": performance},
    )


def run_runtime_trace_compare(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    host_platform = str(request["selection"]["executionContext"]["hostPlatform"])
    _run_checked(
        ["dotnet", "build", str(repo_root / HOST_EMBEDDING_PROJECT_PATH), "-c", "Release", *_dotnet_intermediate_args(HOST_EMBEDDING_PROJECT_PATH.stem, host_platform)],
        repo_root=repo_root,
        failure_message=f"dotnet build failed: {HOST_EMBEDDING_PROJECT_PATH.as_posix()}",
    )

    runtime_manifest_path = _resolve(repo_root, request["paths"]["manifestPath"])
    runtime_manifest = read_json(runtime_manifest_path)
    if not isinstance(runtime_manifest, dict):
        raise RuntimeError("runtime manifest must be an object")

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    trace_path = runtime_root / "trace.runtime.json"
    _run_checked(
        [
            "dotnet",
            str(repo_root / HOST_EMBEDDING_DLL_PATH),
            "--trace-platform",
            "windows",
            "--trace-output",
            str(trace_path),
        ],
        repo_root=repo_root,
        failure_message="HostEmbeddingLite windows trace export failed",
    )
    _run_checked(
        [
            sys.executable,
            str(repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"),
            str(repo_root / WINDOWS_TRACE_SNAPSHOT_PATH),
            str(trace_path),
        ],
        repo_root=repo_root,
        failure_message="Windows reference desktop trace compare failed",
    )

    runtime_manifest["tracePaths"] = [_relative(repo_root, trace_path)]
    write_json(runtime_manifest_path, runtime_manifest)
    report_path = _resolve(repo_root, request["paths"]["reportPaths"][0])
    write_json(
        report_path,
        {
            "subjectId": str(request["selection"]["subjectId"]),
            "matrixId": str(request["selection"]["matrixId"]),
            "status": "ok",
            "variant": _selection_variant(dict(request["selection"])),
            "schemaPath": WINDOWS_TRACE_SCHEMA_PATH.as_posix(),
            "expectedSnapshotPath": WINDOWS_TRACE_SNAPSHOT_PATH.as_posix(),
            "actualTracePath": _relative(repo_root, trace_path),
            "errors": [],
        },
    )
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[_relative(repo_root, trace_path)],
    )


DEFAULT_STAGE_WORKERS = {
    "source-resolve": run_source_resolve,
    "host-input-build": run_dotnet_host_input_builder,
    "analysis-frontend": run_frontend_pipeline_worker,
    "generated-native-proof": run_native_proof_emitter,
    "build-target": run_build_target,
    "runtime-observe": run_runtime_observe,
    "runtime-managed-output": run_managed_runtime_output,
    "runtime-perf-collect": run_runtime_perf_collect,
    "runtime-trace-compare": run_runtime_trace_compare,
}
