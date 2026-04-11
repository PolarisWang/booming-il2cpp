from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from uuid import uuid4
import sys

try:
    from ..core.common import combine_process_output, read_json, run_process, write_json
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import combine_process_output, read_json, run_process, write_json


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def _relative(repo_root: Path, path: Path) -> str:
    return path.resolve().relative_to(repo_root.resolve()).as_posix()


def _load_config(config_path: Path) -> dict[str, Any]:
    payload = read_json(config_path)
    if not isinstance(payload, dict):
        raise RuntimeError(f"compatibility matrix config must be an object: {config_path}")

    required_keys = [
        "schemaVersion",
        "subjectId",
        "projectPath",
        "aotVersions",
        "hotUpdateVersions",
        "platforms",
    ]
    for required_key in required_keys:
        if required_key not in payload:
            raise RuntimeError(f"compatibility matrix config missing '{required_key}': {config_path}")

    if str(payload.get("schemaVersion") or "") != "v1":
        raise RuntimeError(f"unsupported compatibility matrix schemaVersion: {payload.get('schemaVersion')}")

    return payload


def run_compatibility_matrix(
    repo_root: Path,
    config_path: Path,
    *,
    run_id: str | None = None,
    artifact_root: Path | None = None,
) -> dict[str, Any]:
    resolved_repo_root = repo_root.resolve()
    resolved_config_path = config_path if config_path.is_absolute() else resolved_repo_root / config_path
    config = _load_config(resolved_config_path)
    resolved_run_id = run_id or f"compatibility-matrix-{uuid4().hex[:8]}"
    resolved_artifact_root = artifact_root if artifact_root is not None else Path("artifacts") / "compatibility-matrix"
    if not resolved_artifact_root.is_absolute():
        resolved_artifact_root = resolved_repo_root / resolved_artifact_root

    run_root = resolved_artifact_root / resolved_run_id
    build_root = run_root / "proof-build"
    stdout_path = run_root / "stdout.log"
    stderr_path = run_root / "stderr.log"
    report_path = run_root / "report.json"

    project_path = resolved_repo_root / str(config["projectPath"])
    build_completed = run_process(
        [
            "dotnet",
            "build",
            str(project_path),
            "-c",
            "Release",
            "-o",
            str(build_root),
        ],
        cwd=resolved_repo_root,
    )
    if build_completed.returncode != 0:
        raise RuntimeError(
            "compatibility matrix proof build failed:\n"
            f"{combine_process_output(build_completed)}"
        )

    proof_dll_path = build_root / f"{project_path.stem}.dll"
    completed = run_process(
        [
            "dotnet",
            str(proof_dll_path),
            "--config",
            str(resolved_config_path),
        ],
        cwd=resolved_repo_root,
    )
    stdout_path.parent.mkdir(parents=True, exist_ok=True)
    stdout_path.write_text(completed.stdout or "", encoding="utf-8")
    stderr_path.write_text(completed.stderr or "", encoding="utf-8")

    stdout_lines = [line for line in (completed.stdout or "").splitlines() if line.strip()]
    if not stdout_lines:
        raise RuntimeError("compatibility matrix proof produced no JSON output")

    try:
        report = json.loads(stdout_lines[-1])
    except ValueError as error:
        raise RuntimeError("compatibility matrix proof output is not valid JSON") from error

    if not isinstance(report, dict):
        raise RuntimeError("compatibility matrix proof report must be an object")

    report["runId"] = resolved_run_id
    report["generatedAt"] = _utc_timestamp()
    report["configPath"] = _relative(resolved_repo_root, resolved_config_path)
    report["projectPath"] = _relative(resolved_repo_root, project_path)
    report["proofDllPath"] = _relative(resolved_repo_root, proof_dll_path)
    report["stdoutPath"] = _relative(resolved_repo_root, stdout_path)
    report["stderrPath"] = _relative(resolved_repo_root, stderr_path)
    report["runnerVersion"] = "v1"
    write_json(report_path, report)

    return {
        "status": str(report.get("status") or "aborted"),
        "runId": resolved_run_id,
        "reportPath": _relative(resolved_repo_root, report_path),
        "stdoutPath": _relative(resolved_repo_root, stdout_path),
        "stderrPath": _relative(resolved_repo_root, stderr_path),
    }
