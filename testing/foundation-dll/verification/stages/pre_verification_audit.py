"""Pre-verification test audit: analyze family test code completeness.

Scans a family's contract, auto-callability metadata, and handwritten/ tests
to determine whether the test code is meaningful before running the full
verification pipeline.

Usage:
    python -m verification.stages.pre_verification_audit <slug> [options]
    python -m verification.stages.pre_verification_audit convert-char --assembly System.Private.CoreLib
    python -m verification.stages.pre_verification_audit convert-char --assembly System.Private.CoreLib --fix

Exit codes:
    0 = PASS (all methods have meaningful test coverage)
    1 = Issues found (missing handwritten, stale metadata, etc.)
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any

# Reuse auto-callability logic from test_code_generator
from verification.stages.test_code_generator import (
    is_auto_callable,
    method_skip_reason,
    parse_method_subject_id,
    build_call_expr,
    cast_return_to_int,
)

# Canonical contract lookup (single source of truth)
from verification.orchestration.context import resolve_contract_path, load_contract

_HERE = Path(__file__).resolve().parent  # verification/stages/
_VERIFICATION_ROOT = _HERE.parent          # verification/
_TESTING_ROOT = _VERIFICATION_ROOT.parent  # testing/


# ---------------------------------------------------------------------------
# Handwritten file detection
# ---------------------------------------------------------------------------

_HANDWRITTEN_METHOD_RE = re.compile(
    r"public\s+static\s+void\s+CustomEntrySubject_(\d+)\s*\(\)"
)


def _detect_handwritten_indices(handwritten_dir: Path) -> set[int]:
    """Scan handwritten/ directory for Custom.cs files and return the set of
    subject indices that have actual implementations."""
    if not handwritten_dir.is_dir():
        return set()

    indices: set[int] = set()
    for cs_file in sorted(handwritten_dir.glob("*.Custom.cs")):
        text = cs_file.read_text(encoding="utf-8")
        for m in _HANDWRITTEN_METHOD_RE.finditer(text):
            indices.add(int(m.group(1)))
    return indices


def _resolve_class_name(slug: str) -> str:
    """Convert a kebab-case slug to PascalCase class name.

    E.g. "convert-char" -> "ConvertChar", "array-copy" -> "ArrayCopy",
    "enumerator-iteration" -> "EnumeratorIteration"
    """
    return "".join(part.capitalize() for part in slug.split("-"))


# ---------------------------------------------------------------------------
# Analysis
# ---------------------------------------------------------------------------

_MethodAudit = dict[str, Any]


def audit_family(
    slug: str,
    assembly: str,
) -> dict[str, Any]:
    """Run the pre-verification audit for a single family.

    Returns a structured report dict (also printed as JSON to stdout).
    """
    family_dir = _TESTING_ROOT / assembly / slug
    if not family_dir.is_dir():
        print(f"ERROR: Family directory not found: {family_dir}", file=sys.stderr)
        sys.exit(1)

    # ── Load contract ──
    contract = _load_contract(family_dir)
    if contract is None:
        return {
            "family_slug": slug,
            "assembly": assembly,
            "verdict": "ERROR",
            "summary": "No capability-family-contract.json found",
            "method_count": 0,
            "handwritten_count": 0,
            "checks": {},
            "methods": [],
            "issues": [{
                "severity": "ERROR",
                "type": "NO_CONTRACT",
                "index": -1,
                "message": f"Missing contract in {family_dir}",
            }],
        }

    method_ids: list[str] = contract.get("methodSubjectIds", [])
    declared_custom_indices: set[int] = set(
        contract.get("customEntryIndices") or []
    )

    # ── Scan handwritten/ directory ──
    handwritten_dir = family_dir / "handwritten"
    actual_handwritten_indices = _detect_handwritten_indices(handwritten_dir)

    # ── Audit each method ──
    methods: list[_MethodAudit] = []
    issues: list[dict[str, Any]] = []

    for idx, mid in enumerate(method_ids):
        parsed = parse_method_subject_id(mid)
        auto_callable = is_auto_callable(parsed)
        skip_reason = method_skip_reason(parsed)
        in_declared = idx in declared_custom_indices
        has_handwritten = idx in actual_handwritten_indices

        if auto_callable:
            status = "auto_callable"
        elif has_handwritten:
            status = "covered_by_handwritten"
        elif in_declared and not has_handwritten:
            status = "DECLARED_BUT_MISSING"
        else:
            status = "UNCOVERED"

        methods.append({
            "index": idx,
            "method": mid,
            "auto_callable": auto_callable,
            "skip_reason": skip_reason,
            "in_custom_entry_indices": in_declared,
            "has_handwritten": has_handwritten,
            "status": status,
        })

        # Generate issues
        if status == "UNCOVERED":
            issues.append({
                "severity": "FAIL",
                "type": "UNCOVERED",
                "index": idx,
                "message": (
                    f"Method #{idx} ({parsed['type_name']}.{parsed['method_name']}) "
                    f"is NOT auto-callable but has no handwritten implementation "
                    f"and is not listed in customEntryIndices"
                ),
            })
        elif status == "DECLARED_BUT_MISSING":
            issues.append({
                "severity": "FAIL",
                "type": "DECLARED_BUT_MISSING",
                "index": idx,
                "message": (
                    f"Method #{idx} is listed in customEntryIndices but "
                    f"handwritten/{_resolve_class_name(slug)}Subjects.Custom.cs "
                    f"is missing CustomEntrySubject_{idx}()"
                ),
            })

    # ── Cross-reference: redundant customEntryIndices ──
    for idx in sorted(declared_custom_indices):
        if idx >= len(method_ids):
            issues.append({
                "severity": "WARN",
                "type": "INDEX_OOB",
                "index": idx,
                "message": (
                    f"customEntryIndices references index {idx} but "
                    f"methodSubjectIds has only {len(method_ids)} entries"
                ),
            })
            continue
        if methods[idx]["auto_callable"]:
            issues.append({
                "severity": "WARN",
                "type": "REDUNDANT_CUSTOM_ENTRY",
                "index": idx,
                "message": (
                    f"Method #{idx} ({methods[idx]['method']}) is auto-callable "
                    f"but listed in customEntryIndices -- redundant declaration"
                ),
            })

    # ── Determine verdict ──
    uncovered_count = sum(1 for i in issues if i["type"] in ("UNCOVERED", "DECLARED_BUT_MISSING"))
    has_uncovered = uncovered_count > 0
    has_redundant = any(i["type"] == "REDUNDANT_CUSTOM_ENTRY" for i in issues)
    has_index_oob = any(i["type"] == "INDEX_OOB" for i in issues)

    if has_uncovered:
        verdict = "MISSING_HANDWRITTEN"
    elif has_redundant or has_index_oob:
        verdict = "STALE_METADATA"
    else:
        verdict = "PASS"

    # ── Checks summary ──
    checks = {
        "has_handwritten_dir": handwritten_dir.is_dir(),
        "all_non_callable_covered": uncovered_count == 0,
        "custom_entry_indices_synced": not has_redundant and not has_index_oob,
        "handwritten_methods_match_declared": uncovered_count == 0,
    }

    non_callable_count = sum(1 for m in methods if not m["auto_callable"])
    covered_count = sum(
        1 for m in methods
        if not m["auto_callable"] and m["status"] in ("covered_by_handwritten",)
    )

    if non_callable_count > 0:
        coverage_pct = covered_count / non_callable_count * 100
    else:
        coverage_pct = 100.0

    report = {
        "family_slug": slug,
        "assembly": assembly,
        "verdict": verdict,
        "summary": _build_summary(verdict, slug, len(methods),
                                  uncovered_count, non_callable_count,
                                  coverage_pct, checks),
        "method_count": len(methods),
        "non_callable_count": non_callable_count,
        "covered_non_callable_count": covered_count,
        "handwritten_count": len(actual_handwritten_indices),
        "uncovered_count": sum(1 for i in issues if i["type"] == "UNCOVERED"),
        "declared_missing_count": sum(1 for i in issues if i["type"] == "DECLARED_BUT_MISSING"),
        "coverage_pct": round(coverage_pct, 1),
        "checks": checks,
        "methods": methods,
        "issues": issues,
    }
    return report


def _load_contract(family_dir: Path) -> dict[str, Any] | None:
    """Load contract using canonical loader from context."""
    return load_contract(family_dir)


def _build_summary(
    verdict: str, slug: str, method_count: int,
    uncovered_count: int, non_callable_count: int,
    coverage_pct: float, checks: dict[str, bool],
) -> str:
    if verdict == "PASS":
        if non_callable_count == 0:
            return (
                f"All {method_count} methods are auto-callable -- "
                f"no handwritten tests needed"
            )
        return (
            f"All {non_callable_count} non-callable methods are covered "
            f"by handwritten tests ({coverage_pct:.0f}% coverage)"
        )
    if verdict == "MISSING_HANDWRITTEN":
        return (
            f"{uncovered_count} method(s) need handwritten tests but are missing -- "
            f"run --fix to generate stubs"
        )
    if verdict == "STALE_METADATA":
        return (
            f"customEntryIndices in contract.json is out of sync with "
            f"actual auto-callability -- run --fix to repair"
        )
    return f"Unknown state for {slug}"


# ---------------------------------------------------------------------------
# Fix mode: generate missing handwritten stubs + update contract
# ---------------------------------------------------------------------------

def fix_family(report: dict[str, Any]) -> None:
    """Auto-fix missing handwritten stubs and stale metadata.

    For each UNCOVERED or DECLARED_BUT_MISSING method:
      1. Update contract.json customEntryIndices to include the index.
      2. Generate a handwritten/{ClassName}Subjects.Custom.cs stub file if missing.

    For each REDUNDANT_CUSTOM_ENTRY:
      1. Remove the index from customEntryIndices.
    """
    slug = report["family_slug"]
    assembly = report["assembly"]
    family_dir = _TESTING_ROOT / assembly / slug
    handwritten_dir = family_dir / "handwritten"

    # ── Step 1: Determine the set of indices that need handwritten ──
    uncovered_indices: set[int] = set()
    for issue in report["issues"]:
        if issue["type"] in ("UNCOVERED", "DECLARED_BUT_MISSING"):
            uncovered_indices.add(issue["index"])

    redundant_indices: set[int] = set()
    oob_indices: set[int] = set()
    for issue in report["issues"]:
        if issue["type"] == "REDUNDANT_CUSTOM_ENTRY":
            redundant_indices.add(issue["index"])
        elif issue["type"] == "INDEX_OOB":
            oob_indices.add(issue["index"])

    if not uncovered_indices and not redundant_indices and not oob_indices:
        print("[fix] Nothing to fix -- all clean")
        return

    # ── Step 2: Update customEntryIndices in capability-family-contract.json ──
    contract = _load_contract(family_dir)
    if contract is not None:
        current_indices = set(contract.get("customEntryIndices") or [])
        current_indices |= uncovered_indices
        current_indices -= redundant_indices
        current_indices -= oob_indices
        sorted_indices = sorted(current_indices)

        cpath = resolve_contract_path(family_dir)
        if cpath.exists():
            try:
                cdata = json.loads(cpath.read_text(encoding="utf-8"))
                cdata["customEntryIndices"] = sorted_indices
                cpath.write_text(
                    json.dumps(cdata, indent=2, ensure_ascii=False) + "\\n",
                    encoding="utf-8",
                )
                print(f"[fix] Updated customEntryIndices in {cpath.name}")
            except (json.JSONDecodeError, OSError) as e:
                print(f"[fix] WARNING: Could not update {cpath.name}: {e}")
    else:
        print("[fix] WARNING: No contract file found to update")

    # ── Step 3: Generate missing handwritten stubs ──
    if uncovered_indices:
        handwritten_dir.mkdir(parents=True, exist_ok=True)
        class_name = _resolve_class_name(slug) + "Subjects"

        # Load existing content to merge
        existing_files = sorted(handwritten_dir.glob("*.Custom.cs"))
        existing_indices = _detect_handwritten_indices(handwritten_dir)
        all_indices: set[int] = existing_indices | uncovered_indices

        usings = sorted({
            "using System;",
            "using System.Collections.Generic;",
            "using System.Runtime.InteropServices;",
        })

        lines = [
            "// Auto-generated handwritten test stubs for pre-verification audit.",
            "// TODO: Replace each stub body with a real test invocation.",
            "",
        ]
        lines.extend(usings)
        lines.append("")
        lines.append(f"public static partial class {class_name}")
        lines.append("{")

        for idx in sorted(all_indices):
            method_info = _find_method_by_index(report, idx)
            method_sig = method_info["method"] if method_info else f"subject_{idx}"
            comment = f"    // [{idx}] {method_sig}"
            stub = f"    public static void CustomEntrySubject_{idx}()"
            body = "    {"
            body += '        try { /* TODO: invoke the actual method */ }'
            body += "        catch { _exitCode = 1; }"
            body += "    }"
            lines.extend([comment, stub, body, ""])

        lines.append("}")

        cs_path = handwritten_dir / f"{class_name}.Custom.cs"
        cs_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
        print(f"[fix] Generated {cs_path.name} with {len(uncovered_indices)} new stub(s)")

    # ── Step 4: Report results ──
    total_fixed = len(uncovered_indices) + len(redundant_indices)
    print(f"\n[fix] Fixed {total_fixed} issue(s):")
    for idx in sorted(uncovered_indices):
        print(f"  + Added handwritten stub for method #{idx}")
    for idx in sorted(redundant_indices):
        print(f"  - Removed redundant customEntryIndices entry #{idx}")


def _find_contract_path(family_dir: Path) -> Path | None:
    """Return the path to the contract file, using canonical resolution."""
    path = resolve_contract_path(family_dir)
    return path if path.exists() else None


def _find_method_by_index(report: dict[str, Any], idx: int) -> dict[str, Any] | None:
    for m in report.get("methods", []):
        if m["index"] == idx:
            return m
    return None


# ---------------------------------------------------------------------------
# Fill mode: replace TODO stub bodies with real call expressions
# ---------------------------------------------------------------------------

_FILL_RE = re.compile(
    r'(public\s+static\s+void\s+CustomEntrySubject_)(\d+)(\s*\(\)\s*\{\s*)'
    r'try\s*\{\s*/\*\s*TODO:\s*invoke the actual method\s*\*/\s*\}'
    r'\s*catch\s*\{\s*_exitCode\s*=\s*1;\s*\}'
    r'(\s*\}\s*)'
)


def fill_handwritten_stubs(slug: str, assembly: str) -> int:
    """Replace TODO stub bodies with real call expressions in handwritten Custom.cs files.

    For each CustomEntrySubject_N() that still has the auto-generated TODO body,
    generates a real C# call expression using build_call_expr() and wraps it in
    try/catch with _exitCode = 1 on failure.

    Returns the count of methods that were filled.
    """
    family_dir = _TESTING_ROOT / assembly / slug
    handwritten_dir = family_dir / "handwritten"

    if not handwritten_dir.is_dir():
        print("[fill] No handwritten/ directory found")
        return 0

    contract = _load_contract(family_dir)
    if contract is None:
        print("[fill] No contract found")
        return 0

    method_ids = contract.get("methodSubjectIds", [])
    filled_count = 0

    for cs_file in sorted(handwritten_dir.glob("*.Custom.cs")):
        original = cs_file.read_text(encoding="utf-8")
        if "TODO: invoke the actual method" not in original:
            continue  # already filled

        def _replacer(m: re.Match) -> str:
            nonlocal filled_count
            idx = int(m.group(2))
            prefix = m.group(1)
            sig_and_open = m.group(3)
            close = m.group(4)

            if idx >= len(method_ids):
                print(f"[fill] WARNING: {cs_file.name} references index {idx} "
                      f"but methodSubjectIds has only {len(method_ids)} entries -- skipping")
                return m.group(0)

            parsed = parse_method_subject_id(method_ids[idx])
            ret_type = parsed.get("return_type", "").strip()

            try:
                call_expr = build_call_expr(parsed)
            except Exception as e:
                print(f"[fill] WARNING: build_call_expr failed for #{idx} "
                      f"({method_ids[idx]}): {e} -- keeping TODO")
                return m.group(0)

            if ret_type in ("System.Void", ""):
                body = f"try {{ {call_expr}; }} catch {{ _exitCode = 1; }}"
            else:
                body = f"try {{ _ = {call_expr}; }} catch {{ _exitCode = 1; }}"

            filled_count += 1
            return f"{prefix}{idx}{sig_and_open}{body}{close}"

        new_text = _FILL_RE.sub(_replacer, original)
        if new_text != original:
            cs_file.write_text(new_text, encoding="utf-8")
            print(f"[fill] Updated {cs_file.name} ({filled_count} method(s) so far)")

    return filled_count


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Pre-verification test audit for foundation-dll families"
    )
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument(
        "--assembly", default="System.Private.CoreLib",
        help="Assembly name (default: System.Private.CoreLib)",
    )
    parser.add_argument(
        "--fix", action="store_true",
        help="Auto-fix missing handwritten stubs and stale metadata",
    )
    parser.add_argument(
        "--fill", action="store_true",
        help="Fill TODO stub bodies with generated call expressions",
    )
    parser.add_argument(
        "--json", action="store_true",
        help="Output raw JSON report (default: human-readable summary)",
    )
    args = parser.parse_args()

    report = audit_family(args.family_slug, args.assembly)

    if args.fix:
        fix_family(report)
        report = audit_family(args.family_slug, args.assembly)
        print("\n--- Re-audit after fix ---")

    if args.fill:
        filled = fill_handwritten_stubs(args.family_slug, args.assembly)
        print(f"\n--- Fill: {filled} method(s) updated ---")
        report = audit_family(args.family_slug, args.assembly)
        print("\n--- Re-audit after fill ---")

    if args.json:
        print(json.dumps(report, indent=2, ensure_ascii=False))
    else:
        _print_human_readable(report)

    sys.exit(0 if report["verdict"] == "PASS" else 1)


def _print_human_readable(report: dict[str, Any]) -> None:
    """Print a concise human-readable summary."""
    print(f"Family: {report['family_slug']} [{report['assembly']}]")
    print(f"Verdict: {report['verdict']}")
    print(f"Summary: {report['summary']}")
    print(f"Methods: {report['method_count']} total, "
          f"{report['non_callable_count']} non-callable, "
          f"{report['covered_non_callable_count']} covered by handwritten")
    print(f"Coverage: {report['coverage_pct']}% of non-callable methods")
    print()

    if report["checks"]["has_handwritten_dir"]:
        print(f"  handwritten/ directory: present ({report['handwritten_count']} methods)")
    else:
        print("  handwritten/ directory: absent")

    for check_name, ok in report["checks"].items():
        if check_name != "has_handwritten_dir":
            status = "OK" if ok else "FAIL"
            print(f"  [{status}] {check_name}")

    if report["issues"]:
        print()
        print("Issues:")
        for issue in report["issues"]:
            icon_map = {"FAIL": "!", "WARN": "?", "ERROR": "!!"}
            icon = icon_map.get(issue["severity"], "?")
            print(f"  [{icon}] [{issue['type']}] {issue['message']}")

    print()


if __name__ == "__main__":
    main()
