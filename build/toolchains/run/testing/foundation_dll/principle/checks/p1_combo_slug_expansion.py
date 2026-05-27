"""P1 (monitoring, cross-family): Combo-slug expansion viability.

Verifies that comma-separated combo slugs have a corresponding individual
family directory for each constituent slug. Each constituent should exist
as its own family so combo-slug testing validates cross-family interaction.
"""

from __future__ import annotations

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck
from ..context import CrossFamilyContext


class P1ComboSlugExpansionCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p1_combo_slug_expansion",
        principle="P1",
        severity="monitoring",
        scope="cross_family",
        description="Combo slugs must have individual family dirs for each constituent slug",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        cross_ctx = getattr(ctx, "_cross_ctx", None)
        if cross_ctx is None or not isinstance(cross_ctx, CrossFamilyContext):
            return CheckResult(
                check_id="p1_combo_slug_expansion", status="NOT_APPLICABLE",
                summary="Cross-family check requires CrossFamilyContext",
            )

        issues = []
        for slug, fc in cross_ctx.get_all_families():
            if "," not in slug:
                continue
            constituents = [s.strip() for s in slug.split(",") if s.strip()]
            missing = []
            for part in constituents:
                if part not in cross_ctx.family_contexts:
                    asm_dir = fc.family_dir.parent
                    if not (asm_dir / part).is_dir():
                        missing.append(part)

            if missing:
                issues.append({
                    "combo_slug": slug,
                    "constituents": constituents,
                    "missing_constituents": missing,
                })

        if not issues:
            return CheckResult(
                check_id="p1_combo_slug_expansion", status="ALIGNED",
                summary="All combo-slug constituents have corresponding family directories",
            )

        return CheckResult(
            check_id="p1_combo_slug_expansion", status="CONCERN",
            summary=f"{len(issues)} combo slug(s) with missing constituent directories",
            details=issues,
            evidence={"issues": issues},
        )
