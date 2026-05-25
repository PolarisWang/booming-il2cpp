"""P3: Patch data files exist for hotupdate support."""

from __future__ import annotations

from ..base import (
    CheckMeta,
    CheckResult,
    FamilyContext,
    PrincipleCheck,
    _family_dir,
    _VERIFICATION_BASE,
)


class P3PatchdataCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p3_patchdata",
        principle="P3",
        severity="blocking",
        scope="family",
        description="Patch data files exist for hotupdate support",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        family_dir = _family_dir(ctx.assembly, ctx.family_slug)

        patchdata_files = list(family_dir.rglob("*.patchdata"))
        hotupdate_dir = family_dir / "il2cpp_dist" / "hotupdate"
        hotupdate_files = list(hotupdate_dir.rglob("*.patchdata")) if hotupdate_dir.exists() else []
        all_files = patchdata_files + hotupdate_files

        if not all_files:
            hu_runner = family_dir / "hotupdate-verification-report.json"
            if hu_runner.exists():
                return CheckResult(
                    check_id="p3_patchdata", status="VIOLATION",
                    summary="HotUpdate test exists but no .patchdata files found",
                    evidence={"patchdata_count": 0, "hotupdate_report_exists": True},
                )
            return CheckResult(
                check_id="p3_patchdata", status="NOT_APPLICABLE",
                summary="No patch data needed (not on hotupdate path)",
            )

        valid = [f for f in all_files if f.stat().st_size > 10]

        return CheckResult(
            check_id="p3_patchdata",
            status="ALIGNED" if len(valid) == len(all_files) else "CONCERN",
            summary=f"{len(valid)}/{len(all_files)} valid .patchdata files",
            evidence={
                "patchdata_count": len(all_files),
                "valid_count": len(valid),
                "stub_patchdata_count": len(all_files) - len(valid),
                "files": [str(f.relative_to(_VERIFICATION_BASE)) for f in all_files],
            },
        )
