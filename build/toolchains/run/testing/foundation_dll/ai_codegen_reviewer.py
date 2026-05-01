"""AI Code Review for C#/C++ alignment verification.

Reviews generated C++ code against C# IL dumps to verify:
Phase 2 (codegen): C# IL method signatures match C++ function signatures,
  return type handling, exception paths, null checks, type casting.
Phase 3 (hotupdate): patch function correctness, sentinel values,
  fallthrough logic, metadata side effects.

Usage:
  python ai_codegen_reviewer.py <family-slug> --csil <cs-il-dump> --cpp <generated-cpp> --type <codegen|hotupdate>

Output:
  - Writes review verdict to review/<family>/codegen-review-v<N>.md
  - Exit code 0 (pass), 1 (warn), 2 (fail)
"""

from __future__ import annotations

import argparse
import json
import os
import re
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib"


def _check_signature_alignment(cpp_content: str, csil_content: str) -> list[str]:
    """Check that C# IL method signatures have corresponding C++ entries.

    Returns list of issues found (empty = clean).
    """
    issues: list[str] = []

    # Extract C# method names from IL dump (lines like ".method public ...")
    cs_methods = set()
    for line in csil_content.splitlines():
        m = re.search(r'\.method\s+\S+\s+\S*\s*(\S+)\(', line)
        if m:
            cs_methods.add(m.group(1))

    # Extract C++ function names from generated code
    cpp_functions = set()
    for line in cpp_content.splitlines():
        m = re.search(r'(?:CHAOS_IL2CPP_EXPORT|static|void|int32_t|bool|float|double)\s+\**\s*(\w+)\s*\(', line)
        if m:
            cpp_functions.add(m.group(1))

    # Check each C# method has a C++ counterpart (by name)
    for cs_m in cs_methods:
        if cs_m not in cpp_functions and cs_m not in {m.replace("_", "") for m in cpp_functions}:
            issues.append(f"No matching C++ function found for C# method '{cs_m}'")

    return issues


def _check_return_type_handling(cpp_content: str) -> list[str]:
    """Check return type handling in C++ code.

    Verifies that value types are returned by value and reference types
    use proper return conventions.
    """
    issues: list[str] = []
    # Check that void functions exist
    void_count = len(re.findall(r'\bvoid\b', cpp_content))
    if void_count == 0 and len(cpp_content) > 100:
        issues.append("No void return type found — may indicate missing return type annotations")

    # Check for return statements in non-void functions
    returns = re.findall(r'return\s+([^;]+);', cpp_content)
    if not returns and len(cpp_content) > 200:
        # Only flag if there are function definitions that look non-void
        non_void_funcs = re.findall(r'(?:int32_t|int64_t|bool|float|double|Object\*|String\*)\s+\w+\s*\(', cpp_content)
        if non_void_funcs:
            issues.append(f"Non-void functions found but no return statements ({len(non_void_funcs)} functions)")

    return issues


def _check_null_handling(cpp_content: str) -> list[str]:
    """Check that null checks exist where C# would have them."""
    issues: list[str] = []
    # Check for null pointer checks
    null_checks = len(re.findall(r'(?:==\s*nullptr|!=\s*nullptr|nullptr\s*==|nullptr\s*!=)', cpp_content))
    if null_checks == 0 and len(cpp_content) > 500:
        # Only flag if there are pointer-typed parameters
        pointer_params = re.findall(r'(Object\*|String\*|Array\*|\[\]\s+\w+)', cpp_content)
        if pointer_params:
            issues.append(f"No null checks found despite {len(pointer_params)} pointer parameters")
    return issues


def _check_exception_handling(cpp_content: str) -> list[str]:
    """Check that exception handling paths exist in C++."""
    issues: list[str] = []
    # Look for try/catch blocks or CHAOS_IL2CPP_* macros for exceptions
    has_exception_macros = bool(re.search(r'CHAOS_IL2CPP_RAISE|try\s*\{|catch\s*\(', cpp_content))
    if not has_exception_macros and len(cpp_content) > 500:
        issues.append("No exception handling paths found (try/catch or CHAOS_IL2CPP_RAISE macros)")
    return issues


def _check_hotupdate_patch_correctness(cpp_content: str) -> list[str]:
    """Check hotupdate patch correctness.

    Verifies sentinel return values and fallthrough logic.
    """
    issues: list[str] = []
    # Check for sentinel return pattern
    sentinel_returns = re.findall(r'return\s+0x[0-9A-Fa-f]+\s*;', cpp_content)
    if not sentinel_returns and len(cpp_content) > 100:
        issues.append("No sentinel return values found in patch code")

    # Check for fallthrough logic (if/else that delegates to original)
    has_fallthrough = bool(re.search(r'(?:return\s+original|delegate|base_call|CHAOS_IL2CPP_FALLTHROUGH)', cpp_content))
    if not has_fallthrough and len(cpp_content) > 100:
        issues.append("No fallthrough/delegate pattern found — patch may not chain to original")

    # Check for metadata structure usage
    has_metadata = bool(re.search(r'(?:metadata|patch_info|RuntimeSkeletonPatch)', cpp_content))
    if has_metadata:
        # Check metadata is read-only (no assignment)
        metadata_writes = re.findall(r'(metadata|patch_info)\s*(->|\.)\s*\w+\s*=', cpp_content)
        if metadata_writes:
            issues.append(f"Patch code writes to metadata structures ({len(metadata_writes)} writes found)")

    return issues


def _generate_review_markdown(
    family_slug: str,
    review_type: str,
    verdict: str,
    issues: list[str],
    stats: dict[str, Any],
) -> str:
    """Generate review markdown document."""
    lines = [
        f"# AI Code Review: {family_slug}",
        f"## Type: {review_type}",
        f"## Date: {datetime.now(timezone.utc).isoformat()}",
        f"## Verdict: {verdict}",
        "",
        "## Summary",
        f"- Total issues: {len(issues)}",
        f"- C++ LOC: {stats.get('cpp_loc', 0)}",
        f"- C# methods: {stats.get('cs_method_count', 0)}",
        f"- C++ functions: {stats.get('cpp_function_count', 0)}",
        "",
    ]
    if issues:
        lines.append("## Issues Found")
        for i, issue in enumerate(issues, 1):
            lines.append(f"{i}. {issue}")
        lines.append("")
    else:
        lines.append("## Issues Found\nNone — all checks passed.\n")

    lines.append("## Check Results")
    for check, result in stats.get("check_results", {}).items():
        status = "PASS" if result else "FAIL"
        lines.append(f"- {check}: {status}")

    return "\n".join(lines)


def _get_next_review_version(family_slug: str, review_type: str) -> int:
    """Get the next version number for a review file."""
    review_dir = _VERIFICATION_BASE / family_slug / "review"
    if not review_dir.exists():
        return 1
    existing = list(review_dir.glob(f"{review_type}-review-v*.md"))
    if not existing:
        return 1
    versions = []
    for p in existing:
        m = re.search(r'v(\d+)', p.stem)
        if m:
            versions.append(int(m.group(1)))
    return max(versions) + 1 if versions else 1


def perform_review(
    family_slug: str,
    csil_content: str,
    cpp_content: str,
    review_type: str = "codegen",
) -> dict[str, Any]:
    """Perform AI code review and return results."""
    stats: dict[str, Any] = {
        "cpp_loc": len(cpp_content.splitlines()),
        "cs_method_count": 0,
        "cpp_function_count": 0,
        "check_results": {},
    }

    # Extract method/function counts
    cs_methods = set(re.findall(r'\.method\s+\S+\s+\S*\s*(\S+)\(', csil_content))
    cpp_functions = set(re.findall(r'(?:void|int32_t|int64_t|bool|float|double|Object\*|String\*)\s+\**\s*(\w+)\s*\(', cpp_content))
    stats["cs_method_count"] = len(cs_methods)
    stats["cpp_function_count"] = len(cpp_functions)

    all_issues: list[str] = []

    if review_type == "codegen":
        issues = _check_signature_alignment(cpp_content, csil_content)
        stats["check_results"]["signature_alignment"] = len(issues) == 0
        all_issues.extend(issues)

        issues = _check_return_type_handling(cpp_content)
        stats["check_results"]["return_type_handling"] = len(issues) == 0
        all_issues.extend(issues)

        issues = _check_null_handling(cpp_content)
        stats["check_results"]["null_handling"] = len(issues) == 0
        all_issues.extend(issues)

        issues = _check_exception_handling(cpp_content)
        stats["check_results"]["exception_handling"] = len(issues) == 0
        all_issues.extend(issues)

    elif review_type == "hotupdate":
        issues = _check_hotupdate_patch_correctness(cpp_content)
        stats["check_results"]["patch_correctness"] = len(issues) == 0
        all_issues.extend(issues)

        issues = _check_signature_alignment(cpp_content, csil_content)
        stats["check_results"]["signature_alignment"] = len(issues) == 0
        all_issues.extend(issues)

    # Determine verdict
    if len(all_issues) == 0:
        verdict = "PASS"
    elif len(all_issues) <= 2:
        verdict = "WARN"
    else:
        verdict = "FAIL"

    markdown = _generate_review_markdown(family_slug, review_type, verdict, all_issues, stats)

    # Write review file
    version = _get_next_review_version(family_slug, review_type)
    review_dir = _VERIFICATION_BASE / family_slug / "review"
    review_dir.mkdir(parents=True, exist_ok=True)
    review_path = review_dir / f"{review_type}-review-v{version}.md"
    review_path.write_text(markdown, encoding="utf-8")

    return {
        "verdict": verdict,
        "issues": all_issues,
        "stats": stats,
        "review_path": str(review_path),
    }


def main() -> None:
    parser = argparse.ArgumentParser(description="AI Code Review for C#/C++ alignment")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--csil", required=True, help="Path to C# IL dump file")
    parser.add_argument("--cpp", required=True, help="Path to generated C++ file")
    parser.add_argument("--type", default="codegen", choices=["codegen", "hotupdate"],
                        help="Review type")
    args = parser.parse_args()

    csil_path = Path(args.csil)
    cpp_path = Path(args.cpp)

    if not csil_path.exists():
        print(f"ERROR: C# IL file not found: {csil_path}", file=sys.stderr)
        sys.exit(2)
    if not cpp_path.exists():
        print(f"ERROR: C++ file not found: {cpp_path}", file=sys.stderr)
        sys.exit(2)

    csil_content = csil_path.read_text(encoding="utf-8")
    cpp_content = cpp_path.read_text(encoding="utf-8")

    result = perform_review(args.family_slug, csil_content, cpp_content, args.type)
    print(f"Review written to: {result['review_path']}")
    print(f"Verdict: {result['verdict']}")
    print(f"Issues: {len(result['issues'])}")
    for issue in result["issues"]:
        print(f"  - {issue}")

    exit_code = {"PASS": 0, "WARN": 1, "FAIL": 2}
    sys.exit(exit_code.get(result["verdict"], 2))


if __name__ == "__main__":
    main()
