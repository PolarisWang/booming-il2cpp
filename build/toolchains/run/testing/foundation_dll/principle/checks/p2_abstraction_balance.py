"""P2 (monitoring, cross-family): Abstraction balance — call chain depth deviation.

Compares the ratio of generated C++ code size to method count across families.
Families with significantly different code-to-method ratios may indicate
unbalanced abstraction levels.
"""

from __future__ import annotations

import statistics

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck
from ..context import CrossFamilyContext


class P2AbstractionBalanceCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p2_abstraction_balance",
        principle="P2",
        severity="monitoring",
        scope="cross_family",
        description="Call chain depth / abstraction level deviation across families",
    )

    # Z-score threshold for flagging a family as imbalanced
    Z_THRESHOLD = 2.0

    def run(self, ctx: FamilyContext) -> CheckResult:
        cross_ctx = getattr(ctx, "_cross_ctx", None)
        if cross_ctx is None or not isinstance(cross_ctx, CrossFamilyContext):
            return CheckResult(
                check_id="p2_abstraction_balance", status="NOT_APPLICABLE",
                summary="Cross-family check requires CrossFamilyContext",
            )

        if cross_ctx.family_count() < 3:
            return CheckResult(
                check_id="p2_abstraction_balance", status="NOT_APPLICABLE",
                summary="Need at least 3 families for abstraction balance analysis",
            )

        # Analyze each family's code-to-method ratio
        family_ratios: dict[str, float] = {}
        for slug, fc in cross_ctx.get_all_families():
            cpp = fc.generated_cpp_content
            methods = fc.cpp_method_names()
            if not cpp or len(methods) < 3:
                continue
            code_size = len(cpp)
            ratio = code_size / len(methods)
            family_ratios[slug] = ratio

        if len(family_ratios) < 3:
            return CheckResult(
                check_id="p2_abstraction_balance", status="NOT_APPLICABLE",
                summary=f"Only {len(family_ratios)} families have sufficient data",
            )

        ratios = list(family_ratios.values())
        mean = statistics.mean(ratios)
        stdev = statistics.stdev(ratios) if len(ratios) > 1 else 0.0

        if stdev == 0:
            return CheckResult(
                check_id="p2_abstraction_balance", status="ALIGNED",
                summary="All families have identical code-to-method ratios",
            )

        outliers = []
        for slug, ratio in sorted(family_ratios.items()):
            z = (ratio - mean) / stdev
            if abs(z) > self.Z_THRESHOLD:
                outliers.append({
                    "family": slug,
                    "code_to_method_ratio": round(ratio, 1),
                    "z_score": round(z, 2),
                    "deviation": "high" if z > 0 else "low",
                })

        if outliers:
            return CheckResult(
                check_id="p2_abstraction_balance", status="CONCERN",
                summary=f"{len(outliers)} families deviate significantly from "
                        f"mean ratio ({mean:.1f} chars/method, σ={stdev:.1f})",
                details=outliers,
                evidence={
                    "mean_ratio": round(mean, 1),
                    "std_dev": round(stdev, 1),
                    "families_analyzed": len(family_ratios),
                    "outliers": outliers,
                },
            )

        return CheckResult(
            check_id="p2_abstraction_balance", status="ALIGNED",
            summary=f"All {len(family_ratios)} families within normal abstraction balance "
                    f"(mean={mean:.1f}, σ={stdev:.1f})",
        )
