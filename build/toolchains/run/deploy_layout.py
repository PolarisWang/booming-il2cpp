from __future__ import annotations

import shutil
import sys
from pathlib import Path
from typing import Any

try:
    from .common import read_json, write_json
    from . import project_workspace as workspace_module
except ImportError:
    root = Path(__file__).resolve().parent
    sys.path.insert(0, str(root))
    from common import read_json, write_json
    import project_workspace as workspace_module


def _path_text(repo_root: Path, path: Path) -> str:
    try:
        return path.relative_to(repo_root).as_posix()
    except ValueError:
        return path.as_posix()


def _clear_dir(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path, ignore_errors=True)


def _copy_tree(source_root: Path, destination_root: Path) -> None:
    _clear_dir(destination_root)
    shutil.copytree(source_root, destination_root, dirs_exist_ok=True)


def _copy_driver_output(driver_output_root: Path, destination_root: Path) -> str:
    if not driver_output_root.is_dir():
        raise RuntimeError(f"driver output is missing: {driver_output_root}")

    destination_root.mkdir(parents=True, exist_ok=True)
    entry_executable = ""
    for candidate in sorted(driver_output_root.iterdir()):
        if not candidate.is_file():
            continue
        if candidate.name == "Chaos.IL2CPP.Driver.exe":
            target_path = destination_root / "chaos-il2cpp.exe"
            shutil.copy2(candidate, target_path)
            entry_executable = target_path.name
            continue
        if candidate.name == "Chaos.IL2CPP.Driver":
            target_path = destination_root / "chaos-il2cpp"
            shutil.copy2(candidate, target_path)
            entry_executable = target_path.name
            continue
        shutil.copy2(candidate, destination_root / candidate.name)

    if not entry_executable:
        raise RuntimeError(f"driver executable is missing from: {driver_output_root}")
    return entry_executable


def materialize_core_deploy(
    *,
    repo_root: Path,
    host_platform: str,
    workspace_manifest_path: str,
    build_report_path: str,
    target_ids: list[str],
) -> dict[str, Any]:
    manifest = read_json(repo_root / workspace_manifest_path)
    if not isinstance(manifest, dict):
        raise RuntimeError("core workspace manifest must be an object")

    native_targets = {
        str(item.get("targetId") or ""): dict(item)
        for item in list(manifest.get("nativeTargets") or [])
        if isinstance(item, dict)
    }
    if not target_ids:
        target_ids = list(native_targets)
    driver_output_root = repo_root / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0"

    artifacts: list[str] = []
    important_outputs: list[dict[str, str]] = []
    for target_id in target_ids:
        if target_id not in native_targets:
            raise RuntimeError(f"core deploy target is missing from workspace: {target_id}")

        target = native_targets[target_id]
        configure_root = repo_root / str(target["configureRoot"])
        if not configure_root.is_dir():
            raise RuntimeError(f"configured native target is missing: {configure_root}")

        deploy_root = repo_root / "deploy" / "core" / host_platform / target_id
        _clear_dir(deploy_root)
        deploy_root.mkdir(parents=True, exist_ok=True)

        package_root = deploy_root / "package-root"
        _copy_tree(configure_root, package_root)

        entry_executable_path: str | None = None
        if str(target.get("deliveryKind") or "") == "direct-run-host":
            bin_root = deploy_root / "bin"
            entry_name = _copy_driver_output(driver_output_root, bin_root)
            entry_executable_path = _path_text(repo_root, bin_root / entry_name)
            artifacts.append(entry_executable_path)
            important_outputs.append({"label": f"{target_id} entry", "path": entry_executable_path})

        deploy_manifest_path = deploy_root / "deploy.manifest.json"
        write_json(
            deploy_manifest_path,
            {
                "hostPlatform": host_platform,
                "targetId": target_id,
                "deliveryKind": str(target.get("deliveryKind") or ""),
                "entryExecutable": entry_executable_path,
                "projectExportSupported": True,
                "workspaceManifestPath": workspace_manifest_path,
                "buildReportPath": build_report_path,
            },
        )
        artifacts.extend(
            [
                _path_text(repo_root, deploy_manifest_path),
                _path_text(repo_root, package_root),
            ]
        )
        important_outputs.append({"label": f"{target_id} deploy manifest", "path": _path_text(repo_root, deploy_manifest_path)})

    return {
        "artifacts": artifacts,
        "importantOutputs": important_outputs,
        "consoleText": "",
    }


def deploy_core_outputs(repo_root: Path, host_platform: str, options: dict[str, object]) -> dict[str, Any]:
    generation = workspace_module.generate_core_workspace(repo_root, host_platform, options)
    build = workspace_module.build_core_workspace(repo_root, host_platform, options)
    workspace_manifest_path = str(generation["manifestPath"])
    build_report_path = str(build["buildReportPath"])
    requested_target = str(dict(options or {}).get("target") or "").strip()
    target_ids = [requested_target] if requested_target else []
    materialized = materialize_core_deploy(
        repo_root=repo_root,
        host_platform=host_platform,
        workspace_manifest_path=workspace_manifest_path,
        build_report_path=build_report_path,
        target_ids=target_ids,
    )
    artifacts = list(generation.get("artifacts") or []) + list(build.get("artifacts") or []) + list(materialized.get("artifacts") or [])
    important_outputs = list(generation.get("importantOutputs") or []) + list(build.get("importantOutputs") or []) + list(materialized.get("importantOutputs") or [])
    console_parts = [
        str(generation.get("consoleText") or "").strip(),
        str(build.get("consoleText") or "").strip(),
        str(materialized.get("consoleText") or "").strip(),
    ]
    return {
        "artifacts": artifacts,
        "importantOutputs": important_outputs,
        "consoleText": "\n".join(part for part in console_parts if part),
    }
