"""P1: Generated code has no runtime family/method dispatch branches."""

from __future__ import annotations

import re

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck


class P1NoRedundantBranchCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p1_no_redundant_branch",
        principle="P1",
        severity="blocking",
        scope="family",
        description="Generated code has no runtime family/method dispatch branches",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        cpp = ctx.generated_cpp_content
        if not cpp:
            return CheckResult(
                check_id="p1_no_redundant_branch", status="VIOLATION",
                summary="Generated C++ not found",
            )

        branch_patterns = [
            (r'if\s*\(\s*family\s*[=!]=', "family-based branch"),
            (r'switch\s*\(\s*methodIndex\b', "methodIndex switch"),
            (r'if\s*\(\s*methodIndex\b', "methodIndex branch"),
            (r'CHAOS_FAMILY_SPECIFIC', "CHAOS_FAMILY_SPECIFIC marker"),
            (r'if\s*\(\s*__family\b', "runtime family check"),
        ]

        violations = []
        for pattern, label in branch_patterns:
            matches = re.findall(pattern, cpp, re.IGNORECASE)
            if matches:
                violations.append(f"{label} ({len(matches)} occurrences)")

        if violations:
            return CheckResult(
                check_id="p1_no_redundant_branch", status="VIOLATION",
                summary="Runtime dispatch branches detected in generated code",
                details=[{"pattern": v} for v in violations],
                evidence={"violation_count": len(violations), "violations": violations},
            )

        return CheckResult(
            check_id="p1_no_redundant_branch", status="ALIGNED",
            summary="No runtime dispatch branches in generated code",
            evidence={"checked_patterns": [p for p, _ in branch_patterns]},
        )
