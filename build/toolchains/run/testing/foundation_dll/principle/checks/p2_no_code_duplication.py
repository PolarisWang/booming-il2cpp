"""P2 (monitoring, cross-family): Code duplicate detection across families.

Computes method-name-based Jaccard similarity between families.
Families with >80% method name overlap are flagged.
"""

from __future__ import annotations

import itertools

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck
from ..context import CrossFamilyContext


class P2NoCodeDuplicationCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p2_no_code_duplication",
        principle="P2",
        severity="monitoring",
        scope="cross_family",
        description="Cross-family code duplication (method name similarity) detection",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        # Access cross-family context
        cross_ctx = getattr(ctx, "_cross_ctx", None)
        if cross_ctx is None or not isinstance(cross_ctx, CrossFamilyContext):
            return CheckResult(
                check_id="p2_no_code_duplication", status="NOT_APPLICABLE",
                summary="Cross-family check requires CrossFamilyContext",
            )

        if cross_ctx.family_count() < 2:
            return CheckResult(
                check_id="p2_no_code_duplication", status="NOT_APPLICABLE",
                summary="Need at least 2 families for cross-family comparison",
            )

        # Collect method names per family
        family_methods: dict[str, set[str]] = {}
        for slug, fc in cross_ctx.get_all_families():
            methods = set(fc.cpp_method_names())
            # Only include families with non-trivial methods
            if len(methods) >= 3:
                family_methods[slug] = methods

        if len(family_methods) < 2:
            return CheckResult(
                check_id="p2_no_code_duplication", status="NOT_APPLICABLE",
                summary="Fewer than 2 families with non-trivial method sets",
            )

        pairs = []
        for (fa, ma), (fb, mb) in itertools.combinations(family_methods.items(), 2):
            intersection = len(ma & mb)
            union = len(ma | mb)
            if union == 0:
                continue
            jaccard = intersection / union
            if jaccard > 0.8:
                pairs.append({
                    "familyA": fa,
                    "familyB": fb,
                    "method_overlap": round(jaccard, 2),
                    "shared_methods": intersection,
                })

        if not pairs:
            return CheckResult(
                check_id="p2_no_code_duplication", status="ALIGNED",
                summary=f"No significant cross-family method overlap detected "
                        f"(compared {len(family_methods)} families)",
            )

        return CheckResult(
            check_id="p2_no_code_duplication", status="CONCERN",
            summary=f"{len(pairs)} family pair(s) with >80% method name overlap",
            details=pairs,
            evidence={"pairs": pairs, "families_compared": len(family_methods)},
        )
