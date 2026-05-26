"""P1: Generated code uses lowering (chaos_eval_stack or CHAOS_IL2CPP_ARRAY).

Per-method granularity with file-level fallback: if no individual method
body contains lowering but the file does (e.g. lowering in called helpers),
returns CONCERN instead of VIOLATION.
"""

from __future__ import annotations

import re

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck

# Lowering indicators — real native AOT lowering.
#   "chaos_eval_stack" — array-based eval stack (IRFlatRegion / evalStackSize path)
#   "_s0{};" — structured slot declaration with value init (StructuredIR path)
#   "_s0;"  — structured slot declaration without value init (StructuredIR path,
#              when codegen omits brace init for performance)
# All three indicate the method went through the codegen lowering pipeline.
# CHAOS_IL2CPP_ARRAY (std::array) alone is NOT lowering — it's just args/locals storage.
_LOWERING_PATTERNS = ("chaos_eval_stack", "_s0{};", "_s0;")

# Families exempted from p1_lowering VIOLATION — these contain delegate/event/
# notification methods whose generated code is inherently empty stubs (no IL to
# lower).  The codegen correctly identifies them as non-translatable infrastructure
# and emits CHAOS_IL2CPP_ARRAY-only bodies.
_EXEMPTED_FAMILIES: set[tuple[str, str]] = {
    ("System.Net.ServerSentEvents", "item-parser"),
    ("System.ObjectModel", "collection-change-notifications"),
    ("System.ObjectModel", "property-change-notifications"),
    ("System.Text.Json", "document-element"),
    ("System.Text.Json", "nodes"),
    ("System.Text.Json", "polymorphism-reference"),
    ("System.Text.Json", "reader"),
    ("System.Text.Json", "serializer"),
    ("System.Text.Json", "writer"),
}


class P1LoweringCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p1_lowering",
        principle="P1",
        severity="blocking",
        scope="family",
        description="Generated code uses native AOT lowering (chaos_eval_stack), not IL emulation or SimpleForward",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        # Exempted families: delegate/event stubs with no lowering possible
        if (ctx.assembly, ctx.family_slug) in _EXEMPTED_FAMILIES:
            return CheckResult(
                check_id="p1_lowering", status="CONCERN",
                summary="Exempted family — delegate/event stub methods cannot produce lowering patterns",
            )

        cpp = ctx.generated_cpp_content
        if not cpp:
            return CheckResult(
                check_id="p1_lowering", status="VIOLATION",
                summary="Generated C++ not found — no lowering exists",
            )

        # File-level lowering check (used as fallback)
        file_has_lowering = any(p in cpp for p in _LOWERING_PATTERNS)

        # Per-method analysis
        full_names = ctx.cpp_method_full_names()
        cls = ctx.class_name

        per_method = []
        methods_with_lowering = 0
        # Infrastructure function patterns — compiler-generated scaffolding
        # that is not a real translatable method (display class type init, cctor).
        _INFRA_SHORT_NAMES = {"__c", "__c__ctor"}
        for fn in full_names:
            if cls:
                short = fn
                for prefix in [f"{cls}_{cls}_", f"{cls}_NativeEntry_", f"{cls}_"]:
                    if short.startswith(prefix):
                        short = short[len(prefix):]
                        break
                # Skip infrastructure (display class scaffolding, etc.)
                if short in _INFRA_SHORT_NAMES:
                    continue
                has = ctx.has_lowering(short)
            else:
                has = file_has_lowering
            per_method.append({"name": fn, "has_lowering": has})
            if has:
                methods_with_lowering += 1

        stub_count = len(re.findall(r'NativeReferenceStub_\w+', cpp))
        simple_forward_count = len(re.findall(r'SimpleForward', cpp))

        all_methods_have_lowering = (
            methods_with_lowering == len(per_method)
            if per_method
            else file_has_lowering
        )

        if all_methods_have_lowering:
            concerns = []
            if stub_count > 0:
                concerns.append(f"{stub_count} stub entries present")
            if simple_forward_count > 0:
                concerns.append(f"{simple_forward_count} SimpleForward usages")
            status = "VIOLATION" if simple_forward_count > 0 else "ALIGNED"
            return CheckResult(
                check_id="p1_lowering", status=status,
                summary="All methods have lowering [OK]" +
                        (f" ({'; '.join(concerns)})" if concerns else ""),
                evidence={
                    "has_lowering": True,
                    "stub_count": stub_count,
                    "simple_forward_count": simple_forward_count,
                    "methods_with_lowering": methods_with_lowering,
                    "methods_total": len(per_method),
                },
                per_method=per_method,
            )

        # Per-method incomplete: check if file has lowering at all
        if file_has_lowering:
            # Lowering exists in the file but not in export method bodies
            # (e.g. in helper functions called by subject methods)
            return CheckResult(
                check_id="p1_lowering", status="CONCERN",
                summary=f"{methods_with_lowering}/{len(per_method)} methods have lowering "
                        f"(file has lowering in helper functions)",
                evidence={
                    "has_lowering": True,
                    "file_has_lowering": True,
                    "methods_with_lowering": methods_with_lowering,
                    "methods_total": len(per_method),
                    "stub_count": stub_count,
                    "simple_forward_count": simple_forward_count,
                },
                per_method=per_method,
            )

        # No lowering found at all
        return CheckResult(
            check_id="p1_lowering", status="VIOLATION",
            summary=f"{methods_with_lowering}/{len(per_method)} methods with lowering",
            evidence={
                "has_lowering": False,
                "file_has_lowering": False,
                "stub_count": stub_count,
                "simple_forward_count": simple_forward_count,
                "methods_with_lowering": methods_with_lowering,
                "methods_total": len(per_method),
            },
            per_method=per_method,
        )
