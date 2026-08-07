"""Value checker — fact value correctness and cross-technology comparison.

Compares AOT fact result values against:
  1. Expected values from metadata (if available)
  2. .NET 8 managed fact values (cross-tech comparison)

Uses configurable float tolerance to avoid floating-point false positives.
"""
from __future__ import annotations

from typing import Any

_DEFAULT_TOLERANCE = 0.01


def values_match(a: Any, b: Any, tolerance: float = _DEFAULT_TOLERANCE) -> bool:
    """Check if two values match, with float tolerance.

    Handles:
    - Exact equality (int, bool, None, string)
    - Float comparison with relative tolerance
    - None matches None only
    - Type mismatch = no match
    """
    if a is None and b is None:
        return True
    if a is None or b is None:
        return False

    if type(a) != type(b):
        # float vs int is OK
        if isinstance(a, (int, float)) and isinstance(b, (int, float)):
            a = float(a)
            b = float(b)
        else:
            return False

    if isinstance(a, float):
        if a == b:
            return True
        if b == 0.0:
            return abs(a) < tolerance
        return abs(a - b) / max(abs(b), 1e-10) < tolerance

    if isinstance(a, int):
        return a == b

    if isinstance(a, bool):
        return a == b

    return a == b


def check_method_values(
    aot_results: list[dict],
    metadata_methods: list[dict],
    managed_results: list[dict] | None = None,
    tolerance: float = _DEFAULT_TOLERANCE,
) -> dict[str, Any]:
    """Compare AOT fact values against expected and .NET 8 managed values.

    Args:
        aot_results: Per-method fact results from _run_single_fact.
        metadata_methods: Subjects metadata method list (with value fields).
        managed_results: Per-method managed (.NET 8) fact results (optional).
        tolerance: Float comparison tolerance.

    Returns:
        dict with keys:
            valueCheckCount: Number of methods with value data.
            valueMatchCount: Number of methods with matching values.
            valueMismatchCount: Number of methods with mismatching values.
            crossTechMatchCount: Number of cross-tech comparisons done.
            crossTechMatchRate: Percentage of cross-tech matches.
            mismatches: List of mismatched method subject IDs (max 20).
    """
    # Build metadata lookup: si → expected_value
    expected_values: dict[int, tuple[str, Any]] = {}
    for i, m in enumerate(metadata_methods):
        subject_id = m.get("methodSubjectId", "")
        expected = m.get("expectedValue")
        if expected is not None:
            expected_values[i] = (subject_id, expected)

    # Build managed lookup: methodSubjectId → value
    managed_values: dict[str, Any] = {}
    if managed_results:
        for r in managed_results:
            sid = r.get("methodSubjectId", "")
            val = r.get("value")
            if sid and val is not None:
                managed_values[sid] = val

    mismatches: list[dict] = []
    value_check_count = 0
    value_match_count = 0
    cross_tech_count = 0
    cross_tech_match = 0

    for r in aot_results:
        si = r.get("si", -1)
        aot_val = r.get("value")
        subject_id = r.get("methodSubjectId", "")

        # 1. Compare against expected value from metadata
        if si in expected_values:
            expected_sid, expected_val = expected_values[si]
            value_check_count += 1
            if values_match(aot_val, expected_val, tolerance):
                value_match_count += 1
            else:
                mismatches.append({
                    "methodSubjectId": subject_id or expected_sid,
                    "si": si,
                    "aotValue": aot_val,
                    "expectedValue": expected_val,
                    "type": "expected_value_mismatch",
                })

        # 2. Cross-tech: compare AOT value vs .NET 8 managed value
        if subject_id and subject_id in managed_values:
            cross_tech_count += 1
            managed_val = managed_values[subject_id]
            if values_match(aot_val, managed_val, tolerance):
                cross_tech_match += 1
            else:
                mismatches.append({
                    "methodSubjectId": subject_id,
                    "si": si,
                    "aotValue": aot_val,
                    "managedValue": managed_val,
                    "type": "cross_tech_mismatch",
                })

    return {
        "valueCheckCount": value_check_count,
        "valueMatchCount": value_match_count,
        "valueMatchRate": round(value_match_count / value_check_count * 100, 1) if value_check_count > 0 else None,
        "valueMismatchCount": len([m for m in mismatches if m["type"] == "expected_value_mismatch"]),
        "crossTechCheckCount": cross_tech_count,
        "crossTechMatchCount": cross_tech_match,
        "crossTechMatchRate": round(cross_tech_match / cross_tech_count * 100, 1) if cross_tech_count > 0 else None,
        "mismatches": mismatches[:20],
        "totalMismatches": len(mismatches),
    }
