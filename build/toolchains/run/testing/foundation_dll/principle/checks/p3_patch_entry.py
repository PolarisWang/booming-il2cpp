"""P3: Methods have dispatch_table entries (interrupt_ptr for hotpatch)."""

from __future__ import annotations

import re

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck


class P3PatchEntryCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p3_patch_entry",
        principle="P3",
        severity="blocking",
        scope="family",
        description="Methods have dispatch_table entries (interrupt_ptr for hotpatch)",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        cpp = ctx.generated_cpp_content
        if not cpp:
            return CheckResult(
                check_id="p3_patch_entry", status="NOT_APPLICABLE",
                summary="Generated C++ not found",
            )

        dispatch_match = re.search(
            r'(?:static|extern\s+"C")\s+HotpatchEntryV0\s+\w+\[(\d+)\]\s*=\s*\{(.*?)\};',
            cpp, re.DOTALL,
        )
        if not dispatch_match:
            return CheckResult(
                check_id="p3_patch_entry", status="VIOLATION",
                summary="No dispatch table found in generated code",
            )

        table_size = int(dispatch_match.group(1))
        table_body = dispatch_match.group(2)

        entries = re.findall(
            r'\{\s*reinterpret_cast<.*?>\s*\(&(\w+)\).*?,\s*'
            r'reinterpret_cast<.*?>\s*\(&(\w+)\).*?\}',
            table_body,
        )

        missing_interrupt = 0
        present = 0
        for native_fn, interrupt_fn in entries:
            if "InterpreterEntryDirect" in interrupt_fn:
                present += 1
            else:
                missing_interrupt += 1

        if missing_interrupt > 0:
            return CheckResult(
                check_id="p3_patch_entry", status="VIOLATION",
                summary=f"{missing_interrupt}/{table_size} dispatch entries lack interrupt_ptr",
                evidence={
                    "table_size": table_size,
                    "with_interrupt_ptr": present,
                    "without_interrupt_ptr": missing_interrupt,
                },
            )

        has_hotpatch_module = "HotpatchModuleV0" in cpp and "s_hotpatch_module" in cpp

        return CheckResult(
            check_id="p3_patch_entry", status="ALIGNED" if has_hotpatch_module else "CONCERN",
            summary=f"Dispatch table: {table_size} entries, all with interrupt_ptr [OK]" +
                    ("" if has_hotpatch_module
                     else " (HotpatchModule not found — runtime registration may be incomplete)"),
            evidence={
                "table_size": table_size,
                "entries_with_interrupt_ptr": present,
                "has_hotpatch_module": has_hotpatch_module,
            },
        )
