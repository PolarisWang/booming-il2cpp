"""Principle Auto-Checks — 7 fully automatable principle alignment checks.

Each check maps to one item in the Principle Evaluation Matrix (SKILL.md).
All checks are objective — no subjective judgment required.

Usage:
    python principle_auto_checks.py --family convert-char
    python principle_auto_checks.py --family convert-char --verbose
    python principle_auto_checks.py --family convert-char --output report.json
    python principle_auto_checks.py --assembly System.Private.CoreLib  # all families
"""

from __future__ import annotations

import json
import re
import sys
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"

try:
    from testing.trace import trace
except ImportError:
    def trace(*args, **kwargs):
        pass


# ── Data types ─────────────────────────────────────────────────────────

@dataclass
class PrincipleCheckResult:
    check_id: str
    principle: str          # "P1" | "P2" | "P3"
    status: str             # "ALIGNED" | "VIOLATION" | "CONCERN" | "NOT_APPLICABLE"
    summary: str = ""
    details: list[dict] = field(default_factory=list)
    evidence: dict = field(default_factory=dict)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


# ── Helpers ────────────────────────────────────────────────────────────

def _family_dir(assembly: str, family_slug: str) -> Path:
    return _VERIFICATION_BASE / assembly / family_slug


def _generated_cpp(assembly: str, family_slug: str) -> Path | None:
    path = _family_dir(assembly, family_slug) / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    return path if path.exists() else None


def _method_count_in_cpp(cpp: str) -> int:
    """Count the number of native entry methods in generated C++."""
    return len(re.findall(r'(?:NativeReferenceStub_|ConvertCharNativeEntry_|NativeEntry_|BenchmarkEntry_)\w+\s*\(', cpp))


def _has_real_lowering(cpp: str) -> bool:
    """Check if the generated C++ contains chaos_eval_stack lowering."""
    return "chaos_eval_stack" in cpp


def _parse_method_names(cpp: str) -> list[str]:
    """Extract method names from the generated C++."""
    methods = []
    for m in re.finditer(r'(?:NativeReferenceStub_|ConvertCharNativeEntry_|NativeEntry_|RunNativeAot_)(\w+)\s*\(', cpp):
        methods.append(m.group(0).rstrip('('))
    return methods


# ── P1 Checks ──────────────────────────────────────────────────────────

def check_p1_lowering(assembly: str, family_slug: str) -> PrincipleCheckResult:
    """P1: Generated code uses chaos_eval_stack lowering, not SimpleForward."""
    cpp_path = _generated_cpp(assembly, family_slug)
    if cpp_path is None:
        return PrincipleCheckResult(
            check_id="p1_lowering", principle="P1",
            status="VIOLATION",
            summary="Generated C++ not found — no lowering exists",
        )

    cpp = cpp_path.read_text(encoding="utf-8")

    # Check for chaos_eval_stack (real lowering)
    has_lowering = _has_real_lowering(cpp)

    # Check for stub patterns
    stub_count = len(re.findall(r'NativeReferenceStub_\w+', cpp))
    simple_forward_count = len(re.findall(r'SimpleForward', cpp))

    if not has_lowering:
        return PrincipleCheckResult(
            check_id="p1_lowering", principle="P1",
            status="VIOLATION",
            summary="No chaos_eval_stack lowering found in generated C++",
            evidence={"has_lowering": False, "stub_count": stub_count, "simple_forward_count": simple_forward_count},
        )

    concerns = []
    if stub_count > 0:
        concerns.append(f"{stub_count} stub entries present")
    if simple_forward_count > 0:
        concerns.append(f"{simple_forward_count} SimpleForward usages — lowering may be incomplete")

    status = "VIOLATION" if simple_forward_count > 0 else "ALIGNED"
    return PrincipleCheckResult(
        check_id="p1_lowering", principle="P1",
        status=status,
        summary="chaos_eval_stack present [OK]" + (f" ({'; '.join(concerns)})" if concerns else ""),
        evidence={
            "has_lowering": True,
            "stub_count": stub_count,
            "simple_forward_count": simple_forward_count,
        },
    )


def check_p1_no_redundant_branch(assembly: str, family_slug: str) -> PrincipleCheckResult:
    """P1: Generated code has no runtime family/method dispatch branches."""
    cpp_path = _generated_cpp(assembly, family_slug)
    if cpp_path is None:
        return PrincipleCheckResult(
            check_id="p1_no_redundant_branch", principle="P1",
            status="VIOLATION",
            summary="Generated C++ not found",
        )

    cpp = cpp_path.read_text(encoding="utf-8")

    # Patterns that indicate runtime dispatch branches in generated code
    branch_patterns = [
        (r'if\s*\(\s*family\s*[=!]=', "family-based branch"),
        (r'switch\s*\(\s*methodIndex\b', "methodIndex switch"),
        (r'if\s*\(\s*methodIndex\b', "methodIndex branch"),
        (r'CHAOS_FAMILY_SPECIFIC', "CHAOS_FAMILY_SPECIFIC marker"),
        (r'if\s*\(\s*__family\b', "runtime family check"),
    ]

    violations = []
    for pattern, label in branch_patterns:
        matches = re.findall(pattern, cpp, re.IGNORECASE)
        if matches:
            violations.append(f"{label} ({len(matches)} occurrences)")

    if violations:
        return PrincipleCheckResult(
            check_id="p1_no_redundant_branch", principle="P1",
            status="VIOLATION",
            summary="Runtime dispatch branches detected in generated code",
            details=[{"pattern": v} for v in violations],
            evidence={"violation_count": len(violations), "violations": violations},
        )

    return PrincipleCheckResult(
        check_id="p1_no_redundant_branch", principle="P1",
        status="ALIGNED",
        summary="No runtime dispatch branches in generated code",
        evidence={"checked_patterns": [p for p, _ in branch_patterns]},
    )


def check_p1_no_unnecessary_indirection(assembly: str, family_slug: str) -> PrincipleCheckResult:
    """P1: Generated code calls runtime_stubs directly instead of through unnecessary wrappers."""
    cpp_path = _generated_cpp(assembly, family_slug)
    if cpp_path is None:
        return PrincipleCheckResult(
            check_id="p1_no_unnecessary_indirection", principle="P1",
            status="VIOLATION",
            summary="Generated C++ not found",
        )

    cpp = cpp_path.read_text(encoding="utf-8")

    # Known runtime_stubs functions that indicate indirection
    stub_functions = [
        "ChaosArrayClear", "ChaosArrayGetLength",
        "ChaosDatetimeGetUtcNow", "ChaosDatetimeGetHashCode",
        "ChaosMathSqrt", "ChaosInterlockedMemoryBarrier",
        "ChaosExceptionGetBaseException", "ChaosExceptionGetInnerException",
        "ChaosObjectEqualsStatic", "ChaosObjectCtor",
        "ChaosGuidNewGuid", "ChaosRandomNextBytes", "ChaosRandomNextDouble",
        "ChaosStringContains", "ChaosStringStartsWith", "ChaosStringJoinSs",
        "ChaosFormattablestringFactoryCreate",
    ]

    violations = []
    for fn in stub_functions:
        count = len(re.findall(rf'\b{fn}\s*\(', cpp))
        if count > 0:
            violations.append({"function": fn, "call_count": count})

    if violations:
        return PrincipleCheckResult(
            check_id="p1_no_unnecessary_indirection", principle="P1",
            status="CONCERN",
            summary=f"{len(violations)} runtime_stub calls in generated code",
            details=violations,
            evidence={"stub_call_count": sum(v["call_count"] for v in violations)},
        )

    return PrincipleCheckResult(
        check_id="p1_no_unnecessary_indirection", principle="P1",
        status="ALIGNED",
        summary="No unnecessary runtime_stub calls in generated code",
    )


def check_p1_benchmark(assembly: str, family_slug: str) -> PrincipleCheckResult:
    """P1: Benchmark shows no significant performance regression."""
    bm_path = _family_dir(assembly, family_slug) / "benchmark-comparison-report.json"
    if not bm_path.exists():
        return PrincipleCheckResult(
            check_id="p1_benchmark", principle="P1",
            status="NOT_APPLICABLE",
            summary="No benchmark data available",
        )

    try:
        bm = json.loads(bm_path.read_bytes())
    except (json.JSONDecodeError, OSError):
        return PrincipleCheckResult(
            check_id="p1_benchmark", principle="P1",
            status="CONCERN",
            summary="Benchmark report unreadable",
        )

    summary = bm.get("summary", {})
    avg_speedup = summary.get("averageSpeedupPercent", 0)
    managed_faster = summary.get("managedFasterCount", 0)
    native_faster = summary.get("nativeFasterCount", 0)

    if isinstance(avg_speedup, (int, float)) and avg_speedup < -50:
        return PrincipleCheckResult(
            check_id="p1_benchmark", principle="P1",
            status="VIOLATION",
            summary=f"Average speedup {avg_speedup}% — native significantly slower than managed",
            evidence={
                "average_speedup_percent": avg_speedup,
                "managed_faster_count": managed_faster,
                "native_faster_count": native_faster,
            },
        )

    concerns = []
    if managed_faster > 0:
        concerns.append(f"{managed_faster} methods where native is slower than managed")
    if isinstance(avg_speedup, (int, float)) and avg_speedup < -5:
        concerns.append(f"average speedup {avg_speedup}% (native slightly slower overall)")

    status = "ALIGNED" if not concerns else "CONCERN"
    return PrincipleCheckResult(
        check_id="p1_benchmark", principle="P1",
        status=status,
        summary=f"avg_speedup={avg_speedup}%" + (f" ({'; '.join(concerns)})" if concerns else ""),
        evidence={
            "average_speedup_percent": avg_speedup,
            "managed_faster_count": managed_faster,
            "native_faster_count": native_faster,
        },
    )


# ── P2 Checks ──────────────────────────────────────────────────────────

def check_p2_template_hygiene(assembly: str = "", family_slug: str = "") -> PrincipleCheckResult:
    """P2: Scriban templates don't contain semantic decision logic.

    Scans all .scriban templates in the codegen Templates directory.
    This is a repo-level check, not family-specific.
    """
    templates_dir = _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Templates"
    if not templates_dir.exists():
        return PrincipleCheckResult(
            check_id="p2_template_hygiene", principle="P2",
            status="NOT_APPLICABLE",
            summary="Templates directory not found",
        )

    # Patterns in Scriban that suggest semantic decision logic
    semantic_patterns = [
        (r'\{\%\s*if\s+method\.\w+\s*\%\}', "method-level semantic if"),
        (r'\{\%\s*if\s+type_shape\s*==', "type_shape-based semantic branch"),
        (r'\{\%\s*if\s+\.\w+_kind\b', "kind-based semantic branch"),
        (r'\{\%\s*if\s+\.\w+_shape\b', "shape-based semantic branch"),
    ]

    violations = []
    for tpl_file in sorted(templates_dir.rglob("*.scriban")):
        rel = tpl_file.relative_to(templates_dir)
        content = tpl_file.read_text(encoding="utf-8")
        for pattern, label in semantic_patterns:
            matches = re.findall(pattern, content)
            if matches:
                violations.append({
                    "template": str(rel),
                    "pattern": label,
                    "count": len(matches),
                })

    if violations:
        return PrincipleCheckResult(
            check_id="p2_template_hygiene", principle="P2",
            status="CONCERN",
            summary=f"{len(violations)} semantic patterns in {len(set(v['template'] for v in violations))} templates",
            details=violations,
            evidence={"template_count_with_concerns": len(set(v["template"] for v in violations))},
        )

    return PrincipleCheckResult(
        check_id="p2_template_hygiene", principle="P2",
        status="ALIGNED",
        summary="All Scriban templates clean — no semantic decision logic",
    )


# ── P3 Checks ──────────────────────────────────────────────────────────

def check_p3_patch_entry(assembly: str, family_slug: str) -> PrincipleCheckResult:
    """P3: Methods have dispatch_table entries (interrupt_ptr for hotpatch)."""
    cpp_path = _generated_cpp(assembly, family_slug)
    if cpp_path is None:
        return PrincipleCheckResult(
            check_id="p3_patch_entry", principle="P3",
            status="NOT_APPLICABLE",
            summary="Generated C++ not found",
        )

    cpp = cpp_path.read_text(encoding="utf-8")

    # Parse dispatch table entries from the generated C++
    dispatch_match = re.search(r'static HotpatchEntryV0\s+\w+\[(\d+)\]\s*=\s*\{(.*?)\};', cpp, re.DOTALL)
    if not dispatch_match:
        return PrincipleCheckResult(
            check_id="p3_patch_entry", principle="P3",
            status="VIOLATION",
            summary="No dispatch table found in generated code",
        )

    table_size = int(dispatch_match.group(1))
    table_body = dispatch_match.group(2)

    # Check each entry has both native function and interpreter entry (interrupt_ptr)
    entries = re.findall(r'\{\s*reinterpret_cast<.*?>\s*\(&(\w+)\).*?,\s*reinterpret_cast<.*?>\s*\(&(\w+)\).*?\}', table_body)

    missing_interrupt = 0
    present = 0
    for native_fn, interrupt_fn in entries:
        if "InterpreterEntryDirect" in interrupt_fn:
            present += 1
        else:
            missing_interrupt += 1

    if missing_interrupt > 0:
        return PrincipleCheckResult(
            check_id="p3_patch_entry", principle="P3",
            status="VIOLATION",
            summary=f"{missing_interrupt}/{table_size} dispatch entries lack interrupt_ptr",
            evidence={
                "table_size": table_size,
                "with_interrupt_ptr": present,
                "without_interrupt_ptr": missing_interrupt,
            },
        )

    # Also check for HotpatchModuleV0 registration
    has_hotpatch_module = "HotpatchModuleV0" in cpp and "s_hotpatch_module" in cpp

    return PrincipleCheckResult(
        check_id="p3_patch_entry", principle="P3",
        status="ALIGNED" if has_hotpatch_module else "CONCERN",
        summary=f"Dispatch table: {table_size} entries, all with interrupt_ptr [OK]" +
                ("" if has_hotpatch_module else " (HotpatchModule not found — runtime registration may be incomplete)"),
        evidence={
            "table_size": table_size,
            "entries_with_interrupt_ptr": present,
            "has_hotpatch_module": has_hotpatch_module,
        },
    )


def check_p3_patchdata(assembly: str, family_slug: str) -> PrincipleCheckResult:
    """P3: Patch data files exist for hotupdate support."""
    family_dir = _family_dir(assembly, family_slug)

    # Search for .patchdata files in the family directory
    patchdata_files = list(family_dir.rglob("*.patchdata"))

    # Also check hotupdate-related generated files
    hotupdate_dir = family_dir / "il2cpp_dist" / "hotupdate"
    hotupdate_files = list(hotupdate_dir.rglob("*.patchdata")) if hotupdate_dir.exists() else []

    all_files = patchdata_files + hotupdate_files

    if not all_files:
        # Maybe hotupdate isn't applicable for this family — check if hotupdate runner exists
        hu_runner = family_dir / "hotupdate-verification-report.json"
        if hu_runner.exists():
            return PrincipleCheckResult(
                check_id="p3_patchdata", principle="P3",
                status="VIOLATION",
                summary="HotUpdate test exists but no .patchdata files found",
                evidence={"patchdata_count": 0, "hotupdate_report_exists": True},
            )
        return PrincipleCheckResult(
            check_id="p3_patchdata", principle="P3",
            status="NOT_APPLICABLE",
            summary="No patch data needed (not on hotupdate path)",
        )

    # Check files are non-trivial (not empty stubs)
    valid = [f for f in all_files if f.stat().st_size > 10]

    return PrincipleCheckResult(
        check_id="p3_patchdata", principle="P3",
        status="ALIGNED" if len(valid) == len(all_files) else "CONCERN",
        summary=f"{len(valid)}/{len(all_files)} valid .patchdata files",
        evidence={
            "patchdata_count": len(all_files),
            "valid_count": len(valid),
            "stub_patchdata_count": len(all_files) - len(valid),
            "files": [str(f.relative_to(_VERIFICATION_BASE)) for f in all_files],
        },
    )


# ── Aggregator ─────────────────────────────────────────────────────────

def run_all_checks(assembly: str, family_slug: str) -> dict[str, Any]:
    """Run all 7 automated principle checks for a family.

    Returns:
        {
            "family": "convert-char",
            "assembly": "System.Private.CoreLib",
            "checks": { ... per-check results ... },
            "summary": {
                "total": 7,
                "aligned": N,
                "concern": N,
                "violation": N,
                "not_applicable": N,
                "overall": "ALIGNED" | "CONCERN" | "VIOLATION"
            }
        }
    """
    trace("principle_auto_checks.run_all", stage="audit", family=family_slug)

    checks: dict[str, PrincipleCheckResult] = {}

    # P1 checks
    checks["p1_lowering"] = check_p1_lowering(assembly, family_slug)
    checks["p1_no_redundant_branch"] = check_p1_no_redundant_branch(assembly, family_slug)
    checks["p1_no_unnecessary_indirection"] = check_p1_no_unnecessary_indirection(assembly, family_slug)
    checks["p1_benchmark"] = check_p1_benchmark(assembly, family_slug)

    # P2 checks
    checks["p2_template_hygiene"] = check_p2_template_hygiene(assembly, family_slug)

    # P3 checks
    checks["p3_patch_entry"] = check_p3_patch_entry(assembly, family_slug)
    checks["p3_patchdata"] = check_p3_patchdata(assembly, family_slug)

    # Build summary
    status_counts: dict[str, int] = {}
    for c in checks.values():
        status_counts[c.status] = status_counts.get(c.status, 0) + 1

    # Determine overall: VIOLATION > CONCERN > ALIGNED > NOT_APPLICABLE
    if status_counts.get("VIOLATION", 0) > 0:
        overall = "VIOLATION"
    elif status_counts.get("CONCERN", 0) > 0:
        overall = "CONCERN"
    elif status_counts.get("ALIGNED", 0) > 0:
        overall = "ALIGNED"
    else:
        overall = "NOT_APPLICABLE"

    result = {
        "family": family_slug,
        "assembly": assembly,
        "generated_at": __import__("datetime").datetime.now().isoformat(),
        "checks": {k: v.to_dict() for k, v in checks.items()},
        "summary": {
            "total": len(checks),
            "aligned": status_counts.get("ALIGNED", 0),
            "concern": status_counts.get("CONCERN", 0),
            "violation": status_counts.get("VIOLATION", 0),
            "not_applicable": status_counts.get("NOT_APPLICABLE", 0),
            "overall": overall,
        },
    }

    trace("principle_auto_checks.result", stage="audit", family=family_slug,
          overall=overall, violations=status_counts.get("VIOLATION", 0))
    return result


def run_for_assembly(assembly: str) -> dict[str, Any]:
    """Run principle checks for all families in an assembly."""
    asm_dir = _VERIFICATION_BASE / assembly
    if not asm_dir.exists():
        return {"error": f"Assembly directory not found: {asm_dir}"}

    results: dict[str, Any] = {}
    for item in sorted(asm_dir.iterdir()):
        if not item.is_dir() or item.name.startswith("_"):
            continue
        # Only check families that have generated C++
        if _generated_cpp(assembly, item.name) is None:
            continue
        results[item.name] = run_all_checks(assembly, item.name)

    # Cross-family summary
    overall_statuses = [r["summary"]["overall"] for r in results.values()]
    violation_count = sum(1 for s in overall_statuses if s == "VIOLATION")
    concern_count = sum(1 for s in overall_statuses if s == "CONCERN")
    aligned_count = sum(1 for s in overall_statuses if s == "ALIGNED")

    return {
        "assembly": assembly,
        "generated_at": __import__("datetime").datetime.now().isoformat(),
        "families": results,
        "cross_family_summary": {
            "total_families": len(results),
            "aligned": aligned_count,
            "concern": concern_count,
            "violation": violation_count,
        },
    }


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Principle Auto-Checks")
    parser.add_argument("--family", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib",
                        help="Assembly name (default: System.Private.CoreLib)")
    parser.add_argument("--verbose", "-v", action="store_true")
    parser.add_argument("-o", "--output", help="Output JSON file path")
    args = parser.parse_args()

    if args.family:
        result = run_all_checks(args.assembly, args.family)
    else:
        result = run_for_assembly(args.assembly)

    output = json.dumps(result, indent=2, ensure_ascii=False)

    if args.output:
        Path(args.output).write_text(output, encoding="utf-8")
        print(f"Report written to: {args.output}")
    else:
        print(output)

    # Exit code reflects overall status
    if isinstance(result, dict) and "summary" in result:
        sys.exit(0 if result["summary"]["overall"] != "VIOLATION" else 1)
    elif isinstance(result, dict) and "cross_family_summary" in result:
        sys.exit(0 if result["cross_family_summary"]["violation"] == 0 else 1)


if __name__ == "__main__":
    main()
