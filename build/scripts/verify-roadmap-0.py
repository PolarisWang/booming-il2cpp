#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import platform
import shutil
import subprocess
import sys
import uuid
from pathlib import Path

toolchains_root = Path(__file__).resolve().parents[1] / "toolchains" / "run"
if str(toolchains_root) not in sys.path:
    sys.path.insert(0, str(toolchains_root))

from testing import contracts as contracts_module


def get_repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def write_step(message: str) -> None:
    print(f"==> {message}")


def get_host_routing_generator(host_profile: str) -> str:
    if host_profile == "windows":
        return "Visual Studio 17 2022"
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


def allocate_run_scoped_binary_dir(base_dir: Path) -> Path:
    parent = base_dir.parent
    scoped = parent / f"{base_dir.name}-{os.getpid()}-{uuid.uuid4().hex[:8]}"
    scoped.mkdir(parents=True, exist_ok=True)
    return scoped


def invoke_native_smoke_build(source_dir: Path, binary_dir: Path, repo_root: Path) -> None:
    run_binary_dir = allocate_run_scoped_binary_dir(binary_dir)
    run_checked(["cmake", "-S", str(source_dir), "-B", str(run_binary_dir)], cwd=repo_root, failure_message=f"cmake configure failed: {source_dir}")
    run_checked(["cmake", "--build", str(run_binary_dir)], cwd=repo_root, failure_message=f"cmake build failed: {run_binary_dir}")


def invoke_dotnet_build(project_path: Path, repo_root: Path) -> None:
    run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=repo_root, failure_message=f"dotnet build failed: {project_path}")


def invoke_dotnet_runtime_smoke(dll_path: Path, expected_patterns: list[str], repo_root: Path) -> None:
    completed = run_checked(["dotnet", str(dll_path)], cwd=repo_root, capture_output=True, failure_message=f"dotnet runtime smoke failed: {dll_path}")
    joined_output = "\n".join(part for part in [completed.stdout.strip(), completed.stderr.strip()] if part)
    for pattern in expected_patterns:
        if pattern not in joined_output:
            raise RuntimeError(f"runtime smoke output mismatch for {dll_path}: missing '{pattern}'")


def invoke_preset_build_smoke(preset_name: str, repo_root: Path, *, validate_only: bool = False) -> None:
    requested_binary_dir = repo_root / "artifacts" / "presets" / preset_name
    run_binary_dir = allocate_run_scoped_binary_dir(requested_binary_dir)
    arguments = ["cmake", "--preset", preset_name, "-B", str(run_binary_dir)]
    if validate_only:
        arguments.append("-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON")

    run_checked(arguments, cwd=repo_root, failure_message=f"cmake preset configure failed: {preset_name}")
    run_checked(["cmake", "--build", str(run_binary_dir)], cwd=repo_root, failure_message=f"cmake preset build failed: {preset_name}")


def invoke_routing_build_smoke(preset_target: str, toolchain_file: Path, binary_dir: Path, generator: str, repo_root: Path) -> None:
    run_binary_dir = allocate_run_scoped_binary_dir(binary_dir)

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
    artifact_root = repo_root / "artifacts" / "verify-roadmap-0" / host_profile
    common_artifact_root = artifact_root / "common"
    compare_script = repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"
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
    invoke_native_smoke_build(repo_root / "tests" / "contracts" / "native" / "abi", common_artifact_root / "native-abi-config", repo_root)

    write_step("Build native bridge compile-only smoke")
    invoke_native_smoke_build(repo_root / "tests" / "contracts" / "native" / "bridge", common_artifact_root / "native-bridge-config", repo_root)

    write_step("Build smoke input projects")
    for project_name in ("HelloWorld", "GenericEcho", "ReflectionLite", "PInvokeLite", "HostEmbeddingLite"):
        invoke_dotnet_build(repo_root / "tests" / "smoke" / "input" / project_name / f"{project_name}.csproj", repo_root)

    write_step("Run managed smoke projects")
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "HelloWorld" / "Release" / "net8.0" / "HelloWorld.dll", ["HelloWorld smoke entry reached.", "register:Main"], repo_root)
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "GenericEcho" / "Release" / "net8.0" / "GenericEcho.dll", ["roadmap0", "42", "roadmap0:roadmap0"], repo_root)
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "ReflectionLite" / "Release" / "net8.0" / "ReflectionLite.dll", ["field=BackingField:Int32", "generic-method=String"], repo_root)
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "PInvokeLite" / "Release" / "net8.0" / "PInvokeLite.dll", ["marshal=interop-smoke", "export=boom_smoke_add:7", "symbol=True"], repo_root)
    invoke_dotnet_runtime_smoke(repo_root / "artifacts" / "smoke" / "bin" / "HostEmbeddingLite" / "Release" / "net8.0" / "HostEmbeddingLite.dll", ["HostEmbeddingSession:InvokeManagedEntry:True", "guards=invalid-detach:True|double-start:True|unattached-entry:True"], repo_root)

    write_step("Validate Linux packaging routing smoke")
    invoke_routing_build_smoke("linux-x64-packaging", repo_root / "build" / "toolchains" / "linux-x64.cmake", common_artifact_root / "linux-packaging-routing", get_platform_gate_generator("linux-x64-packaging", host_profile), repo_root)

    write_step("Register Linux packaging shell gate")
    write_gate_record(
        artifact_root / "linux-packaging.gate.json",
        "linux-packaging",
        "routing-validated",
        "linux-x64-packaging",
        "Linux preset remains visible in CMakePresets, and its toolchain/router/harness path was validated with a host-compatible generator; full cross toolchain remains outside Roadmap 0.",
        host_profile,
    )

    if host_profile == "windows":
        write_step("Build Windows reference preset smoke")
        invoke_preset_build_smoke("windows-x64-reference", repo_root)

        write_step("Run Windows reference desktop trace compare")
        trace_path = artifact_root / "windows-warmup-trace.runtime.json"
        run_checked(["dotnet", str(host_embedding_dll), "--trace-platform", "windows", "--trace-output", str(trace_path)], cwd=repo_root, failure_message="HostEmbeddingLite windows trace export failed")
        run_checked([sys.executable, str(compare_script), str(repo_root / "tests" / "contracts" / "trace" / "snapshots" / "windows-warmup-trace.snapshot.json"), str(trace_path)], cwd=repo_root, failure_message="Windows reference desktop trace compare failed")

        write_gate_record(
            artifact_root / "windows-reference-desktop.gate.json",
            "windows-reference-desktop",
            "passed",
            "windows-x64-reference",
            "Windows reference desktop gate passed with warmup trace compare.",
            host_profile,
        )

        write_step("Validate Android startup smoke routing smoke")
        invoke_routing_build_smoke("android-arm64-smoke", repo_root / "build" / "toolchains" / "android-arm64.cmake", common_artifact_root / "android-smoke-routing", get_platform_gate_generator("android-arm64-smoke", host_profile), repo_root)

        write_step("Register Android startup smoke shell gate")
        write_gate_record(
            artifact_root / "android-startup-smoke.gate.json",
            "android-startup-smoke",
            "routing-validated",
            "android-arm64-smoke",
            "Android preset remains visible in CMakePresets, and its toolchain/router/harness path was validated with a host-compatible generator; real Android toolchain execution remains outside Roadmap 0.",
            host_profile,
        )

    if host_profile == "macos":
        write_step("Build macOS reference preset smoke")
        invoke_preset_build_smoke("macos-reference", repo_root)

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

        write_step("Register iOS packaging shell gate")
        write_gate_record(
            artifact_root / "ios-packaging.gate.json",
            "ios-packaging",
            "routing-validated",
            "ios-arm64-packaging",
            "iOS preset remains visible in CMakePresets, and its toolchain/router/harness path was validated with a host-compatible generator; full Apple packaging flow remains outside Roadmap 0.",
            host_profile,
        )

    write_step(f"verify-roadmap-0 completed for host profile '{host_profile}'")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
