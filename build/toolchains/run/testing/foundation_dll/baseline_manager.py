"""Baseline Manager — checksum + performance baseline read/write/compare.

Stores per-family baselines in testing/verification-history/:
  testing/verification-history/<assembly>/<family>/baseline-checksums.json
  testing/verification-history/<assembly>/<family>/baseline-benchmark.json

Supports:
  - Save/reload checksum baselines from Fact Static verification
  - Save/reload benchmark performance baselines
  - Regression detection: compare current results vs stored baseline
  - Automatic baseline creation when no baseline exists yet
"""

from __future__ import annotations

import json
import sys
import time
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_DEFAULT_BASELINE_ROOT = _REPO_ROOT / "testing" / "verification-history"

sys.path.insert(0, str(_HERE.parent.parent))
try:
    from testing.trace import trace
except ImportError:
    def trace(*args, **kwargs):
        pass


# ── Path helpers ───────────────────────────────────────────────────────

def _checksum_path(family_slug: str, assembly: str,
                   baseline_root: Path | None = None) -> Path:
    root = baseline_root or _DEFAULT_BASELINE_ROOT
    return root / assembly / family_slug / "baseline-checksums.json"


def _benchmark_path(family_slug: str, assembly: str,
                    baseline_root: Path | None = None) -> Path:
    root = baseline_root or _DEFAULT_BASELINE_ROOT
    return root / assembly / family_slug / "baseline-benchmark.json"


# ── Checksum baseline ─────────────────────────────────────────────────

def save_checksum_baseline(
    family_slug: str,
    assembly: str,
    checksums: list[dict[str, Any]],
    *,
    baseline_root: Path | None = None,
    source: str = "fact_static",
) -> dict[str, Any]:
    """Save Fact Static checksum results as a baseline for regression detection.

    Args:
        checksums: List of per-method checksum dicts:
            [{"methodIndex": 0, "expected": 42, "actual": 42, "status": "passed"}, ...]
        source: Origin identifier ("fact_static", "fact_runtime", etc.)

    Returns:
        The saved baseline dict.
    """
    path = _checksum_path(family_slug, assembly, baseline_root)
    path.parent.mkdir(parents=True, exist_ok=True)

    baseline = {
        "schemaVersion": 1,
        "family": family_slug,
        "assembly": assembly,
        "source": source,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "methodCount": len(checksums),
        "checksums": checksums,
    }
    path.write_text(json.dumps(baseline, indent=2, ensure_ascii=False), encoding="utf-8")
    trace("baseline_saved", family=family_slug, type="checksum", count=len(checksums))
    return baseline


def load_checksum_baseline(
    family_slug: str,
    assembly: str,
    *,
    baseline_root: Path | None = None,
) -> dict[str, Any] | None:
    """Load the stored checksum baseline for a family.

    Returns None if no baseline exists.
    """
    path = _checksum_path(family_slug, assembly, baseline_root)
    if not path.exists():
        return None
    try:
        with open(path, encoding="utf-8") as f:
            return json.load(f)
    except (json.JSONDecodeError, OSError):
        return None


def compare_checksum_baseline(
    family_slug: str,
    assembly: str,
    current_checksums: list[dict[str, Any]],
    *,
    baseline_root: Path | None = None,
) -> dict[str, Any]:
    """Compare current checksums against stored baseline.

    Args:
        current_checksums: Current run's per-method checksum results.

    Returns:
        Regression dict with keys:
          - hasBaseline: True if a baseline exists
          - changed: list of method indices with different checksums
          - added: list of method indices in current but not baseline
          - removed: list of method indices in baseline but not current
          - unchangedCount: number of methods with same checksum
          - totalChanged: total methods with differences
          - status: "matched" | "changed" | "no_baseline"
    """
    baseline = load_checksum_baseline(family_slug, assembly, baseline_root=baseline_root)

    if baseline is None:
        # Auto-create baseline from current results
        save_checksum_baseline(family_slug, assembly, current_checksums,
                               baseline_root=baseline_root)
        return {
            "hasBaseline": False,
            "status": "no_baseline",
            "message": "Baseline auto-created from current run",
            "changed": [],
            "added": [],
            "removed": [],
            "unchangedCount": len(current_checksums),
            "totalChanged": 0,
        }

    baseline_map: dict[int, int] = {}
    for entry in baseline.get("checksums", []):
        idx = entry.get("methodIndex")
        if idx is not None:
            baseline_map[idx] = entry.get("expected", 0)

    current_map: dict[int, int] = {}
    for entry in current_checksums:
        idx = entry.get("methodIndex")
        if idx is not None:
            current_map[idx] = entry.get("actual", entry.get("expected", 0))

    changed = []
    added = []
    removed = []
    unchanged = 0

    all_indices = set(baseline_map.keys()) | set(current_map.keys())
    for idx in sorted(all_indices):
        if idx in baseline_map and idx in current_map:
            if baseline_map[idx] != current_map[idx]:
                changed.append({
                    "methodIndex": idx,
                    "baseline": baseline_map[idx],
                    "current": current_map[idx],
                })
            else:
                unchanged += 1
        elif idx in current_map:
            added.append({"methodIndex": idx, "current": current_map[idx]})
        else:
            removed.append({"methodIndex": idx, "baseline": baseline_map[idx]})

    total_changed = len(changed) + len(added) + len(removed)
    status = "matched" if total_changed == 0 else "changed"

    return {
        "hasBaseline": True,
        "status": status,
        "changed": changed,
        "added": added,
        "removed": removed,
        "unchangedCount": unchanged,
        "totalChanged": total_changed,
    }


# ── Benchmark baseline ────────────────────────────────────────────────

def save_benchmark_baseline(
    family_slug: str,
    assembly: str,
    benchmark_results: list[dict[str, Any]],
    *,
    baseline_root: Path | None = None,
) -> dict[str, Any]:
    """Save benchmark results as a performance baseline."""
    path = _benchmark_path(family_slug, assembly, baseline_root)
    path.parent.mkdir(parents=True, exist_ok=True)

    baseline = {
        "schemaVersion": 1,
        "family": family_slug,
        "assembly": assembly,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "methodCount": len(benchmark_results),
        "results": benchmark_results,
    }
    path.write_text(json.dumps(baseline, indent=2, ensure_ascii=False), encoding="utf-8")
    trace("baseline_saved", family=family_slug, type="benchmark", count=len(benchmark_results))
    return baseline


def load_benchmark_baseline(
    family_slug: str,
    assembly: str,
    *,
    baseline_root: Path | None = None,
) -> dict[str, Any] | None:
    """Load stored benchmark baseline."""
    path = _benchmark_path(family_slug, assembly, baseline_root)
    if not path.exists():
        return None
    try:
        with open(path, encoding="utf-8") as f:
            return json.load(f)
    except (json.JSONDecodeError, OSError):
        return None


def compare_benchmark_baseline(
    family_slug: str,
    assembly: str,
    current_results: list[dict[str, Any]],
    *,
    baseline_root: Path | None = None,
    slowdown_threshold: float = 1.5,
) -> dict[str, Any]:
    """Compare current benchmark results against stored baseline.

    Args:
        current_results: Current run's per-method benchmark results.
        slowdown_threshold: Factor above which a slowdown is flagged (default 1.5x).

    Returns:
        Regression dict with keys:
          - hasBaseline: True if a baseline exists
          - regressions: list of methods with significant slowdown
          - improvements: list of methods with significant speedup (>2x)
          - status: "matched" | "regressed" | "improved" | "no_baseline"
    """
    baseline = load_benchmark_baseline(family_slug, assembly, baseline_root=baseline_root)

    if baseline is None:
        save_benchmark_baseline(family_slug, assembly, current_results,
                                baseline_root=baseline_root)
        return {
            "hasBaseline": False,
            "status": "no_baseline",
            "message": "Benchmark baseline auto-created from current run",
            "regressions": [],
            "improvements": [],
        }

    baseline_map: dict[int, float] = {}
    for entry in baseline.get("results", []):
        idx = entry.get("methodIndex")
        if idx is not None:
            baseline_map[idx] = entry.get("elapsedMilliseconds", 0.0)

    current_map: dict[int, float] = {}
    for entry in current_results:
        idx = entry.get("methodIndex")
        if idx is not None:
            current_map[idx] = entry.get("elapsedMilliseconds", 0.0)

    regressions = []
    improvements = []
    all_indices = set(baseline_map.keys()) | set(current_map.keys())

    for idx in sorted(all_indices):
        if idx not in baseline_map or idx not in current_map:
            continue
        b = baseline_map[idx]
        c = current_map[idx]
        if b <= 0 or c <= 0:
            continue
        ratio = c / b
        if ratio > slowdown_threshold:
            regressions.append({
                "methodIndex": idx,
                "baselineMs": b,
                "currentMs": c,
                "slowdownFactor": round(ratio, 2),
            })
        elif ratio < 0.5:
            improvements.append({
                "methodIndex": idx,
                "baselineMs": b,
                "currentMs": c,
                "speedupFactor": round(b / c, 2),
            })

    if regressions:
        status = "regressed"
    elif improvements:
        status = "improved"
    else:
        status = "matched"

    return {
        "hasBaseline": True,
        "status": status,
        "regressions": regressions,
        "improvements": improvements,
        "totalCompared": len(all_indices),
    }


# ── CLI ───────────────────────────────────────────────────────────────

def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Baseline manager for regression detection")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--save-checksums", type=argparse.FileType("r"),
                        help="JSON file with checksums to save as baseline")
    parser.add_argument("--save-benchmark", type=argparse.FileType("r"),
                        help="JSON file with benchmark results to save as baseline")
    parser.add_argument("--compare-checksums", type=argparse.FileType("r"),
                        help="JSON file with checksums to compare against baseline")
    parser.add_argument("--compare-benchmark", type=argparse.FileType("r"),
                        help="JSON file with benchmark results to compare against baseline")

    args = parser.parse_args()

    if args.save_checksums:
        data = json.load(args.save_checksums)
        result = save_checksum_baseline(args.family_slug, args.assembly, data)
        print(f"Checksum baseline saved: {len(data)} methods")

    if args.save_benchmark:
        data = json.load(args.save_benchmark)
        result = save_benchmark_baseline(args.family_slug, args.assembly, data)
        print(f"Benchmark baseline saved: {len(data)} results")

    if args.compare_checksums:
        data = json.load(args.compare_checksums)
        result = compare_checksum_baseline(args.family_slug, args.assembly, data)
        print(f"Checksum comparison: {result['status']}")
        if result.get("changed"):
            print(f"  Changed: {len(result['changed'])} methods")
        if result.get("added"):
            print(f"  Added: {len(result['added'])} methods")

    if args.compare_benchmark:
        data = json.load(args.compare_benchmark)
        result = compare_benchmark_baseline(args.family_slug, args.assembly, data)
        print(f"Benchmark comparison: {result['status']}")
        if result.get("regressions"):
            print(f"  Regressions: {len(result['regressions'])} methods")
        if result.get("improvements"):
            print(f"  Improvements: {len(result['improvements'])} methods")


if __name__ == "__main__":
    main()
