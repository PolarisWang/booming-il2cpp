from __future__ import annotations

import json
from pathlib import Path
from typing import Any
import sys

try:
    from ..common import combine_process_output, run_process
    from .. import tooling as tooling_module
    from . import subjects as subjects_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import combine_process_output, run_process
    import tooling as tooling_module
    from testing import subjects as subjects_module


def _relative(repo_root: Path, path: Path) -> str:
    return path.relative_to(repo_root).as_posix()


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


def _aggregate_status(results: list[dict[str, Any]]) -> str:
    if not results:
        return "ok"

    statuses = [str(result.get("status") or "aborted") for result in results]
    if any(status == "fail" for status in statuses):
        return "fail"
    if any(status == "aborted" for status in statuses):
        return "aborted"
    if all(status == "skip" for status in statuses):
        return "skip"
    return "ok"


def _validation_root(repo_root: Path, subject_id: str, matrix_id: str, run_id: str, validation_kind: str) -> Path:
    return repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "matrices" / matrix_id / "validations" / validation_kind


def _write_json_document(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def _run_xunit_validation(
    repo_root: Path,
    *,
    subject_id: str,
    matrix_id: str,
    run_id: str,
    host_platform: str,
    variant: str,
    validation_spec: dict[str, Any],
) -> dict[str, Any]:
    project_path_text = str(validation_spec.get("project") or "").strip()
    if not project_path_text:
        raise RuntimeError(f"unit validation project is required: {subject_id}")

    project_path = repo_root / project_path_text
    if not project_path.is_file():
        raise FileNotFoundError(f"unit validation project missing: {project_path}")

    validation_root = _validation_root(repo_root, subject_id, matrix_id, run_id, "unit")
    validation_root.mkdir(parents=True, exist_ok=True)

    stdout_path = validation_root / "stdout.log"
    stderr_path = validation_root / "stderr.log"
    trx_path = validation_root / "results.trx"
    summary_path = validation_root / "summary.json"

    arguments = [
        "dotnet",
        "test",
        str(project_path),
        "-c",
        "Release",
        "--logger",
        f"trx;LogFileName={trx_path.name}",
        "--results-directory",
        str(validation_root),
        *_dotnet_intermediate_args(project_path.stem, host_platform),
    ]
    completed = run_process(arguments, cwd=repo_root)
    stdout_text = completed.stdout or ""
    stderr_text = completed.stderr or ""
    stdout_path.write_text(stdout_text, encoding="utf-8")
    stderr_path.write_text(stderr_text, encoding="utf-8")

    status = "ok" if completed.returncode == 0 else "fail"
    summary = {
        "summaryVersion": "v1",
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "validationKind": "unit",
        "framework": str(validation_spec.get("framework") or "xunit"),
        "variant": variant,
        "projectPath": project_path_text,
        "status": status,
        "command": arguments,
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "trxPath": _relative(repo_root, trx_path),
    }
    _write_json_document(summary_path, summary)

    error_text = combine_process_output(completed).strip()
    return {
        "validationKind": "unit",
        "framework": str(validation_spec.get("framework") or "xunit"),
        "matrixId": matrix_id,
        "status": status,
        "projectPath": project_path_text,
        "summaryPath": _relative(repo_root, summary_path),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "trxPath": _relative(repo_root, trx_path),
        "artifacts": [
            _relative(repo_root, summary_path),
            _relative(repo_root, stdout_path),
            _relative(repo_root, stderr_path),
            _relative(repo_root, trx_path),
        ],
        "errors": [] if status == "ok" else [error_text or f"xunit validation failed: {project_path_text}"],
    }


def run_subject_validations(repo_root: Path, plan: dict[str, Any], *, run_id: str) -> dict[str, Any]:
    selection = dict(plan.get("selection") or {})
    subject_id = str(selection.get("subjectId") or "")
    matrix_id = str(selection.get("matrixId") or "")
    host_platform = str(dict(selection.get("executionContext") or {}).get("hostPlatform") or "")
    variant = str(selection.get("variant") or "")
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)

    validation_results: list[dict[str, Any]] = []
    artifacts: list[str] = []
    errors: list[str] = []

    for validation_kind in list(selection.get("validationKinds") or []):
        if validation_kind == "proof":
            continue

        validation_spec = subjects_module.find_validation(manifest, str(validation_kind))
        framework = str(validation_spec.get("framework") or "")
        driver = str(validation_spec.get("driver") or "")

        if framework == "xunit":
            result = _run_xunit_validation(
                repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                run_id=run_id,
                host_platform=host_platform,
                variant=variant,
                validation_spec=validation_spec,
            )
            validation_results.append(result)
            artifacts.extend(list(result.get("artifacts") or []))
            errors.extend(list(result.get("errors") or []))
            continue

        if driver == "csharp-perf-harness":
            continue

    return {
        "status": _aggregate_status(validation_results),
        "validationResults": validation_results,
        "artifacts": artifacts,
        "errors": errors,
    }
