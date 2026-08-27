#!/usr/bin/env python3
"""gc-baseline — GC benchmark baseline management tool.

Records benchmark results as baselines and compares current runs
against stored baselines to detect regressions.

Usage:
  # Record a new baseline (run benchmark and save)
  python gc-baseline.py --record baseline_name

  # Run benchmark and compare against stored baseline
  python gc-baseline.py --check baseline_name

  # Run benchmark and compare against a specific baseline file
  python gc-baseline.py --check baseline_name --baseline-file path/to/baseline.txt

  # List all stored baselines
  python gc-baseline.py --list

  # Update an existing baseline (after verifying improvement)
  python gc-baseline.py --update baseline_name
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path
from typing import Any

# Tool location and defaults
TOOLS_DIR = Path(__file__).resolve().parent
BENCH_COMPARE = TOOLS_DIR / "gc-bench-compare.py"
BASELINE_DIR = TOOLS_DIR / "gc-baselines"
DEFAULT_BENCHMARK_EXE = "testing/build/runtime-core/gc/RelWithDebInfo/test_gc_regression_benchmark.exe"

# Resolve repo root (tools/ is one level below repo root)
REPO_ROOT = TOOLS_DIR.parent


def _resolve_exe(exe: str | None) -> str:
    """Resolve benchmark executable path relative to repo root."""
    if exe is None or exe == "default":
        return str((REPO_ROOT / DEFAULT_BENCHMARK_EXE).resolve())
    # If relative, resolve relative to repo root
    p = Path(exe)
    if not p.is_absolute():
        p = (REPO_ROOT / exe).resolve()
    return str(p.resolve())


def run_benchmark(exe: str) -> str:
    """Run the benchmark executable and return its stdout (BENCH| lines)."""
    import io
    result = subprocess.run(
        [exe],
        capture_output=True,
        text=False,  # raw bytes to handle encoding issues
        timeout=120,
    )
    # Decode with replacement for non-UTF-8 bytes (e.g., ANSI escape sequences).
    stdout = result.stdout.decode("utf-8", errors="replace") if result.stdout else ""
    stderr = result.stderr.decode("utf-8", errors="replace") if result.stderr else ""
    if result.returncode != 0:
        print(f"Warning: benchmark exited with code {result.returncode}", file=sys.stderr)
        for line in stderr.splitlines():
            if not line.startswith("BENCH|"):
                print(f"  stderr: {line}", file=sys.stderr)
    return stdout


def extract_bench_lines(output: str) -> str:
    """Extract only BENCH| lines from benchmark output."""
    lines = []
    for line in output.splitlines():
        if line.startswith("BENCH|"):
            lines.append(line)
    return "\n".join(lines) + "\n"


def save_baseline(name: str, bench_output: str, exe: str) -> Path:
    """Save BENCH| output as a baseline file."""
    BASELINE_DIR.mkdir(parents=True, exist_ok=True)
    filename = f"{name}.txt"
    path = BASELINE_DIR / filename

    header = (
        f"# GC Benchmark Baseline: {name}\n"
        f"# Date: {datetime.now().isoformat()}\n"
        f"# Executable: {exe}\n"
        f"#\n"
    )

    with open(path, "w", encoding="utf-8") as f:
        f.write(header)
        f.write(extract_bench_lines(bench_output))

    return path


def enumerate_baselines() -> dict[str, list[str]]:
    """Return {name: [filename, ...]} for all stored baselines."""
    if not BASELINE_DIR.exists():
        return {}
    baselines: dict[str, list[str]] = {}
    for f in sorted(BASELINE_DIR.iterdir()):
        if f.suffix == ".txt" and not f.name.startswith("."):
            name = f.stem  # Use full stem as the name (e.g., "baseline-v1")
            if name not in baselines:
                baselines[name] = []
            baselines[name].append(f.name)
    return baselines


def list_baselines() -> None:
    """Print all stored baselines."""
    baselines = enumerate_baselines()
    if not baselines:
        print("No baselines stored.")
        return

    print(f"Stored baselines (in {BASELINE_DIR}):")
    print(f"  {'Name':<30} {'Versions':<15} {'Latest file'}")
    print(f"  {'-'*30} {'-'*15} {'-'*40}")
    for name in sorted(baselines.keys()):
        files = baselines[name]
        versions = len(files)
        latest = files[-1]
        print(f"  {name:<30} {versions:<15} {latest}")


def get_latest_baseline_path(name: str) -> Path | None:
    """Get the path to the latest version of a named baseline."""
    baselines = enumerate_baselines()
    if name not in baselines:
        return None
    latest = baselines[name][-1]
    return BASELINE_DIR / latest


def load_baseline(path: Path) -> str:
    """Load a baseline file, stripping comment lines."""
    with open(path, "r", encoding="utf-8") as f:
        lines = [l for l in f if not l.startswith("#")]
    return "".join(lines)


def run_check(
    name: str,
    current_output: str,
    baseline_path: Path,
    warn: float,
    fail: float,
) -> int:
    """Compare current output against a baseline. Returns exit code."""
    baseline_content = load_baseline(baseline_path)

    # Write current output to temp file for comparison.
    current_path = BASELINE_DIR / f".current-{name}.txt"
    current_path.write_text(extract_bench_lines(current_output), encoding="utf-8")

    try:
        result = subprocess.run(
            [sys.executable, str(BENCH_COMPARE),
             "--baseline", str(baseline_path),
             "--current", str(current_path),
             "--warn", str(warn),
             "--fail", str(fail)],
            capture_output=True,
            text=True,
        )
        print(result.stdout)
        if result.stderr:
            print(result.stderr, file=sys.stderr)
        return result.returncode
    finally:
        if current_path.exists():
            current_path.unlink()


def main() -> None:
    parser = argparse.ArgumentParser(
        description="GC benchmark baseline management tool",
    )
    parser.add_argument("--record", "-r", metavar="NAME",
                        help="Run benchmark and record a new baseline")
    parser.add_argument("--check", "-c", metavar="NAME",
                        help="Run benchmark and compare against stored baseline")
    parser.add_argument("--update", "-u", metavar="NAME",
                        help="Update an existing baseline with current run")
    parser.add_argument("--list", "-l", action="store_true",
                        help="List all stored baselines")
    parser.add_argument("--baseline-file", metavar="PATH",
                        help="Specific baseline file path (for --check)")
    parser.add_argument("--exe", metavar="PATH",
                        default="default",
                        help=f"Benchmark executable path (default: {DEFAULT_BENCHMARK_EXE})")
    parser.add_argument("--warn", type=float, default=5.0,
                        help="Warn threshold %% (default: 5.0)")
    parser.add_argument("--fail", type=float, default=10.0,
                        help="Fail threshold %% (default: 10.0)")
    args = parser.parse_args()

    exe = _resolve_exe(args.exe)

    if args.list:
        list_baselines()
        return

    if args.record:
        print(f"Running benchmark ({exe})...")
        output = run_benchmark(exe)
        path = save_baseline(args.record, output, exe)
        print(f"Baseline '{args.record}' saved to: {path}")
        return

    if args.check:
        if args.baseline_file:
            baseline_path = Path(args.baseline_file)
            if not baseline_path.exists():
                print(f"Error: baseline file not found: {baseline_path}", file=sys.stderr)
                sys.exit(1)
        else:
            baseline_path = get_latest_baseline_path(args.check)
            if baseline_path is None:
                print(f"Error: no baseline found for '{args.check}'",
                      file=sys.stderr)
                print("Use --record to create one, or --baseline-file to specify a path.",
                      file=sys.stderr)
                sys.exit(1)

        print(f"Running benchmark ({exe})...")
        output = run_benchmark(exe)
        print(f"Comparing against baseline: {baseline_path}")
        exit_code = run_check(args.check, output, baseline_path, args.warn, args.fail)
        sys.exit(exit_code)

    if args.update:
        baseline_path = get_latest_baseline_path(args.update)
        if baseline_path is None:
            print(f"Error: no baseline found for '{args.update}'", file=sys.stderr)
            sys.exit(1)

        print(f"Running benchmark ({exe})...")
        output = run_benchmark(exe)
        path = save_baseline(args.update, output, exe)
        print(f"Baseline '{args.update}' updated: {path}")
        return

    # No action specified.
    parser.print_help()


if __name__ == "__main__":
    main()
