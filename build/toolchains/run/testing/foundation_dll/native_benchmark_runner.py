"""Run native benchmark using il2cpp-translated entry EXE directly.

No C++ host compilation needed — the entry EXE is already a native executable
translated by il2cpp. Just run it with --benchmark arguments.

Usage:
  python native_benchmark_runner.py <family-slug> [--iterations N] [--entry-index IDX]
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION = _REPO_ROOT / "testing" / "foundation-dll" / "System.Private.CoreLib"

_RUN_DIR = _REPO_ROOT / "build" / "toolchains" / "run"
if str(_RUN_DIR) not in sys.path:
    sys.path.insert(0, str(_RUN_DIR))

from testing.trace import trace_init, trace


def _locate_entry_exe(family_slug: str) -> Path | None:
    """Find the native entry EXE in native/entry.exe"""
    family_dir = _VERIFICATION / family_slug
    candidate = family_dir / "native" / "entry.exe"
    return candidate if candidate.exists() else None


def run_benchmark(exe_path: str, method_count: int, iterations: int = 10000) -> list[dict]:
    """Run the benchmark for each method index using the entry EXE."""
    results = []
    print(f"\nRunning native benchmark ({iterations} iterations per method)...")
    for idx in range(method_count):
        cmd = [exe_path, "--benchmark", str(idx), str(iterations)]
        t0 = time.time()
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
            elapsed = time.time() - t0
            if r.returncode == 0:
                data = {"methodIndex": idx, "elapsedMilliseconds": elapsed * 1000,
                        "iterations": iterations, "opsPerSecond": iterations / elapsed if elapsed > 0 else 0}
                results.append(data)
                print(f"  [{idx}] {data['opsPerSecond']:.0f} ops/s  ({data['elapsedMilliseconds']:.2f} ms)")
            else:
                results.append({"methodIndex": idx, "error": f"exit code {r.returncode}"})
                print(f"  [{idx}] FAILED (rc={r.returncode})")
        except subprocess.TimeoutExpired:
            results.append({"methodIndex": idx, "error": "timeout"})
            print(f"  [{idx}] TIMEOUT")
    return results


def _load_method_count(family_slug: str) -> int:
    """Load method count from capability family contract."""
    contract_path = _VERIFICATION / family_slug / "capability-family-contract.json"
    if not contract_path.exists():
        return 0
    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)
    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]
    return len(mids)


def main() -> None:
    parser = argparse.ArgumentParser(description="Native benchmark runner")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--iterations", type=int, default=10000, help="Iterations per method")
    parser.add_argument("--entry-index", type=int, default=None, help="Single entry index to benchmark")
    parser.add_argument("--no-trace", action="store_true", help="Disable JSONL trace logging")
    args = parser.parse_args()

    trace_init(_REPO_ROOT, stage="native-benchmark")
    trace("benchmark_start", family=args.family_slug, iterations=args.iterations)

    exe_path = _locate_entry_exe(args.family_slug)
    if exe_path is None:
        print(f"Entry EXE not found for {args.family_slug}")
        sys.exit(1)
    print(f"Entry EXE: {exe_path}")

    method_count = _load_method_count(args.family_slug) if args.entry_index is None else args.entry_index + 1
    if method_count == 0:
        print(f"No method subject IDs found for {args.family_slug}")
        sys.exit(1)
    print(f"Family: {args.family_slug} ({method_count} methods)")

    run_results = run_benchmark(str(exe_path), method_count, iterations=args.iterations)

    passes = sum(1 for r in run_results if "elapsedMilliseconds" in r)
    fails = sum(1 for r in run_results if "error" in r)
    trace("benchmark_done", family=args.family_slug, passed=passes, failed=fails, total=len(run_results))
    print(f"\n{'='*50}")
    print(f"Results: {passes} passed, {fails} failed, {len(run_results)} total")

    output = {"family": args.family_slug, "iterations": args.iterations, "results": run_results}
    print(f"\nJSON summary:")
    print(json.dumps(output, indent=2))

    if fails > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()