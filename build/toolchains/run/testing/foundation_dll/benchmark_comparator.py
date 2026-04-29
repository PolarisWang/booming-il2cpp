"""Load managed-baseline.json and native-benchmark.json, compute per-method
speedup %, and emit benchmark-comparison-report.json.

Usage:
    python benchmark_comparator.py \\
        --managed managed-baseline.json \\
        --native native-benchmark.json \\
        --output benchmark-comparison-report.json
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


def _load_json(path: Path) -> dict[str, Any]:
    if not path.exists():
        print(f"ERROR: {path} not found", file=sys.stderr)
        sys.exit(1)
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def _compute_speedup_pct(managed_ms: float, native_ms: float) -> tuple[float, str]:
    """Positive = native is faster; negative = managed is faster.

    Returns (speedup_pct, status) where status is one of:
    "matched", "invalid".
    """
    if managed_ms <= 0.0 or native_ms <= 0.0:
        return 0.0, "invalid"
    return ((managed_ms - native_ms) / managed_ms) * 100.0, "matched"


def compare(
    managed_path: Path,
    native_path: Path,
    output_path: Path,
) -> dict[str, Any]:
    managed_data = _load_json(managed_path)
    native_data = _load_json(native_path)

    managed_results: list[dict[str, Any]] = managed_data.get("results", [])
    native_results: list[dict[str, Any]] = native_data.get("results", [])

    # Build lookup by methodSubjectId
    managed_by_subject: dict[str, dict[str, Any]] = {}
    for r in managed_results:
        sid = r.get("methodSubjectId", "")
        if sid:
            managed_by_subject[sid] = r

    native_by_subject: dict[str, dict[str, Any]] = {}
    for r in native_results:
        sid = r.get("methodSubjectId", "")
        if sid:
            native_by_subject[sid] = r

    all_subjects = sorted(set(managed_by_subject.keys()) | set(native_by_subject.keys()))

    method_results: list[dict[str, Any]] = []
    native_faster_count = 0
    managed_faster_count = 0
    equal_count = 0
    unmatched_count = 0
    invalid_count = 0
    total_speedup = 0.0
    matched_count = 0

    for sid in all_subjects:
        managed_entry = managed_by_subject.get(sid)
        native_entry = native_by_subject.get(sid)

        if managed_entry is None or native_entry is None:
            method_results.append({
                "methodSubjectId": sid,
                "status": "unmatched",
                "managedElapsedMs": managed_entry.get("elapsedMilliseconds") if managed_entry else None,
                "nativeElapsedMs": native_entry.get("elapsedMilliseconds") if native_entry else None,
            })
            unmatched_count += 1
            continue

        managed_ms = managed_entry.get("elapsedMilliseconds", 0.0)
        native_ms = native_entry.get("elapsedMilliseconds", 0.0)
        speedup_pct, status = _compute_speedup_pct(managed_ms, native_ms)

        if status == "invalid":
            invalid_count += 1
        elif speedup_pct > 1.0:
            native_faster_count += 1
        elif speedup_pct < -1.0:
            managed_faster_count += 1
        else:
            equal_count += 1

        if status == "matched":
            matched_count += 1
            total_speedup += speedup_pct

        method_results.append({
            "methodSubjectId": sid,
            "status": status,
            "managedElapsedMs": managed_ms,
            "nativeElapsedMs": native_ms,
            "speedupPercent": round(speedup_pct, 2) if status == "matched" else None,
        })

    average_speedup = round(total_speedup / matched_count, 2) if matched_count > 0 else 0.0

    report = {
        "schemaVersion": 1,
        "assemblyName": managed_data.get("assemblyName", native_data.get("assemblyName", "")),
        "familyId": managed_data.get("familyId", native_data.get("familyId", "")),
        "summary": {
            "totalMethods": len(all_subjects),
            "matchedCount": matched_count,
            "unmatchedCount": unmatched_count,
            "invalidCount": invalid_count,
            "nativeFasterCount": native_faster_count,
            "managedFasterCount": managed_faster_count,
            "equalCount": equal_count,
            "averageSpeedupPercent": average_speedup,
        },
        "methodResults": method_results,
    }

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)
        f.write("\n")

    print(f"Benchmark comparison report written to {output_path}")
    print(f"  Total: {len(all_subjects)}, Matched: {matched_count}, Unmatched: {unmatched_count}")
    print(f"  Native faster: {native_faster_count}, Managed faster: {managed_faster_count}, Equal: {equal_count}")
    print(f"  Average speedup: {average_speedup}%")

    return report


def main() -> None:
    parser = argparse.ArgumentParser(description="Compare managed vs native benchmark results")
    parser.add_argument("--managed", required=True, type=Path, help="Path to managed-baseline.json")
    parser.add_argument("--native", required=True, type=Path, help="Path to native-benchmark.json")
    parser.add_argument("--output", required=True, type=Path, help="Path for benchmark-comparison-report.json")
    args = parser.parse_args()

    compare(
        managed_path=args.managed.resolve(),
        native_path=args.native.resolve(),
        output_path=args.output.resolve(),
    )


if __name__ == "__main__":
    main()
