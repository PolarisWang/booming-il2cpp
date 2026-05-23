"""interpreter_benchmark_pipeline.py — Benchmark regression pipeline for interpreter tiering.

Runs the native interpreter tiering benchmark, parses SUMMARY output,
records to JSONL via the existing benchmark_records infrastructure, and
compares against baselines to detect regressions.

Usage:
    python interpreter_benchmark_pipeline.py [--record] [--compare] [--threshold 20]
        [--bin-dir <path>] [--baseline-dir <path>]

    --record       Record results to JSONL benchmark store
    --compare      Compare against latest baseline (exit 1 on regression)
    --threshold N  Regression threshold in percent (default 20)
    --bin-dir      Directory containing test executables (auto-detected)
"""
from __future__ import annotations

import argparse
import json
import os
import re
import sys
import time
import uuid
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

# ---------------------------------------------------------------------------
# Try importing existing infrastructure
# ---------------------------------------------------------------------------

try:
    from . import benchmark_records as br
    from . import verification_layout as vl
except ImportError:
    run_root = Path(__file__).resolve().parents[1]
    if str(run_root) not in sys.path:
        sys.path.insert(0, str(run_root))
    from testing import benchmark_records as br
    from testing import verification_layout as vl

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

SUBJECT_ID = "InterpreterTiering"
MODE = "interpreter"
REGRESSION_THRESHOLD_PCT = 20.0  # default

# ---------------------------------------------------------------------------
# Data model
# ---------------------------------------------------------------------------


@dataclass
class TierResult:
    scenario: str
    t1_ns: float
    t2_ns: float
    t3_ns: float
    t4_ns: float | None = None


@dataclass
class BenchmarkRun:
    results: list[TierResult] = field(default_factory=list)
    raw_output: str = ""
    return_code: int = 0
    duration_ms: float = 0.0


# ---------------------------------------------------------------------------
# Device info for record metadata
# ---------------------------------------------------------------------------


def _collect_device_info() -> dict[str, Any]:
    """Collect stable device identifier from the environment."""
    import platform as _platform

    machine = _platform.machine() or "unknown"
    processor = _platform.processor() or "unknown"
    system = _platform.system() or "unknown"
    node = _platform.node() or "unknown"

    # Build a semi-stable device id
    cpu_bits = "64" if sys.maxsize > 2**32 else "32"
    device_id = f"{system.lower()}-{machine.lower()}-{cpu_bits}bit"

    return {
        "id": device_id,
        "name": f"{system} ({machine}, {processor})",
        "os": f"{system} {_platform.version() or ''}",
        "cpu": processor,
        "arch": machine,
        "isSimulator": False,
    }


# ---------------------------------------------------------------------------
# Git info
# ---------------------------------------------------------------------------


def _git_info(repo_root: Path) -> dict[str, str]:
    """Get git commit and branch from the repo."""
    import subprocess

    info = {"gitCommit": "unknown", "gitBranch": "unknown"}
    try:
        result = subprocess.run(
            ["git", "rev-parse", "--short", "HEAD"],
            capture_output=True, text=True, cwd=repo_root, timeout=30,
        )
        if result.returncode == 0:
            info["gitCommit"] = result.stdout.strip()
    except Exception:
        pass

    try:
        result = subprocess.run(
            ["git", "rev-parse", "--abbrev-ref", "HEAD"],
            capture_output=True, text=True, cwd=repo_root, timeout=30,
        )
        if result.returncode == 0:
            info["gitBranch"] = result.stdout.strip()
    except Exception:
        pass
    return info


# ---------------------------------------------------------------------------
# Find repo root (walk up from this script's directory)
# ---------------------------------------------------------------------------


def _find_repo_root() -> Path:
    script = Path(__file__).resolve()
    for parent in script.parents:
        if (parent / ".git").is_dir() or (parent / "CMakeLists.txt").is_file():
            return parent
    return script.parents[3]  # fallback


# ---------------------------------------------------------------------------
# Locate benchmark executable
# ---------------------------------------------------------------------------


def _find_benchmark_exe(bin_dir: str | None = None) -> Path:
    """Find the interpreter tiering benchmark executable."""
    candidates = []
    if bin_dir:
        candidates.append(Path(bin_dir))

    repo = _find_repo_root()
    # Common build output paths
    candidates.extend([
        repo / "build" / "testing" / "RelWithDebInfo",
        repo / "build" / "testing" / "Release",
        repo / "build" / "testing" / "Debug",
        repo / "build" / "testing",
        repo / "artifacts" / "presets" / "debug" / "bin",
        repo / "artifacts" / "presets" / "release" / "bin",
    ])

    exe_name = "test_interpreter_tiering_benchmark.exe"
    for d in candidates:
        p = d / exe_name
        if p.is_file():
            return p
    raise FileNotFoundError(
        f"Cannot find {exe_name}. Searched: {[str(d) for d in candidates]}"
    )


# ---------------------------------------------------------------------------
# Run the benchmark
# ---------------------------------------------------------------------------


def run_benchmark(exe_path: Path, timeout_sec: int = 300) -> BenchmarkRun:
    """Run the native benchmark executable and capture output."""
    import subprocess

    run = BenchmarkRun()
    start = time.monotonic()

    try:
        result = subprocess.run(
            [str(exe_path)],
            capture_output=True, text=True, timeout=timeout_sec,
        )
        run.raw_output = result.stdout
        run.return_code = result.returncode
    except subprocess.TimeoutExpired as e:
        run.raw_output = e.stdout or ""
        run.return_code = -1
    except Exception as e:
        run.raw_output = f"Error running benchmark: {e}"
        run.return_code = -1

    run.duration_ms = (time.monotonic() - start) * 1000.0

    return run


# ---------------------------------------------------------------------------
# Parser
# ---------------------------------------------------------------------------

# Pattern: --- bench_<name> ---
_SCENARIO_HEADER_RE = re.compile(r"---\s+bench_(\w+)\s+---")

# Pattern:   SUMMARY: T1=<num>ns  T2=<num>ns  T3=<num>ns  [T4=<num>ns]
_SUMMARY_RE = re.compile(
    r"SUMMARY:\s+"
    r"T1=([\d.]+)ns\s+"
    r"T2=([\d.]+)ns\s+"
    r"T3=([\d.]+)ns"
    r"(?:\s+T4=([\d.]+)ns)?"
)


def parse_output(output: str) -> list[TierResult]:
    """Parse benchmark output into structured results.

    Matches scenario headers (--- bench_<name> ---) with subsequent
    SUMMARY lines. Each scenario must produce exactly one SUMMARY.
    """
    results: list[TierResult] = []
    current_scenario: str | None = None

    for line in output.splitlines():
        # Check for scenario header
        m = _SCENARIO_HEADER_RE.search(line)
        if m:
            current_scenario = m.group(1)
            continue

        # Check for SUMMARY line
        m = _SUMMARY_RE.search(line)
        if m and current_scenario:
            t4_str = m.group(4)
            results.append(TierResult(
                scenario=current_scenario,
                t1_ns=float(m.group(1)),
                t2_ns=float(m.group(2)),
                t3_ns=float(m.group(3)),
                t4_ns=float(t4_str) if t4_str else None,
            ))
            current_scenario = None  # reset after consuming

    return results


# ---------------------------------------------------------------------------
# Build benchmark records (JSONL format)
# ---------------------------------------------------------------------------


def _build_records(
    run: BenchmarkRun, repo_root: Path, run_id: str | None = None,
) -> list[dict[str, Any]]:
    """Build a list of benchmark record dicts (one per scenario)."""
    if not run_id:
        run_id = (
            f"{time.strftime('%Y%m%d-%H%M%S')}-{SUBJECT_ID}-{MODE}"
        )

    device = _collect_device_info()
    git = _git_info(repo_root)
    recorded_at = time.strftime("%Y-%m-%dT%H:%M:%S.000000Z", time.gmtime())

    records: list[dict[str, Any]] = []
    for result in run.results:
        # Compute metrics per-tier
        metrics = {
            "sampleCount": 1,
            "meanDurationMs": result.t3_ns / 1_000_000.0,  # T3 ns→ms as primary
            "t1NsPerOp": result.t1_ns,
            "t2NsPerOp": result.t2_ns,
            "t3NsPerOp": result.t3_ns,
        }
        if result.t4_ns is not None:
            metrics["t4NsPerOp"] = result.t4_ns

        record = {
            "runId": f"{run_id}-{result.scenario}",
            "subject": SUBJECT_ID,
            "mode": MODE,
            "platform": device["os"].split()[0].lower(),
            "device": device,
            "recordedAt": recorded_at,
            **git,
            "metrics": metrics,
            "benchmarkCase": {
                "stableId": f"{SUBJECT_ID}::{result.scenario}",
                "alias": result.scenario,
                "displayName": result.scenario,
                "assemblyName": "Chaos.Interpreter",
                "declaringType": f"TieringBenchmark.{result.scenario}",
                "methodName": result.scenario,
                "category": 1,
                "metrics": 4,
                "modes": 4,
                "requires": 0,
                "archetype": 0,
                "hotUpdateCapability": 0,
                "warmupCount": 50,
                "iterationCount": 1,
                "invocationCount": 1,
                "supportedModes": [MODE],
                "entryIndex": 0,
            },
        }
        records.append(record)

    return records


# ---------------------------------------------------------------------------
# Record to JSONL
# ---------------------------------------------------------------------------


def record_results(run: BenchmarkRun, repo_root: Path) -> None:
    """Append benchmark records to the JSONL store."""
    records = _build_records(run, repo_root)
    for rec in records:
        br.append_record(repo_root, rec)
    print(f"  Recorded {len(records)} scenarios to benchmark store")


# ---------------------------------------------------------------------------
# Baseline comparison
# ---------------------------------------------------------------------------


@dataclass
class Regression:
    scenario: str
    tier: str
    baseline_ns: float
    current_ns: float
    change_pct: float


def compare_against_baseline(
    run: BenchmarkRun,
    repo_root: Path,
    threshold_pct: float = REGRESSION_THRESHOLD_PCT,
) -> list[Regression]:
    """Compare current results against per-scenario baselines.

    Queries the latest record for each scenario individually and compares
    T1/T2/T3 ns/op values. Returns a list of regressions (empty = all pass).
    """
    regressions: list[Regression] = []
    device_id = _collect_device_info()["id"]
    records_path = vl.raw_benchmark_records_path(repo_root, SUBJECT_ID)

    # Load latest record per-scenario from the JSONL file by scanning in reverse
    scenario_baselines: dict[str, dict[str, Any]] = {}
    if records_path.exists():
        for line in reversed(records_path.read_text(encoding="utf-8").splitlines()):
            line = line.strip()
            if not line:
                continue
            try:
                record = json.loads(line)
            except json.JSONDecodeError:
                continue
            if record.get("mode") != MODE:
                continue
            if record.get("device", {}).get("id") != device_id:
                continue
            alias = record.get("benchmarkCase", {}).get("alias", "")
            if alias and alias not in scenario_baselines:
                scenario_baselines[alias] = record.get("metrics", {})

    for result in run.results:
        metrics = scenario_baselines.get(result.scenario)
        if metrics is None:
            continue  # no baseline yet

        tiers = [("T1", result.t1_ns, "t1NsPerOp"),
                 ("T2", result.t2_ns, "t2NsPerOp"),
                 ("T3", result.t3_ns, "t3NsPerOp")]
        if result.t4_ns is not None:
            tiers.append(("T4", result.t4_ns, "t4NsPerOp"))

        for tier_name, current_val, metric_key in tiers:
            baseline_val = metrics.get(metric_key)
            if baseline_val is None or baseline_val == 0:
                continue
            change_pct = ((current_val - baseline_val) / baseline_val) * 100.0
            if change_pct > threshold_pct:
                regressions.append(Regression(
                    scenario=result.scenario,
                    tier=tier_name,
                    baseline_ns=float(baseline_val),
                    current_ns=current_val,
                    change_pct=change_pct,
                ))

    return regressions


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Interpreter tiering benchmark regression pipeline"
    )
    parser.add_argument("--record", action="store_true",
                        help="Record results to JSONL benchmark store")
    parser.add_argument("--compare", action="store_true",
                        help="Compare against latest baseline (exit 1 on regression)")
    parser.add_argument("--threshold", type=float, default=REGRESSION_THRESHOLD_PCT,
                        help=f"Regression threshold in %% (default {REGRESSION_THRESHOLD_PCT})")
    parser.add_argument("--bin-dir", type=str, default=None,
                        help="Directory containing benchmark executables")
    parser.add_argument("--timeout", type=int, default=300,
                        help="Benchmark timeout in seconds (default 300)")
    args = parser.parse_args()

    repo_root = _find_repo_root()
    print(f"  Repo root: {repo_root}")

    try:
        exe = _find_benchmark_exe(args.bin_dir)
        print(f"  Benchmark: {exe}")
    except FileNotFoundError as e:
        print(f"  ERROR: {e}", file=sys.stderr)
        return 1

    print(f"  Running benchmark (timeout={args.timeout}s)...")
    run = run_benchmark(exe, timeout_sec=args.timeout)

    if run.return_code != 0 and run.return_code != -1:
        print(f"  WARNING: benchmark exited with code {run.return_code}")
    print(f"  Duration: {run.duration_ms:.0f} ms")

    # Parse output
    results = parse_output(run.raw_output)
    print(f"  Parsed {len(results)} scenarios:")
    for r in results:
        print(f"    {r.scenario}: T1={r.t1_ns:.0f} T2={r.t2_ns:.0f} T3={r.t3_ns:.0f}ns")
    run.results = results

    if not results:
        print("  WARNING: No results parsed from benchmark output", file=sys.stderr)
        # Print tail of output for debugging
        lines = run.raw_output.splitlines()
        print(f"  Last 20 lines of output:")
        for l in lines[-20:]:
            print(f"    {l}")
        return 0 if run.return_code == 0 else 1

    # Record
    if args.record:
        record_results(run, repo_root)

    # Compare
    if args.compare:
        print(f"  Comparing against baseline (threshold={args.threshold:.0f}%)...")
        regressions = compare_against_baseline(run, repo_root, args.threshold)
        if regressions:
            print(f"  REGRESSIONS DETECTED ({len(regressions)}):")
            for reg in regressions:
                print(f"    {reg.scenario} {reg.tier}: "
                      f"{reg.baseline_ns:.0f} → {reg.current_ns:.0f} ns "
                      f"(+{reg.change_pct:.1f}%)")
            return 1
        else:
            print("  No regressions detected.")

    return 0


if __name__ == "__main__":
    sys.exit(main())
