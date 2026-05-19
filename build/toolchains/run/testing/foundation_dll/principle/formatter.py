"""JSON/Markdown output formatter for principle check reports."""

from __future__ import annotations

import json
from datetime import datetime
from pathlib import Path
from typing import Any


def format_family_report(
    results: list[dict],
    assembly: str,
    family_slug: str,
    overall: str,
    cross_family: dict[str, list[dict]] | None = None,
) -> dict[str, Any]:
    """Build the full family report dict, producing a schema compatible with
    the existing principle-alignment-report.json format.

    Returns:
        {
            "schemaVersion": 2,
            "assembly": "...",
            "family": "...",
            "generatedAt": "...",
            "overall": "ALIGNED",
            "checks": { "check_id": { ... }, ... },
            "crossFamily": { ... }   # optional
        }
    """
    checks_dict: dict[str, dict] = {}
    for r in results:
        checks_dict[r["check_id"]] = r

    report: dict[str, Any] = {
        "schemaVersion": 2,
        "assembly": assembly,
        "family": family_slug,
        "generatedAt": datetime.now().isoformat(),
        "overall": overall,
        "checks": checks_dict,
    }

    if cross_family:
        report["crossFamily"] = cross_family

    return report


def write_report(report: dict[str, Any], path: Path) -> None:
    """Write report to path as JSON."""
    path.write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")
