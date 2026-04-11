from __future__ import annotations

import re
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
import sys

try:
    from ..core.common import write_json
    from . import subjects as subjects_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import write_json
    from testing import subjects as subjects_module


RULES: list[dict[str, Any]] = [
    {
        "id": "appdomain",
        "title": "AppDomain API",
        "severity": "error",
        "pattern": re.compile(r"\bAppDomain\b"),
        "summary": "AppDomain lifetime and isolation APIs are outside the supported IL2CPP/mobile host surface.",
    },
    {
        "id": "reflection-emit",
        "title": "Reflection.Emit",
        "severity": "error",
        "pattern": re.compile(r"\bSystem\.Reflection\.Emit\b|\bDynamicMethod\b|\bAssemblyBuilder\b"),
        "summary": "Runtime code emission depends on JIT-style capabilities and should be reported as unsupported.",
    },
]


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def _relative(base: Path, path: Path) -> str:
    return path.resolve().relative_to(base.resolve()).as_posix()


def _status_counts(findings: list[dict[str, Any]]) -> dict[str, int]:
    return {
        "total": len(findings),
        "ok": 0,
        "fail": len(findings),
    }


def build_unsupported_feature_report_for_source_tree(
    source_root: Path,
    *,
    subject_id: str,
) -> dict[str, Any]:
    findings: list[dict[str, Any]] = []
    if source_root.is_dir():
        for source_path in sorted(source_root.rglob("*.cs")):
            lines = source_path.read_text(encoding="utf-8").splitlines()
            matched_rule_ids: set[str] = set()
            for line_number, line in enumerate(lines, start=1):
                for rule in RULES:
                    rule_id = str(rule["id"])
                    if rule_id in matched_rule_ids or not rule["pattern"].search(line):
                        continue
                    matched_rule_ids.add(rule_id)
                    findings.append(
                        {
                            "ruleId": rule_id,
                            "title": str(rule["title"]),
                            "severity": str(rule["severity"]),
                            "summary": str(rule["summary"]),
                            "filePath": _relative(source_root, source_path),
                            "line": line_number,
                            "excerpt": line.strip(),
                        }
                    )

    counts = _status_counts(findings)
    return {
        "reportVersion": "v1",
        "generatedAt": _utc_timestamp(),
        "subjectId": subject_id,
        "sourceRoot": source_root.as_posix(),
        "status": "fail" if findings else "ok",
        "statusCounts": counts,
        "ruleCount": len(RULES),
        "rules": [
            {
                "ruleId": str(rule["id"]),
                "title": str(rule["title"]),
                "severity": str(rule["severity"]),
                "summary": str(rule["summary"]),
            }
            for rule in RULES
        ],
        "findings": findings,
    }


def build_repo_unsupported_feature_report(repo_root: Path) -> dict[str, Any]:
    subject_results: list[dict[str, Any]] = []
    all_findings: list[dict[str, Any]] = []

    for manifest_path in subjects_module.discover_subject_manifests(repo_root):
        subject_id = manifest_path.parent.name
        source_root = repo_root / "subjects" / subject_id / "source"
        subject_report = build_unsupported_feature_report_for_source_tree(
            source_root,
            subject_id=subject_id,
        )
        subject_results.append(subject_report)
        for finding in list(subject_report.get("findings") or []):
            finding_with_subject = dict(finding)
            finding_with_subject["subjectId"] = subject_id
            all_findings.append(finding_with_subject)

    counts = _status_counts(all_findings)
    return {
        "reportVersion": "v1",
        "generatedAt": _utc_timestamp(),
        "status": "fail" if all_findings else "ok",
        "statusCounts": counts,
        "ruleCount": len(RULES),
        "rules": [
            {
                "ruleId": str(rule["id"]),
                "title": str(rule["title"]),
                "severity": str(rule["severity"]),
                "summary": str(rule["summary"]),
            }
            for rule in RULES
        ],
        "subjectResults": subject_results,
        "findings": all_findings,
    }


def write_unsupported_feature_report(path: Path, payload: dict[str, Any]) -> None:
    write_json(path, payload)
