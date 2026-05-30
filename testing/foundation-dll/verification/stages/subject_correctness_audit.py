"""Subject correctness audit — Phase B of pre-verification audit.

Detects test quality issues in auto-generated Subject_N() and handwritten
CustomEntrySubject_N() methods before optimization begins.

Checks:
  - Triviality Detection: Subject_N() always takes catch path
  - Assertion Quality: CustomEntrySubject_N() has Assert.* calls
  - Stub Detection: `_ = 0;` / `_ = default(T)!.Member` patterns
  - Benchmark Input Check: benchmark-annotated methods always throw

Usage:
    python -m verification.stages.subject_correctness_audit <slug> --assembly System.Private.CoreLib
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path
from typing import Any

from verification.orchestration.context import resolve_contract_path, load_contract

_HERE = Path(__file__).resolve().parent
_VERIFICATION_ROOT = _HERE.parent
_TESTING_ROOT = _VERIFICATION_ROOT.parent


# Patterns for stub detection
_STUB_PATTERN_ASSIGN_ZERO = re.compile(r"_\s*=\s*0\s*;")
_STUB_PATTERN_DEFAULT_MEMBER = re.compile(r"_\s*=\s*default\s*\([^)]*\)\s*!?\.")
_STUB_PATTERN_DISCARD = re.compile(r"_\s+=\s+\w")
_ASSERT_PATTERN = re.compile(r"Assert\.\w+\s*\(")
_EXCEPTION_EXPECT_PATTERN = re.compile(
    r"catch\s*\(\s*(System\.)?\w+(Exception|Error)\s*\)"
)
_BLANKET_CATCH_PATTERN = re.compile(r"catch\s*(\s*\{|\s*\(\s*\)\s*\{)")
_SPECIFIC_CATCH_PATTERN = re.compile(
    r"catch\s*\(\s*(System\.)?\w+Exception\s*"
)
_TRIVIAL_PATTERN = re.compile(r"_\s*=\s*0\s*;?\s*$", re.MULTILINE)


def _resolve_class_name(slug: str) -> str:
    return "".join(part.capitalize() for part in slug.split("-"))


def _find_handwritten_cs_files(family_dir: Path) -> list[Path]:
    """Find all handwritten .cs files in managed/subjects/ and handwritten/."""
    files: list[Path] = []
    for base in ("managed/subjects", "handwritten"):
        d = family_dir / base
        if d.is_dir():
            files.extend(sorted(d.glob("*.Custom.cs")))
    return files


def _find_subject_methods(
    cs_files: list[Path],
) -> dict[str, list[dict[str, Any]]]:
    """Extract method bodies from Custom.cs files, grouped by method name."""
    methods: dict[str, list[dict[str, Any]]] = {}

    _METHOD_RE = re.compile(
        r"public\s+static\s+void\s+(CustomEntrySubject_\d+)\s*\(\)\s*\{(.*?)\}",
        re.DOTALL,
    )

    for cs_file in cs_files:
        text = cs_file.read_text(encoding="utf-8")
        for m in _METHOD_RE.finditer(text):
            method_name = m.group(1)
            body = m.group(2).strip()
            methods.setdefault(method_name, []).append({
                "file": str(cs_file),
                "body": body,
            })
    return methods


def audit_assertion_quality(
    family_dir: Path,
    slug: str,
    contract: dict[str, Any],
) -> list[dict[str, Any]]:
    """Check handwritten CustomEntrySubject_N() methods for Assert.* calls.

    Returns list of issues for methods with no assertions or blanket catch.
    """
    cs_files = _find_handwritten_cs_files(family_dir)
    if not cs_files:
        return []

    methods = _find_subject_methods(cs_files)
    issues: list[dict[str, Any]] = []

    declared_custom = set(contract.get("customEntryIndices") or [])
    method_ids = contract.get("methodSubjectIds", [])

    for idx in sorted(declared_custom):
        method_name = f"CustomEntrySubject_{idx}"
        if method_name not in methods:
            continue

        for entry in methods[method_name]:
            body = entry["body"]

            # Check for Assert.* calls
            if not _ASSERT_PATTERN.search(body):
                mid = method_ids[idx] if idx < len(method_ids) else "?"
                issues.append({
                    "severity": "WARN",
                    "type": "NO_ASSERTION",
                    "index": idx,
                    "method": method_name,
                    "file": entry["file"],
                    "message": (
                        f"{method_name} ({mid}) has no Assert.* call "
                        f"— test passes vacuously"
                    ),
                })

            # Check for blanket catch { } (not specific exception type)
            if _BLANKET_CATCH_PATTERN.search(body) and not _SPECIFIC_CATCH_PATTERN.search(body):
                issues.append({
                    "severity": "WARN",
                    "type": "BLANKET_CATCH",
                    "index": idx,
                    "method": method_name,
                    "file": entry["file"],
                    "message": (
                        f"{method_name} uses blanket catch {{ }} instead of "
                        f"specific exception type — may hide real failures"
                    ),
                })

    return issues


def audit_stub_patterns(
    family_dir: Path,
    slug: str,
    contract: dict[str, Any],
) -> list[dict[str, Any]]:
    """Detect stub patterns in handwritten methods.

    Detects:
      - `_ = 0;` (no-op)
      - `_ = default(T)!.Member` (always throws NRE)
    """
    cs_files = _find_handwritten_cs_files(family_dir)
    if not cs_files:
        return []

    methods = _find_subject_methods(cs_files)
    issues: list[dict[str, Any]] = []

    _METHOD_DETAIL_RE = re.compile(
        r"public\s+static\s+void\s+(CustomEntrySubject_\d+)\s*\(\)\s*\{(.*?)\}",
        re.DOTALL,
    )

    for cs_file in cs_files:
        text = cs_file.read_text(encoding="utf-8")
        for m in _METHOD_DETAIL_RE.finditer(text):
            method_name = m.group(1)
            body = m.group(2).strip()

            # `_ = 0;` — literal no-op stub
            if _STUB_PATTERN_ASSIGN_ZERO.search(body):
                # Only flag if the entire body IS the no-op (not part of real code)
                lines = [l.strip() for l in body.split("\n") if l.strip()]
                if any(l == "_ = 0;" for l in lines):
                    issues.append({
                        "severity": "WARN",
                        "type": "STUB_NOOP",
                        "index": int(method_name.split("_")[-1]),
                        "method": method_name,
                        "file": str(cs_file),
                        "message": (
                            f"{method_name} body is `_ = 0;` — "
                            f"no-op stub, does not test anything"
                        ),
                    })

            # `_ = default(T)!.Member` — always throws NRE
            if _STUB_PATTERN_DEFAULT_MEMBER.search(body):
                issues.append({
                    "severity": "WARN",
                    "type": "STUB_DEFAULT_THROWS",
                    "index": int(method_name.split("_")[-1]),
                    "method": method_name,
                    "file": str(cs_file),
                    "message": (
                        f"{method_name} uses `default(T)!` pattern "
                        f"— always throws NullReferenceException, "
                        f"test exercises exception path only"
                    ),
                })

    return issues


def audit_benchmark_inputs(
    contract: dict[str, Any],
    managed_fact_results: list[dict[str, Any]] | None = None,
) -> list[dict[str, Any]]:
    """Check benchmark-annotated methods for always-throw inputs.

    Cross-references benchmarkMethodIndices against managed_fact results.
    """
    issues: list[dict[str, Any]] = []
    bench_indices = set(contract.get("benchmarkMethodIndices") or [])
    method_ids = contract.get("methodSubjectIds", [])

    if not managed_fact_results:
        return issues

    # Build index map from managed_fact results
    fact_by_index: dict[int, dict[str, Any]] = {}
    for result in managed_fact_results:
        idx = result.get("methodIndex", -1)
        if idx >= 0:
            fact_by_index[idx] = result

    for idx in sorted(bench_indices):
        fact = fact_by_index.get(idx)
        if fact and fact.get("status") == "failed":
            mid = method_ids[idx] if idx < len(method_ids) else "?"
            issues.append({
                "severity": "WARN",
                "type": "BENCHMARK_ALWAYS_THROWS",
                "index": idx,
                "message": (
                    f"Method #{idx} ({mid}) is in benchmarkMethodIndices "
                    f"but always throws under managed .NET — benchmark "
                    f"timing reflects exception overhead, not real performance"
                ),
            })

    return issues


def audit_triviality(
    family_dir: Path,
    slug: str,
    contract: dict[str, Any],
    managed_record_path: Path | None = None,
) -> list[dict[str, Any]]:
    """Detect methods that always take the catch path.

    Uses managed_record golden-record.json to find methods that
    consistently pass via exception handling rather than real execution.
    """
    issues: list[dict[str, Any]] = []

    if not managed_record_path or not managed_record_path.exists():
        return issues

    try:
        golden = json.loads(managed_record_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return issues

    method_ids = contract.get("methodSubjectIds", [])
    results = golden if isinstance(golden, list) else golden.get("results", [])

    for result in results:
        idx = result.get("methodIndex", -1)
        mid = result.get("methodSubjectId", "")
        status = result.get("status", "")
        exc_msg = result.get("exceptionMessage", "")

        # A method that always passes via exception handling
        # is suspicious — it may be trivially passing
        if status == "passed" and exc_msg and "exception" in exc_msg.lower():
            issues.append({
                "severity": "INFO",
                "type": "TRIVIAL_PASS_VIA_EXCEPTION",
                "index": idx,
                "message": (
                    f"Method #{idx} ({mid}) passed via exception handling "
                    f"({exc_msg[:80]}) — test may not exercise real logic"
                ),
            })

    return issues


def run_full_correctness_audit(
    slug: str,
    assembly: str,
    managed_record_path: Path | None = None,
    managed_fact_results: list[dict[str, Any]] | None = None,
) -> dict[str, Any]:
    """Run all correctness checks and return structured report."""
    family_dir = _TESTING_ROOT / assembly / slug
    contract = load_contract(family_dir)
    if contract is None:
        return {"correctness_verdict": "ERROR", "issues": [],
                "summary": "No contract found"}

    all_issues: list[dict[str, Any]] = []

    # Run all checks
    assertion_issues = audit_assertion_quality(family_dir, slug, contract)
    all_issues.extend(assertion_issues)

    stub_issues = audit_stub_patterns(family_dir, slug, contract)
    all_issues.extend(stub_issues)

    benchmark_issues = audit_benchmark_inputs(contract, managed_fact_results)
    all_issues.extend(benchmark_issues)

    trivial_issues = audit_triviality(family_dir, slug, contract, managed_record_path)
    all_issues.extend(trivial_issues)

    # Determine verdict
    blocker_issues = [i for i in all_issues if i.get("severity") in ("FAIL", "ERROR")]
    warn_issues = [i for i in all_issues if i.get("severity") == "WARN"]

    if blocker_issues:
        verdict = "BLOCKER"
    elif warn_issues:
        verdict = "ISSUES_FOUND"
    else:
        verdict = "PASS"

    return {
        "correctness_verdict": verdict,
        "total_issues": len(all_issues),
        "blocker_count": len(blocker_issues),
        "warning_count": len(warn_issues),
        "issues": all_issues,
        "summary": _build_correctness_summary(verdict, all_issues),
    }


def _build_correctness_summary(
    verdict: str,
    issues: list[dict[str, Any]],
) -> str:
    if verdict == "PASS":
        return "All subject correctness checks passed"
    if verdict == "BLOCKER":
        blockers = [i for i in issues if i.get("severity") in ("FAIL", "ERROR")]
        return (
            f"{len(blockers)} blocker(s) found: "
            + "; ".join(i["message"][:60] for i in blockers[:3])
        )
    warns = [i for i in issues if i.get("severity") == "WARN"]
    return (
        f"{len(warns)} warning(s) found: "
        + "; ".join(i["message"][:60] for i in warns[:3])
    )


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Subject correctness audit")
    parser.add_argument("slug", help="Family slug")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--managed-record", type=Path, default=None,
                        help="Path to golden-record.json")
    parser.add_argument("--managed-fact", type=Path, default=None,
                        help="Path to managed_fact golden-values.json")
    parser.add_argument("--force", action="store_true",
                        help="Skip blocking exit on ISSUES_FOUND (manual override)")
    args = parser.parse_args()

    managed_fact_results = None
    if args.managed_fact and args.managed_fact.exists():
        try:
            data = json.loads(args.managed_fact.read_text(encoding="utf-8"))
            managed_fact_results = data if isinstance(data, list) else data.get("results")
        except (OSError, json.JSONDecodeError):
            pass

    report = run_full_correctness_audit(
        args.slug, args.assembly,
        managed_record_path=args.managed_record,
        managed_fact_results=managed_fact_results,
    )

    # ── Determine exit code and blocking status ──
    verdict = report.get("correctness_verdict", "")

    if verdict == "ISSUES_FOUND" and args.force:
        effective_exit_code = 0
    elif verdict == "BLOCKER":
        effective_exit_code = 2
    elif verdict == "ERROR":
        effective_exit_code = 1
    elif verdict == "ISSUES_FOUND":
        effective_exit_code = 1
    else:  # PASS
        effective_exit_code = 0

    # Add exit/blocking fields to JSON output (backward-compat: correctness_verdict is unchanged)
    report["exit_code"] = effective_exit_code
    report["blocking"] = effective_exit_code != 0
    if verdict == "ISSUES_FOUND" and effective_exit_code != 0:
        report["blocking_verdict"] = "ISSUES_FOUND_BLOCKING"

    print(json.dumps(report, indent=2, ensure_ascii=False))

    sys.exit(effective_exit_code)


if __name__ == "__main__":
    main()
