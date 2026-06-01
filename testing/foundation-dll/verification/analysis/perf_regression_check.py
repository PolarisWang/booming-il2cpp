"""Performance regression check — compares benchmark data pre/post optimization.

Used in Step 5.3 of the optimization campaign to verify that optimization
did not degrade performance beyond the allowed threshold.

Compares:
  - Per-method ns/op between baseline and post-optimization
  - Flags methods exceeding max_degradation threshold (default 5%)

Usage:
    python -m verification.analysis.perf_regression_check <slug> --assembly System.Private.CoreLib
"""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_VERIFICATION_ROOT = _HERE.parent
_TESTING_ROOT = _VERIFICATION_ROOT.parent

# Default max allowed degradation ratio (5% = 1.05x slower)
_DEFAULT_MAX_DEGRADATION = 1.05


def _load_benchmark_from_pipeline_report(
    report_path: Path,
) -> list[dict[str, Any]]:
    """Extract per-method benchmark results from unified-verification-report.json."""
    try:
        data = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return []

    stages = data.get("stages", data)
    if not isinstance(stages, dict):
        return []

    # Try benchmark stage, then microbench stage
    for stage_name in ("benchmark", "microbench"):
        stage = stages.get(stage_name, {})
        if isinstance(stage, dict):
            details = stage.get("details", {})
            if isinstance(details, dict):
                # Check for per-method results
                results = details.get("results", details.get("native-aot", {}).get("results", []))
                if results:
                    return results
    return []


def _load_benchmark_from_jsonl(
    jsonl_path: Path,
) -> list[dict[str, Any]]:
    """Load the latest run from a benchmark-history.jsonl file."""
    try:
        lines = jsonl_path.read_text(encoding="utf-8").strip().splitlines()
    except OSError:
        return []

    if not lines:
        return []

    # Group by methodSubjectId, keep only latest per method
    latest_per_method: dict[str, dict[str, Any]] = {}
    for line in lines:
        try:
            entry = json.loads(line)
        except json.JSONDecodeError:
            continue

        mid = entry.get("methodSubjectId", entry.get("name", ""))
        ts = entry.get("timestamp", "")
        if mid not in latest_per_method or ts > latest_per_method[mid].get("timestamp", ""):
            latest_per_method[mid] = entry

    return list(latest_per_method.values())


def _load_benchmark_from_multirun(
    report_path: Path,
) -> list[dict[str, Any]]:
    """Extract per-method benchmark results from multi-run-report.json.

    The multi-run report has the format:
      { "methods": [{ "subject_id": "...", "samples": { "chaos-aot": { "mean_ns": ... }}}] }
    """
    try:
        data = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return []

    methods = data.get("methods", [])
    if not methods:
        return []

    entries: list[dict[str, Any]] = []
    for m in methods:
        subject_id = m.get("subject_id", "")
        if not subject_id:
            continue

        # Prefer chaos-aot sample, fall back to first available
        samples = m.get("samples", {})
        chaos = samples.get("chaos-aot")
        if chaos:
            mean_ns = chaos.get("mean_ns", 0.0)
            status = chaos.get("status", "ok")
            entries.append({
                "methodSubjectId": subject_id,
                "method_index": m.get("method_index"),
                "label": m.get("label"),
                "mean_ns": mean_ns,
                "status": status,
            })
        elif samples:
            # Fall back to first available runtime
            first_runtime = next(iter(samples.values()))
            if isinstance(first_runtime, dict):
                entries.append({
                    "methodSubjectId": subject_id,
                    "method_index": m.get("method_index"),
                    "label": m.get("label"),
                    "mean_ns": first_runtime.get("mean_ns", 0.0),
                    "status": first_runtime.get("status", "ok"),
                })

    return entries


def _ns_per_op(entry: dict[str, Any]) -> float:
    """Extract ns/op from a benchmark entry.

    Handles various metric formats:
      - ns_per_op, mean_ns, average_ns (direct)
      - elapsedMilliseconds + iterations (compute)
      - opsPerSecond (compute)
    """
    metrics = entry.get("metrics", entry)
    if not isinstance(metrics, dict):
        return 0.0

    # Direct ns/op fields
    for key in ("ns_per_op", "mean_ns", "average_ns", "ns/op"):
        val = metrics.get(key, 0)
        if isinstance(val, (int, float)) and val > 0:
            return float(val)

    # Compute from elapsed + iterations
    elapsed = metrics.get("elapsedMilliseconds", 0)
    iterations = entry.get("iterations", 0)
    if isinstance(elapsed, (int, float)) and isinstance(iterations, (int, float)):
        if elapsed > 0 and iterations > 0:
            return (elapsed * 1_000_000) / iterations

    # Compute from ops/sec
    ops = metrics.get("opsPerSecond", 0)
    if isinstance(ops, (int, float)) and ops > 0:
        return 1_000_000_000.0 / ops

    return 0.0


def compare_benchmark(
    baseline_entries: list[dict[str, Any]],
    current_entries: list[dict[str, Any]],
    max_degradation: float = _DEFAULT_MAX_DEGRADATION,
) -> dict[str, Any]:
    """Compare per-method benchmark ns/op between baseline and current.

    Returns structured report with regressions, improvements, and unchanged methods.
    """
    # Index by methodSubjectId
    baseline_by_name: dict[str, dict[str, Any]] = {}
    for e in baseline_entries:
        mid = e.get("methodSubjectId", e.get("name", ""))
        if mid:
            baseline_by_name[mid] = e

    current_by_name: dict[str, dict[str, Any]] = {}
    for e in current_entries:
        mid = e.get("methodSubjectId", e.get("name", ""))
        if mid:
            current_by_name[mid] = e

    all_methods = sorted(set(baseline_by_name.keys()) | set(current_by_name.keys()))
    regressions: list[dict[str, Any]] = []
    improvements: list[dict[str, Any]] = []
    unchanged: list[dict[str, Any]] = []
    insufficient_data: list[str] = []

    for mid in all_methods:
        b_entry = baseline_by_name.get(mid)
        c_entry = current_by_name.get(mid)

        b_ns = _ns_per_op(b_entry) if b_entry else 0.0
        c_ns = _ns_per_op(c_entry) if c_entry else 0.0

        if b_ns <= 0 or c_ns <= 0:
            insufficient_data.append(mid)
            continue

        ratio = c_ns / b_ns if b_ns > 0 else float("inf")

        entry = {
            "methodSubjectId": mid,
            "baseline_ns": round(b_ns, 2),
            "current_ns": round(c_ns, 2),
            "ratio": round(ratio, 4),
        }

        if ratio > max_degradation:
            entry["degradation_pct"] = round((ratio - 1.0) * 100, 2)
            regressions.append(entry)
        elif ratio < (1.0 / max_degradation):
            entry["speedup_pct"] = round((1.0 - ratio) * 100, 2)
            improvements.append(entry)
        else:
            unchanged.append(entry)

    has_regression = len(regressions) > 0

    return {
        "has_regression": has_regression,
        "max_degradation": max_degradation,
        "total_methods": len(all_methods),
        "regression_count": len(regressions),
        "improvement_count": len(improvements),
        "unchanged_count": len(unchanged),
        "insufficient_data_count": len(insufficient_data),
        "regressions": regressions,
        "improvements": improvements,
        "unchanged": unchanged,
        "insufficient_data": insufficient_data,
        "summary": (
            f"{len(regressions)} regression(s), {len(improvements)} improvement(s), "
            f"{len(unchanged)} unchanged, {len(insufficient_data)} insufficient data"
            f" out of {len(all_methods)} methods"
        ),
    }


def check_perf_regression(
    slug: str,
    assembly: str,
    max_degradation: float = _DEFAULT_MAX_DEGRADATION,
) -> dict[str, Any]:
    """Entry point: load baseline and current benchmark data, compare."""
    family_dir = _TESTING_ROOT / assembly / slug
    results_dir = _TESTING_ROOT / "results" / assembly / slug

    # ── Load baseline benchmark history ──────────────────────────────────
    jsonl_candidates = [
        results_dir / "perf" / "benchmark-history.jsonl",
        family_dir / "perf" / "benchmark-history.jsonl",
    ]
    baseline_jsonl = None
    for p in jsonl_candidates:
        if p.exists():
            baseline_jsonl = p
            break

    if not baseline_jsonl:
        return {
            "has_regression": False,
            "no_baseline": True,
            "error": f"No benchmark history found in: {[str(p) for p in jsonl_candidates]}",
            "details": [],
        }
    baseline_entries = _load_benchmark_from_jsonl(baseline_jsonl)

    # ── Load current multi-run report ────────────────────────────────────
    current_candidates = [
        family_dir / "multi-run" / "multi-run-report.json",
        family_dir / "unified-verification-report.json",
        results_dir / "unified-verification-report.json",
    ]
    current_entries = []
    for rp in current_candidates:
        if rp.exists():
            if rp.name == "multi-run-report.json":
                current_entries = _load_benchmark_from_multirun(rp)
            else:
                current_entries = _load_benchmark_from_pipeline_report(rp)
            break

    if not current_entries:
        return {
            "has_regression": True,
            "error": "No current benchmark data found",
            "details": [],
        }

    return compare_benchmark(baseline_entries, current_entries, max_degradation)


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(
        description="Performance regression check for benchmarks"
    )
    parser.add_argument("slug", help="Family slug")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument(
        "--max-degradation", type=float, default=_DEFAULT_MAX_DEGRADATION,
        help=f"Max allowed degradation ratio (default: {_DEFAULT_MAX_DEGRADATION})",
    )
    args = parser.parse_args()

    report = check_perf_regression(args.slug, args.assembly, args.max_degradation)

    # NO_BASELINE handling: print status and exit 0
    if report.get("no_baseline"):
        print(json.dumps({
            "status": "NO_BASELINE",
            "message": report.get("error", "No benchmark history found."),
            "family": args.slug,
            "assembly": args.assembly,
        }, indent=2, ensure_ascii=False))
        sys.exit(0)

    # Write report to family_dir/regression-check-report.json
    family_dir = _TESTING_ROOT / args.assembly / args.slug
    if family_dir.is_dir():
        report_path = family_dir / "regression-check-report.json"
        try:
            report_path.write_text(
                json.dumps(report, indent=2, ensure_ascii=False),
                encoding="utf-8",
            )
        except OSError as exc:
            print(f"Warning: failed to write report to {report_path}: {exc}",
                  file=sys.stderr)

    print(json.dumps(report, indent=2, ensure_ascii=False))
    sys.exit(1 if report.get("has_regression") else 0)


if __name__ == "__main__":
    main()
