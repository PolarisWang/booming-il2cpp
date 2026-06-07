"""GC health check — consumes profile.json and flags GC-related anomalies.

Checks:
  - GC-01: Max GC pause > 5ms
  - GC-02: GC pause count > 100 per method
  - GC-03: Fast path rate < 80%
  - GC-04: Heap delta / alloc_bytes ratio > 2.0 (indicates fragmentation)
  - GC-05: Alloc count / iteration > 10000
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

# ── Default thresholds (tunable) ──
DEFAULT_THRESHOLDS: dict[str, float] = {
    "max_gc_pause_ns": 5_000_000,       # 5ms in nanoseconds
    "gc_pause_count": 100,
    "min_fast_path_rate": 0.80,         # 80%
    "max_heap_delta_ratio": 2.0,
    "max_alloc_count": 10_000,
}


def check_profile(
    profile_data: list[dict],
    thresholds: dict[str, float] | None = None,
) -> dict[str, Any]:
    """Run all GC health checks on profile data.

    Args:
        profile_data: List of per-method profile dicts from profile.json.
        thresholds: Custom threshold overrides. Uses DEFAULT_THRESHOLDS if None.

    Returns:
        Dict with keys: status, issues, summary.
    """
    t = {**DEFAULT_THRESHOLDS, **(thresholds or {})}
    issues: list[dict] = []

    for method in profile_data:
        method_index = method.get("methodIndex", -1)

        # GC-01: Max GC pause > 5ms
        max_pause_ns = method.get("maxGcPauseNs", 0)
        if max_pause_ns > t["max_gc_pause_ns"]:
            issues.append({
                "check": "GC-01",
                "severity": "warning",
                "methodIndex": method_index,
                "message": f"Max GC pause {max_pause_ns}ns exceeds {t['max_gc_pause_ns']}ns",
                "actual": max_pause_ns,
                "threshold": t["max_gc_pause_ns"],
            })

        # GC-02: GC pause count > 100
        pause_count = method.get("gcPauseCount", 0)
        if pause_count > t["gc_pause_count"]:
            issues.append({
                "check": "GC-02",
                "severity": "warning",
                "methodIndex": method_index,
                "message": f"GC pause count {pause_count} exceeds {t['gc_pause_count']}",
                "actual": pause_count,
                "threshold": t["gc_pause_count"],
            })

        # GC-03: Fast path rate < 80%
        fast_count = method.get("fastPathCount", 0)
        slow_count = method.get("slowPathCount", 0)
        total_path = fast_count + slow_count
        if total_path > 0:
            fast_path_rate = fast_count / total_path
            if fast_path_rate < t["min_fast_path_rate"]:
                issues.append({
                    "check": "GC-03",
                    "severity": "warning",
                    "methodIndex": method_index,
                    "message": f"Fast path rate {fast_path_rate:.1%} below {t['min_fast_path_rate']:.0%}",
                    "actual": round(fast_path_rate, 4),
                    "threshold": t["min_fast_path_rate"],
                })

        # GC-04: Heap delta / alloc_bytes ratio > 2.0
        heap_before = method.get("heapBefore", 0)
        heap_after = method.get("heapAfter", 0)
        nursery_bytes = method.get("nurseryAllocBytes", 0)
        if heap_before > 0 and nursery_bytes > 0:
            heap_delta = heap_after - heap_before
            if heap_delta > 0:
                ratio = heap_delta / nursery_bytes
                if ratio > t["max_heap_delta_ratio"]:
                    issues.append({
                        "check": "GC-04",
                        "severity": "info",
                        "methodIndex": method_index,
                        "message": f"Heap delta/alloc ratio {ratio:.1f} exceeds {t['max_heap_delta_ratio']}",
                        "actual": round(ratio, 2),
                        "threshold": t["max_heap_delta_ratio"],
                    })

        # GC-05: Alloc count > 10000
        alloc_count = method.get("allocCount", 0)
        if alloc_count > t["max_alloc_count"]:
            issues.append({
                "check": "GC-05",
                "severity": "info",
                "methodIndex": method_index,
                "message": f"Allocation count {alloc_count} exceeds {t['max_alloc_count']}",
                "actual": alloc_count,
                "threshold": t["max_alloc_count"],
            })

    # Compute summary
    by_severity: dict[str, int] = {}
    for issue in issues:
        sev = issue.get("severity", "info")
        by_severity[sev] = by_severity.get(sev, 0) + 1

    if not issues:
        status = "healthy"
    elif by_severity.get("error", 0) > 0:
        status = "unhealthy"
    elif by_severity.get("warning", 0) > 5:
        status = "degraded"
    elif by_severity.get("warning", 0) > 0:
        status = "warning"
    else:
        status = "healthy"

    return {
        "status": status,
        "issueCount": len(issues),
        "bySeverity": by_severity,
        "issues": issues,
        "summary": {
            "status": status,
            "totalChecks": len(profile_data) * 5,
            "totalIssues": len(issues),
            "methodsWithGcPause": sum(1 for m in profile_data if m.get("gcPauseCount", 0) > 0),
            "methodsWithAllocation": sum(1 for m in profile_data if m.get("allocCount", 0) > 0),
        },
    }


def check_profile_file(profile_path: Path) -> dict[str, Any]:
    """Load profile.json and run health checks."""
    if not profile_path.exists():
        return {"status": "no_data", "issueCount": 0, "issues": [],
                "summary": {"status": "no_data", "error": f"File not found: {profile_path}"}}

    try:
        data = json.loads(profile_path.read_text(encoding="utf-8"))
        profile_data = data.get("profileData", [])
        if not profile_data:
            return {"status": "no_data", "issueCount": 0, "issues": [],
                    "summary": {"status": "no_data", "error": "No profile data in file"}}
        return check_profile(profile_data)
    except (json.JSONDecodeError, OSError) as e:
        return {"status": "error", "issueCount": 0, "issues": [],
                "summary": {"status": "error", "error": str(e)}}


def print_report(result: dict[str, Any]) -> None:
    """Print a human-readable GC health report."""
    summary = result.get("summary", {})
    status = result.get("status", "unknown")
    issues = result.get("issues", [])

    print(f"  [gc-check] Status: {status}")
    print(f"  [gc-check] Issues: {result.get('issueCount', 0)} "
          f"(error={summary.get('methodsWithGcPause', 0)}, "
          f"warning={summary.get('methodsWithAllocation', 0)})")

    if issues:
        # Group by severity
        for sev in ("error", "warning", "info"):
            sev_issues = [i for i in issues if i.get("severity") == sev]
            if not sev_issues:
                continue
            print(f"  [gc-check]   {sev.upper()} issues ({len(sev_issues)}):")
            for issue in sev_issues[:5]:  # show top 5 per severity
                print(f"    [{issue['check']}] method {issue['methodIndex']}: {issue['message']}")
            if len(sev_issues) > 5:
                print(f"    ... and {len(sev_issues) - 5} more")


# ── CLI entry point ──
def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(
        description="GC health check — analyze profile.json output")
    parser.add_argument("profile_json", nargs="?",
                        help="Path to profile.json (default: reads from stdin or looks in cwd)")
    parser.add_argument("--threshold", type=float, default=None,
                        help="Override max GC pause threshold in ms")
    args = parser.parse_args()

    thresholds = None
    if args.threshold is not None:
        thresholds = {"max_gc_pause_ns": args.threshold * 1_000_000}

    if args.profile_json:
        result = check_profile_file(Path(args.profile_json))
    else:
        # Try reading from stdin
        import sys
        try:
            data = json.loads(sys.stdin.read())
            profile_data = data.get("profileData", [])
            result = check_profile(profile_data, thresholds)
        except (json.JSONDecodeError, OSError):
            print("ERROR: Provide profile.json path or pipe data to stdin", file=sys.stderr)
            sys.exit(1)

    print_report(result)
    if result.get("status") in ("unhealthy", "error"):
        sys.exit(1)


if __name__ == "__main__":
    main()
