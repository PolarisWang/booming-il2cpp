#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import platform
import subprocess
import sys
import uuid
from pathlib import Path

toolchains_root = Path(__file__).resolve().parents[1] / "toolchains" / "run"
if str(toolchains_root) not in sys.path:
    sys.path.insert(0, str(toolchains_root))

from core import tooling as tooling_module
from testing import contracts as contracts_module
from testing import subject_executor as subject_executor_module
from testing import subject_planner as subject_planner_module
from testing import subjects as subjects_module


DEFAULT_WINDOWS_VISUAL_STUDIO_GENERATOR = "Visual Studio 17 2022"
SOLUTION_CORE_PACK_SUBJECT_ID = "SolutionCorePack"
SOLUTION_CORE_PACK_NATIVE_MATRIX_ID = "windows-native-check"
SOLUTION_CORE_PACK_WINDOWS_TRACE_MATRIX_ID = "windows-managed-trace"
SOLUTION_CORE_PACK_MACOS_TRACE_MATRIX_ID = "macos-managed-trace"


def get_repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def write_step(message: str) -> None:
    print(f"==> {message}")


def get_host_routing_generator(host_profile: str) -> str:
    if host_profile == "windows":
        cmake_path = tooling_module.find_cmake_executable(get_repo_root()) or "cmake"
        return tooling_module.detect_visual_studio_generator(cmake_path) or DEFAULT_WINDOWS_VISUAL_STUDIO_GENERATOR
    if host_profile == "macos":
        return "Xcode"
    raise RuntimeError(f"unsupported host profile: {host_profile}")


def get_platform_gate_generator(preset_target: str, host_profile: str) -> str:
    if preset_target in {"android-arm64-smoke", "ios-arm64-packaging", "linux-x64-packaging"}:
        return "Ninja"
    return get_host_routing_generator(host_profile)


def run_checked(arguments: list[str], *, cwd: Path, failure_message: str, capture_output: bool = False) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        text=True,
        capture_output=capture_output,
        check=False,
    )
    if capture_output:
        if completed.stdout:
            print(completed.stdout, end="" if completed.stdout.endswith("\n") else "\n")
        if completed.stderr:
            print(completed.stderr, end="" if completed.stderr.endswith("\n") else "\n", file=sys.stderr)
    if completed.returncode != 0:
        raise RuntimeError(failure_message)
    return completed


def allocate_run_scoped_binary_dir(base_dir: Path, *, host_profile: str = "", generator: str | None = None) -> Path:
    return tooling_module.allocate_cmake_binary_dir(
        base_dir,
        host_platform=host_profile,
        generator=generator,
    )


def invoke_native_smoke_build(source_dir: Path, binary_dir: Path, repo_root: Path, *, host_profile: str = "") -> None:
    generator = get_host_routing_generator(host_profile) if host_profile == "windows" else None
    run_binary_dir = allocate_run_scoped_binary_dir(binary_dir, host_profile=host_profile, generator=generator)
    configure_args = ["cmake", "-S", str(source_dir), "-B", str(run_binary_dir)]
    if generator:
        configure_args.extend(["-G", generator])
        instance_spec = tooling_module.detect_visual_studio_instance_spec(generator)
        if instance_spec:
            configure_args.append(f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}")
    run_checked(configure_args, cwd=repo_root, failure_message=f"cmake configure failed: {source_dir}")
    run_checked(["cmake", "--build", str(run_binary_dir)], cwd=repo_root, failure_message=f"cmake build failed: {run_binary_dir}")


def invoke_dotnet_build(project_path: Path, repo_root: Path, *, host_profile: str = "") -> None:
    arguments = ["dotnet", "build", str(project_path), "-c", "Release"]
    intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(project_path.stem, host_platform=host_profile)
    if intermediate_root is not None:
        intermediate_text = intermediate_root.as_posix() + "/$(MSBuildProjectName)/"
        arguments.extend(
            [
                f"-p:BaseIntermediateOutputPath={intermediate_text}",
                f"-p:MSBuildProjectExtensionsPath={intermediate_text}",
            ]
        )
    run_checked(arguments, cwd=repo_root, failure_message=f"dotnet build failed: {project_path}")


def invoke_dotnet_runtime_smoke(dll_path: Path, expected_patterns: list[str], repo_root: Path) -> None:
    completed = run_checked(["dotnet", str(dll_path)], cwd=repo_root, capture_output=True, failure_message=f"dotnet runtime smoke failed: {dll_path}")
    joined_output = "\n".join(part for part in [completed.stdout.strip(), completed.stderr.strip()] if part)
    for pattern in expected_patterns:
        if pattern not in joined_output:
            raise RuntimeError(f"runtime smoke output mismatch for {dll_path}: missing '{pattern}'")


def invoke_preset_build_smoke(preset_name: str, repo_root: Path, *, host_profile: str = "", validate_only: bool = False) -> None:
    requested_binary_dir = repo_root / "artifacts" / "presets" / preset_name
    generator = get_host_routing_generator(host_profile) if host_profile == "windows" else None
    run_binary_dir = allocate_run_scoped_binary_dir(requested_binary_dir, host_profile=host_profile, generator=generator)
    arguments = ["cmake", "--preset", preset_name, "-B", str(run_binary_dir)]
    if generator:
        arguments.extend(["-G", generator])
        instance_spec = tooling_module.detect_visual_studio_instance_spec(generator)
        if instance_spec:
            arguments.append(f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}")
    if validate_only:
        arguments.append("-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON")

    run_checked(arguments, cwd=repo_root, failure_message=f"cmake preset configure failed: {preset_name}")
    run_checked(["cmake", "--build", str(run_binary_dir)], cwd=repo_root, failure_message=f"cmake preset build failed: {preset_name}")


def invoke_routing_build_smoke(preset_target: str, toolchain_file: Path, binary_dir: Path, generator: str, repo_root: Path) -> None:
    host_profile = "windows" if generator.startswith("Visual Studio") else ""
    run_binary_dir = allocate_run_scoped_binary_dir(binary_dir, host_profile=host_profile, generator=generator)
    arguments = [
        "cmake",
        "-S",
        str(repo_root),
        "-B",
        str(run_binary_dir),
        "-G",
        generator,
        f"-DROADMAP0_PRESET_TARGET={preset_target}",
        "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
        f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file}",
    ]
    if host_profile == "windows":
        instance_spec = tooling_module.detect_visual_studio_instance_spec(generator)
        if instance_spec:
            arguments.append(f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}")
    run_checked(arguments, cwd=repo_root, failure_message=f"cmake routing smoke failed: {preset_target}")
    run_checked(["cmake", "--build", str(run_binary_dir)], cwd=repo_root, failure_message=f"cmake routing build failed: {preset_target}")


def write_gate_record(output_path: Path, gate_name: str, status: str, preset: str, notes: str, host_profile: str) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    record = {
        "gateName": gate_name,
        "hostProfile": host_profile,
        "status": status,
        "preset": preset,
        "notes": notes,
    }
    output_path.write_text(json.dumps(record, indent=2), encoding="utf-8")


def resolve_stage4_runtime_root(runtime_root: Path) -> Path:
    legacy_runtime_root = runtime_root / "run"
    if not (runtime_root / "stdout.log").is_file() and legacy_runtime_root.is_dir():
        return legacy_runtime_root
    return runtime_root


def validate_stage4_proof_run_artifacts(runtime_root: Path) -> None:
    runtime_root = resolve_stage4_runtime_root(runtime_root)
    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"

    required_paths = [stdout_path, stderr_path, exit_code_path]
    for required_path in required_paths:
        if not required_path.is_file():
            raise RuntimeError(f"missing Stage 4 proof run artifact: {required_path}")

    stdout_text = stdout_path.read_text(encoding="utf-8")
    stderr_text = stderr_path.read_text(encoding="utf-8")
    exit_code_text = exit_code_path.read_text(encoding="utf-8").strip()

    try:
        exit_code = int(exit_code_text)
    except ValueError as exception:
        raise RuntimeError(f"invalid Stage 4 proof exit code record: {exit_code_path}") from exception

    if exit_code != 0:
        message = f"stage4 native reference proof exit code mismatch: expected 0 actual {exit_code}"
        if stderr_text.strip():
            message = f"{message}\n{stderr_text.strip()}"
        raise RuntimeError(message)

    expected_stdout = "Hello, World!"
    if expected_stdout not in stdout_text:
        raise RuntimeError(f"stage4 native reference proof stdout mismatch: missing '{expected_stdout}'")


def build_subject_run_id(matrix_id: str) -> str:
    return f"verify-runtime-baseline-{matrix_id}-{uuid.uuid4().hex[:8]}"


def resolve_subject_matrix_subject_id(matrix_id: str) -> str:
    del matrix_id
    return SOLUTION_CORE_PACK_SUBJECT_ID


def subject_runtime_root(repo_root: Path, subject_id: str, run_id: str, matrix_id: str) -> Path:
    return repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "matrices" / matrix_id / "runtime"


def execute_subject_matrix(repo_root: Path, *, matrix_id: str, goal_id: str) -> dict:
    subject_id = resolve_subject_matrix_subject_id(matrix_id)
    run_id = build_subject_run_id(matrix_id)
    plan = subject_planner_module.build_plan(
        repo_root,
        subject_id,
        goal_id=goal_id,
        matrix_id=matrix_id,
        run_id=run_id,
    )
    result = subject_executor_module.execute_plan(repo_root, plan, run_id=run_id)
    if result["status"] != "ok":
        raise RuntimeError(f"subject matrix failed: {matrix_id}: {result['errors']}")
    result["runId"] = run_id
    return result


def parse_host_profile(argv: list[str] | None = None) -> str:
    parser = argparse.ArgumentParser()
    parser.add_argument("host_profile", nargs="?")
    parser.add_argument("-HostProfile", "--host-profile", dest="host_profile_flag")
    args = parser.parse_args(argv)

    host_profile = args.host_profile_flag or args.host_profile
    if host_profile not in {"windows", "macos"}:
        parser.error("host profile must be one of: windows, macos")
    return host_profile


def main(argv: list[str] | None = None) -> int:
    host_profile = parse_host_profile(argv)
    repo_root = get_repo_root()
    artifact_root = repo_root / "artifacts" / "verify-runtime-baseline" / host_profile
    common_artifact_root = artifact_root / "common"
    compare_script = repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"
    windows_trace_snapshot = repo_root / "tests" / "contracts" / "trace" / "snapshots" / "windows-warmup-trace.snapshot.json"
    host_embedding_dll = repo_root / "artifacts" / "smoke" / "bin" / "HostEmbeddingLite" / "Release" / "net8.0" / "HostEmbeddingLite.dll"

    artifact_root.mkdir(parents=True, exist_ok=True)
    common_artifact_root.mkdir(parents=True, exist_ok=True)

    system_name = platform.system()
    is_windows = system_name == "Windows"
    is_macos = system_name == "Darwin"
    if host_profile == "windows" and not is_windows:
        raise RuntimeError("HostProfile=windows requires a Windows host")
    if host_profile == "macos" and not is_macos:
        raise RuntimeError("HostProfile=macos requires a macOS host")

    write_step("List CMake presets")
    preset_list = run_checked(["cmake", "--list-presets=all"], cwd=repo_root, capture_output=True, failure_message="cmake --list-presets=all failed")
    (artifact_root / "cmake-presets.txt").write_text(preset_list.stdout, encoding="utf-8")

    write_step("Parse schema and trace JSON assets")
    json_paths = [repo_root / "CMakePresets.json"]
    json_paths.extend(contracts_module.analysis_contract_json_paths(repo_root))
    json_paths.extend(contracts_module.trace_contract_json_paths(repo_root))
    contracts_module.assert_json_files_parse(json_paths)

    write_step("Validate analysis schema/example/snapshot contracts")
    contracts_module.validate_analysis_contracts(repo_root)

    write_step("Validate trace schema and snapshots")
    contracts_module.validate_trace_schema_contracts(repo_root)

    write_step("Build native ABI compile-only smoke")
    invoke_native_smoke_build(repo_root / "tests" / "contracts" / "native" / "abi", common_artifact_root / "native-abi-config", repo_root, host_profile=host_profile)

    write_step("Build native bridge compile-only smoke")
    invoke_native_smoke_build(repo_root / "tests" / "contracts" / "native" / "bridge", common_artifact_root / "native-bridge-config", repo_root, host_profile=host_profile)

    write_step("Build smoke input projects")
    for project_name in ("HelloWorld", "GenericEcho", "ReflectionLite", "PInvokeLite", "HostEmbeddingLite"):
        invoke_dotnet_build(
            repo_root / "subjects" / "SolutionCorePack" / "source" / "Slices" / project_name / f"{project_name}.csproj",
            repo_root,
            host_profile=host_profile,
        )

    write_step("Run managed smoke projects")
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "HelloWorld" / "Release" / "net8.0" / "HelloWorld.dll", ["HelloWorld smoke entry reached.", "register:Main"], repo_root)
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "GenericEcho" / "Release" / "net8.0" / "GenericEcho.dll", ["roadmap0", "42", "roadmap0:roadmap0"], repo_root)
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "ReflectionLite" / "Release" / "net8.0" / "ReflectionLite.dll", ["field=BackingField:Int32", "generic-method=String"], repo_root)
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "PInvokeLite" / "Release" / "net8.0" / "PInvokeLite.dll", ["marshal=interop-smoke", "export=chaos_smoke_add:7", "symbol=True"], repo_root)
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "HostEmbeddingLite" / "Release" / "net8.0" / "HostEmbeddingLite.dll", ["HostEmbeddingSession:InvokeManagedEntry:True", "guards=invalid-detach:True|double-start:True|unattached-entry:True"], repo_root)

    if host_profile == "windows":
        if not windows_trace_snapshot.is_file():
            raise RuntimeError(f"missing Windows trace snapshot contract: {windows_trace_snapshot}")
        subject_id = SOLUTION_CORE_PACK_SUBJECT_ID

        write_step(f"Run {subject_id} {SOLUTION_CORE_PACK_NATIVE_MATRIX_ID} subject matrix")
        dev_output_result = execute_subject_matrix(
            repo_root,
            matrix_id=SOLUTION_CORE_PACK_NATIVE_MATRIX_ID,
            goal_id="correctness.dev",
        )
        validate_stage4_proof_run_artifacts(
            subject_runtime_root(
                repo_root,
                subject_id,
                str(dev_output_result["runId"]),
                SOLUTION_CORE_PACK_NATIVE_MATRIX_ID,
            )
        )
        write_gate_record(
            artifact_root / "windows-stage4-native-reference.gate.json",
            "windows-stage4-native-reference",
            "passed",
            "windows-x64-reference",
            f"Windows Stage 4 native reference proof compatibility gate passed via the {subject_id} subject matrix.",
            host_profile,
        )

        write_step(f"Run {subject_id} {SOLUTION_CORE_PACK_WINDOWS_TRACE_MATRIX_ID} subject matrix")
        execute_subject_matrix(
            repo_root,
            matrix_id=SOLUTION_CORE_PACK_WINDOWS_TRACE_MATRIX_ID,
            goal_id="correctness.platform",
        )
        write_gate_record(
            artifact_root / "windows-reference-desktop.gate.json",
            "windows-reference-desktop",
            "passed",
            "windows-x64-reference",
            f"Windows reference desktop gate passed via the {subject_id} windows-reference-trace subject matrix and the canonical snapshot contract at {windows_trace_snapshot}.",
            host_profile,
        )

        write_step("Validate Android startup routing smoke")
        invoke_routing_build_smoke(
            "android-arm64-smoke",
            repo_root / "build" / "toolchains" / "android-arm64.cmake",
            common_artifact_root / "android-startup-routing",
            get_platform_gate_generator("android-arm64-smoke", host_profile),
            repo_root,
        )
        write_gate_record(
            artifact_root / "android-startup-smoke.gate.json",
            "android-startup-smoke",
            "routing-validated",
            "android-arm64-smoke",
            "Android preset remains visible in CMakePresets, and its toolchain/router path was validated with the host-compatible generator.",
            host_profile,
        )

        write_step("Validate Linux packaging routing smoke")
        invoke_routing_build_smoke(
            "linux-x64-packaging",
            repo_root / "build" / "toolchains" / "linux-x64.cmake",
            common_artifact_root / "linux-packaging-routing",
            get_platform_gate_generator("linux-x64-packaging", host_profile),
            repo_root,
        )
        write_gate_record(
            artifact_root / "linux-packaging.gate.json",
            "linux-packaging",
            "routing-validated",
            "linux-x64-packaging",
            "Linux preset remains visible in CMakePresets, and its toolchain/router path was validated with the host-compatible generator.",
            host_profile,
        )

    if host_profile == "macos":
        write_step("Validate Linux packaging routing smoke")
        invoke_routing_build_smoke("linux-x64-packaging", repo_root / "build" / "toolchains" / "linux-x64.cmake", common_artifact_root / "linux-packaging-routing", get_platform_gate_generator("linux-x64-packaging", host_profile), repo_root)
        write_gate_record(
            artifact_root / "linux-packaging.gate.json",
            "linux-packaging",
            "routing-validated",
            "linux-x64-packaging",
            "Linux preset remains visible in CMakePresets, and its toolchain/router/harness path was validated with a host-compatible generator; full cross toolchain remains outside Roadmap 0.",
            host_profile,
        )

        write_step("Build macOS reference preset smoke")
        invoke_preset_build_smoke("macos-reference", repo_root, host_profile=host_profile)

        write_step("Run macOS reference desktop trace compare")
        trace_path = artifact_root / "macos-warmup-trace.runtime.json"
        run_checked(["dotnet", str(host_embedding_dll), "--trace-platform", "macos", "--trace-output", str(trace_path)], cwd=repo_root, failure_message="HostEmbeddingLite macOS trace export failed")
        run_checked([sys.executable, str(compare_script), str(repo_root / "tests" / "contracts" / "trace" / "snapshots" / "macos-warmup-trace.snapshot.json"), str(trace_path)], cwd=repo_root, failure_message="macOS reference desktop trace compare failed")
        write_gate_record(
            artifact_root / "macos-reference-desktop.gate.json",
            "macos-reference-desktop",
            "passed",
            "macos-reference",
            "macOS reference desktop gate passed with warmup trace compare.",
            host_profile,
        )

        write_step("Validate iOS packaging routing smoke")
        invoke_routing_build_smoke("ios-arm64-packaging", repo_root / "build" / "toolchains" / "ios-arm64.cmake", common_artifact_root / "ios-packaging-routing", get_platform_gate_generator("ios-arm64-packaging", host_profile), repo_root)
        write_gate_record(
            artifact_root / "ios-packaging.gate.json",
            "ios-packaging",
            "routing-validated",
            "ios-arm64-packaging",
            "iOS preset remains visible in CMakePresets, and its toolchain/router/harness path was validated with a host-compatible generator; full Apple packaging flow remains outside Roadmap 0.",
            host_profile,
        )

    write_step(f"verify-runtime-baseline completed for host profile '{host_profile}'")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
