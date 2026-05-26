"""3-way benchmark comparison: managed (.NET JIT) vs native-aot vs native-jit.

Loads managed-baseline.json, native-aot-benchmark.json, and native-jit-benchmark.json,
computes per-method speedup % for both native paths, emits benchmark-comparison-report.json.

Schema v2:
  - methodResults[].nativeAotElapsedMs / nativeJitElapsedMs
  - methodResults[].nativeAotSpeedupPercent / nativeJitSpeedupPercent
  - summary.averageNativeAotSpeedupPercent / averageNativeJitSpeedupPercent
  - summary.nativeJitSlowdownFactor

Usage:
    python benchmark_comparator.py \\
        --managed managed-baseline.json \\
        --aot native-aot-benchmark.json \\
        --jit native-jit-benchmark.json \\
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


def _build_subject_lookup(results: list[dict]) -> dict[str, dict[str, Any]]:
    lookup: dict[str, dict[str, Any]] = {}
    for r in results:
        sid = r.get("methodSubjectId", "")
        if sid:
            lookup[sid] = r
    return lookup


def compare(
    managed_path: Path,
    aot_path: Path,
    jit_path: Path | None,
    output_path: Path,
) -> dict[str, Any]:
    """3-way comparison: managed vs native-aot vs native-jit.

    Args:
        managed_path: Path to managed-baseline.json (.NET JIT results)
        aot_path: Path to native-aot-benchmark.json (chaos AOT results)
        jit_path: Path to native-jit-benchmark.json (chaos interpreter results), optional
        output_path: Path for benchmark-comparison-report.json
    """
    managed_data = _load_json(managed_path)
    aot_data = _load_json(aot_path)

    managed_results: list[dict] = managed_data.get("results", [])
    aot_results: list[dict] = aot_data.get("results", [])

    managed_by_subject = _build_subject_lookup(managed_results)
    aot_by_subject = _build_subject_lookup(aot_results)

    # Load JIT data if available
    jit_by_subject: dict[str, dict] = {}
    if jit_path and jit_path.exists():
        try:
            jit_data = json.loads(jit_path.read_bytes())
            jit_by_subject = _build_subject_lookup(jit_data.get("results", []))
        except (json.JSONDecodeError, OSError):
            print(f"  WARNING: native-jit-benchmark.json unreadable, JIT column omitted")

    all_subjects = sorted(
        set(managed_by_subject.keys()) | set(aot_by_subject.keys()) | set(jit_by_subject.keys())
    )

    method_results: list[dict[str, Any]] = []
    native_aot_faster = 0
    native_jit_faster = 0
    managed_faster = 0
    equal_count = 0
    unmatched_count = 0
    invalid_count = 0
    jit_elided_count = 0
    total_aot_speedup = 0.0
    total_jit_speedup = 0.0
    matched_count = 0

    for sid in all_subjects:
        managed_entry = managed_by_subject.get(sid)
        aot_entry = aot_by_subject.get(sid)
        jit_entry = jit_by_subject.get(sid)

        # Unmatched: missing managed or both native results
        if managed_entry is None or (aot_entry is None and jit_entry is None):
            method_results.append({
                "methodSubjectId": sid,
                "status": "unmatched",
                "managedElapsedMs": managed_entry.get("elapsedMilliseconds") if managed_entry else None,
                "nativeAotElapsedMs": aot_entry.get("elapsedMilliseconds") if aot_entry else None,
                "nativeJitElapsedMs": jit_entry.get("elapsedMilliseconds") if jit_entry else None,
            })
            unmatched_count += 1
            continue

        managed_ms = managed_entry.get("elapsedMilliseconds", 0.0)

        # AOT time: prefer calibratedMs if available
        aot_calibrated = aot_entry.get("calibratedMs") if aot_entry else None
        aot_ms = aot_calibrated if aot_calibrated is not None and aot_calibrated >= 0 else (
            aot_entry.get("elapsedMilliseconds", -1.0) if aot_entry else -1.0
        )

        # JIT time
        jit_calibrated = jit_entry.get("calibratedMs") if jit_entry else None
        jit_ms = jit_calibrated if jit_calibrated is not None and jit_calibrated >= 0 else (
            jit_entry.get("elapsedMilliseconds", -1.0) if jit_entry else -1.0
        )

        # Determine validity
        managed_body_real = managed_entry.get("isBodyReal", True)
        managed_has_exception = managed_entry.get("isException", False)
        aot_status = aot_entry.get("status", "completed") if aot_entry else "missing"
        jit_status = jit_entry.get("status", "completed") if jit_entry else "missing"

        is_invalid = (
            not managed_body_real
            or managed_has_exception
            or managed_ms <= 0.0
            or (aot_ms <= 0.0 and jit_ms <= 0.0)
        )
        if is_invalid:
            invalid_count += 1
            method_results.append({
                "methodSubjectId": sid,
                "status": "invalid",
                "managedElapsedMs": managed_ms,
                "nativeAotElapsedMs": aot_ms if aot_ms > 0 else None,
                "nativeJitElapsedMs": jit_ms if jit_ms > 0 else None,
                "nativeAotStatus": aot_status,
                "nativeJitStatus": jit_status,
                "nativeAotSpeedupPercent": None,
                "nativeJitSpeedupPercent": None,
            })
            continue

        # JIT elision detection: if managed ns/op < 1.0, JIT optimized the call away
        managed_ns_per_op = (managed_ms * 1_000_000) / max(managed_entry.get("iterations", 1), 1)
        is_jit_elided = managed_ns_per_op < 1.0

        if is_jit_elided:
            jit_elided_count += 1
            method_results.append({
                "methodSubjectId": sid,
                "status": "jit_elided",
                "managedElapsedMs": managed_ms,
                "nativeAotElapsedMs": aot_ms if aot_ms > 0 else None,
                "nativeJitElapsedMs": jit_ms if jit_ms > 0 else None,
                "nativeAotSpeedupPercent": None,
                "nativeJitSpeedupPercent": None,
                "jitNote": f"Managed ns/op ({managed_ns_per_op:.2f}) below physical minimum",
            })
            continue

        # Compute speedups
        aot_speedup = ((managed_ms - aot_ms) / managed_ms) * 100.0 if aot_ms > 0 else None
        jit_speedup = ((managed_ms - jit_ms) / managed_ms) * 100.0 if jit_ms > 0 else None

        matched_count += 1
        if aot_speedup is not None:
            total_aot_speedup += aot_speedup
            if aot_speedup > 1.0:
                native_aot_faster += 1
        if jit_speedup is not None:
            total_jit_speedup += jit_speedup
            if jit_speedup > 1.0:
                native_jit_faster += 1

        # "managed_faster" counts methods where BOTH native paths are slower
        aot_is_slower = aot_speedup is not None and aot_speedup < -1.0
        jit_is_slower = jit_speedup is not None and jit_speedup < -1.0
        if aot_is_slower and (jit_is_slower or jit_speedup is None):
            managed_faster += 1

        entry: dict[str, Any] = {
            "methodSubjectId": sid,
            "status": "matched",
            "managedElapsedMs": round(managed_ms, 4),
            "nativeAotElapsedMs": round(aot_ms, 4) if aot_ms > 0 else None,
            "nativeJitElapsedMs": round(jit_ms, 4) if jit_ms > 0 else None,
            "nativeAotSpeedupPercent": round(aot_speedup, 2) if aot_speedup is not None else None,
            "nativeJitSpeedupPercent": round(jit_speedup, 2) if jit_speedup is not None else None,
        }
        method_results.append(entry)

    average_aot = round(total_aot_speedup / matched_count, 2) if matched_count > 0 else 0.0
    average_jit = round(total_jit_speedup / matched_count, 2) if matched_count > 0 else None

    # slowdownFactor = jit_ms / aot_ms (how many times slower interpreter is vs AOT)
    # Computed from all matched methods that have both AOT and JIT data
    jit_vs_aot_ratios: list[float] = []
    for mr in method_results:
        if mr.get("status") == "matched":
            aot_ms = mr.get("nativeAotElapsedMs")
            jit_ms = mr.get("nativeJitElapsedMs")
            if aot_ms and jit_ms and aot_ms > 0:
                jit_vs_aot_ratios.append(jit_ms / aot_ms)
    jit_slowdown = round(sum(jit_vs_aot_ratios) / len(jit_vs_aot_ratios), 2) if jit_vs_aot_ratios else None

    report = {
        "schemaVersion": 2,
        "assemblyName": managed_data.get("assemblyName", aot_data.get("assemblyName", "")),
        "familyId": managed_data.get("familyId", aot_data.get("familyId", "")),
        "summary": {
            "totalMethods": len(all_subjects),
            "matchedCount": matched_count,
            "unmatchedCount": unmatched_count,
            "invalidCount": invalid_count,
            "jitElidedCount": jit_elided_count,
            "nativeAotFasterCount": native_aot_faster,
            "nativeJitFasterCount": native_jit_faster,
            "managedFasterCount": managed_faster,
            "equalCount": equal_count,
            "averageSpeedupPercent": average_aot,
            "averageNativeAotSpeedupPercent": average_aot,
            "averageNativeJitSpeedupPercent": average_jit,
            "nativeJitSlowdownFactor": jit_slowdown,
        },
        "methodResults": method_results,
    }

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)
        f.write("\n")

    print(f"Benchmark comparison report written to {output_path}")
    print(f"  Total: {len(all_subjects)}, Matched: {matched_count}")
    print(f"  Native-AOT faster: {native_aot_faster}, Native-JIT faster: {native_jit_faster}, Managed faster: {managed_faster}")
    print(f"  Avg AOT speedup: {average_aot}%", end="")
    if average_jit is not None:
        print(f", Avg JIT speedup: {average_jit}%", end="")
    if jit_slowdown is not None:
        print(f", JIT slowdown factor: {jit_slowdown}x", end="")
    print()

    return report


def main() -> None:
    parser = argparse.ArgumentParser(description="3-way benchmark comparison")
    parser.add_argument("--managed", required=True, type=Path, help="Path to managed-baseline.json")
    parser.add_argument("--aot", required=True, type=Path, help="Path to native-aot-benchmark.json")
    parser.add_argument("--jit", type=Path, default=None, help="Path to native-jit-benchmark.json")
    parser.add_argument("--output", required=True, type=Path, help="Path for benchmark-comparison-report.json")
    args = parser.parse_args()

    compare(
        managed_path=args.managed.resolve(),
        aot_path=args.aot.resolve(),
        jit_path=args.jit.resolve() if args.jit else None,
        output_path=args.output.resolve(),
    )


if __name__ == "__main__":
    main()
