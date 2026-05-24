#!/usr/bin/env python3
"""Collect JIT performance metrics from benchmark and native test output.

Accepts either direct executable paths (--benchmark, --native) or pre-captured
text output (--benchmark-output, --native-output).  When both are provided for
the same source, the executable is run and its output is used.

Outputs a JSON metrics file suitable for baseline comparison.

Usage:
    # Direct execution (local use):
    python collect-jit-metrics.py --benchmark=<path> --native=<path> --output=<path>

    # From captured output (CI use):
    ctest -R bench_hybrid ... > bench_output.txt
    ctest -R test_jit_native ... > native_output.txt
    python collect-jit-metrics.py --benchmark-output=bench_output.txt \\
                                  --native-output=native_output.txt \\
                                  --output=<path>
"""

import argparse
import json
import os
import re
import subprocess
import sys


def run_test(exe_path: str, args: list[str] | None = None) -> str:
    """Run a test executable and return stdout+stderr."""
    cmd = [os.path.abspath(exe_path)]
    if args:
        cmd.extend(args)
    result = subprocess.run(cmd, capture_output=True, text=False, timeout=300)
    stdout = result.stdout.decode('utf-8', errors='replace') if result.stdout else ''
    stderr = result.stderr.decode('utf-8', errors='replace') if result.stderr else ''
    return stdout + stderr


def read_file(path: str) -> str:
    """Read text from a file with encoding fallback."""
    for enc in ('utf-8', 'gbk', 'latin-1'):
        try:
            with open(path, encoding=enc) as f:
                return f.read()
        except UnicodeDecodeError:
            continue
    with open(path, encoding='utf-8', errors='replace') as f:
        return f.read()


def parse_bench_lines(text: str) -> dict:
    """Parse BENCH,<name>,KEY=VAL,... lines into a structured dict.

    Example input:
        BENCH,upgrade-latency,P50=123.4,P99=567.8,MIN=10.0,MAX=999.9,AVG=345.6,N=50
        BENCH,compile-time-only,P50=45.6,P99=123.4,MIN=20.0,MAX=200.0,AVG=67.8,N=30
        BENCH,aot-dispatch-overhead,PER_CALL_US=1.23,TOTAL_US=1234.5,N=1000
        BENCH,multi-method-upgrade,BATCH=10,TOTAL_US=5000.0,PER_METHOD_US=500.0
    """
    metrics = {}
    for line in text.splitlines():
        if not line.startswith("BENCH,"):
            continue
        parts = line.split(",")
        if len(parts) < 3:
            continue
        name = parts[1]
        kv_pairs = {}
        for kv in parts[2:]:
            if "=" in kv:
                key, val = kv.split("=", 1)
                try:
                    kv_pairs[key] = float(val)
                except ValueError:
                    kv_pairs[key] = val
        metrics[name] = kv_pairs
    return metrics


def parse_code_size(text: str) -> list[dict]:
    """Parse 'Compile: N instrs, M bytes code, ...' lines for code sizes."""
    sizes = []
    pattern = re.compile(r"Compile:\s+(\d+)\s+instrs?,\s+(\d+)\s+bytes?\s+code")
    for line in text.splitlines():
        m = pattern.search(line)
        if m:
            sizes.append({
                "instr_count": int(m.group(1)),
                "code_size": int(m.group(2)),
            })
    return sizes


def compute_code_size_stats(sizes: list[dict]) -> dict:
    """Compute aggregate stats from code size samples."""
    if not sizes:
        return {"samples": 0}
    values = [s["code_size"] for s in sizes]
    values.sort()
    n = len(values)
    return {
        "samples": n,
        "min": values[0],
        "max": values[-1],
        "p50": values[n * 50 // 100],
        "p95": values[n * 95 // 100],
        "p99": values[n * 99 // 100],
        "avg": sum(values) / n,
        "total_instrs": sum(s["instr_count"] for s in sizes),
    }


def get_text(source_exe: str | None, source_file: str | None, label: str) -> str | None:
    """Get text output from either an executable or a captured file."""
    if source_exe:
        print(f"Running {label}: {source_exe}", file=sys.stderr)
        return run_test(source_exe)
    if source_file:
        print(f"Reading {label} output from: {source_file}", file=sys.stderr)
        return read_file(source_file)
    return None


def main():
    parser = argparse.ArgumentParser(description="Collect JIT performance metrics")
    parser.add_argument("--benchmark", help="Path to benchmark executable")
    parser.add_argument("--benchmark-output", help="Pre-captured benchmark output file")
    parser.add_argument("--native", help="Path to test_jit_native executable")
    parser.add_argument("--native-output", help="Pre-captured native test output file")
    parser.add_argument("--output", required=True, help="Output JSON file path")
    args = parser.parse_args()

    report = {
        "benchmarks": {},
        "code_size": {},
        "test_counts": {},
    }

    # Benchmark metrics
    bench_text = get_text(args.benchmark, args.benchmark_output, "benchmark")
    if bench_text:
        report["benchmarks"] = parse_bench_lines(bench_text)
        print(f"  Parsed {len(report['benchmarks'])} benchmark entries", file=sys.stderr)

    # Native test metrics (code size)
    native_text = get_text(args.native, args.native_output, "native tests")
    if native_text:
        sizes = parse_code_size(native_text)
        report["code_size"] = compute_code_size_stats(sizes)
        print(f"  Parsed {report['code_size']['samples']} code_size samples", file=sys.stderr)

        passed = len(re.findall(r"\[  PASSED  \]", native_text))
        failed = len(re.findall(r"\[  FAILED  \]", native_text))
        report["test_counts"] = {"passed": passed, "failed": failed}

    # Write output
    os.makedirs(os.path.dirname(args.output) or ".", exist_ok=True)
    with open(args.output, "w") as f:
        json.dump(report, f, indent=2)

    print(f"Metrics written to {args.output}", file=sys.stderr)


if __name__ == "__main__":
    main()

