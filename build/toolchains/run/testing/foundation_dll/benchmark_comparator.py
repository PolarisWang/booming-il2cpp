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
    jit_elided_count = 0
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
        # Use calibrated native time (dispatch overhead subtracted) when available
        native_calibrated = native_entry.get("calibratedMs")
        native_ms = native_calibrated if native_calibrated is not None and native_calibrated >= 0 else native_entry.get("elapsedMilliseconds", 0.0)

        # invalid if: managed body is empty, native is stub, or elapsed is invalid
        managed_body_real = managed_entry.get("isBodyReal", True)
        managed_has_exception = managed_entry.get("isException", False)
        native_status = native_entry.get("status", "completed")
        is_invalid = (
            not managed_body_real
            or managed_has_exception
            or native_status == "stub"
            or native_status == "throws"
            or managed_ms <= 0.0
            or native_ms <= 0.0
        )

        # JIT elision detection: if managed took fewer than 1 ns per iteration,
        # the JIT almost certainly optimized the call away. Mark as jit_elided
        # and exclude from average speedup computation.
        managed_ns_per_op = (managed_ms * 1_000_000) / max(managed_entry.get("iterations", 1), 1)
        is_jit_elided = (
            not is_invalid
            and not managed_has_exception
            and managed_ns_per_op < 1.0
        )

        if is_invalid:
            invalid_count += 1
            method_results.append({
                "methodSubjectId": sid,
                "status": "invalid",
                "managedElapsedMs": managed_ms,
                "nativeElapsedMs": native_ms,
                "speedupPercent": None,
            })
            continue

        if is_jit_elided:
            jit_elided_count += 1
            method_results.append({
                "methodSubjectId": sid,
                "status": "jit_elided",
                "managedElapsedMs": managed_ms,
                "nativeElapsedMs": native_ms,
                "speedupPercent": None,
                "jitNote": f"Managed ns/op ({managed_ns_per_op:.2f}) below physical minimum — JIT elided the call",
            })
            continue

        speedup_pct = ((managed_ms - native_ms) / managed_ms) * 100.0
        matched_count += 1
        total_speedup += speedup_pct

        if speedup_pct > 1.0:
            native_faster_count += 1
        elif speedup_pct < -1.0:
            managed_faster_count += 1
        else:
            equal_count += 1

        method_results.append({
            "methodSubjectId": sid,
            "status": "matched",
            "managedElapsedMs": managed_ms,
            "nativeElapsedMs": native_ms,
            "speedupPercent": round(speedup_pct, 2),
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
            "jitElidedCount": jit_elided_count,
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
