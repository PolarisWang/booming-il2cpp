"""Cross-family context for aggregation checks."""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

from .base import FamilyContext, _family_dir, _generated_cpp, _class_name_from_slug, _detect_class_name

_HERE = Path(__file__).resolve().parent
_FOUNDATION_DLL = _HERE.parent
_REPO_ROOT = _FOUNDATION_DLL.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "testing" / "foundation-dll"


@dataclass
class CrossFamilyContext:
    """Cross-family context for checks that need data from multiple families."""

    assembly: str
    family_contexts: dict[str, FamilyContext] = field(default_factory=dict)

    # Shared caches
    template_scan_result: dict[str, Any] | None = None
    all_codegen_files: dict[str, list[str]] = field(default_factory=dict)
    fingerprint_cache: dict[str, dict[str, str]] | None = None

    @classmethod
    def build(cls, assembly: str, family_slugs: list[str] | None = None) -> CrossFamilyContext:
        """Build a CrossFamilyContext for all families in an assembly (or specified subset)."""
        asm_dir = _VERIFICATION_BASE / assembly
        if not asm_dir.exists():
            return cls(assembly=assembly)

        if family_slugs is None:
            family_slugs = sorted(
                d.name for d in asm_dir.iterdir()
                if d.is_dir() and not d.name.startswith("_") and d.name != "reports"
            )

        family_contexts: dict[str, FamilyContext] = {}
        all_codegen_files: dict[str, list[str]] = {}

        for slug in family_slugs:
            fdir = _family_dir(assembly, slug)
            cpp_path = _generated_cpp(assembly, slug)
            if cpp_path is None:
                continue
            ctx = FamilyContext(
                assembly=assembly,
                family_slug=slug,
                family_dir=fdir,
                cpp_path=cpp_path,
            )
            # Pre-load generated C++ and class name
            _ = ctx.generated_cpp_content
            _ = ctx.class_name
            family_contexts[slug] = ctx

            # Collect codegen file paths
            codegen_dir = fdir / "codegen"
            if codegen_dir.exists():
                all_codegen_files[slug] = [
                    str(p.relative_to(_VERIFICATION_BASE))
                    for p in sorted(codegen_dir.rglob("*"))
                    if p.is_file()
                ]

        cross_ctx = cls(
            assembly=assembly,
            family_contexts=family_contexts,
            all_codegen_files=all_codegen_files,
        )

        # Wire back-reference
        for ctx in cross_ctx.family_contexts.values():
            object.__setattr__(ctx, "_cross_ctx", cross_ctx)

        return cross_ctx

    def get_all_families(self) -> list[tuple[str, FamilyContext]]:
        return list(self.family_contexts.items())

    def family_count(self) -> int:
        return len(self.family_contexts)
