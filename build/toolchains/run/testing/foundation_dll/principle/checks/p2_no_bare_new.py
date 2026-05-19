"""P2: Generated C++ uses CHAOS_IL2CPP_NEW_GC instead of bare `new Type{}`."""

from __future__ import annotations

import re

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck


class P2NoBareNewCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p2_no_bare_new",
        principle="P2",
        severity="blocking",
        scope="family",
        description="Generated C++ uses CHAOS_IL2CPP_NEW_GC instead of bare `new Type{}`",
    )

    LEGAL_PREFIXES = (
        "CHAOS_IL2CPP_NEW_GC", "new char", "new CHAOS_IL2CPP_",
        "new TypeInfoHot", "new HotpatchEntry", "new ModuleDescriptor",
        "new ArrayHeader", "new ObjectHeader", "new TypeInfo",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        cpp = ctx.generated_cpp_content
        if not cpp:
            return CheckResult(
                check_id="p2_no_bare_new", status="NOT_APPLICABLE",
                summary="Generated C++ not found",
            )

        bare_new_re = re.compile(r'\bnew\s+([A-Za-z_]\w*(?:::\w+)*)\s*[\{(]')
        violations: list[dict] = []

        for m in bare_new_re.finditer(cpp):
            start_of_line = cpp.rfind('\n', 0, m.start()) + 1
            line_no = cpp[:m.start()].count('\n') + 1
            line_text = cpp[start_of_line:cpp.find('\n', m.start())].strip()

            full_match = m.group(0)
            if any(full_match.startswith(p) for p in self.LEGAL_PREFIXES):
                continue
            type_name = m.group(1)
            if type_name in ("unsigned", "signed", "const"):
                continue

            violations.append({
                "line_no": line_no,
                "match": full_match,
                "line": line_text,
            })

        if violations:
            return CheckResult(
                check_id="p2_no_bare_new", status="VIOLATION",
                summary=f"{len(violations)} bare `new` in generated C++ "
                        "(should use CHAOS_IL2CPP_NEW_GC)",
                details=violations,
                evidence={"violation_count": len(violations), "violations": violations[:50]},
            )

        return CheckResult(
            check_id="p2_no_bare_new", status="ALIGNED",
            summary="All managed allocations use CHAOS_IL2CPP_NEW_GC or equivalent",
        )
