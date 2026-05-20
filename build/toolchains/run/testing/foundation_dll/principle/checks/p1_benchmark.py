"""P1: Benchmark shows no significant performance regression.

When benchmark-comparison-report.json does not exist, returns NOT_APPLICABLE
(no benchmark data to judge). This is expected for families without managed
benchmark harnesses.

Supports both schema v1 (nativeFasterCount) and schema v2 (nativeAotFasterCount).

Known stub architecture exemptions:
  Methods with unavoidable native overhead (e.g. TLS access, GC interaction)
  are excluded from the average-speedup threshold to avoid masking real regressions.
  These are tracked as Phase 4 stub optimization work.
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


# Method short names (after last ::) with documented stub architecture limitations.
# These have unavoidable native overhead vs managed JIT and are tracked separately.
_STUB_LIMITATION_METHODS: set[str] = {
    "get_CurrentThread",  # TLS access overhead inherent in native stubs
}


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

        # Per-method speedup data for all matched methods (schema v2)
        method_results = bm.get("methodResults", [])
        matched_count = summary.get("matchedCount", 0)

        # Check 1: AOT speedup must not be severely negative
        if isinstance(avg_aot_speedup, (int, float)) and avg_aot_speedup < -50:
            # Check if the violation is caused by known stub-limitation methods
            excluded_speedups = []
            included_speedups = []
            for mr in method_results:
                sid = mr.get("methodSubjectId", "")
                short_name = sid.rsplit("::", 1)[-1] if "::" in sid else sid
                aot_sp = mr.get("nativeAotSpeedupPercent")
                if aot_sp is not None:
                    # Match by method name prefix (before the first ':')
                    method_base = short_name.split(":")[0] if ":" in short_name else short_name
                    if method_base in _STUB_LIMITATION_METHODS:
                        excluded_speedups.append(aot_sp)
                    else:
                        included_speedups.append(aot_sp)

            if excluded_speedups and included_speedups:
                # Recompute average without stub-limitation methods
                filtered_avg = sum(included_speedups) / len(included_speedups)
                if filtered_avg >= -50:
                    # Only stub-limitation methods caused the violation
                    return CheckResult(
                        check_id="p1_benchmark", status="CONCERN",
                        summary=(
                            f"avg_aot_speedup={avg_aot_speedup}% "
                            f"(filtered={filtered_avg:.1f}% excluding {len(excluded_speedups)} "
                            f"stub-limitation methods)"
                        ),
                        evidence={
                            "average_speedup_percent": avg_aot_speedup,
                            "filtered_average_speedup_percent": round(filtered_avg, 2),
                            "excluded_stub_methods": len(excluded_speedups),
                            "excluded_stub_details": {
                                mr.get("methodSubjectId", "").rsplit("::", 1)[-1]: mr.get("nativeAotSpeedupPercent")
                                for mr in method_results
                                if mr.get("nativeAotSpeedupPercent") is not None
                                and (mr.get("methodSubjectId", "").rsplit("::", 1)[-1].split(":")[0] in _STUB_LIMITATION_METHODS)
                            },
                            "average_native_jit_speedup_percent": avg_jit_speedup,
                            "managed_faster_count": managed_faster,
                            "native_aot_faster_count": native_aot_faster,
                            "native_jit_faster_count": native_jit_faster,
                            "matched_count": matched_count,
                            "regression_ratio": round(managed_faster / matched_count, 3) if matched_count > 0 else 0,
                        },
                    )

            # Still a real violation
            return CheckResult(
                check_id="p1_benchmark", status="VIOLATION",
                summary=f"Average AOT speedup {avg_aot_speedup}% — native significantly slower than managed",
                evidence={
                    "average_speedup_percent": avg_aot_speedup,
                    "average_native_jit_speedup_percent": avg_jit_speedup,
                    "managed_faster_count": managed_faster,
                    "native_aot_faster_count": native_aot_faster,
                    "native_jit_faster_count": native_jit_faster,
                    "matched_count": matched_count,
                },
            )

        # Check 2: AOT managed_faster ratio concern
        if managed_faster > 0 and matched_count > 0:
            managed_ratio = managed_faster / matched_count
            if managed_ratio > 0.15:
                concerns.append(f"{managed_faster}/{matched_count} methods where native is slower than managed ({managed_ratio:.0%})")

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
                "matched_count": matched_count,
                "regression_ratio": round(managed_faster / matched_count, 3) if matched_count > 0 else 0,
            },
        )
