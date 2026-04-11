from __future__ import annotations

import json
from pathlib import Path
from typing import Any
import sys

try:
    from . import path_resolver as path_resolver_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import path_resolver as path_resolver_module


def _baseline_path(repo_root: Path, suite: str, host_platform: str) -> Path:
    return repo_root / "tests" / "perf" / suite / "baselines" / f"{host_platform}.json"


def _subject_baseline_path(repo_root: Path, subject_id: str, matrix_id: str, host_platform: str) -> Path:
    return path_resolver_module.subject_perf_baseline_path(
        repo_root,
        subject_id,
        matrix_id,
        host_platform,
    )


def _load_baseline(baseline_path: Path) -> dict[str, Any]:
    if baseline_path.is_file():
        return json.loads(baseline_path.read_text(encoding="utf-8"))
    return {}


def _write_baseline(baseline_path: Path, metrics: dict[str, Any]) -> None:
    baseline_path.parent.mkdir(parents=True, exist_ok=True)
    baseline_path.write_text(json.dumps(metrics, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def _compare_metrics(metrics: dict[str, Any], baseline: dict[str, Any]) -> list[dict[str, Any]]:
    regressions: list[dict[str, Any]] = []
    baseline_keys = sorted(str(metric_name) for metric_name in baseline)
    for metric_name in baseline_keys:
        expected = baseline[metric_name]
        if not isinstance(expected, (int, float)):
            continue

        if metric_name not in metrics:
            regressions.append(
                {
                    "metric": metric_name,
                    "baseline": float(expected),
                    "actual": None,
                    "delta": None,
                    "reason": "missing-actual-metric",
                }
            )
            continue

        actual = metrics[metric_name]
        if not isinstance(actual, (int, float)) or not isinstance(expected, (int, float)):
            continue
        if float(actual) <= float(expected):
            continue
        regressions.append(
            {
                "metric": metric_name,
                "baseline": float(expected),
                "actual": float(actual),
                "delta": round(float(actual) - float(expected), 3),
            }
        )
    return regressions


def _evaluate_baseline(
    *,
    baseline_path: Path,
    metrics: dict[str, Any],
    update_baseline: bool,
) -> dict[str, Any]:
    baseline_path.parent.mkdir(parents=True, exist_ok=True)
    baseline_updated = False
    baseline = _load_baseline(baseline_path)

    if update_baseline:
        _write_baseline(baseline_path, metrics)
        baseline = dict(metrics)
        baseline_updated = True

    regressions = _compare_metrics(metrics, baseline)
    if baseline_updated:
        regression_status = "baseline-updated"
    elif not baseline:
        regression_status = "no-baseline"
    elif regressions:
        regression_status = "regressed"
    else:
        regression_status = "ok"

    return {
        "baselinePath": str(baseline_path.as_posix()),
        "baseline": baseline,
        "metrics": dict(metrics),
        "baselineUpdated": baseline_updated,
        "regressionStatus": regression_status,
        "regressions": regressions,
    }


def evaluate_perf_suite(
    *,
    repo_root: Path,
    suite: str,
    host_platform: str,
    metrics: dict[str, Any],
    update_baseline: bool = False,
) -> dict[str, Any]:
    result = _evaluate_baseline(
        baseline_path=_baseline_path(repo_root, suite, host_platform),
        metrics=metrics,
        update_baseline=update_baseline,
    )
    result["suite"] = suite
    result["hostPlatform"] = host_platform
    return result


def evaluate_perf_subject(
    *,
    repo_root: Path,
    subject_id: str,
    matrix_id: str,
    host_platform: str,
    metrics: dict[str, Any],
    update_baseline: bool = False,
) -> dict[str, Any]:
    result = _evaluate_baseline(
        baseline_path=_subject_baseline_path(repo_root, subject_id, matrix_id, host_platform),
        metrics=metrics,
        update_baseline=update_baseline,
    )
    result["subjectId"] = subject_id
    result["matrixId"] = matrix_id
    result["hostPlatform"] = host_platform
    return result
