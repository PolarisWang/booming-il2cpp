from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def _read_json(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def _family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) >= 4:
        return "-".join(parts[2:])
    return str(family_id).replace("/", "-")


def load_family_case_indexes(repo_root: Path, *, assembly_name: str, family_id: str) -> dict[str, list[dict[str, Any]]]:
    family_root = repo_root / "verification" / "foundation-dll" / assembly_name / _family_slug(family_id)
    test_payload = _read_json(family_root / "method-test-case-index.json") or {}
    benchmark_payload = _read_json(family_root / "method-benchmark-case-index.json") or {}
    hotupdate_payload = _read_json(family_root / "method-hotupdate-case-index.json") or {}
    return {
        "testCases": list(test_payload.get("cases") or []),
        "benchmarkCases": list(benchmark_payload.get("cases") or []),
        "hotupdateCases": list(hotupdate_payload.get("cases") or []),
    }


def load_family_review_bundle(repo_root: Path, *, assembly_name: str, family_id: str) -> dict[str, Any]:
    family_root = repo_root / "verification" / "foundation-dll" / assembly_name / _family_slug(family_id)
    return _read_json(family_root / "review" / "bundle.json") or {}
