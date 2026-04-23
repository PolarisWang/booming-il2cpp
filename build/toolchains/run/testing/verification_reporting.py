from __future__ import annotations

from pathlib import Path
from typing import Any


def _relative(repo_root: Path, path: Path) -> str:
    try:
        return path.resolve().relative_to(repo_root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def _string_list(values: Any) -> list[str]:
    if values in (None, ""):
        return []
    if isinstance(values, (list, tuple)):
        return [str(value) for value in values if str(value)]
    return [str(values)]


def _section(title: str, values: list[str]) -> str:
    lines = [f"## {title}"]
    if not values:
        lines.append("- n/a")
        return "\n".join(lines)
    lines.extend(f"- {value}" for value in values)
    return "\n".join(lines)


def build_stage_report_markdown(stage_report: dict[str, Any]) -> str:
    stage_kind = str(stage_report.get("stageKind") or stage_report.get("stageId") or "")
    blockers = [
        "{code} [{lane}] {message}".format(
            code=str(item.get("code") or ""),
            lane=str(item.get("lane") or ""),
            message=str(item.get("message") or ""),
        ).strip()
        for item in list(stage_report.get("blockers") or [])
    ]
    parts = [
        f"# Verification Stage Report: {stage_kind}",
        "",
        f"- stageId: {stage_report.get('stageId') or ''}",
        f"- stageKind: {stage_kind}",
        f"- closureId: {stage_report.get('closureId') or ''}",
        f"- scopeCode: {stage_report.get('scopeCode') or ''}",
        f"- verificationState: {stage_report.get('verificationState') or ''}",
        f"- supportState: {stage_report.get('supportState') or ''}",
        "",
        _section("Covered Obligations", _string_list(stage_report.get("coveredObligationClaimIds"))),
        "",
        _section("Success Evidence", _string_list(stage_report.get("successEvidenceIds"))),
        "",
        _section("Failed Evidence", _string_list(stage_report.get("failedEvidenceIds"))),
        "",
        _section("Blocked Evidence", _string_list(stage_report.get("blockedEvidenceIds"))),
        "",
        _section("Missing Claims", _string_list(stage_report.get("missingEvidenceClaimIds"))),
        "",
        _section("Blockers", blockers),
    ]
    return "\n".join(parts).strip() + "\n"


def build_stage_summary_markdown(
    *,
    closure_kind: str,
    scope_code: str,
    stage_reports: list[dict[str, Any]],
) -> str:
    lines = [
        "# Verification Stage Summary",
        "",
        f"- closureKind: {closure_kind}",
        f"- scopeCode: {scope_code}",
        f"- stageCount: {len(list(stage_reports or []))}",
        "",
        "## Stages",
    ]
    for item in list(stage_reports or []):
        lines.append(
            "- {stage} :: verification={verification} support={support}".format(
                stage=str(item.get("stageKind") or item.get("stageId") or ""),
                verification=str(item.get("verificationState") or ""),
                support=str(item.get("supportState") or ""),
            )
        )
    return "\n".join(lines).strip() + "\n"


def write_stage_reports(
    *,
    repo_root: Path,
    closure_kind: str,
    scope_code: str,
    stage_reports: list[dict[str, Any]],
) -> dict[str, Any]:
    report_root = (
        repo_root
        / "docs"
        / "testing-inventory"
        / "verification"
        / "reports"
        / str(closure_kind or "")
        / str(scope_code or "")
    )
    report_root.mkdir(parents=True, exist_ok=True)

    summary_path = report_root / "summary.md"
    summary_path.write_text(
        build_stage_summary_markdown(
            closure_kind=str(closure_kind or ""),
            scope_code=str(scope_code or ""),
            stage_reports=stage_reports,
        ),
        encoding="utf-8",
    )

    stage_paths: dict[str, str] = {}
    for item in list(stage_reports or []):
        stage_kind = str(item.get("stageKind") or item.get("stageId") or "stage")
        stage_path = report_root / f"{stage_kind}.md"
        stage_path.write_text(build_stage_report_markdown(item), encoding="utf-8")
        stage_paths[stage_kind] = _relative(repo_root, stage_path)

    return {
        "summaryPath": _relative(repo_root, summary_path),
        "stagePaths": stage_paths,
    }
