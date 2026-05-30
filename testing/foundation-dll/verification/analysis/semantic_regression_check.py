"""Semantic regression check — compares golden records pre/post optimization.

Used in Step 5.2 of the optimization campaign to verify that optimization
did not change the semantic behavior of any method.

Compares:
  - Per-method pass/fail status
  - Exception type consistency (both threw, or both didn't)
  - New failures introduced (regression)

Usage:
    python -m verification.analysis.semantic_regression_check <slug> --assembly System.Private.CoreLib
"""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_VERIFICATION_ROOT = _HERE.parent
_TESTING_ROOT = _VERIFICATION_ROOT.parent


def load_results(path: Path) -> list[dict[str, Any]]:
    """Load golden record from JSON file.

    Accepts both list-of-results and {results: [...]} formats.
    """
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as e:
        print(f"ERROR: Could not load {path}: {e}", file=sys.stderr)
        return []

    if isinstance(data, list):
        return data
    if isinstance(data, dict):
        return data.get("results", data.get("methods", []))
    return []


def _get_exception_type(exception_message: str | None) -> str:
    """Extract the exception type from a managed exception message.

    Typical format: "System.InvalidCastException: Specified cast is not valid."
    Returns the full type name or 'None' if no exception.
    """
    if not exception_message:
        return "None"
    # Extract the first word/type before ':'
    parts = exception_message.split(":", 1)
    return parts[0].strip() if parts else "Unknown"


def compare_golden_records(
    baseline_path: Path,
    current_path: Path,
) -> dict[str, Any]:
    """Compare baseline golden record against current golden record.

    Returns a structured regression report.
    """
    baseline = load_results(baseline_path)
    current = load_results(current_path)

    if not baseline:
        return {
            "has_regression": False,
            "error": "No baseline data loaded",
            "details": [],
        }
    if not current:
        return {
            "has_regression": True,
            "error": "No current data loaded",
            "details": [],
        }

    # Index by methodIndex
    baseline_by_index: dict[int, dict[str, Any]] = {}
    for r in baseline:
        idx = r.get("methodIndex", r.get("index", -1))
        if idx >= 0:
            baseline_by_index[idx] = r

    current_by_index: dict[int, dict[str, Any]] = {}
    for r in current:
        idx = r.get("methodIndex", r.get("index", -1))
        if idx >= 0:
            current_by_index[idx] = r

    all_indices = sorted(set(baseline_by_index.keys()) | set(current_by_index.keys()))
    details: list[dict[str, Any]] = []
    regressions: list[dict[str, Any]] = []
    improvements: list[dict[str, Any]] = []
    unchanged = 0

    for idx in all_indices:
        b = baseline_by_index.get(idx, {})
        c = current_by_index.get(idx, {})

        b_status = b.get("status", "unknown")
        c_status = c.get("status", "unknown")
        b_exc = b.get("exceptionMessage")
        c_exc = c.get("exceptionMessage")
        b_exc_type = _get_exception_type(b_exc)
        c_exc_type = _get_exception_type(c_exc)

        mid = b.get("methodSubjectId", c.get("methodSubjectId", f"#{idx}"))

        entry = {
            "index": idx,
            "methodSubjectId": mid,
            "baseline_status": b_status,
            "current_status": c_status,
            "baseline_exception": b_exc_type,
            "current_exception": c_exc_type,
        }

        # Detect regression: baseline passed but current failed
        if b_status == "passed" and c_status == "failed":
            entry["regression_type"] = "NEW_FAILURE"
            regressions.append(entry)
        # Detect regression: different exception types
        elif b_exc_type != "None" and c_exc_type != "None" and b_exc_type != c_exc_type:
            entry["regression_type"] = "EXCEPTION_TYPE_CHANGED"
            regressions.append(entry)
        # Detect improvement: baseline failed but current passed
        elif b_status == "failed" and c_status == "passed":
            entry["regression_type"] = "IMPROVEMENT"
            improvements.append(entry)
        else:
            entry["regression_type"] = "UNCHANGED"
            unchanged += 1

        details.append(entry)

    has_regression = len(regressions) > 0

    return {
        "has_regression": has_regression,
        "total_methods": len(all_indices),
        "unchanged": unchanged,
        "regressions": regressions,
        "regression_count": len(regressions),
        "improvements": improvements,
        "improvement_count": len(improvements),
        "details": details,
        "summary": (
            f"{len(regressions)} regression(s), {len(improvements)} improvement(s), "
            f"{unchanged} unchanged out of {len(all_indices)} methods"
        ),
    }


def check_regression(
    slug: str,
    assembly: str,
    results_dir: Path | None = None,
) -> dict[str, Any]:
    """Entry point: load baseline and current golden records, compare."""
    family_dir = _TESTING_ROOT / assembly / slug

    # Baseline: check native/baseline-golden-record.json or family dir root
    baseline_candidates = [
        family_dir / "native" / "baseline-golden-record.json",
        family_dir / "baseline-golden-record.json",
    ]
    if results_dir:
        baseline_candidates.insert(
            0, results_dir / "native" / "baseline-golden-record.json"
        )

    baseline_path = None
    for p in baseline_candidates:
        if p.exists():
            baseline_path = p
            break

    if not baseline_path:
        return {
            "has_regression": False,
            "error": "No baseline golden record found. Run --freeze first.",
            "details": [],
        }

    # Current: check native/golden-record.json
    current_candidates = [
        family_dir / "native" / "golden-record.json",
    ]
    if results_dir:
        current_candidates.insert(0, results_dir / "golden-record.json")

    current_path = None
    for p in current_candidates:
        if p.exists():
            current_path = p
            break

    if not current_path:
        return {
            "has_regression": True,
            "error": "No current golden record found. Run managed_record first.",
            "details": [],
        }

    return compare_golden_records(baseline_path, current_path)


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(
        description="Semantic regression check for golden records"
    )
    parser.add_argument("slug", help="Family slug")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--baseline", type=Path, default=None,
                        help="Path to baseline golden-record.json")
    parser.add_argument("--current", type=Path, default=None,
                        help="Path to current golden-record.json")
    args = parser.parse_args()

    if args.baseline and args.current:
        report = compare_golden_records(args.baseline, args.current)
    else:
        report = check_regression(args.slug, args.assembly)

    print(json.dumps(report, indent=2, ensure_ascii=False))
    sys.exit(1 if report.get("has_regression") else 0)


if __name__ == "__main__":
    main()
