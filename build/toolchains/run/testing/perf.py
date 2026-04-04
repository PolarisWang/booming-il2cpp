from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def _baseline_path(repo_root: Path, suite: str, host_platform: str) -> Path:
    return repo_root / "tests" / "perf" / suite / "baselines" / f"{host_platform}.json"


def evaluate_perf_suite(
    *,
    repo_root: Path,
    suite: str,
    host_platform: str,
    metrics: dict[str, Any],
    update_baseline: bool = False,
) -> dict[str, Any]:
    baseline_path = _baseline_path(repo_root, suite, host_platform)
    baseline_path.parent.mkdir(parents=True, exist_ok=True)

    baseline_updated = False
    if baseline_path.is_file():
        baseline = json.loads(baseline_path.read_text(encoding="utf-8"))
    else:
        baseline = {}

    if update_baseline:
        baseline_path.write_text(json.dumps(metrics, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        baseline = dict(metrics)
        baseline_updated = True

    return {
        "suite": suite,
        "hostPlatform": host_platform,
        "baselinePath": str(baseline_path.as_posix()),
        "baseline": baseline,
        "metrics": dict(metrics),
        "baselineUpdated": baseline_updated,
    }
