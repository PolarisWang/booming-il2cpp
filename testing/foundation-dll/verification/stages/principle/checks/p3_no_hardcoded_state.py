"""P3 (monitoring): No hardcoded static state in generated code.

Scans generated C++ for persistent static state variables that cannot
be reverted during hotupdate.
"""

from __future__ import annotations

import re

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck

HARDCODED_STATE_PATTERNS: list[tuple[re.Pattern, str]] = [
    (re.compile(r'\bstatic\s+(int|bool|std::atomic)\b'), "primitive static state"),
    (re.compile(r'\bglobal_flag_\b'), "known global flag pattern"),
    (re.compile(r'\bCHAOS_IL2CPP_STATIC_CONSTRUCTOR_HAS_RUN\b'), "static ctor guard"),
    (re.compile(r'\bstatic\s+\w+\s*=\s*(true|false|nullptr)\b'), "static boolean/ptr assignment"),
]


class P3NoHardcodedStateCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p3_no_hardcoded_state",
        principle="P3",
        severity="monitoring",
        scope="family",
        description="No hardcoded static state in generated code "
                    "(ensures hotupdate reversion capability)",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        cpp = ctx.generated_cpp_content
        if not cpp:
            return CheckResult(
                check_id="p3_no_hardcoded_state", status="NOT_APPLICABLE",
                summary="Generated C++ not found",
            )

        violations = []
        for pattern, label in HARDCODED_STATE_PATTERNS:
            matches = pattern.findall(cpp)
            if matches:
                violations.append({
                    "pattern": label,
                    "count": len(matches),
                    "examples": list(set(matches))[:5],
                })

        if not violations:
            return CheckResult(
                check_id="p3_no_hardcoded_state", status="ALIGNED",
                summary="No hardcoded static state in generated code",
            )

        total_violations = sum(v["count"] for v in violations)
        if total_violations <= 3:
            status = "CONCERN"
        else:
            status = "VIOLATION"

        return CheckResult(
            check_id="p3_no_hardcoded_state", status=status,
            summary=f"{total_violations} hardcoded static state pattern(s) detected",
            details=violations,
            evidence={
                "total_violations": total_violations,
                "patterns_found": [
                    {"pattern": v["pattern"], "count": v["count"]}
                    for v in violations
                ],
            },
        )
