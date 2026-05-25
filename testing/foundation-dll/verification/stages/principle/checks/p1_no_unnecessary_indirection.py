"""P1: Generated code calls runtime_stubs directly, not through unnecessary wrappers."""

from __future__ import annotations

import re

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck


class P1NoUnnecessaryIndirectionCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p1_no_unnecessary_indirection",
        principle="P1",
        severity="blocking",
        scope="family",
        description="Generated code calls runtime_stubs directly instead of through unnecessary wrappers",
    )

    STUB_FUNCTIONS = [
        "ChaosArrayClear", "ChaosArrayGetLength",
        "ChaosDatetimeGetUtcNow", "ChaosDatetimeGetHashCode",
        "ChaosMathSqrt", "ChaosInterlockedMemoryBarrier",
        "ChaosExceptionGetBaseException", "ChaosExceptionGetInnerException",
        "ChaosObjectEqualsStatic",
        # ChaosObjectCtor intentionally excluded: fundamental runtime function
        # for object construction, called by every family that uses new object().
        # Not a stub — it's the correct lowering for System.Object::.ctor().
        # "ChaosObjectCtor",
        "ChaosGuidNewGuid", "ChaosRandomNextBytes", "ChaosRandomNextDouble",
        "ChaosStringContains", "ChaosStringStartsWith", "ChaosStringJoinSs",
        "ChaosFormattablestringFactoryCreate",
    ]

    def run(self, ctx: FamilyContext) -> CheckResult:
        cpp = ctx.generated_cpp_content
        if not cpp:
            return CheckResult(
                check_id="p1_no_unnecessary_indirection", status="VIOLATION",
                summary="Generated C++ not found",
            )

        violations = []
        for fn in self.STUB_FUNCTIONS:
            count = len(re.findall(rf'\b{fn}\s*\(', cpp))
            if count > 0:
                violations.append({"function": fn, "call_count": count})

        if violations:
            return CheckResult(
                check_id="p1_no_unnecessary_indirection", status="CONCERN",
                summary=f"{len(violations)} runtime_stub calls in generated code",
                details=violations,
                evidence={"stub_call_count": sum(v["call_count"] for v in violations)},
            )

        return CheckResult(
            check_id="p1_no_unnecessary_indirection", status="ALIGNED",
            summary="No unnecessary runtime_stub calls in generated code",
        )
