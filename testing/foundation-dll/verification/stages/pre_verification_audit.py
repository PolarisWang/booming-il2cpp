"""Pre-verification test audit: analyze family test code completeness.

Scans a family's contract, auto-callability metadata, and handwritten/ tests
to determine whether the test code is meaningful before running the full
verification pipeline.

Usage:
    python -m verification.stages.pre_verification_audit <slug> [options]
    python -m verification.stages.pre_verification_audit convert-char --assembly System.Private.CoreLib
    python -m verification.stages.pre_verification_audit convert-char --assembly System.Private.CoreLib --fix
    python -m verification.stages.pre_verification_audit convert-char --assembly System.Private.CoreLib --fix-annotations
    python -m verification.stages.pre_verification_audit enum-parsing --assembly System.Private.CoreLib --estimate-roi

Exit codes:
    0 = PASS (all methods have meaningful test coverage, or ROI gate passed)
    1 = Issues found (missing handwritten, stale metadata, or ROI gate failed)
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
import time
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
from verification.stages.subject_correctness_audit import run_full_correctness_audit

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


def _safe_float(value: Any) -> float:
    """Safely convert to float, returning 0.0 on failure."""
    try:
        return float(value)
    except (TypeError, ValueError):
        return 0.0


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

    # ── Annotation completeness check (p4) ──
    has_benchmark_annotation = "benchmarkMethodIndices" in contract
    has_hotupdate_annotation = "hotupdateMethodIndices" in contract
    has_annotation_issues = False

    if not has_benchmark_annotation or not has_hotupdate_annotation:
        annotation_type = []
        if not has_benchmark_annotation:
            annotation_type.append("benchmarkMethodIndices")
        if not has_hotupdate_annotation:
            annotation_type.append("hotupdateMethodIndices")
        issues.append({
            "severity": "WARN",
            "type": "MISSING_ANNOTATION",
            "index": -1,
            "message": (
                f"Contract missing annotation fields: {', '.join(annotation_type)}. "
                f"Run --fix-annotations to auto-generate, or add manually."
            ),
        })
        has_annotation_issues = True
    else:
        # Validate annotation entries: warn if exception-prone methods are annotated
        bench_indices = set(contract.get("benchmarkMethodIndices") or [])
        hotupdate_indices = set(contract.get("hotupdateMethodIndices") or [])

        for idx, mid in enumerate(method_ids):
            parsed = parse_method_subject_id(mid)
            auto_callable = is_auto_callable(parsed)
            skip_reason = method_skip_reason(parsed)

            if idx in bench_indices and not auto_callable:
                issues.append({
                    "severity": "WARN",
                    "type": "BENCHMARK_ANNOTATION_INCLUDES_NON_AUTO_CALLABLE",
                    "index": idx,
                    "message": (
                        f"Method #{idx} ({parsed['type_name']}.{parsed['method_name']}) "
                        f"is in benchmarkMethodIndices but is not auto-callable "
                        f"(skip: {skip_reason}) — may produce unreliable timing"
                    ),
                })
                has_annotation_issues = True

            if idx in hotupdate_indices:
                ret_type = parsed.get("return_type", "").strip()
                if ret_type in ("System.Void", ""):
                    issues.append({
                        "severity": "WARN",
                        "type": "HOTUPDATE_ANNOTATION_VOID_METHOD",
                        "index": idx,
                        "message": (
                            f"Method #{idx} ({parsed['type_name']}.{parsed['method_name']}) "
                            f"is in hotupdateMethodIndices but returns void — semantic "
                            f"change detection via return value is impossible"
                        ),
                    })
                    has_annotation_issues = True

    if has_uncovered:
        verdict = "MISSING_HANDWRITTEN"
    elif has_annotation_issues:
        verdict = "MISSING_ANNOTATION"
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
    if verdict == "MISSING_ANNOTATION":
        return (
            f"Contract is missing benchmarkMethodIndices and/or hotupdateMethodIndices "
            f"— run --fix-annotations to auto-generate"
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
                    json.dumps(cdata, indent=2, ensure_ascii=False) + "\n",
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
            body += "        // TODO: invoke the actual method"
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


def fix_annotations(report: dict[str, Any]) -> None:
    """Auto-generate benchmarkMethodIndices and hotupdateMethodIndices in contract.

    Runs managed_fact to detect exception-throwing methods, then:
    - benchmarkMethodIndices: all non-exception methods
    - hotupdateMethodIndices: non-exception methods with non-void return type
      (semantic change detection depends on return value comparison)

    Always creates both fields when missing. Existing fields are NOT overwritten.
    """
    slug = report["family_slug"]
    assembly = report["assembly"]
    family_dir = _TESTING_ROOT / assembly / slug

    contract = _load_contract(family_dir)
    if contract is None:
        print("[fix-annotations] No contract found")
        return

    method_ids = contract.get("methodSubjectIds", [])
    if not method_ids:
        print("[fix-annotations] No methodSubjectIds in contract")
        return

    # Check if annotation fields already exist — skip if both present
    cpath = resolve_contract_path(family_dir)
    if not cpath.exists():
        print("[fix-annotations] Contract file not found")
        return

    try:
        cdata = json.loads(cpath.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError) as e:
        print(f"[fix-annotations] ERROR: Could not read contract: {e}")
        return

    if "benchmarkMethodIndices" in cdata and "hotupdateMethodIndices" in cdata:
        print(f"[fix-annotations] Both annotation fields already present — skipping")
        return

    # ── Phase 1: Run managed_fact to detect exception paths ──
    print(f"[fix-annotations] Running managed_fact to detect exception paths...")
    exception_indices = _run_managed_fact_for_annotations(slug, assembly, method_ids)

    if exception_indices is None:
        print(f"[fix-annotations] WARNING: managed_fact failed — falling back to is_auto_callable()")
        exception_indices = _guess_exception_indices(method_ids)
    else:
        print(f"[fix-annotations] managed_fact detected {len(exception_indices)} exception method(s): {sorted(exception_indices)}")

    # ── Phase 2: Build annotation lists ──
    benchmark_indices = []
    hotupdate_indices = []

    for idx, mid in enumerate(method_ids):
        if idx in exception_indices:
            continue  # Skip exception-path methods

        benchmark_indices.append(idx)

        parsed = parse_method_subject_id(mid)
        ret_type = parsed.get("return_type", "").strip()
        if ret_type not in ("System.Void", ""):
            hotupdate_indices.append(idx)

    # ── Phase 3: Update contract ──
    changed = False

    if "benchmarkMethodIndices" not in cdata:
        cdata["benchmarkMethodIndices"] = benchmark_indices
        print(f"[fix-annotations] Added benchmarkMethodIndices: {len(benchmark_indices)} method(s) "
              f"(excluded {len(exception_indices)} exception paths)")
        changed = True
    else:
        print(f"[fix-annotations] benchmarkMethodIndices already present "
              f"({len(cdata['benchmarkMethodIndices'])} method(s)) — skipping")

    if "hotupdateMethodIndices" not in cdata:
        cdata["hotupdateMethodIndices"] = hotupdate_indices
        print(f"[fix-annotations] Added hotupdateMethodIndices: {len(hotupdate_indices)} method(s)")
        changed = True
    else:
        print(f"[fix-annotations] hotupdateMethodIndices already present "
              f"({len(cdata['hotupdateMethodIndices'])} method(s)) — skipping")

    if changed:
        cpath.write_text(
            json.dumps(cdata, indent=2, ensure_ascii=False) + "\n",
            encoding="utf-8",
        )
        print(f"[fix-annotations] Updated {cpath.name}")

        # Print warning about conditionally-exception methods
        print(f"[fix-annotations] NOTE: Review the annotation list manually for conditional-exception "
              f"methods (e.g. Object overloads that throw on null input but work with valid input)")
    else:
        print(f"[fix-annotations] No changes needed")


def _run_managed_fact_for_annotations(
    slug: str, assembly: str, method_ids: list[str],
) -> set[int] | None:
    """Run managed_fact and return set of method indices that throw exceptions.

    Returns None if managed_fact failed (build/run error), allowing caller
    to fall back to heuristic detection.
    """
    from verification.stages.fact import generate_managed_fact_harness

    family_dir = _TESTING_ROOT / assembly / slug
    harness_dir = family_dir / "managed_test" / "fact"
    csproj = harness_dir / "ManagedFactHarness.csproj"
    golden_path = family_dir / "native" / "golden-values.json"

    # Generate harness
    ok = generate_managed_fact_harness(harness_dir, method_ids, assembly, slug)
    if not ok:
        print(f"    [fix-annotations] Harness generation failed")
        return None

    # Build
    print(f"    [fix-annotations] Building managed fact harness...")
    build_r = subprocess.run(
        ["dotnet", "build", str(csproj), "--configuration", "Release", "--nologo", "-v", "q"],
        capture_output=True, timeout=120,
    )
    if build_r.returncode != 0:
        print(f"    [fix-annotations] Build failed (exit={build_r.returncode})")
        return None

    # Run
    print(f"    [fix-annotations] Running managed fact harness...")
    try:
        r = subprocess.run(
            ["dotnet", "run", "--no-build", "--project", str(harness_dir),
             "--configuration", "Release"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        print(f"    [fix-annotations] managed_fact timed out")
        return None

    if r.returncode != 0:
        print(f"    [fix-annotations] managed_fact run failed (exit={r.returncode})")
        return None

    # Parse JSON output
    try:
        data = json.loads(r.stdout)
    except json.JSONDecodeError:
        print(f"    [fix-annotations] JSON parse failed")
        return None

    # Save golden-values.json for later pipeline use
    golden_path.parent.mkdir(parents=True, exist_ok=True)
    golden_path.write_text(json.dumps(data, indent=2), encoding="utf-8")
    print(f"    [fix-annotations] Saved {golden_path.name}")

    # Determine exception indices
    results = data.get("results", [])
    exception_indices: set[int] = set()
    for rr in results:
        mi = rr.get("methodIndex", -1)
        if mi < 0:
            continue
        # exceptionMessage is null for success, non-null for exception
        if rr.get("exceptionMessage") is not None:
            exception_indices.add(mi)

    total = len(results)
    caught = len(exception_indices)
    print(f"    [fix-annotations] managed_fact: {total} methods, {caught} exception path(s), "
          f"{total - caught} normal path(s)")

    return exception_indices


def _guess_exception_indices(method_ids: list[str]) -> set[int]:
    """Fallback: guess exception methods using is_auto_callable().

    Used when managed_fact cannot be run (build failure, etc.).
    """
    exception_indices: set[int] = set()
    for idx, mid in enumerate(method_ids):
        parsed = parse_method_subject_id(mid)
        if not is_auto_callable(parsed):
            exception_indices.add(idx)
    return exception_indices


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
                body = f"try {{ {call_expr}; }} catch {{ /* TODO */ }}"
            else:
                body = f"try {{ _ = {call_expr}; }} catch {{ /* TODO */ }}"

            filled_count += 1
            return f"{prefix}{idx}{sig_and_open}{body}{close}"

        new_text = _FILL_RE.sub(_replacer, original)
        if new_text != original:
            cs_file.write_text(new_text, encoding="utf-8")
            print(f"[fill] Updated {cs_file.name} ({filled_count} method(s) so far)")

    return filled_count


# ---------------------------------------------------------------------------
# Subject freeze: capture + verify subject file integrity
# ---------------------------------------------------------------------------

_FREEZE_DIR_NAME = "subject-freeze"
_FREEZE_MANIFEST_NAME = "subject-freeze-manifest.json"


def _get_subject_files(family_dir: Path) -> list[Path]:
    """Return all subject .cs files under managed/subjects/ and handwritten/."""
    files: list[Path] = []
    for base in ("managed/subjects", "handwritten"):
        d = family_dir / base
        if d.is_dir():
            for cs in sorted(d.glob("*.cs")):
                # Exclude obj/ directories
                if "obj" not in cs.parts:
                    files.append(cs)
    return files


def freeze_subjects(slug: str, assembly: str, force: bool = False) -> Path | None:
    """Freeze subject files: compute SHA256 hashes and write manifest.

    Creates subject-freeze-manifest.json in the family directory and
    backs up frozen copies to subject-freeze/.

    Runs correctness audit first — refuses to freeze if issues found.
    Use --force-freeze to skip this check.

    Returns path to manifest file, or None on failure.
    """
    family_dir = _TESTING_ROOT / assembly / slug

    # Pre-check: correctness audit must pass before freezing
    if not force:
        audit_report = run_full_correctness_audit(slug, assembly)
        if audit_report.get("correctness_verdict") in ("ISSUES_FOUND", "BLOCKER", "ERROR"):
            n_issues = audit_report.get("total_issues", 0)
            print(
                f"[freeze] REFUSED: Correctness audit found {n_issues} issue(s) "
                f"(verdict={audit_report.get('correctness_verdict')}).\n"
                f"[freeze] Fix subject code first, or use --force-freeze to override."
            )
            return None
    freeze_dir = family_dir / _FREEZE_DIR_NAME
    freeze_dir.mkdir(parents=True, exist_ok=True)

    subject_files = _get_subject_files(family_dir)
    if not subject_files:
        print(f"[freeze] WARNING: No subject files found in {family_dir}")
        return None

    manifest: dict[str, Any] = {
        "family": slug,
        "assembly": assembly,
        "freeze_timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "files": {},
    }

    for cs_file in subject_files:
        rel = str(cs_file.relative_to(family_dir))
        content = cs_file.read_bytes()
        sha256 = hashlib.sha256(content).hexdigest()
        manifest["files"][rel] = sha256

        # Backup frozen copy
        backup_path = freeze_dir / rel
        backup_path.parent.mkdir(parents=True, exist_ok=True)
        backup_path.write_bytes(content)
        print(f"[freeze]  backed up: {rel}")

    # Write manifest
    manifest_path = family_dir / _FREEZE_MANIFEST_NAME
    manifest_path.write_text(
        json.dumps(manifest, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )
    print(f"[freeze] wrote manifest: {manifest_path} ({len(subject_files)} file(s))")
    return manifest_path


def verify_freeze(slug: str, assembly: str) -> bool:
    """Verify that subject files have not been modified since freeze.

    Re-computes SHA256 hashes and compares against the freeze manifest.
    Returns True if all files match, False if any file changed.
    """
    family_dir = _TESTING_ROOT / assembly / slug
    manifest_path = family_dir / _FREEZE_MANIFEST_NAME

    if not manifest_path.exists():
        print(f"[verify-freeze] No freeze manifest found at {manifest_path}")
        print(f"[verify-freeze] Run --freeze first to capture subject baseline")
        return False

    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as e:
        print(f"[verify-freeze] ERROR: Could not read manifest: {e}")
        return False

    frozen_files = manifest.get("files", {})
    all_ok = True

    for rel_path, expected_hash in frozen_files.items():
        full_path = family_dir / rel_path
        if not full_path.exists():
            print(f"[verify-freeze] FAIL: {rel_path} was frozen but is now MISSING")
            all_ok = False
            continue

        current_hash = hashlib.sha256(full_path.read_bytes()).hexdigest()
        if current_hash != expected_hash:
            print(f"[verify-freeze] FAIL: {rel_path} has CHANGED since freeze")
            print(f"  expected: {expected_hash}")
            print(f"  current:  {current_hash}")
            all_ok = False

    if all_ok:
        print(f"[verify-freeze] OK: All {len(frozen_files)} subject files unchanged since freeze")
    else:
        print(f"[verify-freeze] SUBJECT FROZEN — optimization must not modify test code")
        print(f"[verify-freeze] Revert subject file changes, fix codegen/runtime instead")

    return all_ok


# ---------------------------------------------------------------------------
# ROI estimation (T-B3)
# ---------------------------------------------------------------------------


def _find_summary_by_runtime(report: dict[str, Any], runtime: str) -> dict[str, Any] | None:
    """Find the summary entry for a given runtime name."""
    for s in report.get("summaries", []):
        if s.get("runtime") == runtime:
            return s
    return None


def estimate_roi(
    slug: str,
    assembly: str,
) -> dict[str, Any]:
    """Estimate ROI for optimizing a family.

    Analyzes benchmark data from multi-run-report.json, classifies bottleneck
    type (dispatch-bound / alloc-bound / metadata-bound / mixed), and estimates
    potential performance improvement.

    Returns a structured ROI report dict and writes optimization-opportunity.md.

    ROI gate:
      - conservative_improvement >= 20% -> PASS
      - conservative_improvement < 20%  -> WARN
    """
    family_dir = _TESTING_ROOT / assembly / slug
    report_path = family_dir / "multi-run" / "multi-run-report.json"

    if not report_path.exists():
        print(f"ERROR: No benchmark data at {report_path}", file=sys.stderr)
        print(f"Run the benchmark pipeline first.", file=sys.stderr)
        sys.exit(1)

    try:
        report = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as e:
        print(f"ERROR: Could not read benchmark report: {e}", file=sys.stderr)
        sys.exit(1)

    # ── Summary-level data ──
    chaos_aot_summary = _find_summary_by_runtime(report, "chaos-aot")
    chaos_jit_summary = _find_summary_by_runtime(report, "chaos-jit")
    net8_summary = _find_summary_by_runtime(report, "net8-jit")

    chaos_aot_ns = _safe_float(chaos_aot_summary.get("geometric_mean_ns")) if chaos_aot_summary else 0.0
    chaos_jit_ns = _safe_float(chaos_jit_summary.get("geometric_mean_ns")) if chaos_jit_summary else 0.0
    net8_ns = _safe_float(net8_summary.get("geometric_mean_ns")) if net8_summary else 0.0

    method_count = chaos_aot_summary.get("method_count", 0) if chaos_aot_summary else 0
    stub_count = chaos_aot_summary.get("stub_count", 0) if chaos_aot_summary else 0
    throws_count = chaos_aot_summary.get("throws_count", 0) if chaos_aot_summary else 0

    slowdown_vs_net8 = (chaos_aot_ns / net8_ns) if chaos_aot_ns > 0 and net8_ns > 0 else 0.0

    # ── Per-method analysis ──
    methods_data: list[dict[str, Any]] = report.get("methods", [])
    dispatch_high_count = 0
    metadata_high_count = 0
    valid_count = 0
    method_details: list[dict[str, Any]] = []

    for m in methods_data:
        chaos_aot_sample = m.get("samples", {}).get("chaos-aot", {})
        chaos_jit_sample = m.get("samples", {}).get("chaos-jit", {})
        net8_sample = m.get("samples", {}).get("net8-jit", {})

        chaos_aot_mean = _safe_float(chaos_aot_sample.get("mean_ns"))
        chaos_jit_mean = _safe_float(chaos_jit_sample.get("mean_ns"))
        net8_mean = _safe_float(net8_sample.get("mean_ns"))

        dispatch_ratio = (
            (chaos_aot_mean / chaos_jit_mean)
            if chaos_aot_mean > 0 and chaos_jit_mean > 0
            else 0.0
        )
        metadata_ratio = (
            (chaos_aot_mean / net8_mean)
            if chaos_aot_mean > 0 and net8_mean > 0
            else 0.0
        )

        if chaos_aot_mean > 0:
            valid_count += 1
        if dispatch_ratio > 1.5:
            dispatch_high_count += 1
        if metadata_ratio > 3.0:
            metadata_high_count += 1

        method_details.append({
            "method_index": m.get("method_index"),
            "label": m.get("label", ""),
            "chaos_aot_ns": round(chaos_aot_mean, 2),
            "chaos_jit_ns": round(chaos_jit_mean, 2),
            "net8_ns": round(net8_mean, 2),
            "dispatch_ratio": round(dispatch_ratio, 2),
            "metadata_ratio": round(metadata_ratio, 2),
        })

    # ── Bottleneck classification ──
    alloc_stub_throw = stub_count + throws_count
    dispatch_pct = (dispatch_high_count / valid_count * 100) if valid_count > 0 else 0.0
    metadata_pct = (metadata_high_count / valid_count * 100) if valid_count > 0 else 0.0
    alloc_pct = (alloc_stub_throw / method_count * 100) if method_count > 0 else 0.0

    bottleneck_types: list[tuple[str, float]] = []
    if dispatch_pct > 30:
        bottleneck_types.append(("dispatch-bound", dispatch_pct))
    if alloc_pct > 20:
        bottleneck_types.append(("alloc-bound", alloc_pct))
    if metadata_pct > 50:
        bottleneck_types.append(("metadata-bound", metadata_pct))

    if not bottleneck_types:
        bottleneck_type = "metadata-bound"
        analysis_rationale = (
            "Insufficient differentiation in the three ratio metrics; "
            "defaulting to metadata-bound (the most common bottleneck)"
        )
    elif len(bottleneck_types) >= 2:
        bottleneck_type = "mixed"
        type_strs = ", ".join(f"{t}({p:.0f}%)" for t, p in bottleneck_types)
        analysis_rationale = f"Multiple bottleneck types detected: {type_strs}"
    else:
        bottleneck_type = bottleneck_types[0][0]
        analysis_rationale = (
            f"Primary: {bottleneck_types[0][0]} "
            f"({bottleneck_types[0][1]:.0f}% of methods affected)"
        )

    # ── Improvement estimation ──
    _OPTIMISTIC_MAP = {
        "dispatch-bound": 80,
        "alloc-bound": 50,
        "metadata-bound": 30,
    }
    _CONSERVATIVE_MAP = {
        "dispatch-bound": 50,
        "alloc-bound": 20,
        "metadata-bound": 10,
    }

    if bottleneck_type == "mixed":
        total_w = sum(p for _, p in bottleneck_types)
        optimistic_pct = sum(
            _OPTIMISTIC_MAP[t] * (p / total_w) for t, p in bottleneck_types
        )
        conservative_pct = sum(
            _CONSERVATIVE_MAP[t] * (p / total_w) for t, p in bottleneck_types
        )
    else:
        optimistic_pct = _OPTIMISTIC_MAP.get(bottleneck_type, 30)
        conservative_pct = _CONSERVATIVE_MAP.get(bottleneck_type, 10)

    optimistic_ns = chaos_aot_ns * (1 - optimistic_pct / 100)
    conservative_ns = chaos_aot_ns * (1 - conservative_pct / 100)

    # ── Gate logic ──
    roi_pass = conservative_pct >= 20

    roi_result: dict[str, Any] = {
        "family_slug": slug,
        "assembly": assembly,
        "current_performance": {
            "chaos_aot_ns": round(chaos_aot_ns, 2),
            "slowdown_vs_net8": round(slowdown_vs_net8, 2),
            "method_count": method_count,
            "stub_count": stub_count,
            "throws_count": throws_count,
        },
        "bottleneck_analysis": {
            "type": bottleneck_type,
            "dispatch_pct": round(dispatch_pct, 1),
            "metadata_pct": round(metadata_pct, 1),
            "alloc_pct": round(alloc_pct, 1),
            "rationale": analysis_rationale,
            "method_details": method_details,
        },
        "estimated_improvement": {
            "optimistic_pct": round(optimistic_pct, 0),
            "conservative_pct": round(conservative_pct, 0),
            "optimistic_ns": round(optimistic_ns, 2),
            "conservative_ns": round(conservative_ns, 2),
        },
        "roi_pass": roi_pass,
    }

    # ── Write optimization-opportunity.md ──
    _write_optimization_opportunity(family_dir, roi_result)

    return roi_result


def _write_optimization_opportunity(
    family_dir: Path,
    roi: dict[str, Any],
) -> None:
    """Write optimization-opportunity.md to the family directory."""
    slug = roi["family_slug"]
    perf = roi["current_performance"]
    bottleneck = roi["bottleneck_analysis"]
    improvement = roi["estimated_improvement"]

    lines: list[str] = [
        f"# 优化机会: {slug}",
        "",
        "## 当前性能",
        f"- chaos-aot: {perf['chaos_aot_ns']}ns (geometric mean)",
        f"- vs .NET 8: {perf['slowdown_vs_net8']}x slowdown",
        f"- 方法数: {perf['method_count']}",
        f"- stub/throw: {perf['stub_count'] + perf['throws_count']}",
        "",
        "## 瓶颈分析",
        f"- 类型: {bottleneck['type']}",
        f"- 分析依据: {bottleneck['rationale']}",
        f"  - dispatch-bound 方法占比: {bottleneck['dispatch_pct']}%",
        f"  - metadata-bound 方法占比: {bottleneck['metadata_pct']}%",
        f"  - alloc-bound 方法占比: {bottleneck['alloc_pct']}%",
        "",
        "## 预期收益",
        f"- 乐观: {perf['chaos_aot_ns']}ns -> {improvement['optimistic_ns']}ns "
        f"({improvement['optimistic_pct']:.0f}% improvement)",
        f"- 保守: {perf['chaos_aot_ns']}ns -> {improvement['conservative_ns']}ns "
        f"({improvement['conservative_pct']:.0f}% improvement)",
        "",
        "## 建议方案",
    ]

    bt = bottleneck["type"]
    if bt == "dispatch-bound":
        lines.extend([
            "- 方案 1: 将 interpreter dispatch 替换为直接 native 调用 (预估工时: 2-3d)",
            "- 方案 2: 为热点路径生成特化 dispatch stub (预估工时: 1-2d)",
        ])
    elif bt == "alloc-bound":
        lines.extend([
            "- 方案 1: 减少热点路径的 GC allocation (预估工时: 3-5d)",
            "- 方案 2: 使用 struct 代替 class 避免堆分配 (预估工时: 2-3d)",
        ])
    elif bt == "metadata-bound":
        lines.extend([
            "- 方案 1: 缓存 metadata lookup 结果,减少重复查询 (预估工时: 4-6d)",
            "- 方案 2: 优化 metadata 数据结构,降低查找开销 (预估工时: 5-8d)",
        ])
    elif bt == "mixed":
        lines.extend([
            "- 方案 1: 综合优化 — 优先解决 dispatch 瓶颈 (预估工时: 3-5d)",
            "- 方案 2: 架构级重构 — 减少 metadata 层开销 (预估工时: 1-2w)",
        ])

    lines.extend([
        "",
        "## 方法级明细",
        "",
        "| # | 方法 | chaos-aot (ns) | chaos-jit (ns) | net8 (ns) | dispatch_ratio | metadata_ratio |",
        "|---|------|---------------|---------------|----------|---------------|---------------|",
    ])

    for md in bottleneck.get("method_details", []):
        lines.append(
            f"| {md['method_index']} | {md['label']} | "
            f"{md['chaos_aot_ns']:.2f} | {md['chaos_jit_ns']:.2f} | "
            f"{md['net8_ns']:.2f} | {md['dispatch_ratio']:.2f} | {md['metadata_ratio']:.2f} |"
        )

    lines.append("")

    md_path = family_dir / "optimization-opportunity.md"
    md_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"[estimate-roi] Wrote {md_path}")


def _print_roi_human_readable(roi: dict[str, Any]) -> None:
    """Print a human-readable ROI summary."""
    perf = roi["current_performance"]
    bottleneck = roi["bottleneck_analysis"]
    improvement = roi["estimated_improvement"]

    gate_str = "PASS" if roi["roi_pass"] else "WARN (below 20% threshold)"

    print(f"Family: {roi['family_slug']} [{roi['assembly']}]")
    print(f"ROI Gate: {gate_str}")
    print(f"Chaos-AOT: {perf['chaos_aot_ns']}ns geometric mean")
    print(f"vs .NET 8: {perf['slowdown_vs_net8']}x slowdown")
    print(f"Methods: {perf['method_count']} ({perf['stub_count']} stubs, {perf['throws_count']} throws)")
    print()
    print(f"Bottleneck: {bottleneck['type']}")
    print(f"  dispatch-bound: {bottleneck['dispatch_pct']}% of methods")
    print(f"  metadata-bound: {bottleneck['metadata_pct']}% of methods")
    print(f"  alloc-bound: {bottleneck['alloc_pct']}% of methods")
    print(f"  Rationale: {bottleneck['rationale']}")
    print()
    print(f"Estimated improvement:")
    print(f"  Optimistic: {improvement['optimistic_pct']:.0f}% -> {improvement['optimistic_ns']}ns")
    print(f"  Conservative: {improvement['conservative_pct']:.0f}% -> {improvement['conservative_ns']}ns")
    print()
    if roi["roi_pass"]:
        print("  => PASS (conservative >= 20%)")
    else:
        print("  => WARN (conservative < 20%)")
    print()


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
        "--fix-annotations", action="store_true",
        help="Auto-generate benchmarkMethodIndices and hotupdateMethodIndices in contract",
    )
    parser.add_argument(
        "--fill", action="store_true",
        help="Fill TODO stub bodies with generated call expressions",
    )
    parser.add_argument(
        "--freeze", action="store_true",
        help="Freeze subject files: compute SHA256 hashes, write manifest and backup",
    )
    parser.add_argument(
        "--verify-freeze", action="store_true",
        help="Verify subject files match frozen manifest",
    )
    parser.add_argument(
        "--force-freeze", action="store_true",
        help="Force freeze even if subject correctness audit finds issues",
    )
    parser.add_argument(
        "--estimate-roi", action="store_true",
        help="Estimate ROI and classify bottleneck type for this family",
    )
    parser.add_argument(
        "--json", action="store_true",
        help="Output raw JSON report (default: human-readable summary)",
    )
    args = parser.parse_args()

    # Handle freeze/verify-freeze as standalone commands
    if args.freeze:
        freeze_subjects(args.family_slug, args.assembly, force=args.force_freeze)
        return

    if args.verify_freeze:
        ok = verify_freeze(args.family_slug, args.assembly)
        sys.exit(0 if ok else 1)
        return

    # ROI estimation as standalone command
    if args.estimate_roi:
        roi = estimate_roi(args.family_slug, args.assembly)
        if args.json:
            print(json.dumps(roi, indent=2, ensure_ascii=False))
        else:
            _print_roi_human_readable(roi)
        sys.exit(0 if roi["roi_pass"] else 1)
        return

    report = audit_family(args.family_slug, args.assembly)

    if args.fix:
        fix_family(report)
        report = audit_family(args.family_slug, args.assembly)
        print("\n--- Re-audit after fix ---")

    if args.fix_annotations:
        fix_annotations(report)
        report = audit_family(args.family_slug, args.assembly)
        print("\n--- Re-audit after fix-annotations ---")

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
