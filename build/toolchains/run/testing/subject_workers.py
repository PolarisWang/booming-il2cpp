from __future__ import annotations

import json
import os
from pathlib import Path
import shlex
import subprocess
from typing import Any
import statistics
import sys
import time

try:
    from ..core.common import combine_process_output, read_json, run_process, write_json
    from ..core import tooling as tooling_module
    from . import contracts as contracts_module
    from . import mobile_perf_collector
    from . import perf as perf_module
    from . import subjects as subjects_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import combine_process_output, read_json, run_process, write_json
    from core import tooling as tooling_module
    from testing import contracts as contracts_module
    from testing import mobile_perf_collector
    from testing import perf as perf_module
    from testing import subjects as subjects_module


DRIVER_PROJECT_PATH = Path("src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj")
DRIVER_DLL_PATH = Path("src/managed/Chaos.IL2CPP.Driver/bin/Release/net8.0/Chaos.IL2CPP.Driver.dll")
WINDOWS_TRACE_SNAPSHOT_PATH = Path("tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json")
MACOS_TRACE_SNAPSHOT_PATH = Path("tests/contracts/trace/snapshots/macos-warmup-trace.snapshot.json")
TRACE_SCHEMA_PATH = Path("tests/contracts/trace/schema/warmup-trace.schema.json")
WINDOWS_REFERENCE_BUILD_TARGET = "chaos_subject_reference_proof"
WINDOWS_REFERENCE_RUN_TARGET = "chaos_subject_reference_proof_run"
WINDOWS_DIRECT_BUILD_STRATEGY = "direct-msvc"
ANDROID_NATIVE_BUILD_STRATEGY = "android-native-cmake"
ANDROID_RUNTIME_BUILD_TARGET = "mobile_hello_world_android_host_runtime"
ANDROID_RUNTIME_REMOTE_ROOT = "/data/local/tmp/chaos-subjects"
ANDROID_EXIT_CODE_PREFIX = "__CHAOS_EXIT_CODE__="
ANDROID_RUNTIME_ARGUMENT_ENVIRONMENTS = {
    "--soak-duration-seconds=": "CHAOS_MOBILE_HOST_SOAK_DURATION_SECONDS",
    "--heartbeat-interval-seconds=": "CHAOS_MOBILE_HOST_HEARTBEAT_INTERVAL_SECONDS",
    "--subject-id=": "CHAOS_MOBILE_HOST_SUBJECT_ID",
}
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
ENGINE_OBSERVE_PREFIX = "CHAOS_ENGINE_OBSERVE "


def _resolve(repo_root: Path, relative_path: str) -> Path:
    return repo_root / Path(relative_path)


def _relative(repo_root: Path, path: Path) -> str:
    return path.relative_to(repo_root).as_posix()


def _selection_variant(selection: dict[str, Any]) -> str:
    return str(selection.get("variant") or "CHECK")


def _selection_runtime_arguments(selection: dict[str, Any]) -> list[str]:
    execution_context = dict(selection.get("executionContext") or {})
    return [str(value) for value in list(execution_context.get("runtimeArguments") or []) if str(value)]


def _android_runtime_environment_exports(runtime_arguments: list[str]) -> list[str]:
    assignments: list[str] = []
    for argument in runtime_arguments:
        for prefix, environment_name in ANDROID_RUNTIME_ARGUMENT_ENVIRONMENTS.items():
            if not argument.startswith(prefix):
                continue

            value = argument[len(prefix):]
            if value:
                assignments.append(f"export {environment_name}={shlex.quote(value)} >/dev/null")
            break
    return assignments


def _variant_macros(variant: str) -> dict[str, list[str]]:
    if variant not in VARIANT_MACROS:
        raise RuntimeError(f"unsupported subject variant: {variant}")
    return dict(VARIANT_MACROS[variant])


def _trace_platform(selection: dict[str, Any]) -> str:
    execution_context = dict(selection.get("executionContext") or {})
    target_platform = str(execution_context.get("targetPlatform") or execution_context.get("hostPlatform") or "")
    if target_platform.startswith("windows"):
        return "windows"
    if target_platform.startswith("macos"):
        return "macos"
    raise RuntimeError(f"unsupported trace platform: {target_platform}")


def _trace_snapshot_path(trace_platform: str) -> Path:
    if trace_platform == "windows":
        return WINDOWS_TRACE_SNAPSHOT_PATH
    if trace_platform == "macos":
        return MACOS_TRACE_SNAPSHOT_PATH
    raise RuntimeError(f"unsupported trace platform: {trace_platform}")


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


def _windows_variant_build_flags(variant: str) -> tuple[list[str], list[str]]:
    if variant == "CHECK":
        return ["/Od", "/Zi"], []
    if variant == "PROFILE":
        return ["/O2", "/DNDEBUG"], []
    if variant == "SHIP":
        return ["/O2", "/GL", "/DNDEBUG"], ["/LTCG"]
    raise RuntimeError(f"unsupported subject variant: {variant}")


def _subject_mobile_host_root(repo_root: Path, subject_id: str, target_platform: str) -> Path:
    if target_platform == "android-arm64":
        candidate = repo_root / "subjects" / subject_id / "validation" / "mobile" / "android-host"
        fallback = repo_root / "tests" / "gate" / "android-smoke"
    elif target_platform == "ios-arm64":
        candidate = repo_root / "subjects" / subject_id / "validation" / "mobile" / "ios-host"
        fallback = repo_root / "tests" / "gate" / "ios-smoke"
    else:
        raise RuntimeError(f"unsupported mobile target platform: {target_platform}")

    return candidate if candidate.is_dir() else fallback


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


def _engine_profile(repo_root: Path, subject_id: str) -> dict[str, Any]:
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    return dict(manifest.get("engineProofProfile") or {})


def _engine_contract_summary(profile: dict[str, Any]) -> dict[str, Any]:
    if not profile:
        return {}

    focus_area = str(profile.get("focusArea") or "")
    engine_binding_kinds = [str(value) for value in list(profile.get("engineBindingKinds") or []) if str(value)]
    if not engine_binding_kinds and focus_area:
        engine_binding_kinds = [focus_area]

    host_binding_kinds = [str(value) for value in list(profile.get("hostBindingKinds") or []) if str(value)]
    if not host_binding_kinds:
        host_binding_kinds = ["artifact-observe-contract"]

    return {
        "proofKind": str(profile.get("proofKind") or "engine-binding"),
        "focusArea": focus_area,
        "resolvedCapabilityIds": [
            str(value)
            for value in list(profile.get("expectedCapabilityIds") or [])
            if str(value)
        ],
        "engineBindingKinds": engine_binding_kinds,
        "hostBindingKinds": host_binding_kinds,
        "expectedEvidenceKinds": [
            str(value)
            for value in list(profile.get("expectedEvidenceKinds") or [])
            if str(value)
        ],
    }


def _engine_lowering_bindings(lowering_plan: dict[str, Any]) -> tuple[dict[str, Any], dict[str, Any]]:
    engine_bindings = dict(lowering_plan.get("engineBindings") or lowering_plan.get("EngineBindings") or {})
    host_bindings = dict(lowering_plan.get("hostBindings") or lowering_plan.get("HostBindings") or {})
    return engine_bindings, host_bindings


def _engine_emission_summary(
    lowering_plan: dict[str, Any],
    *,
    generated_source_path: str,
    native_reference_manifest_path: str,
) -> dict[str, Any]:
    engine_bindings, host_bindings = _engine_lowering_bindings(lowering_plan)
    if not engine_bindings:
        return {}

    return {
        "proofKind": str(engine_bindings.get("proofKind") or "engine-binding"),
        "focusArea": str(engine_bindings.get("focusArea") or ""),
        "emittedCapabilityIds": [
            str(value)
            for value in list(engine_bindings.get("capabilityIds") or [])
            if str(value)
        ],
        "engineBindingKinds": [
            str(value)
            for value in list(engine_bindings.get("bindingKinds") or [])
            if str(value)
        ],
        "hostBindingKinds": [
            str(value)
            for value in list(host_bindings.get("bindingKinds") or [])
            if str(value)
        ],
        "bridgeArtifactPaths": [generated_source_path],
        "registrationArtifactPaths": [native_reference_manifest_path],
    }


def _parse_engine_observations(stdout_text: str) -> list[dict[str, Any]]:
    observations: list[dict[str, Any]] = []
    for line in stdout_text.splitlines():
        if not line.startswith(ENGINE_OBSERVE_PREFIX):
            continue

        payload_text = line[len(ENGINE_OBSERVE_PREFIX) :].strip()
        if not payload_text:
            continue

        try:
            payload = json.loads(payload_text)
        except ValueError:
            continue

        if isinstance(payload, dict):
            observations.append(payload)
    return observations


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
    driver_arguments = ["dotnet", str(driver_dll_path), str(assembly_path), str(output_root)]
    entry_point_subject_id = str(dict(selection.get("source") or {}).get("entry") or "")
    if entry_point_subject_id:
        driver_arguments.extend(["--entry-point-subject-id", entry_point_subject_id])

    _run_checked(
        driver_arguments,
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
            "snapshotPath": "tests/contracts/analysis/v0/snapshots",
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
            "optimizationFactsPath": _relative(repo_root, output_root / "optimization-facts.json"),
            "preserveDescriptorPath": _relative(repo_root, output_root / "preserve-descriptor.json"),
            "closureManifestPath": _relative(repo_root, output_root / "closure.manifest.json"),
        },
    }
    details: dict[str, Any] = {}
    engine_contract_summary = _engine_contract_summary(
        _engine_profile(repo_root, str(request["selection"]["subjectId"])),
    )
    if engine_contract_summary:
        manifest["engineContractSummary"] = engine_contract_summary
        details["engineContractSummary"] = engine_contract_summary
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[
            manifest["artifacts"]["typedIlIrPath"],
            manifest["artifacts"]["optimizationFactsPath"],
            manifest["artifacts"]["closureManifestPath"],
        ],
        details=details,
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
        "nativeReferenceManifestPath": _relative(repo_root, output_root / "native-reference.manifest.json"),
        "nativeReferencePlanPath": _relative(repo_root, output_root / "native-reference.plan.json"),
    }
    details: dict[str, Any] = {}
    lowering_plan = read_json(output_root / "native-reference.plan.json")
    if isinstance(lowering_plan, dict):
        engine_emission_summary = _engine_emission_summary(
            lowering_plan,
            generated_source_path=generated_manifest["generatedSourcePath"],
            native_reference_manifest_path=generated_manifest["nativeReferenceManifestPath"],
        )
        if engine_emission_summary:
            generated_manifest["engineEmissionSummary"] = engine_emission_summary
            details["engineEmissionSummary"] = engine_emission_summary
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), generated_manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[
            generated_manifest["generatedSourcePath"],
            generated_manifest["nativeReferenceManifestPath"],
        ],
        details=details,
    )


def _windows_subject_build(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    selection = dict(request["selection"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    compiler_path = tooling_module.find_visual_cpp_executable()
    if not compiler_path:
        raise RuntimeError("MSVC cl.exe not found for windows-x64 subject build")

    developer_env = tooling_module.windows_developer_environment() or None
    compile_flags, link_flags = _windows_variant_build_flags(variant)
    out_root = build_root / "out"
    obj_root = build_root / "obj"
    generated_root = _resolve(repo_root, request["upstream"]["generated"]["manifestPath"]).parent
    proof_root = repo_root / "subjects" / str(selection["subjectId"]) / "validation" / "proof" / "native-reference"
    generated_source_path = generated_root / "generated" / "native-reference.generated.cpp"
    proof_exe = out_root / f"{WINDOWS_REFERENCE_BUILD_TARGET}.exe"

    out_root.mkdir(parents=True, exist_ok=True)
    obj_root.mkdir(parents=True, exist_ok=True)

    include_roots = [
        repo_root / "contracts" / "native" / "v0",
        repo_root / "contracts" / "engine" / "v0",
        repo_root / "src" / "native" / "runtime-core",
        repo_root / "src" / "native" / "engine-bridge",
        repo_root / "src" / "native" / "bootstrap",
        repo_root / "src" / "native" / "support",
    ]
    source_files = [
        repo_root / "src" / "native" / "runtime-core" / "runtime_core.cpp",
        repo_root / "src" / "native" / "engine-bridge" / "engine_bridge.cpp",
        repo_root / "src" / "native" / "bootstrap" / "bootstrap.cpp",
        repo_root / "src" / "native" / "support" / "support.cpp",
        proof_root / "main.cpp",
        generated_source_path,
    ]

    for source_file in source_files:
        if not source_file.is_file():
            raise RuntimeError(f"subject proof source is missing: {source_file}")

    _run_checked(
        [
            str(compiler_path),
            "/nologo",
            "/std:c++17",
            "/EHsc",
            "/DWIN32",
            "/D_WINDOWS",
            "/DCHAOS_RUNTIME_ABI_STATIC",
            *[f"/D{macro}" if '"' not in macro else f'/D{macro.replace("\"", "\\\"")}' for macro in variant_macros["native"]],
            *compile_flags,
            *[f"/I{include_root}" for include_root in include_roots],
            f"/Fo{obj_root}\\",
            f"/Fd{obj_root / 'chaos_subject_reference_proof.pdb'}",
            f"/Fe{proof_exe}",
            *[str(source_file) for source_file in source_files],
            *(["/link", *link_flags] if link_flags else []),
        ],
        repo_root=repo_root,
        failure_message="subject proof build failed: windows-x64-reference",
        env=developer_env,
    )

    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "build",
        "targetPlatform": str(request["selection"]["executionContext"]["targetPlatform"]),
        "toolchainProfile": str(request["selection"]["executionContext"]["toolchainProfile"]),
        "variant": variant,
        "buildStrategy": WINDOWS_DIRECT_BUILD_STRATEGY,
        "compilerPath": str(compiler_path),
        "variantMacros": {
            "codegen": list(variant_macros["codegen"]),
            "native": list(variant_macros["native"]),
        },
        "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
        "binaryRoot": _relative(repo_root, out_root),
        "outputs": [_relative(repo_root, proof_exe)],
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, proof_exe)],
    )


def _android_runtime_build_environment(repo_root: Path) -> tuple[str, dict[str, str], str]:
    cmake_path, env = tooling_module.cmake_environment(repo_root)
    if not cmake_path or not dict(env or {}).get("ANDROID_NDK_ROOT"):
        bootstrap = tooling_module.ensure_android_host_tooling_available(
            "subject android runtime build",
            "windows",
            repo_root,
        )
        if not bootstrap.ready:
            raise RuntimeError((bootstrap.output or "android host tooling bootstrap failed").strip())
        cmake_path, env = tooling_module.cmake_environment(repo_root)

    ninja_path = tooling_module.find_ninja_executable()
    if not cmake_path:
        raise RuntimeError("cmake not found for Android runtime build")
    if not ninja_path:
        raise RuntimeError("ninja not found for Android runtime build")
    return cmake_path, dict(env or {}), ninja_path


def _android_subject_runtime_build(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    selection = dict(request["selection"])
    execution_context = dict(selection.get("executionContext") or {})
    subject_id = str(selection["subjectId"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    android_host_root = _subject_mobile_host_root(repo_root, subject_id, "android-arm64")
    out_root = build_root / "out"
    out_root.mkdir(parents=True, exist_ok=True)

    cmake_path, env, ninja_path = _android_runtime_build_environment(repo_root)
    if host_platform == "windows":
        cmake_binary_dir = tooling_module.allocate_cmake_binary_dir(
            build_root / "cmake",
            host_platform=host_platform,
            generator="Ninja",
        )
    else:
        cmake_binary_dir = build_root / "cmake"

    runtime_binary_path = out_root / ANDROID_RUNTIME_BUILD_TARGET
    _run_checked(
        [
            str(cmake_path),
            "-S",
            str(repo_root),
            "-B",
            str(cmake_binary_dir),
            "-G",
            "Ninja",
            "-DROADMAP0_PRESET_TARGET=android-arm64-smoke",
            f"-DCHAOS_SUBJECT_VARIANT={variant}",
            f"-DCHAOS_SUBJECT_ANDROID_HOST_ROOT={android_host_root}",
            f"-DCHAOS_SUBJECT_ANDROID_ARTIFACT_ROOT={out_root}",
            f"-DCMAKE_TOOLCHAIN_FILE={repo_root / 'build' / 'toolchains' / 'android-arm64.cmake'}",
            "-DCMAKE_BUILD_TYPE=Release",
            f"-DCMAKE_MAKE_PROGRAM={ninja_path}",
        ],
        repo_root=repo_root,
        failure_message="subject proof build failed: android-arm64-runtime",
        env=env,
    )
    _run_checked(
        [
            str(cmake_path),
            "--build",
            str(cmake_binary_dir),
            "--target",
            ANDROID_RUNTIME_BUILD_TARGET,
        ],
        repo_root=repo_root,
        failure_message="subject proof build failed: android-arm64-runtime",
        env=env,
    )

    manifest = {
        "subjectId": subject_id,
        "matrixId": str(selection["matrixId"]),
        "bucket": "build",
        "targetPlatform": str(execution_context["targetPlatform"]),
        "toolchainProfile": str(execution_context["toolchainProfile"]),
        "variant": variant,
        "variantMacros": {
            "codegen": list(variant_macros["codegen"]),
            "native": list(variant_macros["native"]),
        },
        "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
        "buildStrategy": ANDROID_NATIVE_BUILD_STRATEGY,
        "binaryRoot": _relative(repo_root, out_root),
        "outputs": [_relative(repo_root, runtime_binary_path)],
        "cmakeBinaryDir": tooling_module.path_text(repo_root, cmake_binary_dir),
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, runtime_binary_path)],
    )


def _validate_only_build(
    *,
    repo_root: Path,
    request: dict[str, Any],
    preset_target: str,
    toolchain_file: Path,
    extra_cache_entries: list[str] | None = None,
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
            *(list(extra_cache_entries or [])),
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
    subject_id = str(request["selection"]["subjectId"])
    execution_context = dict(request["selection"].get("executionContext") or {})
    target_platform = str(execution_context["targetPlatform"])
    if target_platform == "windows-x64":
        return _windows_subject_build(repo_root=repo_root, request=request)
    if target_platform == "android-arm64":
        runtime_profile = str(execution_context.get("runtimeProfile") or "")
        if runtime_profile in {"android-native-runtime", "android-native-perf-profile"}:
            return _android_subject_runtime_build(repo_root=repo_root, request=request)
        android_host_root = _subject_mobile_host_root(repo_root, subject_id, target_platform)
        return _validate_only_build(
            repo_root=repo_root,
            request=request,
            preset_target="android-arm64-smoke",
            toolchain_file=Path("build/toolchains/android-arm64.cmake"),
            extra_cache_entries=[f"-DCHAOS_SUBJECT_ANDROID_HOST_ROOT={android_host_root}"],
        )
    if target_platform == "ios-arm64":
        ios_host_root = _subject_mobile_host_root(repo_root, subject_id, target_platform)
        return _validate_only_build(
            repo_root=repo_root,
            request=request,
            preset_target="ios-arm64-packaging",
            toolchain_file=Path("build/toolchains/ios-arm64.cmake"),
            extra_cache_entries=[f"-DCHAOS_SUBJECT_IOS_HOST_ROOT={ios_host_root}"],
        )
    if target_platform == "linux-x64":
        return _validate_only_build(
            repo_root=repo_root,
            request=request,
            preset_target="linux-x64-packaging",
            toolchain_file=Path("build/toolchains/linux-x64.cmake"),
        )
    raise RuntimeError(f"unsupported build target platform: {target_platform}")


def _android_adb_executable(repo_root: Path) -> str:
    candidate = tooling_module.android_adb_path(repo_root)
    return str(candidate if candidate.is_file() else Path("adb"))


def _android_emulator_executable(repo_root: Path) -> str:
    candidate = tooling_module.android_emulator_path(repo_root)
    return str(candidate if candidate.is_file() else Path("emulator"))


def _run_android_host_command(
    arguments: list[str],
    *,
    repo_root: Path,
    env: dict[str, str],
    failure_message: str,
) -> subprocess.CompletedProcess[str]:
    completed = run_process(arguments, cwd=repo_root, env=env)
    if completed.returncode != 0:
        raise RuntimeError(f"{failure_message}\n{combine_process_output(completed)}".strip())
    return completed


def _android_device_serials(*, repo_root: Path, env: dict[str, str]) -> list[str]:
    completed = run_process([_android_adb_executable(repo_root), "devices"], cwd=repo_root, env=env)
    if completed.returncode != 0:
        return []

    serials: list[str] = []
    for line in (completed.stdout or "").splitlines():
        line = line.strip()
        if not line or line.startswith("List of devices attached") or "\t" not in line:
            continue
        serial, state = line.split("\t", 1)
        if serial and state:
            serials.append(serial)
    return serials


def _launch_android_emulator(
    *,
    repo_root: Path,
    env: dict[str, str],
) -> tuple[str, subprocess.Popen[Any], Any, Any]:
    existing_serials = set(_android_device_serials(repo_root=repo_root, env=env))
    merged_env = dict(os.environ)
    merged_env.update(env)
    process = subprocess.Popen(
        [
            _android_emulator_executable(repo_root),
            "-avd",
            tooling_module.ANDROID_AVD_NAME,
            "-no-window",
            "-no-audio",
            "-no-boot-anim",
            "-no-snapshot",
        ],
        cwd=str(repo_root),
        env=merged_env,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )

    deadline = time.time() + 90
    while time.time() < deadline:
        if process.poll() is not None:
            raise RuntimeError("android emulator exited before adb exposed a device serial")

        current_serials = {
            serial
            for serial in _android_device_serials(repo_root=repo_root, env=env)
            if serial.startswith("emulator-")
        }
        new_serials = sorted(current_serials - existing_serials)
        if new_serials:
            return new_serials[0], process, None, None
        time.sleep(2)

    raise RuntimeError("android emulator did not expose an adb device serial in time")


def _wait_for_android_boot_completed(
    *,
    repo_root: Path,
    serial: str,
    env: dict[str, str],
    timeout_seconds: int = 300,
) -> None:
    adb_executable = _android_adb_executable(repo_root)
    _run_android_host_command(
        [adb_executable, "-s", serial, "wait-for-device"],
        repo_root=repo_root,
        env=env,
        failure_message=f"android emulator did not become available: {serial}",
    )

    deadline = time.time() + timeout_seconds
    while time.time() < deadline:
        completed = run_process(
            [adb_executable, "-s", serial, "shell", "getprop", "sys.boot_completed"],
            cwd=repo_root,
            env=env,
        )
        if completed.returncode == 0 and (completed.stdout or "").strip() == "1":
            return
        time.sleep(2)

    raise RuntimeError(f"android emulator boot timed out: {serial}")


def _run_android_binary_via_adb(
    *,
    repo_root: Path,
    executable_path: Path,
    serial: str,
    env: dict[str, str],
    runtime_arguments: list[str] | None = None,
) -> subprocess.CompletedProcess[str]:
    adb_executable = _android_adb_executable(repo_root)
    remote_path = f"{ANDROID_RUNTIME_REMOTE_ROOT}/{executable_path.name}"
    selected_runtime_arguments = list(runtime_arguments or [])
    _run_android_host_command(
        [adb_executable, "-s", serial, "shell", "mkdir", "-p", ANDROID_RUNTIME_REMOTE_ROOT],
        repo_root=repo_root,
        env=env,
        failure_message=f"failed to prepare Android runtime directory: {ANDROID_RUNTIME_REMOTE_ROOT}",
    )
    _run_android_host_command(
        [adb_executable, "-s", serial, "push", str(executable_path), remote_path],
        repo_root=repo_root,
        env=env,
        failure_message=f"failed to push Android runtime binary: {executable_path}",
    )
    _run_android_host_command(
        [adb_executable, "-s", serial, "shell", "chmod", "755", remote_path],
        repo_root=repo_root,
        env=env,
        failure_message=f"failed to chmod Android runtime binary: {remote_path}",
    )
    binary_command_text = " ".join(
        [
            shlex.quote(remote_path),
            *[shlex.quote(argument) for argument in selected_runtime_arguments],
        ]
    ).strip()
    command_text = "; ".join(
        [
            *_android_runtime_environment_exports(selected_runtime_arguments),
            binary_command_text,
            f"status=$?; printf '\\n{ANDROID_EXIT_CODE_PREFIX}%s\\n' \"$status\"",
        ]
    ).strip()
    return run_process(
        [
            adb_executable,
            "-s",
            serial,
            "shell",
            "sh",
            "-c",
            command_text,
        ],
        cwd=repo_root,
        env=env,
    )


def _shutdown_android_emulator(
    *,
    repo_root: Path,
    serial: str | None,
    process: subprocess.Popen[Any] | None,
    stdout_handle: Any,
    stderr_handle: Any,
    env: dict[str, str],
) -> None:
    try:
        if serial:
            run_process([_android_adb_executable(repo_root), "-s", serial, "emu", "kill"], cwd=repo_root, env=env)
    finally:
        if process is not None:
            try:
                process.wait(timeout=20)
            except subprocess.TimeoutExpired:
                process.terminate()
                try:
                    process.wait(timeout=10)
                except subprocess.TimeoutExpired:
                    process.kill()
                    process.wait(timeout=5)
        for handle in [stdout_handle, stderr_handle]:
            if handle is not None and hasattr(handle, "close"):
                handle.close()


def _extract_android_runtime_output(completed: subprocess.CompletedProcess[str]) -> tuple[str, list[str], int]:
    output_lines: list[str] = []
    stdout_lines: list[str] = []
    exit_code: int | None = None
    for line in (completed.stdout or "").splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        if stripped.startswith(ANDROID_EXIT_CODE_PREFIX):
            try:
                exit_code = int(stripped.split("=", 1)[1])
            except ValueError:
                exit_code = completed.returncode
            continue
        stdout_lines.append(line)
        output_lines.append(stripped)

    stdout_text = "\n".join(stdout_lines)
    if stdout_text:
        stdout_text += "\n"
    return stdout_text, output_lines, int(completed.returncode if exit_code is None else exit_code)


def run_runtime_observe(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_manifest = read_json(_resolve(repo_root, request["upstream"]["build"]["manifestPath"]))
    if not isinstance(build_manifest, dict):
        raise RuntimeError("build manifest must be an object")

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    build_strategy = str(build_manifest.get("buildStrategy") or "")

    if build_strategy == WINDOWS_DIRECT_BUILD_STRATEGY:
        output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
        if not output_paths:
            raise RuntimeError("direct-msvc build manifest missing outputs")

        native_executable_path = _resolve(repo_root, output_paths[0])
        completed = run_process([str(native_executable_path)], cwd=runtime_root)
        stdout_path.write_text(completed.stdout or "", encoding="utf-8")
        stderr_path.write_text(completed.stderr or "", encoding="utf-8")
        exit_code_path.write_text(f"{completed.returncode}\n", encoding="utf-8")

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

        if completed.returncode != 0:
            return {
                "status": "fail",
                "bucketManifestPath": request["paths"]["manifestPath"],
                "reportPaths": [],
                "primaryEvidencePaths": [manifest["stdoutPath"], manifest["exitCodePath"]],
                "metrics": {"durationMs": 0},
                "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
                "details": {},
                "failure": f"subject proof run failed: {native_executable_path}",
            }

        return _success_result(
            bucket_manifest_path=request["paths"]["manifestPath"],
            report_paths=[],
            primary_evidence_paths=[manifest["stdoutPath"], manifest["exitCodePath"]],
            stdout_path=manifest["stdoutPath"],
            stderr_path=manifest["stderrPath"],
        )

    if build_strategy == ANDROID_NATIVE_BUILD_STRATEGY:
        output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
        if not output_paths:
            raise RuntimeError("android-native-cmake build manifest missing outputs")

        selection = dict(request["selection"])
        execution_context = dict(selection.get("executionContext") or {})
        runtime_arguments = _selection_runtime_arguments(selection)
        host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
        bootstrap = tooling_module.ensure_android_host_tooling_available(
            "subject runtime observe",
            host_platform,
            repo_root,
        )
        if not bootstrap.ready:
            raise RuntimeError((bootstrap.output or "android host tooling bootstrap failed").strip())

        env = tooling_module.android_environment_overrides(repo_root)
        native_executable_path = _resolve(repo_root, output_paths[0])
        android_serial: str | None = None
        emulator_process: subprocess.Popen[Any] | None = None
        emulator_stdout = None
        emulator_stderr = None
        try:
            android_serial, emulator_process, emulator_stdout, emulator_stderr = _launch_android_emulator(
                repo_root=repo_root,
                env=env,
            )
            _wait_for_android_boot_completed(
                repo_root=repo_root,
                serial=android_serial,
                env=env,
            )
            completed = _run_android_binary_via_adb(
                repo_root=repo_root,
                executable_path=native_executable_path,
                serial=android_serial,
                env=env,
                runtime_arguments=runtime_arguments,
            )
        finally:
            _shutdown_android_emulator(
                repo_root=repo_root,
                serial=android_serial,
                process=emulator_process,
                stdout_handle=emulator_stdout,
                stderr_handle=emulator_stderr,
                env=env,
            )

        cleaned_stdout, output_lines, runtime_exit_code = _extract_android_runtime_output(completed)
        stdout_path.write_text(cleaned_stdout, encoding="utf-8")
        stderr_path.write_text(completed.stderr or "", encoding="utf-8")
        exit_code_path.write_text(f"{runtime_exit_code}\n", encoding="utf-8")

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
            "outputLines": output_lines,
            "androidSerial": android_serial,
            "androidAvdName": tooling_module.ANDROID_AVD_NAME,
            "runtimeArguments": runtime_arguments,
        }
        write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

        failure_reason: str | None = None
        if completed.returncode != 0:
            failure_reason = f"android adb execution failed: {native_executable_path}"
        elif runtime_exit_code != 0:
            failure_reason = f"android runtime observe failed: {native_executable_path}"

        if failure_reason is not None:
            return {
                "status": "fail",
                "bucketManifestPath": request["paths"]["manifestPath"],
                "reportPaths": [],
                "primaryEvidencePaths": [manifest["stdoutPath"], manifest["exitCodePath"]],
                "metrics": {"durationMs": 0},
                "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
                "details": {},
                "failure": failure_reason,
            }

        return _success_result(
            bucket_manifest_path=request["paths"]["manifestPath"],
            report_paths=[],
            primary_evidence_paths=[manifest["stdoutPath"], manifest["exitCodePath"]],
            stdout_path=manifest["stdoutPath"],
            stderr_path=manifest["stderrPath"],
        )

    cmake_path, developer_env, _ninja_path = _windows_native_cmake_context(repo_root)
    cmake_binary_dir = _resolve(repo_root, str(build_manifest["cmakeBinaryDir"]))

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


def run_runtime_engine_observe(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_manifest = read_json(_resolve(repo_root, request["upstream"]["build"]["manifestPath"]))
    if not isinstance(build_manifest, dict):
        raise RuntimeError("build manifest must be an object")
    if str(build_manifest.get("buildStrategy") or "") != WINDOWS_DIRECT_BUILD_STRATEGY:
        raise RuntimeError("engine runtime observe requires direct-msvc build output")

    output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
    if not output_paths:
        raise RuntimeError("direct-msvc build manifest missing outputs")

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    native_executable_path = _resolve(repo_root, output_paths[0])
    started = time.perf_counter()
    completed = run_process([str(native_executable_path)], cwd=runtime_root)
    duration_ms = int(round((time.perf_counter() - started) * 1000))

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    stdout_path.write_text(completed.stdout or "", encoding="utf-8")
    stderr_path.write_text(completed.stderr or "", encoding="utf-8")
    exit_code_path.write_text(f"{completed.returncode}\n", encoding="utf-8")

    observations = _parse_engine_observations(completed.stdout or "")
    evidence_items: list[dict[str, Any]] = []
    evidence_paths: list[str] = []
    for observation in observations:
        kind = str(observation.get("kind") or "")
        if not kind:
            continue

        artifact_file_name = str(observation.get("artifactFileName") or f"{kind}.json")
        evidence_path = runtime_root / artifact_file_name
        write_json(evidence_path, observation)
        evidence_relative_path = _relative(repo_root, evidence_path)
        evidence_paths.append(evidence_relative_path)
        evidence_items.append(
            {
                "kind": kind,
                "path": evidence_relative_path,
                "format": "json",
                "status": str(observation.get("status") or "ok"),
                "relatedCapabilityIds": [
                    str(value)
                    for value in list(observation.get("capabilityIds") or [])
                    if str(value)
                ],
            }
        )

    report_payload = {
        "reportVersion": "v1",
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "status": "ok" if completed.returncode == 0 and evidence_items else "fail",
        "evidenceItems": evidence_items,
    }
    write_json(_resolve(repo_root, request["paths"]["reportPaths"][0]), report_payload)

    engine_observation_summary = {
        "evidenceItems": evidence_items,
        "localReportPaths": [request["paths"]["reportPaths"][0]],
    }
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "runtime",
        "variant": _selection_variant(dict(request["selection"])),
        "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "tracePaths": evidence_paths,
        "engineEvidencePaths": evidence_paths,
        "engineObservationSummary": engine_observation_summary,
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

    if completed.returncode != 0 or not evidence_items:
        failure_reason = (
            f"engine proof run failed: {native_executable_path}"
            if completed.returncode != 0
            else "engine proof run did not emit observation payload"
        )
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": evidence_paths or [manifest["stdoutPath"], manifest["exitCodePath"]],
            "metrics": {"durationMs": duration_ms},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "details": {"engineObservationSummary": engine_observation_summary},
            "failure": failure_reason,
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=evidence_paths,
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
        duration_ms=duration_ms,
        details={"engineObservationSummary": engine_observation_summary},
    )


def run_runtime_engine_trace_compare(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    subject_id = str(request["selection"]["subjectId"])
    matrix_id = str(request["selection"]["matrixId"])
    subject_manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    expected_root_text = str(dict(subject_manifest.get("expected") or {}).get("runtime") or "")
    if not expected_root_text:
        raise RuntimeError(f"subject manifest missing expected.runtime for engine trace compare: {subject_id}")

    runtime_manifest_path = _resolve(repo_root, request["paths"]["manifestPath"])
    runtime_manifest = read_json(runtime_manifest_path) if runtime_manifest_path.is_file() else {}
    if not isinstance(runtime_manifest, dict):
        raise RuntimeError("runtime manifest must be an object")

    observation_summary = dict(runtime_manifest.get("engineObservationSummary") or {})
    evidence_items = [dict(item) for item in list(observation_summary.get("evidenceItems") or [])]
    if not evidence_items:
        raise RuntimeError("engine runtime trace compare requires observed evidence items")

    expected_root = _resolve(repo_root, expected_root_text) / matrix_id
    comparisons: list[dict[str, Any]] = []
    primary_evidence_paths: list[str] = []
    failed = False
    for item in evidence_items:
        observed_relative_path = str(item.get("path") or "")
        if not observed_relative_path:
            continue

        primary_evidence_paths.append(observed_relative_path)
        observed_path = _resolve(repo_root, observed_relative_path)
        expected_path = expected_root / Path(observed_relative_path).name

        observed_payload = read_json(observed_path)
        if not expected_path.is_file():
            failed = True
            comparisons.append(
                {
                    "kind": str(item.get("kind") or ""),
                    "status": "missing-expected",
                    "observedPath": observed_relative_path,
                    "expectedPath": _relative(repo_root, expected_path),
                }
            )
            continue

        expected_payload = read_json(expected_path)
        matched = observed_payload == expected_payload
        if not matched:
            failed = True

        comparisons.append(
            {
                "kind": str(item.get("kind") or ""),
                "status": "ok" if matched else "mismatch",
                "observedPath": observed_relative_path,
                "expectedPath": _relative(repo_root, expected_path),
            }
        )

    report_payload = {
        "reportVersion": "v1",
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "status": "fail" if failed else "ok",
        "expectedRootPath": _relative(repo_root, expected_root),
        "comparisons": comparisons,
    }
    write_json(_resolve(repo_root, request["paths"]["reportPaths"][0]), report_payload)

    runtime_manifest["engineTraceCompareReportPaths"] = [
        *[
            str(value)
            for value in list(runtime_manifest.get("engineTraceCompareReportPaths") or [])
            if str(value)
        ],
        request["paths"]["reportPaths"][0],
    ]
    write_json(runtime_manifest_path, runtime_manifest)

    if failed:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": primary_evidence_paths,
            "metrics": {"durationMs": 0},
            "diagnostics": {
                "stdoutPath": runtime_manifest.get("stdoutPath"),
                "stderrPath": runtime_manifest.get("stderrPath"),
            },
            "failure": f"engine trace compare failed: {subject_id}/{matrix_id}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=primary_evidence_paths,
        stdout_path=str(runtime_manifest.get("stdoutPath") or "") or None,
        stderr_path=str(runtime_manifest.get("stderrPath") or "") or None,
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


def _native_perf_warmup_count(runtime_profile: str) -> int:
    return 1 if "native-perf" in runtime_profile else 0


def _is_numeric_perf_value(value: Any) -> bool:
    return isinstance(value, (int, float)) and not isinstance(value, bool)


def _parse_perf_payload(output_lines: list[str]) -> dict[str, Any]:
    if not output_lines:
        return {}

    try:
        payload = json.loads(output_lines[-1])
    except ValueError:
        return {}

    return dict(payload) if isinstance(payload, dict) else {}


def _payload_custom_perf_metrics(payload: dict[str, Any]) -> dict[str, float]:
    metrics = dict(payload.get("metrics") or {})
    custom_metrics: dict[str, float] = {}
    for metric_name, metric_value in metrics.items():
        key = str(metric_name or "").strip()
        if not key or not _is_numeric_perf_value(metric_value):
            continue

        custom_metrics[key] = round(float(metric_value), 3)

    return custom_metrics


def _summary_metric_name(prefix: str, metric_name: str) -> str:
    if not metric_name:
        return prefix
    return f"{prefix}{metric_name[0].upper()}{metric_name[1:]}"


def _perf_summary_metrics(samples: list[dict[str, Any]]) -> dict[str, float | int]:
    counted_samples = [sample for sample in samples if bool(sample.get("countedInSummary", True))]
    durations = [
        float(sample["durationMs"])
        for sample in counted_samples
        if _is_numeric_perf_value(sample.get("durationMs"))
    ]
    summary_metrics: dict[str, float | int] = {
        "sampleCount": len(durations),
        "meanDurationMs": round(statistics.fmean(durations), 3) if durations else 0.0,
        "minDurationMs": round(min(durations), 3) if durations else 0.0,
        "maxDurationMs": round(max(durations), 3) if durations else 0.0,
    }

    reserved_keys = {"sampleIndex", "durationMs", "exitCode", "countedInSummary"}
    custom_metric_names = sorted(
        {
            str(metric_name)
            for sample in counted_samples
            for metric_name, metric_value in sample.items()
            if metric_name not in reserved_keys and _is_numeric_perf_value(metric_value)
        }
    )
    for metric_name in custom_metric_names:
        metric_values = [
            float(sample[metric_name])
            for sample in counted_samples
            if _is_numeric_perf_value(sample.get(metric_name))
        ]
        if not metric_values:
            continue

        summary_metrics[_summary_metric_name("mean", metric_name)] = round(statistics.fmean(metric_values), 3)
        summary_metrics[_summary_metric_name("min", metric_name)] = round(min(metric_values), 3)
        summary_metrics[_summary_metric_name("max", metric_name)] = round(max(metric_values), 3)

    return summary_metrics


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
        payload = _parse_perf_payload(output_lines)
        if _is_numeric_perf_value(payload.get("elapsedMilliseconds")):
            duration_ms = round(float(payload["elapsedMilliseconds"]), 3)

        sample = {
            "sampleIndex": sample_index + 1,
            "durationMs": duration_ms,
            "exitCode": last_exit_code,
        }
        sample.update(_payload_custom_perf_metrics(payload))
        samples.append(sample)
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


def run_native_runtime_perf(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_manifest = read_json(_resolve(repo_root, request["upstream"]["build"]["manifestPath"]))
    if not isinstance(build_manifest, dict):
        raise RuntimeError("build manifest must be an object")

    selection = dict(request["selection"])
    execution_context = dict(selection.get("executionContext") or {})
    subject_id = str(selection["subjectId"])
    matrix_id = str(selection["matrixId"])
    variant = _selection_variant(selection)
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    runtime_profile = str(execution_context.get("runtimeProfile") or "")
    sample_count = _perf_sample_count(runtime_profile)
    warmup_sample_count = _native_perf_warmup_count(runtime_profile)
    output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
    if not output_paths:
        raise RuntimeError("native perf build manifest missing outputs")

    native_executable_path = _resolve(repo_root, output_paths[0])
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    perf_runtime_path = runtime_root / "perf.runtime.json"
    perf_samples_path = runtime_root / "perf.samples.json"

    samples: list[dict[str, Any]] = []
    stdout_chunks: list[str] = []
    stderr_chunks: list[str] = []
    output_lines: list[str] = []
    last_exit_code = 0

    for sample_index in range(sample_count + warmup_sample_count):
        counted_in_summary = sample_index >= warmup_sample_count
        measured_sample_index = sample_index - warmup_sample_count + 1
        sample_label = (
            f"warmup sample {sample_index + 1}"
            if not counted_in_summary
            else f"sample {measured_sample_index}"
        )
        started = time.perf_counter()
        completed = run_process([str(native_executable_path)], cwd=repo_root)
        duration_ms = round((time.perf_counter() - started) * 1000, 3)
        stdout_text = completed.stdout or ""
        stderr_text = completed.stderr or ""
        output_lines = [line for line in stdout_text.splitlines() if line.strip()]
        last_exit_code = int(completed.returncode)
        payload = _parse_perf_payload(output_lines)
        if _is_numeric_perf_value(payload.get("elapsedMilliseconds")):
            duration_ms = round(float(payload["elapsedMilliseconds"]), 3)

        sample = {
            "sampleIndex": sample_index + 1,
            "durationMs": duration_ms,
            "exitCode": last_exit_code,
            "countedInSummary": counted_in_summary,
        }
        sample.update(_payload_custom_perf_metrics(payload))
        samples.append(sample)
        stdout_chunks.append(f"=== {sample_label} ({duration_ms:.3f} ms) ===\n{stdout_text}".rstrip() + "\n")
        if stderr_text:
            stderr_chunks.append(f"=== {sample_label} ({duration_ms:.3f} ms) ===\n{stderr_text}".rstrip() + "\n")
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
    runtime_evidence = {
        "runtimePath": _relative(repo_root, perf_runtime_path),
        "samplesPath": _relative(repo_root, perf_samples_path),
    }
    performance = {
        "samples": samples,
        "warmupSampleCount": warmup_sample_count,
        "metrics": dict(perf_result["metrics"]),
        "baselinePath": str(perf_result["baselinePath"]),
        "baseline": dict(perf_result["baseline"]),
        "baselineUpdated": bool(perf_result["baselineUpdated"]),
        "regressionStatus": str(perf_result["regressionStatus"]),
        "regressions": list(perf_result.get("regressions") or []),
        "runtimeEvidence": runtime_evidence,
    }
    write_json(
        perf_runtime_path,
        {
            "reportVersion": "v1",
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "variant": variant,
            "nativeExecutablePath": _relative(repo_root, native_executable_path),
            "warmupSampleCount": warmup_sample_count,
            "metrics": dict(performance["metrics"]),
            "baselinePath": str(performance["baselinePath"]),
            "baseline": dict(performance["baseline"]),
            "baselineUpdated": bool(performance["baselineUpdated"]),
            "regressionStatus": str(performance["regressionStatus"]),
            "regressions": list(performance["regressions"]),
            "stdoutPath": _relative(repo_root, stdout_path),
            "stderrPath": _relative(repo_root, stderr_path),
        },
    )
    write_json(
        perf_samples_path,
        {
            "reportVersion": "v1",
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "warmupSampleCount": warmup_sample_count,
            "samples": samples,
        },
    )

    manifest = {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "runtime",
        "variant": variant,
        "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
        "nativeExecutablePath": _relative(repo_root, native_executable_path),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "perfRuntimePath": runtime_evidence["runtimePath"],
        "perfSamplesPath": runtime_evidence["samplesPath"],
        "outputLines": output_lines,
        "warmupSampleCount": warmup_sample_count,
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
            "primaryEvidencePaths": [manifest["perfRuntimePath"], manifest["perfSamplesPath"]],
            "metrics": {"durationMs": int(round(sum(float(sample["durationMs"]) for sample in samples)))},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "details": {"performance": performance},
            "failure": f"native perf execution failed: {native_executable_path}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[manifest["perfRuntimePath"], manifest["perfSamplesPath"]],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
        duration_ms=int(round(sum(float(sample["durationMs"]) for sample in samples))),
        details={"performance": performance},
    )


def run_mobile_native_perf(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_manifest = read_json(_resolve(repo_root, request["upstream"]["build"]["manifestPath"]))
    if not isinstance(build_manifest, dict):
        raise RuntimeError("build manifest must be an object")

    selection = dict(request["selection"])
    execution_context = dict(selection.get("executionContext") or {})
    subject_id = str(selection["subjectId"])
    matrix_id = str(selection["matrixId"])
    target_platform = str(execution_context.get("targetPlatform") or "")
    host_platform = str(execution_context.get("hostPlatform") or "")
    runtime_profile = str(execution_context.get("runtimeProfile") or "")
    variant = _selection_variant(selection)
    sample_count = _perf_sample_count(runtime_profile)
    runtime_arguments = _selection_runtime_arguments(selection)

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    perf_runtime_path = runtime_root / "perf.runtime.json"
    perf_samples_path = runtime_root / "perf.samples.json"

    if target_platform == "android-arm64":
        output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
        if not output_paths:
            raise RuntimeError("mobile perf build manifest missing outputs")

        collector_result = mobile_perf_collector.collect_android_perf(
            repo_root=repo_root,
            runtime_root=runtime_root,
            native_executable_path=_resolve(repo_root, output_paths[0]),
            runtime_arguments=runtime_arguments,
            sample_count=sample_count,
            host_platform=host_platform,
        )
    elif target_platform == "ios-arm64":
        cmake_binary_dir_text = str(build_manifest.get("cmakeBinaryDir") or "")
        if not cmake_binary_dir_text:
            raise RuntimeError("mobile perf build manifest missing cmakeBinaryDir")

        collector_result = mobile_perf_collector.collect_ios_perf(
            repo_root=repo_root,
            runtime_root=runtime_root,
            cmake_binary_dir=_resolve(repo_root, cmake_binary_dir_text),
            sample_count=sample_count,
            host_platform=host_platform,
        )
    else:
        raise RuntimeError(f"unsupported mobile target platform: {target_platform}")

    stdout_text = str(collector_result.get("stdout") or "")
    stderr_text = str(collector_result.get("stderr") or "")
    output_lines = [line for line in stdout_text.splitlines() if line.strip()]
    exit_code = int(collector_result.get("exitCode") or 0)
    samples = [
        dict(sample)
        for sample in list(collector_result.get("samples") or [])
        if isinstance(sample, dict)
    ]
    collector_details = dict(collector_result.get("details") or {})
    collector_evidence_paths = [str(value) for value in list(collector_result.get("evidencePaths") or []) if str(value)]

    stdout_path.write_text(stdout_text, encoding="utf-8")
    stderr_path.write_text(stderr_text, encoding="utf-8")
    exit_code_path.write_text(f"{exit_code}\n", encoding="utf-8")

    summary_metrics = _perf_summary_metrics(samples)
    perf_result = perf_module.evaluate_perf_subject(
        repo_root=repo_root,
        subject_id=subject_id,
        matrix_id=matrix_id,
        host_platform=target_platform,
        metrics=summary_metrics,
        update_baseline=False,
    )
    runtime_evidence = {
        "runtimePath": _relative(repo_root, perf_runtime_path),
        "samplesPath": _relative(repo_root, perf_samples_path),
    }
    performance = {
        "samples": samples,
        "metrics": dict(perf_result["metrics"]),
        "baselinePath": str(perf_result["baselinePath"]),
        "baseline": dict(perf_result["baseline"]),
        "baselineUpdated": bool(perf_result["baselineUpdated"]),
        "regressionStatus": str(perf_result["regressionStatus"]),
        "regressions": list(perf_result.get("regressions") or []),
        "runtimeEvidence": runtime_evidence,
        "collectorDetails": collector_details,
        "collectorEvidencePaths": collector_evidence_paths,
    }
    write_json(
        perf_runtime_path,
        {
            "reportVersion": "v1",
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "variant": variant,
            "targetPlatform": target_platform,
            "metrics": dict(performance["metrics"]),
            "baselinePath": str(performance["baselinePath"]),
            "baseline": dict(performance["baseline"]),
            "baselineUpdated": bool(performance["baselineUpdated"]),
            "regressionStatus": str(performance["regressionStatus"]),
            "regressions": list(performance["regressions"]),
            "collectorDetails": collector_details,
            "collectorEvidencePaths": collector_evidence_paths,
            "stdoutPath": _relative(repo_root, stdout_path),
            "stderrPath": _relative(repo_root, stderr_path),
        },
    )
    write_json(
        perf_samples_path,
        {
            "reportVersion": "v1",
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "targetPlatform": target_platform,
            "samples": samples,
        },
    )

    manifest = {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "runtime",
        "variant": variant,
        "targetPlatform": target_platform,
        "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "perfRuntimePath": runtime_evidence["runtimePath"],
        "perfSamplesPath": runtime_evidence["samplesPath"],
        "collectorDetails": collector_details,
        "collectorEvidencePaths": collector_evidence_paths,
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

    failure_reason: str | None = None
    if exit_code != 0:
        failure_reason = f"mobile perf execution failed: {target_platform}"
    elif not samples:
        failure_reason = f"mobile perf collector returned no samples: {target_platform}"

    if failure_reason is not None:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": [manifest["perfRuntimePath"], manifest["perfSamplesPath"], *collector_evidence_paths],
            "metrics": {"durationMs": int(round(sum(float(sample.get("durationMs") or 0.0) for sample in samples)))},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "details": {"performance": performance},
            "failure": failure_reason,
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[manifest["perfRuntimePath"], manifest["perfSamplesPath"], *collector_evidence_paths],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
        duration_ms=int(round(sum(float(sample.get("durationMs") or 0.0) for sample in samples))),
        details={"performance": performance},
    )


def run_runtime_trace_compare(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    selection = dict(request["selection"])
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

    runtime_manifest_path = _resolve(repo_root, request["paths"]["manifestPath"])
    if runtime_manifest_path.is_file():
        runtime_manifest = read_json(runtime_manifest_path)
        if not isinstance(runtime_manifest, dict):
            raise RuntimeError("runtime manifest must be an object")
    else:
        runtime_manifest = {}

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)
    trace_path = runtime_root / "trace.runtime.json"
    trace_platform = _trace_platform(selection)
    expected_snapshot_path = _trace_snapshot_path(trace_platform)
    assembly_path = _resolve(repo_root, str(host_input_manifest["primaryAssemblyPath"]))
    _run_checked(
        [
            "dotnet",
            str(assembly_path),
            "--trace-platform",
            trace_platform,
            "--trace-output",
            str(trace_path),
        ],
        repo_root=repo_root,
        failure_message=f"trace export failed: {assembly_path}",
    )
    _run_checked(
        [
            sys.executable,
            str(repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"),
            str(repo_root / expected_snapshot_path),
            str(trace_path),
        ],
        repo_root=repo_root,
        failure_message=f"{trace_platform} trace compare failed",
    )

    runtime_manifest["subjectId"] = str(selection["subjectId"])
    runtime_manifest["matrixId"] = str(selection["matrixId"])
    runtime_manifest["bucket"] = "runtime"
    runtime_manifest["variant"] = _selection_variant(selection)
    runtime_manifest["hostInputManifestPath"] = str(request["upstream"]["host-input"]["manifestPath"])
    runtime_manifest["tracePaths"] = [_relative(repo_root, trace_path)]
    write_json(runtime_manifest_path, runtime_manifest)
    report_path = _resolve(repo_root, request["paths"]["reportPaths"][0])
    write_json(
        report_path,
        {
            "subjectId": str(selection["subjectId"]),
            "matrixId": str(selection["matrixId"]),
            "status": "ok",
            "variant": _selection_variant(selection),
            "schemaPath": TRACE_SCHEMA_PATH.as_posix(),
            "expectedSnapshotPath": expected_snapshot_path.as_posix(),
            "actualTracePath": _relative(repo_root, trace_path),
            "errors": [],
        },
    )
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[_relative(repo_root, trace_path)],
    )


def run_interpreter_runtime_perf(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    """Run the managed interpreter performance harness.

    Reads the compiled subject assembly from the host-input stage and invokes the
    interpreter harness project (if one is declared in the subject's manifest perf
    validation spec).  Output format is identical to run_runtime_perf_collect so
    that downstream comparison workers can treat all three modes uniformly.
    """
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

    # Look for an interpreter-specific harness project.
    # Subject may declare:  "validation": { "perf": { "driver": "interpreter-runtime-perf",
    #                                                  "project": "subjects/.../Harness.csproj" } }
    validation_spec = subjects_module.find_validation(manifest, "perf") or {}
    harness_project_path_str = str(validation_spec.get("project") or "")

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)
    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"

    if not harness_project_path_str:
        # No interpreter harness configured: return a clearly-labelled skip result so the
        # comparison stage can treat the missing data as "interpreter: N/A" rather than
        # propagating a hard failure.
        msg = (
            f"interpreter-runtime-perf: no harness project configured for subject '{subject_id}'. "
            "Add 'validation.perf.project' to the subject manifest to enable interpreter benchmarking."
        )
        stdout_path.write_text(msg + "\n", encoding="utf-8")
        stderr_path.write_text("", encoding="utf-8")
        exit_code_path.write_text("0\n", encoding="utf-8")

        skip_manifest = {
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "bucket": "runtime",
            "variant": variant,
            "mode": "interpreter",
            "stdoutPath": _relative(repo_root, stdout_path),
            "stderrPath": _relative(repo_root, stderr_path),
            "exitCodePath": _relative(repo_root, exit_code_path),
            "skipped": True,
            "skipReason": "no-harness-configured",
            "samples": [],
            "summaryMetrics": {},
            "regressionStatus": "no-baseline",
        }
        write_json(_resolve(repo_root, request["paths"]["manifestPath"]), skip_manifest)
        return _success_result(
            bucket_manifest_path=request["paths"]["manifestPath"],
            report_paths=list(request["paths"]["reportPaths"]),
            primary_evidence_paths=[_relative(repo_root, stdout_path)],
            stdout_path=_relative(repo_root, stdout_path),
            stderr_path=_relative(repo_root, stderr_path),
            duration_ms=0,
            details={"mode": "interpreter", "skipped": True},
        )

    # Harness project is configured — build and run it.
    project_path = _resolve(repo_root, harness_project_path_str)
    harness_root = runtime_root / "harness"
    harness_dll_path = harness_root / f"{project_path.stem}.dll"

    _run_checked(
        [
            "dotnet", "build", str(project_path),
            "-c", "Release",
            "-o", str(harness_root),
            *_dotnet_intermediate_args(project_path.stem, host_platform),
        ],
        repo_root=repo_root,
        failure_message=f"interpreter harness build failed: {harness_project_path_str}",
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
        payload = _parse_perf_payload(output_lines)
        if _is_numeric_perf_value(payload.get("elapsedMilliseconds")):
            duration_ms = round(float(payload["elapsedMilliseconds"]), 3)

        sample: dict[str, Any] = {
            "sampleIndex": sample_index + 1,
            "durationMs": duration_ms,
            "exitCode": last_exit_code,
            "mode": "interpreter",
        }
        sample.update(_payload_custom_perf_metrics(payload))
        samples.append(sample)
        stdout_chunks.append(f"=== sample {sample_index + 1} ({duration_ms:.3f} ms) ===\n{stdout_text}".rstrip() + "\n")
        if stderr_text:
            stderr_chunks.append(f"=== sample {sample_index + 1} ===\n{stderr_text}".rstrip() + "\n")
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
    result_manifest = {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "runtime",
        "variant": variant,
        "mode": "interpreter",
        "harnessProjectPath": harness_project_path_str,
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
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), result_manifest)

    if last_exit_code != 0:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": [result_manifest["stdoutPath"]],
            "metrics": {"durationMs": int(round(sum(float(s["durationMs"]) for s in samples)))},
            "diagnostics": {"stdoutPath": result_manifest["stdoutPath"], "stderrPath": result_manifest["stderrPath"]},
            "details": {"performance": performance},
            "failure": f"interpreter perf execution failed: {harness_dll_path}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[result_manifest["stdoutPath"]],
        stdout_path=result_manifest["stdoutPath"],
        stderr_path=result_manifest["stderrPath"],
        duration_ms=int(round(sum(float(s["durationMs"]) for s in samples))),
        details={"performance": performance},
    )


def run_benchmark_comparison_aggregate(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    """Aggregate perf results from managed, native and interpreter stages into a comparison report.

    Upstream buckets expected (any subset is acceptable — missing modes produce null entries):
      - "managed-runtime"   or  "runtime-perf"
      - "native-runtime"    or  "native-perf"
      - "interpreter-runtime"
    """
    import importlib
    benchmark_comparison = importlib.import_module("benchmark_comparison")

    selection = dict(request["selection"])
    subject_id = str(selection["subjectId"])
    matrix_id = str(selection["matrixId"])
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    def _load_mode_metrics(bucket_key: str) -> dict[str, Any] | None:
        upstream = dict(request.get("upstream") or {})
        entry = upstream.get(bucket_key)
        if not entry:
            return None
        try:
            m = read_json(_resolve(repo_root, entry["manifestPath"]))
            return dict(m.get("summaryMetrics") or {}) if isinstance(m, dict) else None
        except Exception:
            return None

    # Try common upstream bucket names for each mode.
    managed_metrics = _load_mode_metrics("managed-runtime") or _load_mode_metrics("runtime-perf")
    native_metrics = _load_mode_metrics("native-runtime") or _load_mode_metrics("native-perf")
    interpreter_metrics = _load_mode_metrics("interpreter-runtime")

    comparison = benchmark_comparison.compute_comparison(managed_metrics, native_metrics, interpreter_metrics)
    verdict = benchmark_comparison.evaluate_targets(comparison)

    report = {
        "reportVersion": "v1",
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "modes": {
            "managed": managed_metrics,
            "native": native_metrics,
            "interpreter": interpreter_metrics,
        },
        "comparison": comparison,
        "verdict": verdict,
    }

    comparison_path = runtime_root / "comparison.json"
    write_json(comparison_path, report)

    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "report",
        "comparisonPath": _relative(repo_root, comparison_path),
        "overallPass": bool(verdict.get("overallPass")),
    })

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[_relative(repo_root, comparison_path)],
        details={"comparison": comparison, "verdict": verdict},
    )


DEFAULT_STAGE_WORKERS = {
    "source-resolve": run_source_resolve,
    "host-input-build": run_dotnet_host_input_builder,
    "analysis-frontend": run_frontend_pipeline_worker,
    "generated-native-proof": run_native_proof_emitter,
    "generated-engine-proof": run_native_proof_emitter,
    "build-target": run_build_target,
    "runtime-observe": run_runtime_observe,
    "runtime-engine-observe": run_runtime_engine_observe,
    "runtime-engine-trace-compare": run_runtime_engine_trace_compare,
    "runtime-managed-output": run_managed_runtime_output,
    "runtime-perf-collect": run_runtime_perf_collect,
    "native-runtime-perf": run_native_runtime_perf,
    "mobile-native-perf": run_mobile_native_perf,
    "runtime-trace-compare": run_runtime_trace_compare,
    "interpreter-runtime-perf": run_interpreter_runtime_perf,
    "benchmark-comparison-aggregate": run_benchmark_comparison_aggregate,
}
