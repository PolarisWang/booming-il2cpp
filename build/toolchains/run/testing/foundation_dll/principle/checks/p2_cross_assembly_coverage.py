"""P2 (monitoring, cross-family): Cross-assembly contract coverage coherence.

Validates contract metadata consistency across all assemblies:
  1. No duplicate family slugs across assemblies
  2. familyId format consistency (family/{assembly}/{slug} pattern)
  3. Orphan check: families in ledger with no matching contract.json on disk
"""

from __future__ import annotations

import json
from pathlib import Path

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck, _VERIFICATION_BASE
from ..context import CrossFamilyContext

_LEDGER_PATH = _VERIFICATION_BASE / "_contracts" / "ledger.json"


class P2CrossAssemblyCoverageCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p2_cross_assembly_coverage",
        principle="P2",
        severity="monitoring",
        scope="cross_family",
        description="Cross-assembly contract coverage coherence",
    )

    _family_id_pattern = None

    def _load_ledger(self) -> dict | None:
        if not _LEDGER_PATH.exists():
            return None
        try:
            return json.loads(_LEDGER_PATH.read_bytes())
        except (OSError, json.JSONDecodeError):
            return None

    def run(self, ctx: FamilyContext) -> CheckResult:
        cross_ctx = getattr(ctx, "_cross_ctx", None)
        if cross_ctx is None or not isinstance(cross_ctx, CrossFamilyContext):
            return CheckResult(
                check_id="p2_cross_assembly_coverage", status="NOT_APPLICABLE",
                summary="Cross-family check requires CrossFamilyContext",
            )

        ledger = self._load_ledger()
        if ledger is None:
            return CheckResult(
                check_id="p2_cross_assembly_coverage", status="NOT_APPLICABLE",
                summary="Ledger not found — skipping cross-assembly coverage check",
            )

        assemblies = ledger.get("assemblies", [])
        issues: list[dict] = []

        # 1. Check for duplicate slugs across assemblies
        slug_registry: dict[str, list[str]] = {}
        for asm in assemblies:
            aname = asm.get("assemblyName", "")
            for fam in asm.get("families", []):
                slug = fam.get("slug", "")
                if slug:
                    slug_registry.setdefault(slug, []).append(aname)

        duplicates = {s: asms for s, asms in slug_registry.items() if len(asms) > 1}
        if duplicates:
            issues.append({
                "check": "duplicate_slugs",
                "detail": f"{len(duplicates)} slug(s) appear in multiple assemblies",
                "duplicates": duplicates,
            })

        # 2. Check familyId format consistency
        bad_family_ids = []
        for asm in assemblies:
            aname = asm.get("assemblyName", "")
            expected_prefix = f"family/{aname}/"
            for fam in asm.get("families", []):
                fid = fam.get("familyId", "")
                if fid and not fid.startswith(expected_prefix):
                    bad_family_ids.append({
                        "assembly": aname,
                        "slug": fam.get("slug", ""),
                        "familyId": fid,
                        "expected_prefix": expected_prefix,
                    })

        if bad_family_ids:
            issues.append({
                "check": "familyId_format",
                "detail": f"{len(bad_family_ids)} family(s) with unexpected familyId format",
                "bad_family_ids": bad_family_ids,
            })

        # 3. Orphan check: families in ledger without contract.json on disk
        orphans = []
        for asm in assemblies:
            aname = asm.get("assemblyName", "")
            for fam in asm.get("families", []):
                slug = fam.get("slug", "")
                contract_path = _VERIFICATION_BASE / aname / slug / "contract.json"
                if not contract_path.exists() and slug:
                    orphans.append({
                        "assembly": aname,
                        "slug": slug,
                    })

        if orphans:
            issues.append({
                "check": "orphan_families",
                "detail": f"{len(orphans)} ledger family(s) missing contract.json on disk",
                "orphans": orphans,
            })

        if not issues:
            return CheckResult(
                check_id="p2_cross_assembly_coverage", status="ALIGNED",
                summary=f"Contract coverage coherent across {len(assemblies)} assemblies",
                evidence={"assemblies_checked": len(assemblies)},
            )

        return CheckResult(
            check_id="p2_cross_assembly_coverage", status="CONCERN",
            summary=f"{len(issues)} cross-assembly coverage issue(s) detected",
            details=issues,
            evidence={
                "assemblies_checked": len(assemblies),
                "issues": issues,
            },
        )
