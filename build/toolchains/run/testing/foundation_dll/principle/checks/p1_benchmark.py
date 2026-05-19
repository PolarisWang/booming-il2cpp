"""P1: Benchmark shows no significant performance regression.

Behavior change from old implementation: when benchmark-comparison-report.json
does not exist, returns CONCERN instead of NOT_APPLICABLE to surface the fact
that benchmarks were not run.
"""

from __future__ import annotations

import json

from ..base import (
    CheckMeta,
    CheckResult,
    FamilyContext,
    PrincipleCheck,
    _family_dir,
)


class P1BenchmarkCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p1_benchmark",
        principle="P1",
        severity="blocking",
        scope="family",
        description="Benchmark shows no significant performance regression",
        dependencies=["benchmark-comparison-report.json"],
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        bm_path = _family_dir(ctx.assembly, ctx.family_slug) / "benchmark-comparison-report.json"
        if not bm_path.exists():
            return CheckResult(
                check_id="p1_benchmark", status="CONCERN",
                summary="No benchmark data available — benchmarks may not have been run",
            )

        try:
            bm = json.loads(bm_path.read_bytes())
        except (json.JSONDecodeError, OSError):
            return CheckResult(
                check_id="p1_benchmark", status="CONCERN",
                summary="Benchmark report unreadable",
            )

        summary = bm.get("summary", {})
        avg_speedup = summary.get("averageSpeedupPercent", 0)
        managed_faster = summary.get("managedFasterCount", 0)
        native_faster = summary.get("nativeFasterCount", 0)
        invalid_count = summary.get("invalidCount", 0)
        total = summary.get("totalMethods", 0)

        # If most methods are stubs/invalid, benchmark comparison is meaningless
        if total > 0 and invalid_count >= total / 2:
            return CheckResult(
                check_id="p1_benchmark", status="NOT_APPLICABLE",
                summary=f"{invalid_count}/{total} methods are stubs — benchmark data not meaningful",
                evidence={
                    "average_speedup_percent": avg_speedup,
                    "managed_faster_count": managed_faster,
                    "native_faster_count": native_faster,
                    "invalid_count": invalid_count,
                    "total_methods": total,
                },
            )

        if isinstance(avg_speedup, (int, float)) and avg_speedup < -50:
            return CheckResult(
                check_id="p1_benchmark", status="VIOLATION",
                summary=f"Average speedup {avg_speedup}% — native significantly slower than managed",
                evidence={
                    "average_speedup_percent": avg_speedup,
                    "managed_faster_count": managed_faster,
                    "native_faster_count": native_faster,
                },
            )

        concerns = []
        matched = summary.get("matchedCount", 0)
        if managed_faster > 0 and matched > 0:
            # Only flag when a significant proportion of methods regress
            managed_ratio = managed_faster / matched
            if managed_ratio > 0.15:
                concerns.append(f"{managed_faster}/{matched} methods where native is slower than managed ({managed_ratio:.0%})")
            else:
                # Minor noise — log in evidence only, not a CONCERN
                pass
        if isinstance(avg_speedup, (int, float)) and avg_speedup < -5:
            concerns.append(f"average speedup {avg_speedup}% (native slightly slower overall)")

        status = "ALIGNED" if not concerns else "CONCERN"
        return CheckResult(
            check_id="p1_benchmark", status=status,
            summary=f"avg_speedup={avg_speedup}%" +
                    (f" ({'; '.join(concerns)})" if concerns else ""),
            evidence={
                "average_speedup_percent": avg_speedup,
                "managed_faster_count": managed_faster,
                "native_faster_count": native_faster,
                "matched_count": matched,
                "regression_ratio": round(managed_faster / matched, 3) if matched > 0 else 0,
            },
        )
