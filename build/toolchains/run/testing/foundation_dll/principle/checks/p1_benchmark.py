"""P1: Benchmark shows no significant performance regression.

When benchmark-comparison-report.json does not exist, returns NOT_APPLICABLE
(no benchmark data to judge). This is expected for families without managed
benchmark harnesses.

Supports both schema v1 (nativeFasterCount) and schema v2 (nativeAotFasterCount).
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
                check_id="p1_benchmark", status="NOT_APPLICABLE",
                summary="No benchmark data available — benchmarks not run for this family",
            )

        try:
            bm = json.loads(bm_path.read_bytes())
        except (json.JSONDecodeError, OSError):
            return CheckResult(
                check_id="p1_benchmark", status="CONCERN",
                summary="Benchmark report unreadable",
            )

        summary = bm.get("summary", {})

        # Schema v2 uses nativeAotFasterCount; schema v1 uses nativeFasterCount
        native_aot_faster = summary.get("nativeAotFasterCount",
                            summary.get("nativeFasterCount", 0))
        native_jit_faster = summary.get("nativeJitFasterCount", 0)
        managed_faster = summary.get("managedFasterCount", 0)
        invalid_count = summary.get("invalidCount", 0)
        total = summary.get("totalMethods", 0)

        # Primary AOT speedup (works for both v1 and v2)
        avg_aot_speedup = summary.get("averageNativeAotSpeedupPercent") or summary.get("averageSpeedupPercent", 0)

        # JIT speedup (v2 only)
        avg_jit_speedup = summary.get("averageNativeJitSpeedupPercent")

        # If most methods are stubs/invalid, benchmark comparison is meaningless
        if total > 0 and invalid_count >= total / 2:
            return CheckResult(
                check_id="p1_benchmark", status="NOT_APPLICABLE",
                summary=f"{invalid_count}/{total} methods are stubs — benchmark data not meaningful",
                evidence={
                    "average_speedup_percent": avg_aot_speedup,
                    "average_native_jit_speedup_percent": avg_jit_speedup,
                    "managed_faster_count": managed_faster,
                    "native_aot_faster_count": native_aot_faster,
                    "native_jit_faster_count": native_jit_faster,
                    "invalid_count": invalid_count,
                    "total_methods": total,
                },
            )

        concerns = []

        # Check 1: AOT speedup must not be severely negative
        if isinstance(avg_aot_speedup, (int, float)) and avg_aot_speedup < -50:
            return CheckResult(
                check_id="p1_benchmark", status="VIOLATION",
                summary=f"Average AOT speedup {avg_aot_speedup}% — native significantly slower than managed",
                evidence={
                    "average_speedup_percent": avg_aot_speedup,
                    "average_native_jit_speedup_percent": avg_jit_speedup,
                    "managed_faster_count": managed_faster,
                    "native_aot_faster_count": native_aot_faster,
                    "native_jit_faster_count": native_jit_faster,
                },
            )

        # Check 2: AOT managed_faster ratio concern
        matched = summary.get("matchedCount", 0)
        if managed_faster > 0 and matched > 0:
            managed_ratio = managed_faster / matched
            if managed_ratio > 0.15:
                concerns.append(f"{managed_faster}/{matched} methods where native is slower than managed ({managed_ratio:.0%})")

        # Check 3: AOT slight slowdown concern
        if isinstance(avg_aot_speedup, (int, float)) and avg_aot_speedup < -5:
            concerns.append(f"average AOT speedup {avg_aot_speedup}% (native slightly slower overall)")

        # Check 4: JIT speedup informational (no violation — interpreter is expected to be slower)
        jit_concern = ""
        if avg_jit_speedup is not None:
            if isinstance(avg_jit_speedup, (int, float)) and avg_jit_speedup < -500:
                jit_concern = f"JIT speedup {avg_jit_speedup}% (interpreter significantly slower than managed JIT)"
                concerns.append(jit_concern)
            else:
                jit_concern = f"JIT speedup {avg_jit_speedup}%"

        status = "ALIGNED" if not concerns else "CONCERN"
        summary_str = f"avg_aot_speedup={avg_aot_speedup}%"
        if avg_jit_speedup is not None:
            summary_str += f", avg_jit_speedup={avg_jit_speedup}%"
        if concerns:
            summary_str += f" ({'; '.join(concerns)})"

        return CheckResult(
            check_id="p1_benchmark", status=status,
            summary=summary_str,
            evidence={
                "average_speedup_percent": avg_aot_speedup,
                "average_native_jit_speedup_percent": avg_jit_speedup,
                "managed_faster_count": managed_faster,
                "native_aot_faster_count": native_aot_faster,
                "native_jit_faster_count": native_jit_faster,
                "matched_count": matched,
                "regression_ratio": round(managed_faster / matched, 3) if matched > 0 else 0,
            },
        )
