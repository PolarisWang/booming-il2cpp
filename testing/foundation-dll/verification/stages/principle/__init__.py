"""Principle check aggregator — plugin-based principle alignment verification.

Replaces the monolith in principle_auto_checks.py with a plugin-based architecture.
"""

from __future__ import annotations

from datetime import datetime
from pathlib import Path
from typing import Any

from .base import (
    CheckMeta,
    CheckResult,
    FamilyContext,
    PrincipleCheck,
    _class_name_from_slug,
    _family_dir,
    _generated_cpp,
)
from .checks import discover_checks
from .context import CrossFamilyContext
from .formatter import format_family_report

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "testing" / "foundation-dll"


# ── Helpers ───────────────────────────────────────────────────────────

def _get_check_meta(check_id: str) -> CheckMeta:
    """Look up CheckMeta for a given check_id from the registry."""
    registry = discover_checks()
    cls = registry.get(check_id)
    if cls is None:
        return CheckMeta(check_id=check_id, principle="P2", severity="blocking")
    return cls.meta


# ── Overall computation ───────────────────────────────────────────────

def compute_overall(results: list[CheckResult]) -> str:
    """Compute overall from check results.

    blocking VIOLATION → VIOLATION
    No blocking VIOLATION, but blocking CONCERN → CONCERN
    Otherwise → ALIGNED
    """
    violations = [r for r in results if r.status == "VIOLATION"]
    concerns = [r for r in results if r.status == "CONCERN"]

    blocking_violations = [
        r for r in violations
        if _get_check_meta(r.check_id).severity == "blocking"
    ]
    blocking_concerns = [
        r for r in concerns
        if _get_check_meta(r.check_id).severity == "blocking"
    ]

    if blocking_violations:
        return "VIOLATION"
    if blocking_concerns:
        return "CONCERN"
    return "ALIGNED"


# ── Family-level runner ───────────────────────────────────────────────

def run_family_checks(
    assembly: str,
    family_slug: str,
    skip_checks: list[str] | None = None,
    cross_ctx: CrossFamilyContext | None = None,
) -> dict[str, Any]:
    """Run all registered principle checks for a single family.

    Returns the same dict shape as the old principle_auto_checks.run_all_checks()
    for backward compatibility:
        {
            "family": "...",
            "assembly": "...",
            "generated_at": "...",
            "checks": { "check_id": { ... }, ... },
            "summary": { "total": N, "aligned": N, "concern": N, "violation": N,
                         "not_applicable": N, "overall": "..." }
        }
    """
    registry = discover_checks()
    if skip_checks is None:
        skip_checks = []

    family_dir = _family_dir(assembly, family_slug)
    cpp_path = _generated_cpp(assembly, family_slug)

    ctx = FamilyContext(
        assembly=assembly,
        family_slug=family_slug,
        family_dir=family_dir,
        cpp_path=cpp_path,
        _cross_ctx=cross_ctx,
    )

    results: list[CheckResult] = []

    for check_id, check_cls in sorted(registry.items()):
        if check_id in skip_checks:
            results.append(CheckResult(
                check_id=check_id, status="SKIP",
                summary="Skipped by user request",
            ))
            continue
        # Cross-family checks are skipped during per-family runs unless
        # a CrossFamilyContext was provided (defensive — they don't use it)
        if check_cls.meta.scope == "cross_family" and cross_ctx is None:
            results.append(CheckResult(
                check_id=check_id, status="NOT_APPLICABLE",
                summary=f"Cross-family check — run with CrossFamilyContext",
            ))
            continue
        try:
            instance = check_cls()
            result = instance.run(ctx)
            results.append(result)
        except Exception as e:
            results.append(CheckResult(
                check_id=check_id, status="VIOLATION",
                summary=f"Check execution error: {e}",
                evidence={"error": str(e)},
            ))

    overall = compute_overall(results)

    status_counts: dict[str, int] = {}
    for r in results:
        status_counts[r.status] = status_counts.get(r.status, 0) + 1

    return {
        "family": family_slug,
        "assembly": assembly,
        "generated_at": datetime.now().isoformat(),
        "checks": {r.check_id: r.to_dict() for r in results},
        "summary": {
            "total": len(results),
            "aligned": status_counts.get("ALIGNED", 0),
            "concern": status_counts.get("CONCERN", 0),
            "violation": status_counts.get("VIOLATION", 0),
            "not_applicable": status_counts.get("NOT_APPLICABLE", 0),
            "overall": overall,
        },
    }


# ── Cross-family runner ───────────────────────────────────────────────

def run_cross_family_checks(
    assembly: str,
    cross_ctx: CrossFamilyContext,
) -> list[CheckResult]:
    """Run cross-family checks that need data from all families simultaneously.

    Cross-family checks declare scope="cross_family" in their CheckMeta.
    """
    registry = discover_checks()
    results: list[CheckResult] = []

    # Use the first family as a representative context for cross-family checks
    representative_ctx = None
    for ctx in cross_ctx.family_contexts.values():
        representative_ctx = ctx
        break
    if representative_ctx is None:
        return results

    for check_id, check_cls in sorted(registry.items()):
        if check_cls.meta.scope != "cross_family":
            continue
        try:
            instance = check_cls()
            result = instance.run(representative_ctx)
            results.append(result)
        except Exception as e:
            results.append(CheckResult(
                check_id=check_id, status="CONCERN",
                summary=f"Cross-family check error: {e}",
                evidence={"error": str(e)},
            ))

    return results


# ── Assembly-level runner ─────────────────────────────────────────────

def run_for_assembly(assembly: str) -> dict[str, Any]:
    """Run principle checks for all families in an assembly, including cross-family checks."""
    asm_dir = _VERIFICATION_BASE / assembly
    if not asm_dir.exists():
        return {"error": f"Assembly directory not found: {asm_dir}"}

    family_results: dict[str, Any] = {}

    # Collect all families with generated C++
    family_slugs: list[str] = []
    for item in sorted(asm_dir.iterdir()):
        if not item.is_dir() or item.name.startswith("_"):
            continue
        if _generated_cpp(assembly, item.name) is None:
            continue
        family_slugs.append(item.name)

    # Build cross-family context once
    cross_ctx = CrossFamilyContext.build(assembly, family_slugs) if family_slugs else None

    for slug in family_slugs:
        family_results[slug] = run_family_checks(assembly, slug, cross_ctx=cross_ctx)

    # Run cross-family checks
    cross_results: list[dict] = []
    if cross_ctx:
        for r in run_cross_family_checks(assembly, cross_ctx):
            cross_results.append(r.to_dict())

    overall_statuses = [r["summary"]["overall"] for r in family_results.values()]
    violation_count = sum(1 for s in overall_statuses if s == "VIOLATION")
    concern_count = sum(1 for s in overall_statuses if s == "CONCERN")
    aligned_count = sum(1 for s in overall_statuses if s == "ALIGNED")

    return {
        "assembly": assembly,
        "generated_at": datetime.now().isoformat(),
        "families": family_results,
        "cross_family": cross_results,
        "cross_family_summary": {
            "total_families": len(family_results),
            "aligned": aligned_count,
            "concern": concern_count,
            "violation": violation_count,
        },
    }


# ── Public API ────────────────────────────────────────────────────────

__all__ = [
    "run_family_checks",
    "run_cross_family_checks",
    "run_for_assembly",
    "compute_overall",
    "discover_checks",
    "CheckMeta",
    "CheckResult",
    "FamilyContext",
    "PrincipleCheck",
    "CrossFamilyContext",
]
