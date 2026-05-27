"""P2 (monitoring, cross-family): Shared managed type consistency.

Extracts all chaos_mt_XXX method table declarations from each family's
generated C++ and verifies that types shared across families have consistent
hash values. A hash mismatch would indicate codegen inconsistency for the
same managed type.
"""

from __future__ import annotations

import re

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck
from ..context import CrossFamilyContext

_MT_PATTERN = re.compile(
    r'inline\s+MethodTable\s+chaos_mt_(?P<type_name>\S+)\s*=\s*'
    r'\{\s*[^,]+,\s*[^,]+,\s*(?P<hash>[^,]+),'
)


class P2SharedTypeConsistencyCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p2_shared_type_consistency",
        principle="P2",
        severity="monitoring",
        scope="cross_family",
        description="Shared managed type method table hash consistency across families",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        cross_ctx = getattr(ctx, "_cross_ctx", None)
        if cross_ctx is None or not isinstance(cross_ctx, CrossFamilyContext):
            return CheckResult(
                check_id="p2_shared_type_consistency", status="NOT_APPLICABLE",
                summary="Cross-family check requires CrossFamilyContext",
            )

        if cross_ctx.family_count() < 2:
            return CheckResult(
                check_id="p2_shared_type_consistency", status="NOT_APPLICABLE",
                summary="Need at least 2 families for type consistency comparison",
            )

        # Collect type→hash mappings per family
        family_types: dict[str, dict[str, str]] = {}
        for slug, fc in cross_ctx.get_all_families():
            cpp = fc.generated_cpp_content
            if not cpp:
                continue
            types: dict[str, str] = {}
            for m in _MT_PATTERN.finditer(cpp):
                type_name = m.group("type_name")
                hash_val = m.group("hash").strip()
                types[type_name] = hash_val
            family_types[slug] = types

        if len(family_types) < 2:
            return CheckResult(
                check_id="p2_shared_type_consistency", status="NOT_APPLICABLE",
                summary=f"Only {len(family_types)} families have generated C++ with type info",
            )

        # Build cross-family type→hash map and detect conflicts
        type_hash_registry: dict[str, dict[str, str]] = {}  # type_name -> {slug: hash}
        for slug, types in family_types.items():
            for type_name, hash_val in types.items():
                if type_name not in type_hash_registry:
                    type_hash_registry[type_name] = {}
                type_hash_registry[type_name][slug] = hash_val

        # Filter to only shared types (appear in >= 2 families)
        shared_type_conflicts: list[dict] = []
        shared_type_ok = 0
        for type_name, slug_hash_map in sorted(type_hash_registry.items()):
            if len(slug_hash_map) < 2:
                continue
            unique_hashes = set(slug_hash_map.values())
            if len(unique_hashes) > 1:
                shared_type_conflicts.append({
                    "type": type_name,
                    "hash_per_family": slug_hash_map,
                })
            else:
                shared_type_ok += 1

        if not shared_type_conflicts:
            return CheckResult(
                check_id="p2_shared_type_consistency", status="ALIGNED",
                summary=f"All {shared_type_ok} shared types have consistent hashes across "
                        f"{len(family_types)} families",
                evidence={
                    "families_analyzed": len(family_types),
                    "shared_types_consistent": shared_type_ok,
                },
            )

        return CheckResult(
            check_id="p2_shared_type_consistency", status="CONCERN",
            summary=f"{len(shared_type_conflicts)} shared type(s) with inconsistent "
                    f"hashes across families",
            details=shared_type_conflicts,
            evidence={
                "families_analyzed": len(family_types),
                "shared_types_consistent": shared_type_ok,
                "shared_type_conflicts": shared_type_conflicts,
            },
        )
