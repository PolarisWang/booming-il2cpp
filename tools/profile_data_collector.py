#!/usr/bin/env python3
"""Profile data collector — runs entry.exe --benchmark and captures PROFILE_DUMP() stderr.

Usage:
    python tools/profile_data_collector.py \\
        --family convert-char \\
        --entry-exe-path /path/to/entry.exe \\
        --iterations 100000 \\
        --all-methods

Output: JSON report with parsed PROFILE| scopes aggregated across methods.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
import time
from pathlib import Path
from typing import Any

_PROFILE_LINE_RE = re.compile(
    r"^PROFILE\|(?P<name>[^|]+)\|"
    r"avg=(?P<avg_cycles>[0-9.]+)\|"
    r"avg_ns=(?P<avg_ns>[0-9.]+)\|"
    r"min=(?P<min>[0-9]+)\|"
    r"max=(?P<max>[0-9]+)\|"
    r"count=(?P<count>[0-9]+)\|"
    r"total_ns=(?P<total_ns>[0-9.]+)"
)

_CALIBRATION_RE = re.compile(
    r"^PROFILE\|CALIBRATION\|ns_per_cycle=(?P<ns_per_cycle>[0-9.]+)"
)


def parse_profile_lines(stderr_text: str) -> dict[str, Any]:
    """Parse PROFILE_DUMP() stderr output into structured data."""
    scopes: dict[str, dict[str, float]] = {}
    ns_per_cycle: float | None = None

    for line in stderr_text.splitlines():
        line = line.strip()
        m = _CALIBRATION_RE.match(line)
        if m:
            ns_per_cycle = float(m.group("ns_per_cycle"))
            continue
        m = _PROFILE_LINE_RE.match(line)
        if m:
            name = m.group("name")
            scopes[name] = {
                "avg_cycles": float(m.group("avg_cycles")),
                "avg_ns": float(m.group("avg_ns")),
                "min_cycles": int(m.group("min")),
                "max_cycles": int(m.group("max")),
                "count": int(m.group("count")),
                "total_ns": float(m.group("total_ns")),
                "ns_per_call": (
                    float(m.group("total_ns")) / int(m.group("count"))
                    if int(m.group("count")) > 0
                    else 0.0
                ),
            }

    return {"scopes": scopes, "ns_per_cycle": ns_per_cycle}


def run_single_benchmark(
    entry_exe: Path, method_index: int, iterations: int
) -> dict[str, Any]:
    """Run entry.exe --benchmark for one method, return parsed profile data."""
    cmd = [str(entry_exe), "--benchmark", str(method_index), str(iterations)]
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=120,
        )
    except subprocess.TimeoutExpired:
        return {"method_index": method_index, "error": "timeout"}
    except OSError as e:
        return {"method_index": method_index, "error": str(e)}

    stdout = result.stdout or ""
    stderr = result.stderr or ""

    # Parse timing JSON from stdout
    timing: dict[str, Any] = {}
    for line in stdout.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                timing = json.loads(line)
            except json.JSONDecodeError:
                pass

    # Parse profile data from stderr
    profile = parse_profile_lines(stderr)

    return {
        "method_index": method_index,
        "timing": timing,
        "profile": profile,
        "exit_code": result.returncode,
    }


def run_single_hotupdate_benchmark(
    entry_exe: Path, method_index: int, iterations: int
) -> dict[str, Any]:
    """Run entry.exe --hotupdate-and-benchmark for one method (applies patch first).

    The hotupdate-and-benchmark flow:
      1. Apply hotpatch to redirect execution through the interpreter
      2. Warmup call (excluded from PROFILE data via PROFILE_RESET)
      3. Benchmark loop (captured by PROFILE_SCOPE instrumentation)
      4. PROFILE_DUMP() outputs PROFILE| lines to stderr
      5. Revert patch
    """
    cmd = [
        str(entry_exe),
        "--hotupdate-and-benchmark",
        str(method_index),
        str(iterations),
    ]
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=120,
        )
    except subprocess.TimeoutExpired:
        return {"method_index": method_index, "error": "timeout"}
    except OSError as e:
        return {"method_index": method_index, "error": str(e)}

    stdout = result.stdout or ""
    stderr = result.stderr or ""

    # Parse timing JSON from stdout (same format as regular benchmark)
    timing: dict[str, Any] = {}
    for line in stdout.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                timing = json.loads(line)
            except json.JSONDecodeError:
                pass

    # Parse profile data from stderr (interpreter PROFILE_SCOPE data)
    profile = parse_profile_lines(stderr)

    # Also capture any diagnostic output on stderr for debugging
    diag_lines = [
        l for l in stderr.splitlines() if not l.startswith("PROFILE|")
    ]

    return {
        "method_index": method_index,
        "timing": timing,
        "profile": profile,
        "diagnostics": diag_lines,
        "exit_code": result.returncode,
    }


def discover_method_count(entry_exe: Path, hotupdate: bool = False) -> int:
    """Try to discover method count by probing benchmark indices."""
    flag = "--hotupdate-and-benchmark" if hotupdate else "--benchmark"
    for count in [10, 20, 50, 100, 200]:
        cmd = [str(entry_exe), flag, str(count), "1"]
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
        if result.returncode != 0:
            return count
    return 200


def aggregate_profile_results(
    results: list[dict[str, Any]],
) -> dict[str, Any]:
    """Aggregate profile scopes across multiple method benchmark runs."""
    aggregated: dict[str, dict[str, float]] = {}
    total_benchmark_ns: float = 0.0
    ns_per_cycle: float | None = None
    method_timings: list[dict[str, Any]] = []

    for r in results:
        profile = r.get("profile", {})
        # Use the first valid ns_per_cycle
        if ns_per_cycle is None and profile:
            ns_per_cycle = profile.get("ns_per_cycle")

        timing = r.get("timing", {})
        elapsed_ns = timing.get("elapsedMilliseconds", 0) * 1_000_000
        total_benchmark_ns += elapsed_ns

        # Store per-method timing
        method_timings.append({
            "method_index": r["method_index"],
            "elapsed_ms": timing.get("elapsedMilliseconds", 0),
            "ops_per_second": timing.get("opsPerSecond", 0),
            "allocated_bytes": timing.get("allocatedBytes", 0),
        })

        scopes = profile.get("scopes", {}) if profile else {}
        for name, data in scopes.items():
            if name not in aggregated:
                aggregated[name] = {
                    "total_ns": 0.0,
                    "total_count": 0,
                    "min_cycles": float("inf"),
                    "max_cycles": 0,
                    "samples": [],
                }
            agg = aggregated[name]
            agg["total_ns"] += data["total_ns"]
            agg["total_count"] += data["count"]
            agg["min_cycles"] = min(agg["min_cycles"], data["min_cycles"])
            agg["max_cycles"] = max(agg["max_cycles"], data["max_cycles"])
            agg["samples"].append(
                {
                    "method_index": r["method_index"],
                    "avg_ns": data["avg_ns"],
                    "count": data["count"],
                    "total_ns": data["total_ns"],
                }
            )

    # Build sorted scope list
    scope_list = []
    for name, agg in aggregated.items():
        avg_ns = agg["total_ns"] / agg["total_count"] if agg["total_count"] > 0 else 0.0
        pct = (
            (agg["total_ns"] / total_benchmark_ns * 100)
            if total_benchmark_ns > 0
            else 0.0
        )
        scope_list.append({
            "scope_name": name,
            "total_ns": round(agg["total_ns"], 1),
            "total_count": agg["total_count"],
            "avg_ns": round(avg_ns, 1),
            "min_cycles": agg["min_cycles"],
            "max_cycles": agg["max_cycles"],
            "percent_of_total": round(pct, 2),
            "samples": agg["samples"],
        })

    # Sort by total_ns descending (primary ranking metric)
    scope_list.sort(key=lambda s: -s["total_ns"])

    return {
        "ns_per_cycle": ns_per_cycle,
        "total_benchmark_ns": round(total_benchmark_ns, 1),
        "num_methods": len(results),
        "scopes_by_total_ns": scope_list,
        "scopes_by_avg_ns": sorted(scope_list, key=lambda s: -s["avg_ns"]),
        "scopes_by_count": sorted(scope_list, key=lambda s: -s["total_count"]),
        "method_timings": method_timings,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description="Profile data collector")
    parser.add_argument("--family", required=True, help="Family slug (e.g. convert-char)")
    parser.add_argument(
        "--entry-exe-path",
        required=True,
        type=Path,
        help="Path to entry.exe (built with PROFILE config tier)",
    )
    parser.add_argument(
        "--iterations",
        type=int,
        default=100000,
        help="Number of iterations per benchmark run",
    )
    parser.add_argument(
        "--all-methods",
        action="store_true",
        help="Run all methods (auto-discover count)",
    )
    parser.add_argument(
        "--method-index",
        type=int,
        default=None,
        help="Single method index to benchmark",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("profile_data"),
        help="Output directory for profile JSON reports",
    )
    parser.add_argument(
        "--hotupdate",
        action="store_true",
        help="Use --hotupdate-and-benchmark to collect interpreter PROFILE data "
             "(applies patch, runs through interpreter, captures PROFILE_SCOPE data)",
    )
    args = parser.parse_args()

    entry_exe = args.entry_exe_path.resolve()
    if not entry_exe.exists():
        print(f"Error: entry.exe not found at {entry_exe}", file=sys.stderr)
        sys.exit(1)

    mode_label = "hotupdate" if args.hotupdate else "benchmark"

    # Discover or use explicit method indices
    if args.all_methods:
        method_count = discover_method_count(entry_exe, hotupdate=args.hotupdate)
        print(f"Discovered {method_count} methods ({mode_label} mode)", file=sys.stderr)
        indices = list(range(method_count))
    elif args.method_index is not None:
        indices = [args.method_index]
    else:
        print("Error: specify --method-index or --all-methods", file=sys.stderr)
        sys.exit(1)

    # Run benchmarks
    run_fn = run_single_hotupdate_benchmark if args.hotupdate else run_single_benchmark
    results: list[dict[str, Any]] = []
    for idx in indices:
        print(f"  {mode_label} method {idx}/{len(indices)}...", file=sys.stderr)
        r = run_fn(entry_exe, idx, args.iterations)
        results.append(r)
        if "error" in r:
            print(f"    Error: {r['error']}", file=sys.stderr)
        elif r.get("exit_code", 0) != 0:
            print(f"    Exit code: {r['exit_code']}", file=sys.stderr)
        else:
            timing = r.get("timing", {})
            profile = r.get("profile", {})
            scope_count = len(profile.get("scopes", {})) if profile else 0
            print(f"    OK  timing_ms={timing.get('elapsedMilliseconds', 'N/A')}  "
                  f"scopes={scope_count}", file=sys.stderr)

    # Aggregate
    report = aggregate_profile_results(results)
    report["family"] = args.family
    report["iterations"] = args.iterations
    report["captured_at"] = time.strftime("%Y-%m-%dT%H:%M:%S")
    report["mode"] = mode_label

    # Write output
    output_dir = args.output_dir / args.family
    output_dir.mkdir(parents=True, exist_ok=True)
    filename = "profile-report-hotupdate.json" if args.hotupdate else "profile-report.json"
    report_path = output_dir / filename
    with open(report_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)

    print(f"\nProfile report written to {report_path}", file=sys.stderr)

    # Print top 10 scopes
    print("\nTop scopes by total_ns:", file=sys.stderr)
    for s in report["scopes_by_total_ns"][:10]:
        print(
            f"  {s['scope_name']:40s} "
            f"total_ns={s['total_ns']:12.1f}  "
            f"avg_ns={s['avg_ns']:8.1f}  "
            f"count={s['total_count']:8d}  "
            f"({s['percent_of_total']:5.2f}%)",
            file=sys.stderr,
        )


if __name__ == "__main__":
    main()
