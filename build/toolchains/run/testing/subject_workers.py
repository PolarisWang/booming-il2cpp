from __future__ import annotations

from pathlib import Path
from typing import Any
import sys

try:
    from ..common import combine_process_output, read_json, run_process, write_json
    from . import contracts as contracts_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import combine_process_output, read_json, run_process, write_json
    from testing import contracts as contracts_module


DRIVER_PROJECT_PATH = Path("src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj")
DRIVER_DLL_PATH = Path("src/managed/Chaos.IL2CPP.Driver/bin/Release/net8.0/Chaos.IL2CPP.Driver.dll")
HOST_EMBEDDING_PROJECT_PATH = Path("tests/smoke/input/HostEmbeddingLite/HostEmbeddingLite.csproj")
HOST_EMBEDDING_DLL_PATH = Path("artifacts/smoke/bin/HostEmbeddingLite/Release/net8.0/HostEmbeddingLite.dll")
WINDOWS_TRACE_SNAPSHOT_PATH = Path("tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json")
WINDOWS_TRACE_SCHEMA_PATH = Path("tests/contracts/trace/schema/warmup-trace.schema.json")


def _resolve(repo_root: Path, relative_path: str) -> Path:
    return repo_root / Path(relative_path)


def _relative(repo_root: Path, path: Path) -> str:
    return path.relative_to(repo_root).as_posix()


def _run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
    completed = run_process(arguments, cwd=repo_root)
    output = combine_process_output(completed)
    if completed.returncode != 0:
        raise RuntimeError(f"{failure_message}\n{output}".strip())
    return output


def _success_result(
    *,
    bucket_manifest_path: str,
    report_paths: list[str],
    primary_evidence_paths: list[str],
    stdout_path: str | None = None,
    stderr_path: str | None = None,
    duration_ms: int = 0,
) -> dict[str, Any]:
    return {
        "status": "ok",
        "bucketManifestPath": bucket_manifest_path,
        "reportPaths": report_paths,
        "primaryEvidencePaths": primary_evidence_paths,
        "metrics": {"durationMs": duration_ms},
        "diagnostics": {"stdoutPath": stdout_path, "stderrPath": stderr_path},
        "failure": None,
    }


def run_source_resolve(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    source = dict(request["selection"]["source"])
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
    output_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    output_root.mkdir(parents=True, exist_ok=True)

    _run_checked(
        ["dotnet", "build", str(_resolve(repo_root, str(source["path"]))), "-c", "Release", "-o", str(output_root)],
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
        "files": files,
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, primary_assembly_path)],
    )


def _ensure_driver_built(repo_root: Path) -> Path:
    _run_checked(
        ["dotnet", "build", str(repo_root / DRIVER_PROJECT_PATH), "-c", "Release"],
        repo_root=repo_root,
        failure_message=f"dotnet build failed: {DRIVER_PROJECT_PATH.as_posix()}",
    )
    return repo_root / DRIVER_DLL_PATH


def run_frontend_pipeline_worker(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    driver_dll_path = _ensure_driver_built(repo_root)
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

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
            "schemaPath": "contracts/artifacts/v0/schemas",
            "snapshotPath": "tests/contracts/schema",
            "errors": [],
        },
    )

    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "bucket": "analysis",
        "hostInputManifestPath": str(request["upstream"]["host-input"]["manifestPath"]),
        "bundleKind": "proof-input-bundle",
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
    cmake_binary_dir = build_root / "cmake"
    out_root = build_root / "out"
    runtime_root = build_root.parent / "runtime"
    generated_root = _resolve(repo_root, request["upstream"]["generated"]["manifestPath"]).parent

    _run_checked(
        [
            "cmake",
            "--preset",
            "windows-x64-reference",
            "-B",
            str(cmake_binary_dir),
            f"-DCHAOS_HELLOWORLD_GENERATED_ROOT={generated_root}",
            f"-DCHAOS_HELLOWORLD_BUILD_OUT_ROOT={out_root}",
            f"-DCHAOS_HELLOWORLD_RUNTIME_ROOT={runtime_root}",
        ],
        repo_root=repo_root,
        failure_message="cmake preset configure failed: windows-x64-reference",
    )
    _run_checked(
        [
            "cmake",
            "--build",
            str(cmake_binary_dir),
            "--config",
            "Release",
            "--target",
            "chaos_stage4_hello_world_object_proof",
        ],
        repo_root=repo_root,
        failure_message="subject proof build failed: windows-x64-reference",
    )

    proof_exe = out_root / "chaos_stage4_hello_world_object_proof.exe"
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "build",
        "targetPlatform": str(request["selection"]["executionContext"]["targetPlatform"]),
        "toolchainProfile": str(request["selection"]["executionContext"]["toolchainProfile"]),
        "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
        "binaryRoot": _relative(repo_root, out_root),
        "outputs": [_relative(repo_root, proof_exe)],
        "cmakeBinaryDir": _relative(repo_root, cmake_binary_dir),
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
            "Ninja",
            f"-DROADMAP0_PRESET_TARGET={preset_target}",
            "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
            f"-DCMAKE_TOOLCHAIN_FILE={repo_root / toolchain_file}",
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
        "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
        "binaryRoot": _relative(repo_root, build_root),
        "outputs": [_relative(repo_root, success_marker)],
        "cmakeBinaryDir": _relative(repo_root, cmake_binary_dir),
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

    cmake_binary_dir = _resolve(repo_root, str(build_manifest["cmakeBinaryDir"]))
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    _run_checked(
        [
            "cmake",
            "--build",
            str(cmake_binary_dir),
            "--config",
            "Release",
            "--target",
            "chaos_stage4_hello_world_object_proof_run",
        ],
        repo_root=repo_root,
        failure_message="subject proof run failed: windows-x64-reference",
    )

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "runtime",
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


def run_runtime_trace_compare(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    _run_checked(
        ["dotnet", "build", str(repo_root / HOST_EMBEDDING_PROJECT_PATH), "-c", "Release"],
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
    "runtime-trace-compare": run_runtime_trace_compare,
}
