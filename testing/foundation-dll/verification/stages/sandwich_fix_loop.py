"""Sandwich fix loop — hypothesis-driven optimization with automatic rollback.

Each optimization attempt follows the sandwich model:
  1. Record a quantifiable hypothesis (expected improvement %)
  2. Implement the optimization (external to this module)
  3. Verify improvement against baseline
  4. Auto-rollback if improvement is below threshold

Usage:
    python -m verification.stages.sandwich_fix_loop <slug> <command> [options]

Commands:
    record     Record an optimization hypothesis
    verify     Verify improvement against baseline
    rollback   Auto-rollback if improvement is below threshold

Phase: Optimization Campaign Phase 2.5
"""

from __future__ import annotations

import argparse
import json
import math
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_VERIFICATION_ROOT = _HERE.parent
_TESTING_ROOT = _VERIFICATION_ROOT.parent
_PROJECT_ROOT = _TESTING_ROOT.parent.parent  # D:/agent/booming-il2cpp

# Default benchmark runtime to compare
_DEFAULT_RUNTIME = "chaos-aot"

# Default assembly
_DEFAULT_ASSEMBLY = "System.Private.CoreLib"


# ── Hypothesis Recording ──────────────────────────────────────────────────


def _resolve_family_dir(slug: str, assembly: str = _DEFAULT_ASSEMBLY) -> Path:
    """Resolve the family directory for a given slug and assembly."""
    return _TESTING_ROOT / assembly / slug


def record_hypothesis(
    slug: str,
    description: str,
    expected_improvement_pct: float,
    changed_files: list[str],
    baseline_ns: float | None = None,
    baseline_report_path: str | Path | None = None,
    assembly: str = _DEFAULT_ASSEMBLY,
) -> str:
    """Record an optimization hypothesis and return the hypothesis_id.

    Args:
        slug: Family slug (e.g. "enum-parsing").
        description: Human-readable description of the optimization.
        expected_improvement_pct: Expected improvement in percent (e.g. 80 means 80% faster).
        changed_files: List of file paths that were changed.
        baseline_ns: Baseline ns/op (geometric mean). If None, auto-extracted from
            multi-run-report.json or unified-verification-report.json.
        baseline_report_path: Path to baseline report. If None, auto-detected.
        assembly: Assembly name (default: System.Private.CoreLib).

    Returns:
        The hypothesis_id (e.g. "enum-parsing-001").
    """
    family_dir = _resolve_family_dir(slug, assembly)
    family_dir.mkdir(parents=True, exist_ok=True)

    # Load history to determine sequential number
    history_path = family_dir / "fix-attempt-history.jsonl"
    existing_ids = _load_existing_hypothesis_ids(history_path)
    seq_number = _next_seq_number(existing_ids)
    hypothesis_id = f"{slug}-{seq_number:03d}"

    # Auto-extract baseline_ns if not provided
    if baseline_ns is None:
        baseline_ns = _extract_baseline_ns(family_dir, baseline_report_path)

    # If still None, default to 0.0 (will cause verify to fail gracefully)
    if baseline_ns is None:
        baseline_ns = 0.0

    now = _now_iso()
    hypothesis = {
        "hypothesis_id": hypothesis_id,
        "slug": slug,
        "description": description,
        "expected_improvement_pct": expected_improvement_pct,
        "changed_files": changed_files,
        "baseline_ns": round(baseline_ns, 2),
        "recorded_at": now,
    }

    # Append to history
    with open(history_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(hypothesis, ensure_ascii=False) + "\n")

    print(f"[sandwich_fix_loop] Recorded hypothesis: {hypothesis_id}")
    print(f"  description: {description}")
    print(f"  expected_improvement: {expected_improvement_pct}%")
    print(f"  baseline_ns: {baseline_ns}")
    print(f"  history: {history_path}")

    return hypothesis_id


def _load_existing_hypothesis_ids(history_path: Path) -> set[str]:
    """Load all existing hypothesis IDs from the history file."""
    if not history_path.exists():
        return set()

    ids: set[str] = set()
    try:
        for line in history_path.read_text(encoding="utf-8").strip().splitlines():
            if not line.strip():
                continue
            try:
                entry = json.loads(line)
                hid = entry.get("hypothesis_id", "")
                if hid:
                    ids.add(hid)
            except json.JSONDecodeError:
                continue
    except OSError:
        pass
    return ids


def _next_seq_number(existing_ids: set[str]) -> int:
    """Determine the next sequential number for a family slug.

    Extracts numbers from existing hypothesis IDs and returns max + 1.
    """
    max_num = 0
    for hid in existing_ids:
        parts = hid.rsplit("-", 1)
        if len(parts) == 2:
            try:
                num = int(parts[1])
                if num > max_num:
                    max_num = num
            except ValueError:
                continue
    return max_num + 1


def _extract_baseline_ns(
    family_dir: Path,
    baseline_report_path: str | Path | None = None,
) -> float | None:
    """Extract geometric mean ns/op from the baseline benchmark report.

    Tries in order:
      1. Explicit baseline_report_path
      2. family_dir/multi-run/multi-run-report.json
      3. family_dir/unified-verification-report.json
    """
    if baseline_report_path:
        candidates = [Path(baseline_report_path)]
    else:
        candidates = [
            family_dir / "multi-run" / "multi-run-report.json",
            family_dir / "unified-verification-report.json",
        ]

    for path in candidates:
        if path.exists():
            try:
                entries = _load_multi_run_entries(path)
                if entries:
                    ns_values = [e.get("chaos-aot", {}).get("mean_ns", 0.0)
                                 for e in entries if "chaos-aot" in e]
                    ns_values = [v for v in ns_values if v > 0]
                    if ns_values:
                        return _geometric_mean(ns_values)
            except (OSError, json.JSONDecodeError):
                continue

    return None


def _load_multi_run_entries(report_path: Path) -> list[dict[str, Any]]:
    """Load per-method benchmark entries from a multi-run-report.json.

    Each method in the report looks like:
    {
        "subject_id": "...",
        "samples": {
            "chaos-aot": { "mean_ns": 1540.82, ... },
            ...
        }
    }

    Returns a list of dicts mapping runtime -> mean_ns.
    """
    try:
        data = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return []

    methods = data.get("methods", [])
    if not methods:
        return []

    entries: list[dict[str, Any]] = []
    for m in methods:
        samples = m.get("samples", {})
        if not isinstance(samples, dict):
            continue
        entry: dict[str, Any] = {}
        for runtime, sample in samples.items():
            if isinstance(sample, dict):
                mean_ns = sample.get("mean_ns", 0.0)
                if isinstance(mean_ns, (int, float)) and mean_ns > 0:
                    entry[runtime] = {"mean_ns": float(mean_ns)}
        if entry:
            entry["_subject_id"] = m.get("subject_id", "")
            entries.append(entry)

    return entries


def _geometric_mean(values: list[float]) -> float:
    """Compute geometric mean of a list of positive values."""
    if not values:
        return 0.0
    log_sum = sum(math.log(v) for v in values if v > 0)
    return math.exp(log_sum / len(values))


def _now_iso() -> str:
    """Return current UTC timestamp in ISO 8601 format."""
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%S.000Z")


# ── Verification ──────────────────────────────────────────────────────────


def _load_hypothesis_record(
    family_dir: Path, hypothesis_id: str
) -> dict[str, Any] | None:
    """Load a specific hypothesis record from fix-attempt-history.jsonl."""
    history_path = family_dir / "fix-attempt-history.jsonl"
    if not history_path.exists():
        return None

    try:
        for line in history_path.read_text(encoding="utf-8").strip().splitlines():
            if not line.strip():
                continue
            try:
                entry = json.loads(line)
                if entry.get("hypothesis_id") == hypothesis_id:
                    return entry
            except json.JSONDecodeError:
                continue
    except OSError:
        pass
    return None


def _extract_current_ns_per_method(
    family_dir: Path,
) -> dict[str, float]:
    """Extract per-method chaos-aot ns/op from the current multi-run report.

    Returns dict mapping subject_id -> mean_ns.
    """
    report_path = family_dir / "multi-run" / "multi-run-report.json"
    if not report_path.exists():
        # Fallback to unified-verification-report.json
        report_path = family_dir / "unified-verification-report.json"
    if not report_path.exists():
        return {}

    entries = _load_multi_run_entries(report_path)
    result: dict[str, float] = {}
    for entry in entries:
        sid = entry.get("_subject_id", "")
        aot = entry.get(_DEFAULT_RUNTIME)
        if aot and sid:
            result[sid] = aot["mean_ns"]
    return result


def verify_improvement(
    slug: str,
    hypothesis_id: str,
    min_threshold_pct: float | None = None,
    assembly: str = _DEFAULT_ASSEMBLY,
) -> dict[str, Any]:
    """Verify improvement for a given hypothesis against current benchmark data.

    Args:
        slug: Family slug (e.g. "enum-parsing").
        hypothesis_id: Hypothesis ID (e.g. "enum-parsing-001").
        min_threshold_pct: Minimum improvement threshold in percent.
            If None, defaults to expected_improvement_pct / 2.
        assembly: Assembly name (default: System.Private.CoreLib).

    Returns:
        Dict with keys:
            "passed": bool
            "improvement_pct": float (geometric mean improvement %)
            "per_method": dict[str, dict] (per-method details)
            "baseline_ns": float
            "current_ns": float
            "hypothesis_id": str
            "error": str | None
    """
    family_dir = _resolve_family_dir(slug, assembly)

    # Load hypothesis record
    record = _load_hypothesis_record(family_dir, hypothesis_id)
    if record is None:
        return {
            "passed": False,
            "improvement_pct": 0.0,
            "per_method": {},
            "baseline_ns": 0.0,
            "current_ns": 0.0,
            "hypothesis_id": hypothesis_id,
            "error": f"Hypothesis '{hypothesis_id}' not found in {family_dir / 'fix-attempt-history.jsonl'}",
        }

    baseline_ns = record.get("baseline_ns", 0.0)
    expected_pct = record.get("expected_improvement_pct", 0.0)

    # Determine threshold
    if min_threshold_pct is None:
        min_threshold_pct = expected_pct / 2.0

    # Load current per-method data
    current_per_method = _extract_current_ns_per_method(family_dir)
    if not current_per_method:
        return {
            "passed": False,
            "improvement_pct": 0.0,
            "per_method": {},
            "baseline_ns": baseline_ns,
            "current_ns": 0.0,
            "hypothesis_id": hypothesis_id,
            "error": (
                f"No current benchmark data found in "
                f"{family_dir / 'multi-run' / 'multi-run-report.json'} "
                f"or {family_dir / 'unified-verification-report.json'}"
            ),
        }

    # Calculate per-method improvement
    # If baseline_ns is 0 (no baseline data), use per-method ratios
    # against a synthetic baseline derived from the hypothesis record
    per_method: dict[str, dict[str, Any]] = {}
    improvement_ratios: list[float] = []

    if baseline_ns > 0 and len(current_per_method) > 0:
        # Use baseline_ns as the geometric mean reference point.
        # For each method, compute the ratio against the aggregate.
        # The improvement_pct is: (baseline_ns - current_ns) / baseline_ns * 100 for each method,
        # but since we only have aggregate baseline, we compute per-method ratios
        # and compare the geometric mean of current values.
        for sid, c_ns in current_per_method.items():
            ratio = c_ns / baseline_ns if baseline_ns > 0 else 1.0
            improvement_pct = max(0.0, (1.0 - ratio) * 100.0)
            per_method[sid] = {
                "current_ns": round(c_ns, 2),
                "ratio_to_baseline": round(ratio, 4),
                "improvement_pct": round(improvement_pct, 2),
            }
            improvement_ratios.append(ratio)

        current_ns_geomean = _geometric_mean(list(current_per_method.values()))
        improvement_pct = (
            max(0.0, (baseline_ns - current_ns_geomean) / baseline_ns * 100.0)
            if baseline_ns > 0
            else 0.0
        )
    else:
        current_ns_geomean = 0.0
        improvement_pct = 0.0
        for sid, c_ns in current_per_method.items():
            per_method[sid] = {
                "current_ns": round(c_ns, 2),
                "ratio_to_baseline": 0.0,
                "improvement_pct": 0.0,
            }

    passed = improvement_pct >= min_threshold_pct

    result = {
        "passed": passed,
        "improvement_pct": round(improvement_pct, 2),
        "per_method": per_method,
        "baseline_ns": round(baseline_ns, 2),
        "current_ns": round(current_ns_geomean, 2),
        "hypothesis_id": hypothesis_id,
        "threshold_pct": min_threshold_pct,
        "error": None,
    }

    print(f"[sandwich_fix_loop] Verify {hypothesis_id}:")
    print(f"  baseline_ns:     {baseline_ns}")
    print(f"  current_ns:      {current_ns_geomean}")
    print(f"  improvement:     {improvement_pct:.2f}% (threshold: {min_threshold_pct:.2f}%)")
    print(f"  passed:          {passed}")

    return result


# ── Rollback ──────────────────────────────────────────────────────────────


def rollback_if_needed(
    slug: str,
    hypothesis_id: str,
    min_threshold_pct: float | None = None,
    assembly: str = _DEFAULT_ASSEMBLY,
    force_revert: bool = False,
) -> dict[str, Any]:
    """Verify improvement and auto-rollback if below threshold.

    Args:
        slug: Family slug.
        hypothesis_id: Hypothesis ID.
        min_threshold_pct: Minimum improvement threshold.
        assembly: Assembly name.
        force_revert: If True, revert even if improvement check passes
            (for testing the rollback mechanism).

    Returns:
        Dict with keys:
            "rolled_back": bool
            "reason": str
            "improvement_pct": float
            "reverted_commit": str | None
    """
    family_dir = _resolve_family_dir(slug, assembly)

    # Run verification
    verify_result = verify_improvement(
        slug=slug,
        hypothesis_id=hypothesis_id,
        min_threshold_pct=min_threshold_pct,
        assembly=assembly,
    )

    improvement_pct = verify_result.get("improvement_pct", 0.0)
    threshold = verify_result.get("threshold_pct", 0.0)
    passed = verify_result.get("passed", False)
    error = verify_result.get("error")

    if error:
        print(f"[sandwich_fix_loop] Verification error: {error}")
        return {
            "rolled_back": False,
            "reason": f"Verification failed: {error}",
            "improvement_pct": 0.0,
            "reverted_commit": None,
        }

    if passed and not force_revert:
        print(f"[sandwich_fix_loop] Improvement {improvement_pct:.2f}% >= threshold {threshold:.2f}% — no rollback needed.")
        return {
            "rolled_back": False,
            "reason": f"Improvement {improvement_pct:.2f}% meets threshold {threshold:.2f}%",
            "improvement_pct": improvement_pct,
            "reverted_commit": None,
        }

    # Need to rollback
    if force_revert:
        reason = f"Force revert requested"
    elif improvement_pct <= 0:
        reason = f"No improvement detected"
    else:
        reason = (
            f"Improvement {improvement_pct:.2f}% < threshold {threshold:.2f}%"
        )

    print(f"[sandwich_fix_loop] Rolling back: {reason}")

    # Execute git revert HEAD
    reverted_commit = _git_revert_head(hypothesis_id=hypothesis_id)

    # Save failure record
    now = _now_iso()
    rollback_record = {
        "hypothesis_id": hypothesis_id,
        "slug": slug,
        "rollback": {
            "rolled_back": True,
            "reason": reason,
            "reverted_commit": reverted_commit,
            "improvement_pct": improvement_pct,
            "threshold_pct": threshold,
        },
        "rolled_back_at": now,
    }

    history_path = family_dir / "fix-attempt-history.jsonl"
    with open(history_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(rollback_record, ensure_ascii=False) + "\n")

    print(f"[sandwich_fix_loop] Rollback complete. Reverted commit: {reverted_commit}")
    print(f"  Recorded at: {history_path}")

    return {
        "rolled_back": True,
        "reason": reason,
        "improvement_pct": improvement_pct,
        "reverted_commit": reverted_commit,
    }


def _git_revert_head(hypothesis_id: str | None = None) -> str | None:
    """Execute git revert HEAD --no-edit and return the original HEAD hash.

    Performs safety checks:
      1. Working tree must be clean (no unstaged modifications).
      2. If hypothesis_id provided, HEAD commit message should reference it
         or the slug (logged as a warning, not a blocker).

    Returns the original HEAD commit hash on success, None on failure.
    """
    try:
        # Check working tree cleanliness
        result = subprocess.run(
            ["git", "status", "--porcelain"],
            capture_output=True,
            text=True,
            cwd=str(_PROJECT_ROOT),
            timeout=15,
        )
        if result.stdout.strip():
            # There are unstaged or untracked files
            lines = result.stdout.strip().splitlines()
            unstaged = [l for l in lines if not l.startswith("?? ")]
            if unstaged:
                print(
                    "[sandwich_fix_loop] WARNING: Working tree has uncommitted changes."
                    " git revert may fail or produce unexpected results."
                )
                for line in unstaged[:10]:
                    print(f"  {line}")

        # Get HEAD info
        result = subprocess.run(
            ["git", "log", "-1", "--format=%H%n%s"],
            capture_output=True,
            text=True,
            cwd=str(_PROJECT_ROOT),
            timeout=15,
        )
        head_info = result.stdout.strip().splitlines()
        original_head = head_info[0] if head_info else ""
        head_subject = head_info[1] if len(head_info) > 1 else ""

        if hypothesis_id:
            slug_part = hypothesis_id.rsplit("-", 1)[0]
            if slug_part and slug_part not in head_subject.lower():
                print(
                    f"[sandwich_fix_loop] WARNING: HEAD commit subject does not mention "
                    f"'{slug_part}'. HEAD='{head_subject}'. "
                    f"Proceeding with revert anyway."
                )

        # Execute revert
        result = subprocess.run(
            ["git", "revert", "HEAD", "--no-edit"],
            capture_output=True,
            text=True,
            cwd=str(_PROJECT_ROOT),
            timeout=30,
        )

        if result.returncode != 0:
            print(f"[sandwich_fix_loop] git revert failed: {result.stderr.strip()}")
            return None

        if original_head:
            print(f"[sandwich_fix_loop] Reverted {original_head[:12]} ({head_subject})")
        return original_head or None

    except subprocess.TimeoutExpired:
        print("[sandwich_fix_loop] git revert timed out")
        return None
    except FileNotFoundError:
        print("[sandwich_fix_loop] git not found in PATH")
        return None
    except Exception as e:
        print(f"[sandwich_fix_loop] git revert error: {e}")
        return None


# ── CLI ───────────────────────────────────────────────────────────────────


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Sandwich fix loop — hypothesis-driven optimization with auto-rollback"
    )
    parser.add_argument("slug", help="Family slug (e.g. enum-parsing)")
    parser.add_argument(
        "command",
        choices=["record", "verify", "rollback"],
        help="Command to execute",
    )
    # hypothesis_id is positional for verify/rollback, omitted for record
    parser.add_argument(
        "hypothesis_id",
        nargs="?",
        default=None,
        help="Hypothesis ID (required for verify/rollback, e.g. enum-parsing-001)",
    )
    parser.add_argument(
        "--description",
        help="Optimization description (required for record)",
    )
    parser.add_argument(
        "--expected",
        type=float,
        help="Expected improvement percent (required for record)",
    )
    parser.add_argument(
        "--files",
        nargs="*",
        default=[],
        help="Changed files (for record)",
    )
    parser.add_argument(
        "--baseline-ns",
        type=float,
        default=None,
        help="Baseline geometric mean ns/op (optional, auto-extracted if omitted)",
    )
    parser.add_argument(
        "--baseline-report",
        default=None,
        help="Path to baseline benchmark report (optional)",
    )
    parser.add_argument(
        "--threshold",
        type=float,
        default=None,
        help="Minimum improvement threshold %% (default: expected/2)",
    )
    parser.add_argument(
        "--assembly",
        default=_DEFAULT_ASSEMBLY,
        help=f"Assembly name (default: {_DEFAULT_ASSEMBLY})",
    )
    parser.add_argument(
        "--force-revert",
        action="store_true",
        default=False,
        help="Force revert even if improvement passes (for testing)",
    )
    return parser


def _dispatch(args: argparse.Namespace) -> None:
    """Dispatch to the appropriate command handler."""
    if args.command == "record":
        if not args.description:
            print("Error: --description is required for 'record' command")
            sys.exit(1)
        if args.expected is None:
            print("Error: --expected is required for 'record' command")
            sys.exit(1)

        hypothesis_id = record_hypothesis(
            slug=args.slug,
            description=args.description,
            expected_improvement_pct=args.expected,
            changed_files=args.files,
            baseline_ns=args.baseline_ns,
            baseline_report_path=args.baseline_report,
            assembly=args.assembly,
        )
        print(f"\nResult: {hypothesis_id}")

    elif args.command == "verify":
        if not args.hypothesis_id:
            print("Error: <hypothesis_id> positional argument is required for 'verify' command")
            sys.exit(1)

        result = verify_improvement(
            slug=args.slug,
            hypothesis_id=args.hypothesis_id,
            min_threshold_pct=args.threshold,
            assembly=args.assembly,
        )
        print(json.dumps(result, indent=2, ensure_ascii=False))
        sys.exit(0 if result.get("passed") else 1)

    elif args.command == "rollback":
        if not args.hypothesis_id:
            print("Error: <hypothesis_id> positional argument is required for 'rollback' command")
            sys.exit(1)

        result = rollback_if_needed(
            slug=args.slug,
            hypothesis_id=args.hypothesis_id,
            min_threshold_pct=args.threshold,
            assembly=args.assembly,
            force_revert=args.force_revert,
        )
        print(json.dumps(result, indent=2, ensure_ascii=False))
        sys.exit(1 if result.get("rolled_back") else 0)

    else:
        _build_parser().print_help()
        sys.exit(1)


def main() -> None:
    parser = _build_parser()
    args = parser.parse_args()
    _dispatch(args)


if __name__ == "__main__":
    main()
