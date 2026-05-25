"""P2: Scriban templates don't contain semantic decision logic.

This is a repo-level check, not family-specific. The CrossFamilyContext
caches the result so it's only scanned once per assembly.
"""

from __future__ import annotations

import re
from pathlib import Path

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck

_HERE = Path(__file__).resolve().parent
_PRINCIPLE_DIR = _HERE.parent
_FOUNDATION_DLL = _PRINCIPLE_DIR.parent
_REPO_ROOT = _FOUNDATION_DLL.parents[4]
_TEMPLATES_DIR = _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "Templates"

SEMANTIC_PATTERNS = [
    (r'\{\%\s*if\s+method\.\w+\s*\%\}', "method-level semantic if"),
    (r'\{\%\s*if\s+type_shape\s*==', "type_shape-based semantic branch"),
    (r'\{\%\s*if\s+\.\w+_kind\b', "kind-based semantic branch"),
    (r'\{\%\s*if\s+\.\w+_shape\b', "shape-based semantic branch"),
]


class P2TemplateHygieneCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p2_template_hygiene",
        principle="P2",
        severity="blocking",
        scope="family",
        description="Scriban templates don't contain semantic decision logic",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        # Use CrossFamilyContext cache if available
        cross_ctx = getattr(ctx, "_cross_ctx", None)
        if cross_ctx is not None and cross_ctx.template_scan_result is not None:
            cached = cross_ctx.template_scan_result
            return CheckResult(
                check_id="p2_template_hygiene", **cached,
            )

        result = self._scan_templates()

        # Cache in CrossFamilyContext if available
        if cross_ctx is not None:
            cross_ctx.template_scan_result = {
                "status": result.status,
                "summary": result.summary,
                "details": result.details,
                "evidence": result.evidence,
            }

        return result

    def _scan_templates(self) -> CheckResult:
        if not _TEMPLATES_DIR.exists():
            return CheckResult(
                check_id="p2_template_hygiene", status="NOT_APPLICABLE",
                summary="Templates directory not found",
            )

        violations = []
        for tpl_file in sorted(_TEMPLATES_DIR.rglob("*.scriban")):
            rel = tpl_file.relative_to(_TEMPLATES_DIR)
            content = tpl_file.read_text(encoding="utf-8")
            for pattern, label in SEMANTIC_PATTERNS:
                matches = re.findall(pattern, content)
                if matches:
                    violations.append({
                        "template": str(rel),
                        "pattern": label,
                        "count": len(matches),
                    })

        if violations:
            return CheckResult(
                check_id="p2_template_hygiene", status="CONCERN",
                summary=f"{len(violations)} semantic patterns in "
                        f"{len(set(v['template'] for v in violations))} templates",
                details=violations,
                evidence={
                    "template_count_with_concerns":
                        len(set(v["template"] for v in violations)),
                },
            )

        return CheckResult(
            check_id="p2_template_hygiene", status="ALIGNED",
            summary="All Scriban templates clean — no semantic decision logic",
        )
